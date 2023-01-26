
/**
 *  Boilerplate Test Harness
 */

#include "UPnPDevice.h"

using namespace lsc;

#define AP_SSID "MySSID"
#define AP_PSK  "MyPSK"
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
WebContext*      c   = &ctx;

RootDevice       root;
UPnPDevice       d1;
UPnPService      s1;

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
  
  d1.addService(&s1);
  d1.setDisplayName("Device 1");
  d1.setTarget("embededDevice");
  s1.setDisplayName("Service 1");
  s1.setTarget("service1");
  root.setDisplayName("Device Test");
  root.setTarget("device");  
  root.addDevice(&d1);
  root.setup(&ctx);
  
  printInfo(&root,&d1);  
}

void loop() {
  server.handleClient();
}

void printInfo(RootDevice* r, UPnPDevice* d) {
  Serial.printf("RootDevice UUID is %s\n",r->uuid());
  Serial.printf("Device UUID is %s\n",d->uuid());
  char buffer[128];
  char loc[32];
  r->location(buffer,128,WiFi.localIP());
  Serial.printf("Root Location is %s\n",buffer);
  buffer[0] = '\0';
  r->rootLocation(buffer,128,WiFi.localIP());
  Serial.printf("             and %s\n",buffer);
  
  buffer[0] = '\0';
  d->location(buffer,128,WiFi.localIP());
  Serial.printf("%s Location is %s\n",d->getDisplayName(),buffer);
  Serial.printf("%s Services:\n",d->getDisplayName());
  for(int i=0; i<d->numServices(); i++) {
    UPnPService* s = d->service(i);
    buffer[0] = '\0';
    s->location(buffer,128,WiFi.localIP());
    Serial.printf("    Location for %s is %s\n",s->getDisplayName(),buffer);
  } 
}
