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

#include "CustomControl.h"

/** Leelanau Software Company namespace 
*  
*/
using namespace lsc;

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
 *  Static RTT and UPnP Type initialization
 */
INITIALIZE_STATIC_TYPE(CustomControl);
INITIALIZE_UPnP_TYPE(CustomControl,urn:LeelanauSoftware-com:device:CustomControl:1);

/**
 *  The only expected arguments are STATE=ON or STATE=OFF, all other arguments are ignored
 */
void CustomControl::setState(WebContext* svr) {
   int numArgs = svr->argCount();
   if( numArgs > 0 ) {
      for( int i=0; i<numArgs; i++ ) {
         const String& argName = svr->argName(i);
         const String& argVal = svr->arg(i);
         if(argName.equalsIgnoreCase("STATE")) {
            if( argVal.equalsIgnoreCase("ON")) setControlState(ON);
            else if( argVal.equalsIgnoreCase("OFF") ) setControlState(OFF);
            break;
          }
       }
   }

/** Control refresh is only within the iFrame
 */
   displayControl(svr);
}

void  CustomControl::content(char buffer[], int size) {  
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

void CustomControl::setup(WebContext* svr) {
   Control::setup(svr);
   char pathBuffer[100];
   handlerPath(pathBuffer,100,"setState");
   svr->on(pathBuffer,[this](WebContext* svr){this->setState(svr);});  
}
