/**
 * 
 *  UPnPDevice Library
 *  Copyright (C) 2023  Daniel L Toth
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published 
 *  by the Free Software Foundation, either version 3 of the License, or any 
 *  later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *  
 *  The author can be contacted at dan@leelanausoftware.com  
 *
 */

#include "SimpleSensor.h"
#include "SensorWithConfig.h"

#define AP_SSID "My_SSID"
#define AP_PSK  "MY_PSK"
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
SensorWithConfig s;
// SensorWithConfig swc;

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

  root.addDevices(&s);
  root.setDisplayName("Sensor Test");
  root.setTarget("root");  
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
