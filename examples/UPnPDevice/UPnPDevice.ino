/**
 *  Boilerplate Test Harness
 */

#include "CustomDevice.h"

using namespace lsc;

#define AP_SSID "My_SSID"
#define AP_PSK  "My_PSK"
#define SERVER_PORT 80

#ifdef ESP8266
#include <ESP8266WiFi.h>
ESP8266WebServer  server(SERVER_PORT);
ESP8266WebServer* svr = &server;
#define           BOARD "ESP8266"
#elif defined(ESP32)
#include <WiFi.h>
WebServer        server(SERVER_PORT);
WebServer*       svr = &server;
#define          BOARD "ESP32"
#endif

WebContext       ctx;

/**
 *   Device hierarchy will consist of a RootDevice (root) with two embedded devices 
 *   (c and d), and two services (cs and s). 
 */ 
RootDevice    root;
CustomDevice  c;
UPnPDevice    d;
CustomService cs;
UPnPService   s;

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  Serial.println();
  Serial.printf("Starting CustomDevice for Board %s\n",BOARD);

  WiFi.begin(AP_SSID,AP_PSK);
  Serial.printf("Connecting to Access Point %s\n",AP_SSID);
  while(WiFi.status() != WL_CONNECTED) {Serial.print(".");delay(500);}

  Serial.printf("WiFi Connected to %s with IP address: %s\n",WiFi.SSID().c_str(),WiFi.localIP().toString().c_str());

  server.begin();
  ctx.setup(svr,WiFi.localIP(),SERVER_PORT);
  Serial.printf("Web Server started on %s:%d/\n",ctx.getLocalIPAddress().toString().c_str(),ctx.getLocalPort());
  
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
  
/**
 *  Set up the device hierarchy and register HTTP request handlers
 */
  root.setup(&ctx);

/**
 *  Send UPnP device info to Serial
 */
  RootDevice::printInfo(&root);  

/**
 *  Demonstrate RTTI down-cast and up-cast, and virtual vs. static UPnP type info for CustomDevice
 */
  UPnPObject* obj = (UPnPObject*) c.as(UPnPObject::classType());
  Serial.printf("CustomDevice virtual UPnP Type is %s and static upnpType is %s\n",c.getType(),c.upnpType());
  if( obj != NULL ) {
    Serial.printf("Proper down cast from CustomDevice* (&c) to UPnPObject* (obj)\n");
    Serial.printf("obj virtual UPnP Type is %s and (static) upnpType is %s\n",obj->getType(),obj->upnpType());
    UPnPDevice* dev = (UPnPDevice*) obj->as(UPnPDevice::classType());
    if( dev != NULL ) {
       Serial.printf("Proper up cast from UPnPObject* (obj) to UPnPDevice* (dev)\n");
       Serial.printf("dev (virtual) UPnP Type is %s and (static) upnpType is %s\n",dev->getType(),dev->upnpType());
       CustomDevice* cusDev = (CustomDevice*)dev->as(CustomDevice::classType());
       if( cusDev != NULL ) {
         Serial.printf("Proper up cast from UPnPDevice* (dev) to CustomDevice* (cusDev) \n");
         Serial.printf("cusDev (virtual) UPnP Type is %s and (static) upnpType is %s\n",cusDev->getType(),cusDev->upnpType());
       }
       else Serial.printf("Could NOT up cast from UPnPDevice* to CustomDevice*\n");      
    }
    else Serial.printf("Could NOT up cast from UPnPObject* to UPnPDevice*\n");
  }
  else Serial.printf("Could NOT down cast from CustomDevice* to UPnPObject\n");

}

void loop() {
  server.handleClient();
}
