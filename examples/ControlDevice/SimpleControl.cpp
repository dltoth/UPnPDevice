/**
 * 
 */

#include "SimpleControl.h"

/** Leelanau Software Company namespace 
*  
*/
namespace lsc {

const char on_msg[]     PROGMEM = "<br><div align=\"center\">Control is ON</div>";
const char off_msg[]    PROGMEM = "<br><div align=\"center\">Control is OFF</div>";

/**
 * Control Slider ON
 */
const char relay_on[]   PROGMEM = "<div align=\"center\"><a href=\"./setState?STATE=OFF\" class=\"toggle\"><input class=\"toggle-checkbox\" type=\"checkbox\" checked>"
                                   "<span class=\"toggle-switch\"></span></a>&emsp;ON</div>";

/**
 * Control Slider OFF
 */
const char relay_off[]  PROGMEM = "<div align=\"center\">&ensp;<a href=\"./setState?STATE=ON\" class=\"toggle\"><input class=\"toggle-checkbox\" type=\"checkbox\">"
                                   "<span class=\"toggle-switch\"></span></a>&emsp;OFF</div>";

/**
 *  Static RTT initialization
 */
INITIALIZE_STATIC_TYPE(SimpleControl);

SimpleControl::SimpleControl() : Control("urn:LeelanauSoftwareCo-com:device:SimpleControl:1","SimpleControl") {
  setDisplayName("Control Test");
}

SimpleControl::SimpleControl(const char* type, const char* target) : Control(type, target) {
  setDisplayName("Control Test");
}

/**
 *  The only expected arguments are STATE=ON or STATE=OFF, all other arguments are ignored
 */
void SimpleControl::setState(WebContext* svr) {
   Serial.printf("SetState Arguments: "); 
   int numArgs = svr->argCount();
   if( numArgs > 0 ) {
      for( int i=0; i<numArgs; i++ ) {
         const String& argName = svr->argName(i);
         const String& argVal = svr->arg(i);
         Serial.printf(" %s=%s ",argName.c_str(),argVal.c_str());
         if(argName.equalsIgnoreCase("STATE")) {
            if( argVal.equalsIgnoreCase("ON")) setControlState(ON);
            else if( argVal.equalsIgnoreCase("OFF") ) setControlState(OFF);
            break;
          }
       }
    Serial.printf("\n");
   }

/** Control refresh is only within the iFrame
 */
   displayControl(svr);
}

void  SimpleControl::content(char buffer[], int size) {  
  int pos = 0;
  if( isON() ) {
    pos = formatBuffer_P(buffer,size,pos,relay_on);  
    pos = formatBuffer_P(buffer,size,pos,on_msg);          
  }        
  else {
    pos = formatBuffer_P(buffer,size,pos,relay_off); 
    pos = formatBuffer_P(buffer,size,pos,off_msg);          
  }         
}

void SimpleControl::setup(WebContext* svr) {
   Control::setup(svr);
   char pathBuffer[100];
   handlerPath(pathBuffer,100,"setState");
   svr->on(pathBuffer,[this](WebContext* svr){this->setState(svr);});  
}

} // End of namespace lsc