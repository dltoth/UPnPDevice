
/**
 *   Consider refactoring content() to be more like formatBuffer().
 */

#include "SensorDevice.h"

namespace lsc {

INITIALIZE_STATIC_TYPE(Sensor);

Sensor::Sensor() : UPnPDevice("urn:LeelanauSoftwareCo-com:device:Sensor:1","sensor") {
  addServices(getConfiguration(),setConfiguration());   // Add services for configuration
  setDisplayName("Sensor");                             // Set the eisplay name
}

Sensor::Sensor(const char* type, const char* target) : UPnPDevice(type, target) {
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
