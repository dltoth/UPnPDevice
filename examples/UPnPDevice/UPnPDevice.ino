
/**
 *  
 */

#include "UPnPDevice.h"

using namespace lsc;

#define AP_SSID "My_SSID"
#define AP_PSK  "MYPSK"
#define SERVER_PORT 80

/**
 *   Conditional compilation for either ESP8266 or ESP32
 */
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

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  Serial.println();
  Serial.printf("Starting UPnPDevice Test for Board %s\n",BOARD);

  WiFi.begin(AP_SSID,AP_PSK);
  Serial.printf("Connecting to Access Point %s\n",AP_SSID);
  while(WiFi.status() != WL_CONNECTED) {Serial.print(".");delay(500);}

  Serial.printf("WiFi Connected to %s with IP address: %s\n",WiFi.SSID().c_str(),WiFi.localIP().toString().c_str());

  server.begin();
  ctx.setup(svr,WiFi.localIP(),SERVER_PORT);
  Serial.printf("Web Server started on %s:%d/\n",ctx.getLocalIPAddress().toString().c_str(),ctx.getLocalPort());

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

/**
 *  Set up the device hierarchy and register HTTP request handlers
 */
  root.setup(&ctx);
  
/**
 *  Print UPnPDevice info to Serial
 */
  UPnPDevice::printInfo(&root);  

}

void loop() {
  server.handleClient();
}

