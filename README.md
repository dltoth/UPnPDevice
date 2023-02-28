# UPnPDevice
Developing applications for Arduino (ESP8266 and ESP32) requires creating custom HTML for the user interface. UPnPDevice is a framework for HTML user interface that follows the [UPnP Device Arcihtecture](http://upnp.org/specs/arch/UPnP-arch-DeviceArchitecture-v1.1.pdf). When coupled with the additional [SSDP library](https://github.com/dltoth/ssdp) also provides simple service discovery. UPnPDevice is a set of base classes for building a UPnP device hierarchy that can be advertised with SSDP over a local network. 

The set of classes include:

```
  RootDevice       := A UPnP root device container. Root devices can have up to 8 embedded 
                      UPnPDevices and up to 8 UPnPServices
  UPnPDevice       := A base class for UPnP devices. UPnPDevices can have up to 8 
                      UPnPServices
  UPnPService      := A base class for UPnP services. Services have a callable HTTP 
                      interface
  UPnPObject       := The base class for RootDevice, UPnPDevice, and UPnPService
  Sensor           := A virtual base UPnPDevice that has simple output based display, like 
                      a Thermometer or Clock
  Control          := A virtual base UPnPDevice with more complex display, including user 
                      interaction, like a toggle
  GetConfiguration := A UPnPService base class for returning device configuration in XML
  SetConfiguration := A UPnPService base class for setting device configuration from callable 
                      HTTP interface
```

Both Sensor and Control include default GetConfiguration and SetConfiguration Services, as does RootDevice.

In what follows, a conceptual framework for the library is presented, and 4 examples will be detailed:
 1. Creating a custom UPnPDevice and hierarchy
 2. Creating a simple Sensor that displays a message
 3. Adding configuration to that Sensor
 4. Creating a custom control that implements a toggle.

## Conceptual Framework

UPnP Defines three basic constructs: root devices, embedded devices, and services, where both root devices and embedded devices can have services and embedded devices. Services are leaf nodes of the hierarchy and may not have either embedded devices or services. Essentially, a root device is a container for a device heirarchy consisting of embedded devices and services. UPnP does not limit the depth or breadth of a device heirarchy. Root devices publish their functionality over HTTP and discovery (SSDP) over UDP. 

In this library, only root devices ([RootDevice](https://github.com/dltoth/UPnPDevice/blob/main/src/UPnPDevice.h)) can have embedded devices ([UPnPDevice](https://github.com/dltoth/UPnPDevice/blob/main/src/UPnPDevice.h)), and the number of embedded devices is limited to 8. Both RootDevices and embedded devices can have services ([UPnPService](https://github.com/dltoth/UPnPDevice/blob/main/src/UPnPService.h)), and the number of services is also limited to 8. In terms of class heirarchy, RootDevice is a subclass of UPnPDevice, which in turn is a subclass of [UPnPObject](https://github.com/dltoth/UPnPDevice/blob/main/src/UPnPService.h), and UPnPService is a subclass of UPnPObject.

### Runtime Type Identification (RTTI) and UPnP Device Type

UPnPObjects include the notion of class type for runtime type identification (RTTI). RTTI allows typesafe casting and is required by all UPnPDevices and UPnPServices. Additionally, UPnPDevices and UPnPServices have a UPnP device type (or service type). Device Type is a UPnP required construct of the form

```
   urn:CompanyName:device:deviceName:version
or
   urn:CompanyName:service:serviceName:version
```

Where CompanyName is the company domain name with "-" substituting ".". For example "urn:LeelanauSoftware-com:device:Thermometer:1" is the device type for [Thermometer](https://github.com/dltoth/DeviceLib/blob/main/src/Thermometer.h) offered by LeelanauSoftware.com. Device and Service type are used for SSDP service discovery to find specific device types on the local network. UPnP restricts the device (or service) type to 64 characters.

The following macros are used to define RTTI and UPnP Device type in the header file of a UPnPDevice subclass:

```
      DEFINE_RTTI;
      DERIVED_TYPE_CHECK(baseName);
```

The macros add the following lines of code to the header file for the class:

```
      private: static const ClassType  _classType;             
      public:  static const ClassType* classType();   
      public:  virtual void*           as(const ClassType* t);
      public:  virtual boolean         isClassType( const ClassType* t);
      private: static const char*      _upnpType;                                      
      public:  static const char*      upnpType()                  
      public:  virtual const char*     getType()                   
      public:  virtual boolean         isType(const char* t)       
```
**Notes:** 
1. The static form upnpType() is used for device search, and tied to the class, as in Thermometer::upnpType(). The virtual form getType() will provide the device type regardless of how a pointer to the device is cast. 
2. The macro DERIVED_TYPE_CHECK(*baseName*) declares *baseName* as being a subclass of the class being defined. 
3. The static members *_classType* and *_upnpType* must be initialized in the .cpp file with macros:

```
      INITIALIZE_STATIC_TYPE(className);
      INITIALIZE_UPnP_TYPE(className,urn:company-name:device:deviceName:1);
```

where *className* is the class name of the UPnPDevice being defined, *company-name* is the company name described earlier, and *deviceName* is the unique UPnP device name.

**Why RTTI?**

Since each embedded UPnPDevice provides its own bit of functionality, one device may rely on another. For example, a timer controlled relay may require a [SoftwareClock](https://github.com/dltoth/DeviceLib/blob/main/src/SoftwareClock.h), or a humidity controlled fan may require a [Thermometer](https://github.com/dltoth/DeviceLib/blob/main/src/Thermometer.h). A device implementer, being familiar with onboard embedded devices, will know if a device or service is available. 

First note that any UPnPObject can retrieve a pointer to the RootDevice as:

```
   RootDevice* root = rootDevice();
```

So, if a RootDevice is expected to include a [SoftwareClock](https://github.com/dltoth/DeviceLib/blob/main/src/SoftwareClock.h), then the static RootDevice method

```
SoftwareClock* clock = (SoftwareClock*)RootDevice::getDevice(rootDevice(), SoftwareClock::classType());
```

can be used to retrieve a pointer to a SoftwareClock. If SoftwareClock is an embedded device and setup() has been called on the RootDevice, clock will be non-NULL. 

**Important:** RootDevice setup() instantiates the device hierarchy, so RootDevice::getDevice() will necessarily return NULL until all UPnPDevices and UPnPServices have been added and setup has been called.

### Device Instantiation

All of the UPnPDevice classes are expected to be constructed and managed in global scope above the setup() function in an Arduino sketch. Copy construction and and Object destruction are not allowed; objects are expected to live over the life of an executing application. UPnPObjects are passed via pointer. The following macro is used to enforce this behavior:

```
   DEFINE_EXCLUSIONS(className);
```

which adds the following lines of code to a header file:

```
      className(const className&)= delete;
      className& operator=(const className&)= delete;
```

### Custom UPnPDevice

The most basic custom device or service consists of constructors, RTTI definition, and initialization. The header file [CustomDevice.h](https://github.com/dltoth/UPnPDevice/blob/main/examples/UPnPDevice/CustomDevice.h) should consist of:

```
/**
 *
 */
 
#ifndef CUSTOMDEVICE_H
#define CUSTOMDEVICE_H

#include <UPnPDevice.h>

/** Leelanau Software Company namespace 
*  
*/
using namespace lsc;
  
class CustomDevice : public UPnPDevice {
  public:
    CustomDevice() :  UPnPDevice("deviceTarget") {setDisplayName("Custom Device");};
    CustomDevice(const char* target) : UPnPDevice(target) {setDisplayName("Custom Device");};

    DEFINE_RTTI;
    DERIVED_TYPE_CHECK(UPnPDevice);
    DEFINE_EXCLUSIONS(CustomDevice);
};

#endif
```

The implementation .cpp file should contain:

```
/**
 * 
 */

#include "CustomDevice.h"

INITIALIZE_STATIC_TYPE(CustomDevice);
INITIALIZE_UPnP_TYPE(CustomDevice,urn:CompanyName-com:device:CustomDevice:1);
```

The implemention of CustomDevice will provide basic device display, RTTI, and device type information.

## Default Device Hierarchy and Display

All devices are displayed with a set of HTML entities and styles defined in [CommonUtil](https://github.com/dltoth/CommonUtil). A number of the examples presented here also use HTML formatting functions found there as well. In particular, [formatHeader](https://github.com/dltoth/CommonUtil/blob/main/src/CommonProgmem.h) will format an HTML header for a web page that includes a reference to the CSS stylesheet **/styles.css**; RootDevice will register an HTML request handler for **/styles.css** on setup.

See the sketch folder [UPnPDevice](https://github.com/dltoth/UPnPDevice/blob/main/examples/UPnPDevice/) for an example of creating a device hierarchy consisting of a RootDevice, CustomDevice, and a UPnPdevice. Notice header and implementation files for [CustomDevice](https://github.com/dltoth/UPnPDevice/blob/main/examples/UPnPDevice/CustomDevice.h) have been included, and CustomService has been added as well. Looking at the [sketch](https://github.com/dltoth/UPnPDevice/blob/main/examples/UPnPDevice/UPnPDevice.ino), notice the following:

**Namespace Declaration**

The Leelanau Software Company namespace is used for all of the libraries [SSDP](https://github.com/dltoth/ssdp), [UPnPDevice](https://github.com/dltoth/UPnPDevice), [CommonUtil](https://github.com/dltoth/CommonUtil), [WiFiPortal](https://github.com/dltoth/WiFiPortal), and [DeviceLib](https://github.com/dltoth/DeviceLib). The compiler won't find anything without this declaration.

```
using namespace lsc;
```

**Instantiating Devices and Services**

These declarations go above the setup() routine of the sketch. The Web Server *server* has been conditionally compiled for either ESP8266 or ESP32 and *svr* is a pointer to *server*. WebContext is a Web server abstraction and will be initialized with the Web server inside of setup().

```
/**
 *   Device hierarchy will consist of a RootDevice (root) with two embedded devices 
 *   (c and d), and two services (cs and s). 
 */
WebContext    ctx;
RootDevice    root;
CustomDevice  c;
UPnPDevice    d;
CustomService cs;
UPnPService   s;
```

**Initializing the WebContext***

Device hierarchy is defined inside of the setup() routine, but first, WebContext is initialized:

```
  server.begin();
  ctx.setup(svr,WiFi.localIP(),SERVER_PORT);
```

Device hierarchy is defined with base UPnPDevice *d* and CustomDevice *c* added to the RoodDevice *root*, with base UPnPService *s* added to *d*, and CustomService *cs* added to c.

**Building Devices and Setting Hierarchy**

```
/**
 *  Build devices and set names and targets. Note that device and service targets
 *  must be unique relative to the RootDevice (or UPnPDevice) as these are used
 *  to set HTTP request handlers on the web server
 */ 
  root.setDisplayName("Root Device");
  root.setTarget("root");  
  d.setDisplayName("Base Device");
  d.setTarget("baseDevice");
  c.setDisplayName("Custom Device");
  c.setTarget("customDevice");

/**
 *  Build the services and set the heirarchy
 */
  cs.setDisplayName("Custom Service");
  cs.setTarget("customService");
  s.setTarget("baseService");
  s.setDisplayName("Base Service");
  c.addService(&cs);
  d.addService(&s);
  root.addDevices(&c,&d);
  root.setup(&ctx);
```

**Note:** Display name is used in HTML display and target is used in url creation. HTML request handlers are set on target urls, so targets must be unique relative to RootDevice and UPnPDevices. Also note that display name and target can be set in the constructor for CustomDevice, so the default constructor could be used in the sketch.

**Registerring HTTP Request Handlers**

```
/**
 *  Set up the device hierarchy and register HTTP request handlers
 */
  root.setup(&ctx);
```

The RootDevice setup() function runs through each embedded device calling setup() for that device.

**Note:** The RootDevice registers HTTP request handlers for both the base URL (http://<span></span>IPAddress:port/) and root target URL (http://<span></span>IPAddress:port/root/), so each RootDevice requires its own WebServer with unique port. It is customary however, to have only a single RootDevice per ESP device. This shouldn't present a problem since UPnPDevices and UPnPServices provide building blocks for functionality and RootDevice functions mainly as a container.

The remainder of the sketch outputs UPnPDevice info for the heirarchy to Serial, and then runs through down-cast from CustomDevice* to UPnPObject* and up-cast from UPnPObject* to CustomDevice*, using RTTI and UPnP device type.

In the example above, output to Serial will be

```
Starting CustomDevice for Board ESP8266
Connecting to Access Point My_SSID
...........
WiFi Connected to My_SSID with IP address: 10.0.0.78
RootDevice Root Device:
   UUID: 032bd3df-6fc1-47c1-8835-b40d3e968ad5
   Type: urn:LeelanauSoftware-com:device:RootDevice:1
   Location is http://10.0.0.78:80/root
   Root Device has no Services
Root Device Devices:
Custom Device:
   UUID: e1142a5a-5ab1-47ea-a994-10e332df0870
   Type: urn:CompanyName-com:device:CustomDevice:1
   Location is http://10.0.0.78:80/root/customDevice
   Custom Device Services:
      Custom Service:
         Type: urn:CompanyName-com:service:CustomService:1
         Location is http://10.0.0.78:80/root/customDevice/customService
Base Device:
   UUID: 718c97bc-fa47-4dd9-8a7e-291cba6eb82b
   Type: urn:LeelanauSoftware-com:device:Basic:1
   Location is http://10.0.0.78:80/root/baseDevice
   Base Device Services:
      Base Service:
         Type: urn:LeelanauSoftware-com:service:Basic:1
         Location is http://10.0.0.78:80/root/baseDevice/baseService
CustomDevice virtual UPnP Type is urn:CompanyName-com:device:CustomDevice:1 and static upnpType is urn:CompanyName-com:device:CustomDevice:1
Proper down cast from CustomDevice* (&c) to UPnPObject* (obj)
obj virtual UPnP Type is urn:CompanyName-com:device:CustomDevice:1 and (static) upnpType is urn:LeelanauSoftware-com:device:Object:1
Proper up cast from UPnPObject* (obj) to UPnPDevice* (dev)
dev (virtual) UPnP Type is urn:CompanyName-com:device:CustomDevice:1 and (static) upnpType is urn:LeelanauSoftware-com:device:Basic:1
Proper up cast from UPnPDevice* (dev) to CustomDevice* (cusDev) 
cusDev (virtual) UPnP Type is urn:CompanyName-com:device:CustomDevice:1 and (static) upnpType is urn:CompanyName-com:device:CustomDevice:1
```

**Note:** When CustomDevice* is cast as a UPnPObject*, note the difference in UPnPDevice type between the virtual function obj->getType() and the static obj->upnpType(). The static version returns the UPnPDevice type of the pointer class rather than CustomDevice:

UPnPObject* virtual UPnP Type is *urn:CompanyName-com:device:CustomDevice:1* and (static) upnpType is *urn:LeelanauSoftware-com:device:Object:1*

In this example, the RootDevice is displayed at http://<span></span>10.0.0.78:80, and the display will consist of a list of buttons, one for each of *Custom Device* and *Base Device*, and a *This Device* button (see figure 1 below). Selecting *This Device* will go to the url http://<span></span>10.0.0.78:80/root and consist of only the two buttons, one for each device. The reason for this subtle difference will be more apparent in the discussion on Sensors below, but essentially, at the base url http://<span></span>10.0.0.78:80, the display for Sensors and Controls are displayed inline with the RootDevice display, but non-Sensors (Controls) are displayed ad buttons.

*Figure 1 - RootDevice display at http://<span></span>10.0.0.78:80/root*

![image1](/assets/image1.png)

## Creating a Custom Sensor

As noted above, Sensor and Control display will be different at the base url than at the root target. Starting with [SimpleSensor.h](https://github.com/dltoth/UPnPDevice/blob/main/examples/SensorDevice/SimpleSensor.h), notice the following:

**Namespace declaration**

```
using namespace lsc;
```

**Class Declaration**

SimpleSensor derives from Sensor, constructors are required by [UPnPDevice](https://github.com/dltoth/UPnPDevice/blob/main/src/UPnPDevice.h).

```
/**
 *   Simple Sensor Example with default configuration
 **/
class SimpleSensor : public Sensor {

      SimpleSensor();
      SimpleSensor(const char* target);
```

**Additional Required Methods**

Setup() is required for implementation specific initialization. Here it is used to set the initial message, although that could have been done in any number of other ways. Content() is important to supply HTML to RootDevice.

```
/**
 *   Virtual Functions required for UPnPDevice
 */
      void           setup(WebContext* svr);

/**
 *   Virtual Functions required by Sensor
 */
      void           content(char buffer[], int bufferSize);


```

**Runtime Type Identification (RTTI)**

```
      DEFINE_RTTI;
      DERIVED_TYPE_CHECK(Sensor);
```

**Define a Message Buffer**

We use a fixed length character array for the message buffer, which will be reused when configuration is added.

```
   protected:
    void          setMessage(const char* m);

    char          _msg[BUFF_SIZE];
```

**Copy Construction and Destruction are Not Allowed**

```
     DEFINE_EXCLUSIONS(SimpleSensor);         

```

Moving on the the implementation file [SimpleSensor.cpp](https://github.com/dltoth/UPnPDevice/blob/main/examples/SensorDevice/SimpleSensor.cpp), notice the following:

**Message Template in PROGMEM**

A template for the message HTML is defined in PROGMEM

```
const char  sensor_msg[]  PROGMEM = "<p align=\"center\">Sensor Message is:  %s </p>";
``` 

Once again using the lsc namespace, and then static RTTI and UPnP type initialization is done here

```
/** Leelanau Software Company namespace 
*  
*/
using namespace lsc;

INITIALIZE_STATIC_TYPE(SimpleSensor);
INITIALIZE_UPnP_TYPE(SimpleSensor,urn:LeelanauSoftware-com:device:SimpleSensor:1);
```

**UPnPDevice Construction**

UPnPDevice construction requires a *Target*. 

```
SimpleSensor::SimpleSensor() : Sensor("sensor") {setDisplayName("Simple Sensor");}

SimpleSensor::SimpleSensor(const char* target) : Sensor(target) {setDisplayName("Simple Sensor");}
```

**Supply HTML Content to RootDevice**

Next, the content() method is defined, notice it simply fills the input *buffer* with HTML based on the pre-defined PROGMEM template and the message from *getMessage()*.

```
void SimpleSensor::content(char buffer[], int bufferSize) {
/**
 *   Fill buffer with HTML
 */
  snprintf_P(buffer,bufferSize,sensor_msg,getMessage());
}
```

**Define Methods to Set Message and Setup Device**

Setting the message fills the message buffer

```
void SimpleSensor::setMessage(const char* m) {
  if( m != NULL ) {
      snprintf(_msg,BUFF_SIZE,"%s",m);
  }
}
```

**Define the setup Method**

The setup method initializes the message buffer to "Hello from Simple Sensor". Be sure to call *Sensor::setup()* on the base class to assure it is properly setup prior to any subclass setup.

```
void SimpleSensor::setup(WebContext* svr) {
/**
 *   Make sure Sensor::setup() is called prior to any other required setup.
 */
  Sensor::setup(svr);
  setMessage("Hello from Simple Sensor");
}
```

The Sketch that instantiates a SimpleSensor and adds it to a RootDevice can be found [here](https://github.com/dltoth/UPnPDevice/blob/main/examples/SensorDevice/SensorDevice.ino). Once an ESP device is flashed with the sketch, point a browser to the device base URL (http://<span></span>IPAdress:80/). You will see figure 2 below.

*Figure 2 - SimpleSensor display at http://<span></span>10.0.0.78:80/

![image2](/assets/image2.png)

Notice Sensor displays is its message at the base url, and selecting the "This Device" button will send the browser to *http://<span></span>10.0.0.78:80/root/* and display all of the RootDevice embedded devices as buttons. In this case, the single "Simple Sensor" button on figure 3.

*Figure 3 - SimpleSensor display at http://<span></span>10.0.0.78:80/root/*

![image3](/assets/image3.png)

Now, selecting the "Simple Sensor" button will trigger device display, which is Sensor display with a "Configure" button, as in figure 4.

*Figure 4 - SimpleSensor device at http://<span></span>10.0.0.78/root/sensor/*

![image4](/assets/image4.png)

Now, selecting the "Configure" button will bring up default configuration. Default configuration for both Sensors and Controls is simply their display name, as in figure 5.

*Figure 5 - SimpleSensor device at http://<span></span>10.0.0.78/device/sensor/setConfiguration/configForm*

![image5](/assets/image5.png)

## Adding Custom Configuration
Moving on to creating custom configuration for SimpleSensor, see [SensorWithConfig.h](https://github.com/dltoth/UPnPDevice/blob/main/examples/SensorDevice/SensorWithConfig.h) and notice SensorWithConfig is a subclass of SimpleSensor. In what follows, only the important differences are discussed.

**Define Required Methods**

SensorWithConfig has both [SetConfiguration](https://github.com/dltoth/UPnPDevice/blob/main/src/Configuration.h) and [GetConfiguration](https://github.com/dltoth/UPnPDevice/blob/main/src/Configuration.h) services. SetConfiguration requires a form handler to display an HTML configuration form, and a *submit* method for that form. The GetConfiguration service should return an XML document describing Sensor configuration. Since we are customizing configuration, we should also reflect that customization in the *getConfiguation* method.

```
/**
 *    Methods to customize configuration
 */
      void           configForm(WebContext* svr);
      void           getConfiguration(WebContext* svr);
      void           setConfiguration(WebContext* svr);

```

**Dont Forget RTTI**

SensorWithConfig is a subclass of Sensor, and as before copy construction and destruction are excluded.

```
     DEFINE_RTTI;
     DERIVED_TYPE_CHECK(Sensor);
     DEFINE_EXCLUSIONS(SensorWithConfig);         
```

Moving on to the the implementation file [SensorWithConfig.cpp](https://github.com/dltoth/UPnPDevice/blob/main/examples/SensorDevice/SensorWithConfig.cpp), notice the following:

**Define PROGMEM Templates**

Define the template used for GetConfiguration. By convention it has a standard XML style (not enforced). Notice it returns configuration from the base class (display name) and adds the message.

```
/**
 *   Config template defines a bit of XML to return device configuration. It MUST be of the following form:
 *      <?xml version=\"1.0\" encoding=\"UTF-8\"?>"
 *         <config>
 *            ...
 *         </config>
 *
 *   and SHOULD include 
 *         <displayName>Device Display Name</displayName>
 */
const char  SensorWithConfig_config_template[]  PROGMEM = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
                                                             "<config>"
                                                                "<displayName>%s</displayName>"
                                                                "<msg>%s</msg>"
                                                               "</config>";

```

HTML for the config form template includes text input for display name from Sensor, and adds text input for the mesage.

```
/**
 *   Config Form template to allow input of Sensor message and display name
 *   It includes the url for form submit, sensor display name, sensor message, and url for the cancel button
 */
const char  SensorWithConfig_config_form[] PROGMEM = "<br><br><form action=\"%s\">"        // Form submit path
            "<div align=\"center\">"
              "<label for=\"displayName\">Sensor Name &nbsp &nbsp</label>"
              "<input type=\"text\" placeholder=\"%s\" name=\"displayName\"><br><br>"      // Sensor displayName
              "<label for=\"msg\">Sensor Message &nbsp </label>"
              "<input type=\"text\" placeholder=\"%s\" name=\"msg\">&nbsp<br><br>"         // Sensor Message     
              "<button class=\"fmButton\" type=\"submit\">Submit</button>&nbsp&nbsp"
              "<button class=\"fmButton\" type=\"button\" onclick=\"window.location.href=\'%s\';\">Cancel</button>"
            "</div></form>";
```

**Define Namespace and Static RTTI and UPnP Types**

```
using namespace lsc;

INITIALIZE_STATIC_TYPE(SensorWithConfig);
INITIALIZE_UPnP_TYPE(SensorWithConfig,urn:LeelanauSoftware-com:device:SensorWithConfig:1);
```

**Define Constructors**

HTTP request handlers are set on the Sensor GetConfiguration and SetConfiguration UPnPServices directly. A pointer 
to each service is provided by Sensor::setConfiguration() and Sensor::getConfiguration() respectively. The form
handler will present a configuration form, where form submission triggers setConfiguration().

**Note:** The HTTP request handlers for GetConfigutation and SetConfiguration have already been set on the service and registered on setup. Setting the handler here is actually a level of indirection that allows these services to be reused in multiple settings.

```
SensorWithConfig::SensorWithConfig() : SimpleSensor("sensorwc") {
  setDisplayName("Sensor With Config");
  Sensor::setConfiguration()->setHttpHandler([this](WebContext* svr){this->setConfiguration(svr);});
  Sensor::setConfiguration()->setFormHandler([this](WebContext* svr){this->configForm(svr);});
  Sensor::getConfiguration()->setHttpHandler([this](WebContext* svr){this->getConfiguration(svr);});
}
```

**Define HTTP Request Handlers**

The HTTP request handler for setting configuration expects only two possible arguments, either
DISPLAYNAME or MSG. 

```
/**
 *  Configuration has 2 possible arguments:
 *     DISPLAYNAME   :=  Device display name, part of the default Sensor configuration
 *     MSG           :=  Sensor message to display
 *  These are set on the config form.
 */
void SensorWithConfig::setConfiguration(WebContext* svr) {
  int numArgs = svr->argCount();
  for( int i=0; i<numArgs; i++) {
     const String& argName = svr->argName(i);
     const String& arg = svr->arg(i);
     if( argName.equalsIgnoreCase("MSG") ) setMessage(arg.c_str());
     else if( argName.equalsIgnoreCase("DISPLAYNAME") ) {if( arg.length() > 0 ) setDisplayName(arg.c_str());}
  }
  display(svr);
}
```

The HTTP request handler for getConfiguration fills a buffer with the XML template and sends
a response.

```
void SensorWithConfig::getConfiguration(WebContext* svr) {
  char buffer[1000];
  int size = sizeof(buffer);
  snprintf_P(buffer,size,SensorWithConfig_config_template,getDisplayName(),getMessage());
  svr->send(200, "text/xml", buffer);    
}
```

Lastly, the form handler uses HTML formatting functions found in [CommonUtil](https://github.com/dltoth/CommonUtil). The *formatHeader* 
function supplies the HTML document header and style link, and *formatBuffer_P* is designed to take a **PROGMEM** template and fill
an input buffer.

**Note:** The url for form submission is provided by the method setConfigutation::getPath()

```
void SensorWithConfig::configForm(WebContext* svr) {
/**
 *    Config Form HTML Start with Title
 */
  char buffer[1500];
  int size = sizeof(buffer);
  int pos = formatHeader(buffer,size,"Set Sensor Configuration");

  char svcPath[100];
  Sensor::setConfiguration()->getPath(svcPath,100);     // Form submit path (service path)
  pos = formatBuffer_P(buffer,size,pos,SensorWithConfig_config_form,svcPath,getDisplayName(),getMessage());

/**
 *  Config Form HTML Tail
 */
  formatTail(buffer,size,pos);
  svr->send(200,"text/html",buffer); 
}
```

Now, using the same sketch [here](https://github.com/dltoth/UPnPDevice/blob/main/examples/SensorDevice/SensorDevice.ino), and instead instantiating a SensorWithConfig, the Sensor will display its messag. Selecting the "This Device" button will display all of the RootDevice embedded devices as buttons, in this case, a single "Sensor With Config" button. Selecting that button, and then selecting the "Configure" button will present the config form in figure 6 below.

*Figure 6 - SensorWithConfig device at http://<span></span>10.0.0.78/device/sensorwc/setConfiguration/configForm*

![image6](/assets/image6.png)

The Sensor message can now be changed. 

## Creating a Custom Control

This final example will demonstrate how to create a custom Control consisting of a single toggle. The main difference between a Sensor and a Control is that HTML for a Sensor is inserted inline with the display HTML for a RootDevice, and a Control is displayed by inserting a link to its display in an iFrame in the RootDevice HTML. Both Sensors and Controls implement the content method:

```
void             content(char buffer[], int size);
```

for their HTML display but from the RootDevice perspective, Control display is indirect via iFrame. There are a few reasons for this:

1. With up to 8 embedded devices, the RootDevice HTML buffer would have to be quite large to accomodate complex display for each device if it were inline.
2. iFrame refresh is faster than refreshing the entire RootDevice page.

Now, consider the [CustomControl](https://github.com/dltoth/UPnPDevice/blob/main/examples/ControlDevice) example and starting with the header file defining [CustomControl](https://github.com/dltoth/UPnPDevice/blob/main/examples/ControlDevice/CustomControl.h), notice the following:

**CustomControl Derives from Control**

```
class CustomControl : public Control {

  public: 
      CustomControl() : Control("customControl") {setDisplayName("Custom Control");}

      CustomControl( const char* target ) : Control(target) {setDisplayName("Custom Control");}
```

**iFrame Height**

Frame height tells the RootDevice how much vertical space is required by the Control

```
      virtual int     frameHeight()  {return 100;}       // Frame height from Control
```

**State Management**

CustomControl State management is provided by the following methods:

```
      void            setState(WebContext* svr);                                      
      
      boolean         isON()                      {return(getControlState() == ON);}  
      boolean         isOFF()                     {return(getControlState() == OFF);} 
      ControlState    getControlState()           {return(_state);}                  
      const char*     controlState()              {return((isON())?("ON"):("OFF"));}   
```

In particular, 

```
      void            setState(WebContext* svr);                                       
```

is the HTTP request handler for the toggle button press. Ultimately, this method is registerred with the Web server in setup().

Now, moving on to the implementation file [CustomControl.cpp](https://github.com/dltoth/UPnPDevice/blob/main/examples/ControlDevice/CustomControl.cpp), notice the following:

**HTML Templates in PROGMEM**

As before, HTML templates are defined as const char[] in PROGMEM. In particular

```
const char relay_on[]   PROGMEM = "<div align=\"center\"><a href=\"./setState?STATE=OFF\" class=\"toggle\"><input class=\"toggle-checkbox\" type=\"checkbox\" checked>"
                                   "<span class=\"toggle-switch\"></span></a>&emsp;ON</div>";

const char relay_off[]  PROGMEM = "<div align=\"center\">&ensp;<a href=\"./setState?STATE=ON\" class=\"toggle\"><input class=\"toggle-checkbox\" type=\"checkbox\">"
                                   "<span class=\"toggle-switch\"></span></a>&emsp;OFF</div>";
```

define the toggle entity and set the trigger as *setState*, with arguments either *ON* or *OFF*.

**Define the HTTP Request Handler**

The HTTP request handler takes as an argument the Web server abstraction [WebContext](https://github.com/dltoth/CommonUtil/blob/main/src/WebContext.h) which will provide arguments to the Web page call. setState expects ONLY a single argument *STATE* whose value can be either *ON* or *OFF*. Recall, these were coded in PROGMEM at the top. ControlState is then set based on the input argument.

```
void CustomControl::setState(WebContext* svr) {
   int numArgs = svr->argCount();
   if( numArgs > 0 ) {
      for( int i=0; i<numArgs; i++ ) {
         const String& argName = svr->argName(i);
         const String& argVal = svr->arg(i);
         if(argName.equalsIgnoreCase("STATE")) {
            if( argVal.equalsIgnoreCase("ON")) setControlState(ON);
            else if( argVal.equalsIgnoreCase("OFF") ) setControlState(OFF);
            break;
          }
       }
   }

/** Control refresh is only within the iFrame
 */
   displayControl(svr);
}
```

Lastly, notice that the display has to be refreshed based on this state change; *displayControl* refreshes iFrame content ONLY.

```
   displayControl(svr);
```

**Construct HTML Content**

HTML content is inserted into the display buffer provided. Notice the formatting function [formatBuffer_P](https://github.com/dltoth/CommonUtil/blob/main/src/CommonProgmem.h) defined in [CommonUtils](https://github.com/dltoth/CommonUtil) is used here.

```
void  CustomControl::content(char buffer[], int size) {  
  int pos = 0;
  if( isON() ) {
    pos = formatBuffer_P(buffer,size,pos,relay_on);  
    pos = formatBuffer_P(buffer,size,pos,on_msg);          
  }        
  else {
    pos = formatBuffer_P(buffer,size,pos,relay_off); 
    pos = formatBuffer_P(buffer,size,pos,off_msg);          
  }         
}
```

format_P takes as arguments a char *buffer[]*, buffer *size*, a *pos* to start writing, a PROGMEM template, and arguments, and returns an updated writing position. Notice then that consecutive writes use the updated position *pos*.

**Define Setup**

Lastly, define the setup method.

```
void CustomControl::setup(WebContext* svr) {
   Control::setup(svr);
   char pathBuffer[100];
   handlerPath(pathBuffer,100,"setState");
   svr->on(pathBuffer,[this](WebContext* svr){this->setState(svr);});  
}
```

First thing is to call setup for the base Control class, then register *setState* with the Web server. The method

```
void handlerPath(buffer,size,const char*) 
```

copies the full url to *setState* into *buffer*, so it can be registered with the Web server.

The sketch [ControlDevice.ino](https://github.com/dltoth/UPnPDevice/blob/main/examples/ControlDevice/ControlDevice.ino) constructs a RootDevice and adds CustomControl. RootDevice display is shown in Figure 7 below.

*Figure 7 - CustomControl device at http://<span></span>10.0.0.78/*

![image7](/assets/image7.png)

For a full implementation of a device that controls a relay, see [RelayControl](https://github.com/dltoth/DeviceLib/blob/main/src/RelayControl.h). 
