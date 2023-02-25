/**
 * 
 */

#include "SimpleSensor.h"

const char  sensor_msg[]  PROGMEM = "<p align=\"center\">Sensor Message is:  %s </p>";

/** Leelanau Software Company namespace 
*  
*/
using namespace lsc;

/**
 *   UPnP required device type, defined as urn:CompanyName:device:deviceName:version where CompanyName 
 *   substitutes "." with "-". 
 */
INITIALIZE_STATIC_TYPE(SimpleSensor);
INITIALIZE_UPnP_TYPE(SimpleSensor,urn:LeelanauSoftware-com:device:SimpleSensor:1);

/**
 * Target is the Http target for device display, which MUST be unique under the RootDevice. In this case:
 *      http://ip-address:port/rootTarget/sensor 
 *   where rootTarget is set on the RootDevice.
 */
SimpleSensor::SimpleSensor() : Sensor("sensor") {setDisplayName("Simple Sensor");}

SimpleSensor::SimpleSensor(const char* target) : Sensor(target) {setDisplayName("Simple Sensor");}

void SimpleSensor::content(char buffer[], int bufferSize) {
/**
 *   Fill buffer with HTML
 */
  snprintf_P(buffer,bufferSize,sensor_msg,getMessage());
}

void SimpleSensor::setMessage(const char* m) {
  if( m != NULL ) {
      snprintf(_msg,BUFF_SIZE,"%s",m);
  }
}

void SimpleSensor::setup(WebContext* svr) {
/**
 *   Make sure Sendor::setup() is called prior to any other required setup.
 */
  Sensor::setup(svr);
  setMessage("Hello from Simple Sensor");
}


