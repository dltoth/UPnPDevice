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

#include "Control.h"

const char Control_config_template[]  PROGMEM = "<?xml version=\"1.0\" encoding=\"UTF-8\"?><config><displayName>%s</displayName></config>";
const char Control_config_form[]      PROGMEM = "<form action=\"setConfiguration\"><div align=\"center\">"
                                                   "<label for=\"displayName\">Control Name &nbsp &nbsp</label>"
                                                   "<input type=\"text\" placeholder=\"%s\" name=\"displayName\"><br><br>"
                                                   "<button class=\"fmButton\" type=\"submit\">Submit</button>&nbsp&nbsp"
                                                   "<a style=\"text-decoration:none\" href=\"/%s/%s\"><button class=\"fmButton\" type=\"button\">Cancel</button></a>"
                                                "</div></form>";

namespace lsc {
/**
 *  Static initialization for RTT and UPnP device type
 */
INITIALIZE_STATIC_TYPE(Control);
INITIALIZE_UPnP_TYPE(Control,urn:LeelanauSoftware-com:device:Control:1);

Control::Control() : UPnPDevice("control") {
  addServices(getConfiguration(),setConfiguration());   // Add services for configuration
  setDisplayName("Control");
}

Control::Control(const char* target) : UPnPDevice(target) {
  addServices(getConfiguration(),setConfiguration());   // Add services for configuration
  setDisplayName("Control");
}

void Control::display(WebContext* svr) {
  char buffer[500];
  int size = sizeof(buffer);
  int pos = formatHeader(buffer,size,getDisplayName());
  char pathBuff[100];
  contentPath(pathBuff,100);
  
/**
 *   iFrame display takes url, height, and width as arguments
 */
  pos = formatBuffer_P(buffer,size,pos,iframe_html,pathBuff,frameHeight(),frameWidth());

/** 
 *  Add a Config Button to the Control display
 */
  setConfiguration()->formPath(pathBuff,100);
  pos = formatBuffer_P(buffer,size,pos,config_button,pathBuff,"Configure"); 
  formatTail(buffer,size,pos);
  svr->send(200,"text/html",buffer);
}

/**
 *   Provide iFrame content
 */
void Control::displayControl(WebContext* svr) {
  char buffer[1000];
  int size = sizeof(buffer);
  int pos = 0;
  pos = formatBuffer_P(buffer,size,pos,html_header);
  content(buffer+pos,size-pos);
  pos = strlen(buffer);
  pos = formatTail(buffer,size,pos); 
  svr->send(200,"text/html",buffer);
}

void Control::setup(WebContext* svr) {
  UPnPDevice::setup(svr);
  char pathBuff[100];
  contentPath(pathBuff,100);
  svr->on(pathBuff,[this](WebContext* svr){this->displayControl(svr);});
}

void Control::contentPath(char buffer[], size_t size) {handlerPath(buffer,size,"displayControl");}

} // End of namespace lsc
