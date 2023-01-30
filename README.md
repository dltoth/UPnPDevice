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

Both Sensor and Control include GetConfiguration and SetConfiguration.

Default display for RootDevice is to put Sensor HTML inline with it's HTML display, and place Control HTML inline in an iFrame. UPnPDevices that are neither Sensor nor Control are displayed as an HTML button, where selection triggers device display. Default UPnPDevice display is to present each of its services as HTML buttons. 

## Simple Examples

### Default Device Hierarchy and Display

See the sketch [UPnPDevice](https://github.com/dltoth/UPnPDevice/blob/main/examples/UPnPDevice/UPnPDevice.ino) for a simple example of creating a device hierarchy. Note the following parts:

#### Instantiating the Devices and Services

```
/**
 *   Device hierarchy will consist of a RootDevice (root) with two embedded devices (d1 and d2),
 *   and two services (s1 and s2) attached to d1. 
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
 *  Build devices and set names and targets. Note that device targets must be unique
 *  relative to the RootDevice
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

Output to Serial will be

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

From the optput, the root device will be displayed at http://10.0.0.165:80/ and the display will look like



### Simple Sensor Display
