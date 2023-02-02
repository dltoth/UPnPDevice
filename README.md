# UPnPDevice
Developing applications for Arduino (ESP8266 and ESP32) require creating custom HTML for the user interface. UPnPDevice is a framework for HTML user interface that follows the [UPnP Device Arcihtecture](http://upnp.org/specs/arch/UPnP-arch-DeviceArchitecture-v1.1.pdf). When coupled with the additional [SSDP library](https://github.com/dltoth/ssdp) also provides simple service discovery. This library provides a set of base classes for building a UPnP Device hierarchy and advertising it over a local network. 

The set of classes include:

```
  RootDevice       := A UPnP root device container. Root devices can have up to 8 embedded 
                      UPnPDevices and up to 8 UPnPServices
  UPnPDevice       := A base class for UPnP devices. UPnPDevices can have up to 8 
                      UPnPServices
  UPnPService      := A base class for UPnP services. Services have a callable HTTP 
                      interface
  Sensor           := A UPnPDevice that has simple output based display, like a Thermometer 
                      or Clock
  Control          := A UPnPDevice with more complex display, including user interaction, 
                      like a toggle
  GetConfiguration := A UPnPService base class for returning device configuration in XML
  SetConfiguration := A UPnPService base class for setting device configuration from callable 
                      HTTP interface
```

Both Sensor and Control include default GetConfiguration and SetConfiguration Services.

Default display for RootDevice is to put Sensor HTML inline with it's HTML display, and place Control HTML inline in an iFrame. UPnPDevices that are neither Sensor nor Control are displayed as an HTML button, where selection triggers device display. Default UPnPDevice display is to present each of its services as HTML buttons. 

## Simple Examples

### Default Device Hierarchy and Display

See the sketch [UPnPDevice](https://github.com/dltoth/UPnPDevice/blob/main/examples/UPnPDevice/UPnPDevice.ino) for a simple example of creating a device hierarchy. Note the following parts:

#### Namespace Declaration
The Leelanau Software Company namespace is used for all of the libraries [SSDP](https://github.com/dltoth/ssdp), [UPnPDevice](https://github.com/dltoth/UPnPDevice), [CommonUtil](https://github.com/dltoth/CommonUtil), [WiFiPortal](https://github.com/dltoth/WiFiPortal), and [DeviceLib](https://github.com/dltoth/DeviceLib). The compiler won't find anything without this declaration.

```
using namespace lsc;
```

#### Instantiating Devices and Services

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

#### Building Devices and Setting Hierarchy

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

#### Registerring HTTP Request Handlers

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
**Note:** The RootDevice registers HTTP request handlers for both the base URL (http://IPAddress:port/) and root target URL (http://IPAddress:port/root/), so each RootDevice requires its own WebServer with unique port. It is customary however, to have only a single rootDevice per ESP device. This shouldn't present a problem since the RootDevice functions mainly as a container for embedded UPnPDevices, which in turn provide functionality.

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
### Creating a Custom Sensor
As noted above, Sensor and Control display is different at the base url than at the root target. We will see how this works by building a simple Sensor class that displays the message "Hello from SimpleSensor". Starting with the class definition in [SimpleSensor.h](https://github.com/dltoth/UPnPDevice/blob/main/examples/SensorDevice/SimpleSensor.h), notice the following:

#### Namespace declaration
```
using namespace lsc;
```

#### Class Declaration
SimpleSensor derives from Sensor

```
/**
 *   Simple Sensor Example with default configuration
 **/
class SimpleSensor : public Sensor {
```

#### Required Constructors
These constructors are required by [UPnPDevice](https://github.com/dltoth/UPnPDevice/blob/main/src/UPnPDevice.h), we'll see why in the class implementation.

```
      SimpleSensor();
      SimpleSensor( const char* displayName, const char* target);
```

#### Additional Required Methods
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

#### Runtime Type Identification (RTTI)
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

##### Why RTTI
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

**Important:** RootDevice setup() instantiates the device hierarchy, so rootDevice() will necessarily return NULL until all UPnPDevices and UPnPServices have been added and setup has been called.

#### Define a Message Buffer
We use a fixed length character array for the message buffer

```
   protected:
    void          setMessage(const char* m);

    char          _msg[BUFF_SIZE];
```

So now let's move on the the implementation file [SimpleSensor.cpp](https://github.com/dltoth/UPnPDevice/blob/main/examples/SensorDevice/SimpleSensor.cpp) and notice the following:

#### Message Template in PROGMEM
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

#### UPnPDevice Construction
UPnPDevice and UPnPService construction require a *Device Type* and *Target*. Device Type is a UPnP required construct of the form

```
   urn:CompanyName:device:deviceName:version
or
   urn:CompanyName:service:serviceName:version
```

Where CompanyName substitutes "." with "-". Target is the HTTP target for device display and must be unique with respect to the RootDevice. In the case of a UPnPService, target must be unique with respect to the UPnPDevice it is attached to. The constructor below sets the *DeviceType* to "urn:LeelanauSoftwareCo-com:device:SimpleSensor:1" and *target* to "sensor".

```
/**
 *   Type is the UPnP required device type, defined as urn:CompanyName:device:deviceName:version where CompanyName 
 *   substitutes "." with "-". Target is the Http target for device display, which MUST be unique under the 
 *   RootDevice. In this case:
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

#### Supply HTML Content to RootDevice
Next we define the content() method, notice it simply fills the input *buffer* with HTML based on the pre-defined PROGMEM template and the message from *getMessage()*.

```
void SimpleSensor::content(char buffer[], int bufferSize) {
/**
 *   Fill buffer with HTML
 */
  snprintf_P(buffer,bufferSize,sensor_msg,getMessage());
}
```

#### Define Methods to Set Message and Setup Device
Setting the message fills the message buffer

```
void SimpleSensor::setMessage(const char* m) {
  if( m != NULL ) {
      snprintf(_msg,BUFF_SIZE,"%s",m);
  }
}
```
The setup function initializes the message buffer to "Hello from Simple Sensor"

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


