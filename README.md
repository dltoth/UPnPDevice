# UPnPDevice
Developing applications for Arduino (ESP8266 and ESP32) require creating custom HTML for the user interface. UPnPDevice is a framework for HTML user interface that follows the [UPnP Device Arcihtecture](http://upnp.org/specs/arch/UPnP-arch-DeviceArchitecture-v1.1.pdf). When coupled with the additional [SSDP library](https://github.com/dltoth/ssdp) also provides simple service discovery. This library provides a set of base classes for building a UPnP Device hierarchy and advertising it over a local network. 

The set of classes include:

```
  RootDevice       := A UPnP root device container. Root devices can have up to 8 embedded UPnPDevices and 
                      up to 8 UPnPServices
  UPnPDevice       := A base class for UPnP devices. UPnPDevices can have up to 8 UPnPServices
  UPnPService      := A base class for UPnP services. Services have a callable HTTP interface
  Sensor           := A UPnPDevice that has simple output based display, like a Thermometer or Clock
  Control          := A UPnPDevice with more complex display, including user interaction, like a toggle
  GetConfiguration := A UPnPService base class for returning device configuration in XML
  SetConfiguration := A UPnPService base class for setting device configuration from callable HTTP interface
```

Both Sensor and Control include GetConfiguration and SetConfiguration.

Default display for RootDevice is to put Sensor HTML inline with it's HTML display, and place Control HTML inline in an iFrame. 
