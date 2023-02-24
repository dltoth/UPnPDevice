# UPnPDevice
Developing applications for Arduino (ESP8266 and ESP32) requires creating custom HTML for the user interface. UPnPDevice is a framework for HTML user interface that follows the [UPnP Device Arcihtecture](http://upnp.org/specs/arch/UPnP-arch-DeviceArchitecture-v1.1.pdf). When coupled with the additional [SSDP library](https://github.com/dltoth/ssdp) also provides simple service discovery. This library provides a set of base classes for building a UPnP Device hierarchy and advertising it over a local network. 

The set of classes include:

```
  RootDevice       := A UPnP root device container. Root devices can have up to 8 embedded 
                      UPnPDevices and up to 8 UPnPServices
  UPnPDevice       := A base class for UPnP devices. UPnPDevices can have up to 8 
                      UPnPServices
  UPnPService      := A base class for UPnP services. Services have a callable HTTP 
                      interface
  Sensor           := A virtual base UPnPDevice that has simple output based display, like 
                      a Thermometer or Clock
  Control          := A virtual base UPnPDevice with more complex display, including user 
                      interaction, like a toggle
  GetConfiguration := A UPnPService base class for returning device configuration in XML
  SetConfiguration := A UPnPService base class for setting device configuration from callable 
                      HTTP interface
```

Both Sensor and Control include default GetConfiguration and SetConfiguration Services, as does RootDevice.

In what follows a conceptual framework for the library is presented, and 4 examples will be detailed:
 1. UPnPDevice hierarchy
 2. Creating a simple Sensor that displays a message
 3. Adding configuration to that Sensor
 4. Creating a simple control implementing a toggle.

## Conceptual Framework

UPnP Defines three basic constructs: root devices, embedded devices, and services, where both root devices and embedded devices can have services and embedded devices, but services may not have embedded devices. Essentially, a root device is a container for a device heirarchy consisting of embedded devices and services. UPnP does not limit the depth or breadth of a device heirarchy. Root devices publish their functionality over HTTP and discovery (SSDP) over UDP. 

In this library, only root devices ([RootDevice](https://github.com/dltoth/UPnPDevice/blob/main/src/UPnPDevice.h)) can have embedded devices ([UPnPDevice](https://github.com/dltoth/UPnPDevice/blob/main/src/UPnPDevice.h)), and the number of embedded devices is limited to 8. Root devices and embedded devices have services ([UPnPService](https://github.com/dltoth/UPnPDevice/blob/main/src/UPnPService.h)), and the number of services is also limited to 8. In terms of class heirarchy, RootDevice is a subclass of UPnPDevice, which in turn is a subclass of [UPnPObject](https://github.com/dltoth/UPnPDevice/blob/main/src/UPnPService.h), and UPnPService is a subclass of UPnPObject.

### Runtime Type Identification (RTTI) and UPnP Device Type

UPnPObjects include the notion of class type for runtime type identification (RTTI). RTTI provides typesafe casting and is required by all UPNPDevices and UPnPServices. Additionally, UPnPDevices and UPnPServices have a UPnP device type (or service type). Device Type is a UPnP required construct of the form

```
   urn:CompanyName:device:deviceName:version
or
   urn:CompanyName:service:serviceName:version
```

Where CompanyName is the company domain name with "-" substituting ".". For example "urn:LeelanauSoftware-com:device:Thermometer:1" is the device type for [Thermometer](https://github.com/dltoth/DeviceLib/blob/main/src/Thermometer.h) offered by LeelanauSoftware.com. Device and Service type are used for SSDP service discovery, to find specific device types on the local network. UPnP restricts the device (service) type to 64 characters.

The following macros are used to define RTTI and UPnP Device type in the header file of a UPnPDevice subclass:

```
      DEFINE_RTTI;
      DERIVED_TYPE_CHECK(baseName);
```

which adds the following lines of code in the header file for the class:

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

where *className* is the class name of the UPnPDevice being defined, *company-name* is the company name described earlier, and *deviceName* is the unique device name.

**Why RTTI?**

Since each embedded UPnPDevice provides its own functionality, one device may rely on another. For example, a timer controlled relay may require a [SoftwareClock](https://github.com/dltoth/DeviceLib/blob/main/src/SoftwareClock.h), or a humidity controlled fan may require a [Thermometer](https://github.com/dltoth/DeviceLib/blob/main/src/Thermometer.h). A device implementer, being familiar with onboard embedded devices, will know if a device or service is available. 

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

All of the UPnPDevice classes are expected to be constructed and managed in global scope above the setup() function in an Arduino sketch. Copy construction and and Object destruction are not allowed, objects are expected to live over the life of an executing application. UPnPObjects are passed via pointer. The following macro is used to enforce this behavior:

```
   DEFINE_EXCLUSIONS(className);
```

which adds the following lines of code to a header file:

```
      className(const className&)= delete;
      className& operator=(const className&)= delete;
```

### Custom UPnPDevice

The most basic custom device or service consists of constructors, RTTI definition, and initialization. The header file CustomDevice.h should consist of:

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

All devices are displayed with a set of HTML entities and styles defined in [CommonUtil](https://github.com/dltoth/CommonUtil). A number of the examp;es presented here also use HTML formatting functions found there as well. In particular, [formatHeader](https://github.com/dltoth/CommonUtil/blob/main/src/CommonProgmem.h) will format an HTML header for a web page that includes a reference to the CSS stylesheet **/styles.css**; RootDevice will register the HTML request handler for **/styles.css** on setup.

See the sketch [UPnPDevice](https://github.com/dltoth/UPnPDevice/blob/main/examples/UPnPDevice/UPnPDevice.ino) for a simple example of creating a device hierarchy. Note the following parts:

**Namespace Declaration**

The Leelanau Software Company namespace is used for all of the libraries [SSDP](https://github.com/dltoth/ssdp), [UPnPDevice](https://github.com/dltoth/UPnPDevice), [CommonUtil](https://github.com/dltoth/CommonUtil), [WiFiPortal](https://github.com/dltoth/WiFiPortal), and [DeviceLib](https://github.com/dltoth/DeviceLib). The compiler won't find anything without this declaration.

```
using namespace lsc;
```

**Instantiating Devices and Services**

WebContext is a WebServer abstraction for ESP8266 and ESP32.

```
/**
 *   Device hierarchy will consist of a RootDevice (root) with two embedded devices 
 *   (d1 and d2), and two services (s1 and s2) attached to d1. 
 */
WebContext       ctx;
RootDevice       root;
UPnPDevice       d1;
UPnPDevice       d2;
UPnPService      s1;
UPnPService      s2;
```

**Building Devices and Setting Hierarchy**

```
/**
 *  Build devices and set names and targets. Note that device and service targets
 *  must be unique relative to the RootDevice (or UPnPDevice) as these are used
 *  to set HTTP request handlers on the web server
 */ 
  d1.setDisplayName("Device 1");
  d1.setTarget("device1");
  d2.setDisplayName("Device 2");
  d2.setTarget("device2");

/**
 *  Build the services and set the heirarchy
 */
  s1.setDisplayName("Service 1");
  s1.setTarget("service1");
  s2.setDisplayName("Service 2");
  s2.setTarget("service2");
  d1.addServices(&s1,&s2);
  root.setDisplayName("Root Device");
  root.setTarget("root");  
  root.addDevices(&d1,&d2);
```

**Registerring HTTP Request Handlers**

```
/**
 *  Set up the device hierarchy and register HTTP request handlers
 */
  root.setup(&ctx);
  
/**
 *  Print UPnPDevice info to Serial
 */
  UPnPDevice::printInfo(&root);  

}

```
**Note:** The RootDevice registers HTTP request handlers for both the base URL (http://IPAddress:port/) and root target URL (http://IPAddress:port/root/), so each RootDevice requires its own WebServer with unique port. It is customary however, to have only a single RootDevice per ESP device. This shouldn't present a problem since the RootDevice functions mainly as a container for embedded UPnPDevices, which in turn provide functionality.

In the example above, output to Serial will be

```

Starting UPnPDevice Test for Board ESP8266
Connecting to Access Point My_SSID
...........WiFi Connected to My_SSID with IP address: 10.0.0.165
Web Server started on 10.0.0.165:80/
RootDevice Root Device:
   UUID: b2234c12-417f-4e3c-b5d6-4d418143e85d
   Type: urn:LeelanauSoftwareCo-com:device:RootDevice:1
   Location is http://10.0.0.165:80/root
   Root Device has no Services
Root Device Devices:
Device 1:
   UUID: 1fda2c59-0a8e-4355-bebd-68e3af78cbeb
   Type: urn:LeelanauSoftwareCo-com:device:Basic:1
   Location is http://10.0.0.165:80/root/device1
   Device 1 Services:
      Service 1:
         Type: urn:LeelanauSoftwareCo-com:service:Basic:1
         Location is http://10.0.0.165:80/root/device1/service1
      Service 2:
         Type: urn:LeelanauSoftwareCo-com:service:Basic:1
         Location is http://10.0.0.165:80/root/device1/service2
Device 2:
   UUID: 95a1f160-b833-4785-9271-3262de3f49a1
   Type: urn:LeelanauSoftwareCo-com:device:Basic:1
   Location is http://10.0.0.165:80/root/device2
   Device 2 has no Services
```

In this example, the RootDevice is displayed at http://10.0.0.165:80/root, and the display will consist of a list of buttons, one for each of *Device 1* and *Device 2* (see figure 1 below). Note that the RootDevice display is slightly different at the base http://10.0.0.165:80/. In this view, Sensors and Controls are displayed inline and other UPnPDevices are displayed as buttons (see the discussion on Sensors below).

*Figure 1 - RootDevice display at http://10.0.0.165:80/root*

![image1](/assets/image1.png)
## Creating a Custom Sensor
As noted above, Sensor and Control display is different at the base url than at the root target. We will see how this works by building a simple Sensor class that displays the message "Hello from SimpleSensor". Starting with the class definition in [SimpleSensor.h](https://github.com/dltoth/UPnPDevice/blob/main/examples/SensorDevice/SimpleSensor.h), notice the following:

**Namespace declaration**

```
using namespace lsc;
```

**Class Declaration**

SimpleSensor derives from Sensor

```
/**
 *   Simple Sensor Example with default configuration
 **/
class SimpleSensor : public Sensor {
```

**Required Constructors**

These constructors are required by [UPnPDevice](https://github.com/dltoth/UPnPDevice/blob/main/src/UPnPDevice.h), we'll see why in the class implementation.

```
      SimpleSensor();
      SimpleSensor( const char* displayName, const char* target);
```

**Additional Required Methods**

Setup() is required for implementation specific initialization. Here we use it to set the initial message, although that could have been done in any number of other ways. Content() is important, as that's the method that will supply HTML to RootDevice.

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

Runtime Type Identification is required by all UPNPDevices and UPnPServices; it's part of the base UPnPObject class definition allowing typesafe casting. 

The following macros are used to define RTTI:

```
/**
 *   Macros to define the following Runtime Type Info:
 *     private: static const ClassType  _classType;             
 *     public:  static const ClassType* classType();   
 *     public:  virtual void*           as(const ClassType* t);
 *     public:  virtual boolean         isClassType( const ClassType* t);
 */
      DEFINE_RTTI;
      DERIVED_TYPE_CHECK(Sensor);
```

Notice the macro DERIVED_TYPE_CHECK(Sensor) declares SimpleSensor as being a subclass of Sensor. Without explicitly including these macros SimpleSensor would inherit its type from Sensor and be considered a Sensor as far as RTTI.

**Why RTTI?**

Since each embedded UPnPDevice provides its own functionality, one device may rely on another. For example, a timer controlled relay may require a [SoftwareClock](https://github.com/dltoth/DeviceLib/blob/main/src/SoftwareClock.h), or a humidity controlled fan may require a [Thermometer](https://github.com/dltoth/DeviceLib/blob/main/src/Thermometer.h). Since you are building your device, you now about its onboard embedded devices. 

First note that any UPnPObject can retrieve a pointer to the RootDevice as:

```
   RootDevice* root = rootDevice();
```

So, if your RootDevice is expected to include a [SoftwareClock](https://github.com/dltoth/DeviceLib/blob/main/src/SoftwareClock.h), then you can use the static RootDevice method

```
   SoftwareClock* clock = (SoftwareClock*)RootDevice::getDevice(rootDevice(), SoftwareClock::classType());
```

to retrieve a pointer to a SoftwareClock. If SoftwareClock is an embedded device and setup() has been called on the RootDevice, clock will be non-NULL. 

**Important:** RootDevice setup() instantiates the device hierarchy, so RootDevice::getDevice() will necessarily return NULL until all UPnPDevices and UPnPServices have been added and setup has been called.

**Define a Message Buffer**

We use a fixed length character array for the message buffer

```
   protected:
    void          setMessage(const char* m);

    char          _msg[BUFF_SIZE];
```

**Copy Construction and Destruction are Not Allowed**

**Important:** UPnPObjects should be declared above the ESP setup() function as global variables and any reference to these objects should be by pointer to these declared objects. Memory is allocated for the life of the application. 
```
/**
 *   Copy Construction and Deletion are not allowed
 */
    private:
    SimpleSensor(const SimpleSensor&)= delete;
    SimpleSensor& operator=(const SimpleSensor&)= delete;
```

So now let's move on the the implementation file [SimpleSensor.cpp](https://github.com/dltoth/UPnPDevice/blob/main/examples/SensorDevice/SimpleSensor.cpp) and notice the following:

**Message Template in PROGMEM**

A template for the message HTML is defined in PROGMEM

```
const char  sensor_msg[]  PROGMEM = "<p align=\"center\">Sensor Message is:  %s </p>";
``` 

Once again using the lsc namespace, and then static RTTI type initialization is done here

```
/** Leelanau Software Company namespace 
*  
*/
using namespace lsc;

INITIALIZE_STATIC_TYPE(SimpleSensor);
```

**UPnPDevice Construction**

UPnPDevice and UPnPService construction require a *Device Type* and *Target*. Device Type is a UPnP required construct of the form

```
   urn:CompanyName:device:deviceName:version
or
   urn:CompanyName:service:serviceName:version
```

Where CompanyName substitutes "." with "-". Target is the HTTP target for device display and must be unique with respect to the RootDevice. In the case of a UPnPService, target must be unique with respect to the UPnPDevice it is attached to. The constructor below sets the *DeviceType* to "urn:LeelanauSoftwareCo-com:device:SimpleSensor:1" and *target* to "sensor".

```
/**
 *   Type is the UPnP required device type, defined as urn:CompanyName:device:deviceName:version 
 *   where CompanyName substitutes "." with "-". Target is the Http target for device display, 
 *   which MUST be unique under the RootDevice. In this case:
 *      http://ip-address:port/rootTarget/sensor 
 *   where rootTarget is set on the RootDevice.
 */
SimpleSensor::SimpleSensor() : Sensor("urn:LeelanauSoftwareCo-com:device:SimpleSensor:1","sensor") {
  setDisplayName("Simple Sensor");
}

SimpleSensor::SimpleSensor(const char* type, const char* target) : Sensor(type, target) {
  setDisplayName("Simple Sensor");
}
```

**Supply HTML Content to RootDevice**

Next we define the content() method, notice it simply fills the input *buffer* with HTML based on the pre-defined PROGMEM template and the message from *getMessage()*.

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

The Sketch that instantiates a SimpleSensor and adds it to a RootDevice can be found [here](https://github.com/dltoth/UPnPDevice/blob/main/examples/SensorDevice/SensorDevice.ino). We won't go into sketch detail here, but flash your device and point a browser to the device base URL (http://IPAdress:80/). You will see figure 2 below.

*Figure 2 - SimpleSensor display at http://10.0.0.165:80/

![image2](/assets/image2.png)

Notice Sensor displays is its message, and selecting the "This Device" button will display all of the RootDevice embedded devices as buttons. In this case, the single "Simple Sensor" button on figure 3.

*Figure 3 - SimpleSensor display at http://10.0.0.165:80/root/*

![image3](/assets/image3.png)

Now, selecting the "Simple Sensor" button will trigger device display, which is Sensor display with a "Configure" button, as in figure 4.

*Figure 4 - SimpleSensor device at http://10.0.0.165/root/sensor/*

![image4](/assets/image4.png)

Now, selecting the "Configure" button will bring up default configuration. Default configuration for both Sensors and Controls is simply their display name, as in figure 5.

*Figure 5 - SimpleSensor device at http://10.0.0.165/device/sensor/setConfiguration/configForm*

![image5](/assets/image5.png)

## Creating Custom Configuration
Now let's look at creating custom configuration for SimpleSensor, see [SensorWithConfig.h](https://github.com/dltoth/UPnPDevice/blob/main/examples/SensorDevice/SensorWithConfig.h) and notice SensorWithConfig is a subclass of SimpleSensor. In what follows, we will only discuss the important differences.

**Define Required Methods**

SimpleSensor has both SetConfiguration and GetConfiguration services. SetConfiguration requires a form handler to display an HTML configuration form, and a *submit* method for that form. Also since we are customizing configuration, we should also reflect that customization in the *getConfiguation* method.

```
/**
 *    Methods to customize configuration
 */
      void           configForm(WebContext* svr);
      void           getConfiguration(WebContext* svr);
      void           setConfiguration(WebContext* svr);

```

**Dont Forget RTTI**

SensorWithConfig is a subclass of Sensor

```
      DEFINE_RTTI;
      DERIVED_TYPE_CHECK(Sensor);
```

Now let's move on the the implementation file [SensorWithConfig.cpp](https://github.com/dltoth/UPnPDevice/blob/main/examples/SensorDevice/SensorWithConfig.cpp).

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

**Define Namespace and static RTTI**

```
using namespace lsc;

INITIALIZE_STATIC_TYPE(SensorWithConfig);
```

**Define Constructors**

HTTP request handlers are set on the Sensor GetConfiguration and SetConfiguration UPnPServices directly. A pointer 
to each service is provided by Sensor::setConfiguration() and Sensor::getConfiguration() respectively. The form
handler will present a configuration form, where form submission triggers setConfiguration().
```
/**
 *   Type is the UPnP required device type, defined as urn:CompanyName:device:deviceName:version where CompanyName 
 *   substitutes "." with "_". Target is the Http target for device display, which MUST be unique under the RootDevice. 
 *   In this case:
 *      http://ip-address:port/rootTarget/sensorwc 
 *   where rootTarget is set on the RootDevice.
 *   Configuration is managed via http handler and form handler set on the Get/SetConfiguration services, included with Sensor.
 */
SensorWithConfig::SensorWithConfig() : SimpleSensor("urn:LeelanauSoftwareCo-com:device:SensorWithConfig:1","sensorwc") {
  setDisplayName("Sensor With Config");
  Sensor::setConfiguration()->setHttpHandler([this](WebContext* svr){this->setConfiguration(svr);});
  Sensor::setConfiguration()->setFormHandler([this](WebContext* svr){this->configForm(svr);});
  Sensor::getConfiguration()->setHttpHandler([this](WebContext* svr){this->getConfiguration(svr);});
}
```

***Define HTTP Request Handlers***

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

**Note: The url for form submission is provided by setConfigutation::getPath**

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

We can go back to the same sketch [here](https://github.com/dltoth/UPnPDevice/blob/main/examples/SensorDevice/SensorDevice.ino), and instead instantiat a SensorWithConfig. Flash your device and point a browser to the device base URL (http://IPAdress:80/). As before, the Sensor displays is its message, and selecting the "This Device" button will display all of the RootDevice embedded devices as buttons. In this case, a single "Sensor With Config" button. Selecting that button, and then selecting the "Configure" button you will see figure 6 below.

*Figure 6 - SensorWithConfig device at http://10.0.0.165/device/sensorwc/setConfiguration/configForm*

![image6](/assets/image6.png)


