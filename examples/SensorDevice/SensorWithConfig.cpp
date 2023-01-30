/**
 * 
 */

#include "SensorWithConfig.h"

/**
 *   Config template defines a bit of XML to return device configuration. It MUST be of the following form:
 *      <?xml version=\"1.0\" encoding=\"UTF-8\"?>"
 *         <config>
 *            ...
 *         </config>
 *
 *   and SHOULD include 
 *         <displayName>Device Display Name</displayName>
 */
const char  SensorWithConfig_config_template[]  PROGMEM = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
                                                             "<config>"
                                                                "<displayName>%s</displayName>"
                                                                "<msg>%s</msg>"
                                                               "</config>";

/**
 *   Config Form template to allow input of Sensor message and display name
 */
const char  SensorWithConfig_config_form[] PROGMEM = "<br><br><form action=\"%s\">"             // Form submit path                                                                              // Service path
            "<div align=\"center\">"
              "<label for=\"displayName\">Sensor Name &nbsp &nbsp</label>"
              "<input type=\"text\" placeholder=\"%s\" name=\"displayName\"><br><br>"           // Sensor displayName                                                                     // Device display name
              "<label for=\"msg\">Sensor Message &nbsp </label>"
              "<input type=\"text\" placeholder=\"%s\" name=\"msg\">&nbsp<br><br>"              // Sensor Message     
              "<button class=\"fmButton\" type=\"submit\">Submit</button>&nbsp&nbsp"
              "<button class=\"fmButton\" type=\"button\" onclick=\"window.location.href=\'%s\';\">Cancel</button>"                                             // Device path
            "</div></form>";

/** Leelanau Software Company namespace 
*  
*/
using namespace lsc;

INITIALIZE_STATIC_TYPE(SensorWithConfig);

/**
 *   Type is the UPnP required device type, defined as urn:CompanyName:device:deviceName:version where CompanyName 
 *   substitutes "." with "_". Target is the Http target for device display, which MUST be unique under the RootDevice. 
 *   In this case:
 *      http://ip-address:port/rootTarget/sensorwc 
 *   where rootTarget is set on the RootDevice.
 *   Configuration is managed via http handler and form handler set on the Get/SetConfiguration services, included with Sensor.
 */
SensorWithConfig::SensorWithConfig() : SimpleSensor("urn:LeelanauSoftwareCo-com:device:SensorWithConfig:1","sensorwc") {
  setDisplayName("Sensor With Config");
  Sensor::setConfiguration()->setHttpHandler([this](WebContext* svr){this->setConfiguration(svr);});
  Sensor::setConfiguration()->setFormHandler([this](WebContext* svr){this->configForm(svr);});
  Sensor::getConfiguration()->setHttpHandler([this](WebContext* svr){this->getConfiguration(svr);});
}

SensorWithConfig::SensorWithConfig(const char* type, const char* target) : SimpleSensor(type, target) {
  setDisplayName("Sensor With Config");
  Sensor::setConfiguration()->setHttpHandler([this](WebContext* svr){this->setConfiguration(svr);});
  Sensor::setConfiguration()->setFormHandler([this](WebContext* svr){this->configForm(svr);});
  Sensor::getConfiguration()->setHttpHandler([this](WebContext* svr){this->getConfiguration(svr);});
}

/**
 *  Configuration has 2 possible arguments:
 *     DISPLAYNAME   :=  Device display name, part of the default Sensor configuration
 *     MSG           :=  Sensor message to display
 *  These are set on the config form.
 */
void SensorWithConfig::setConfiguration(WebContext* svr) {
  int numArgs = svr->argCount();
  for( int i=0; i<numArgs; i++) {
     const String& argName = svr->argName(i);
     const String& arg = svr->arg(i);
     if( argName.equalsIgnoreCase("MSG") ) setMessage(arg.c_str());
     else if( argName.equalsIgnoreCase("DISPLAYNAME") ) {if( arg.length() > 0 ) setDisplayName(arg.c_str());}
  }
  display(svr);
}

void SensorWithConfig::getConfiguration(WebContext* svr) {
  char buffer[1000];
  int size = sizeof(buffer);
  snprintf_P(buffer,size,SensorWithConfig_config_template,getDisplayName(),getMessage());
  svr->send(200, "text/xml", buffer);    
}

void SensorWithConfig::configForm(WebContext* svr) {
/**
 *    Config Form HTML Start with Title
 */
  char buffer[1500];
  int size = sizeof(buffer);
  int pos = formatHeader(buffer,size,"Set Sensor Configuration");

  char svcPath[100];
  Sensor::setConfiguration()->getPath(svcPath,100);     // Form submit path (service path)
  pos = formatBuffer_P(buffer,size,pos,SensorWithConfig_config_form,svcPath,getDisplayName(),getMessage());

/**
 *  Config Form HTML Tail
 */
  formatTail(buffer,size,pos);
  svr->send(200,"text/html",buffer); 
}

void SensorWithConfig::setup(WebContext* svr) {
/**
 *   Make sure Sendor::setup() is called prior to any other required setup.
 */
  Sensor::setup(svr);
  setMessage("Hello from Sensor with Config");
}

