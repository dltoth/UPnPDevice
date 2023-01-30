
/**
 *  Boilerplate Test Harness
 */

#include "SimpleSensor.h"
#include "SensorWithConfig.h"

#define AP_SSID "MySSID"
#define AP_PSK  "MYPSK"
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

using namespace lsc;

WebContext       ctx;
WebContext*      c   = &ctx;

RootDevice       root;
SimpleSensor     s;
SensorWithConfig swc;

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  Serial.println();
  Serial.printf("Starting Sensor Test for Board %s\n",BOARD);

  WiFi.begin(AP_SSID,AP_PSK);
  Serial.printf("Connecting to Access Point %s\n",AP_SSID);
  while(WiFi.status() != WL_CONNECTED) {Serial.print(".");delay(500);}

  Serial.printf("WiFi Connected to %s with IP address: %s\n",WiFi.SSID().c_str(),WiFi.localIP().toString().c_str());

  server.begin();
  ctx.setup(svr,WiFi.localIP(),SERVER_PORT);
  Serial.printf("Web Server started on %s:%d/\n",ctx.getLocalIPAddress().toString().c_str(),ctx.getLocalPort());

  root.addDevices(&s,&swc);
  root.setDisplayName("Sensor Test");
  root.setTarget("device");  
  root.setup(&ctx);
  
/**
 *  
 * Print UPnP Info about RootDevice, Services, and embedded Devices
 * 
 */
  printInfo(&root);
  for(int i=0;i<root.numDevices(); i++) {printInfo(root.device(i));}
  
}

void loop() {
  server.handleClient();
}

void printInfo(UPnPDevice* d) {
  Serial.printf("%s:\n   UUID: %s\n   Type: %s\n",d->getDisplayName(),d->uuid(),d->getType());
  char buffer[128];
  d->location(buffer,128,WiFi.localIP());
  Serial.printf("   Location is %s\n",buffer);
  Serial.printf("   %s Services:\n",d->getDisplayName());
  for(int i=0; i<d->numServices(); i++) {
    UPnPService* s = d->service(i);
    buffer[0] = '\0';
    s->location(buffer,128,WiFi.localIP());
    Serial.printf("      %s:\n         Type: %s\n         Location is %s\n",s->getDisplayName(),s->getType(),buffer);
  } 
}
