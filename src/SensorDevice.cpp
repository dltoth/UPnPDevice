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

#include "SensorDevice.h"

namespace lsc {

INITIALIZE_STATIC_TYPE(Sensor);
INITIALIZE_UPnP_TYPE(Sensor,urn:LeelanauSoftware-com:device:Sensor:1);

Sensor::Sensor() : UPnPDevice("sensor") {
  addServices(getConfiguration(),setConfiguration());   // Add services for configuration
  setDisplayName("Sensor");                             // Set the eisplay name
}

Sensor::Sensor(const char* target) : UPnPDevice(target) {
  addServices(getConfiguration(),setConfiguration());   // Add services configuration
  setDisplayName("Sensor");                             // Set the eisplay name
}

void Sensor::display(WebContext* svr) {
  char buffer[500];
  int size = sizeof(buffer);
  int pos = formatHeader(buffer,size,getDisplayName());
  content(buffer+pos,size-pos);
  pos = strlen(buffer);
 
/** 
 *  Parent of a Sensor is a RootDevice and thus is non-null and provides a complete path
 *  Add a Config Button to the Sensor display
 */
  char pathBuff[100];
  setConfiguration()->formPath(pathBuff,100);
  pos = formatBuffer_P(buffer,size,pos,config_button,pathBuff,"Configure"); 
   
  formatTail(buffer,size,pos);
  svr->send(200,"text/html",buffer);
}

}
