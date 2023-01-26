
/**
 * 
 */

#include "Configuration.h"

/** Leelanau Software Company namespace 
*  
*/
namespace lsc {

const char Config_form[]      PROGMEM = "<form action=\"%s\"><div align=\"center\">"                                                                // Form Path
                                             "<label for=\"displayName\">Device Name &nbsp &nbsp</label>"
                                             "<input type=\"text\" placeholder=\"%s\" name=\"displayName\"><br><br>"                                // DisplayName
                                             "<button class=\"fmButton\" type=\"submit\">Submit</button>&nbsp&nbsp"
                                             "<button class=\"fmButton\" type=\"button\" onclick=\"window.location.href=\'%s\';\">Cancel</button>"  // Cancel path
                                        "</div></form>";
   
const char Config_template[]  PROGMEM = "<?xml version=\"1.0\" encoding=\"UTF-8\"?><config><displayName>%s</displayName></config>";

INITIALIZE_STATIC_TYPE(GetConfiguration);
INITIALIZE_STATIC_TYPE(SetConfiguration);

SetConfiguration::SetConfiguration() : UPnPService("urn:LeelanauSoftwareCo-com:service:SetConfiguration:1","setConfiguration") {
  setDisplayName("Set Configuration");
  setHttpHandler([this](WebContext*svr){this->defaultHandler(svr);});
  setFormHandler([this](WebContext*svr){this->defaultFormHandler(svr);});
}

SetConfiguration::SetConfiguration(const char* type, const char* target) : UPnPService(type,target) {
  setDisplayName("Set Configuration");
  setHttpHandler([this](WebContext*svr){this->defaultHandler(svr);});
  setFormHandler([this](WebContext*svr){this->defaultFormHandler(svr);});
}

/**
 *  Default handler takes DISPLAYNAME from the argument list and sets it on the parent UPnPDevice.
 */
void SetConfiguration::defaultHandler(WebContext* svr) {
  int numArgs = svr->argCount();
  UPnPObject* p = getParent();
  UPnPDevice* d = ((p!=NULL)?(p->asDevice()):(NULL));
  for( int i=0; i<numArgs; i++) {
     const String& argName = svr->argName(i);
     const String& arg = svr->arg(i);
     if( argName.equalsIgnoreCase("DISPLAYNAME") ) {if( (arg.length() > 0) && (p != NULL ) ) p->setDisplayName(arg.c_str());}
  }
  if( d != NULL ) d->display(svr);    
}

/** Default Form Handler presents a form that allows display name change.
 *  This can be included in either RootDevice or UPnPDevice and form path should resolve correctly.
 */
void SetConfiguration::defaultFormHandler(WebContext* svr) {
  char buffer[1000];
  int size = sizeof(buffer);
  int pos = formatHeader(buffer,size,"Set Display Name");

  char parentPath[100];
  parentPath[0] = '\0';
  UPnPObject* parent = getParent();
  if( parent != NULL ) parent->getPath(parentPath,100);
  else sprintf(parentPath,"/%s",getTarget()); 
  char path[100];
  getPath(path,100);

/**
 *  path is the url of the form action (HttpHandler), and parentPath is the url of the device for the cancel button
 *  The Config_form takes path, displayName, and parentPath
 *  Note that displayName is that of the parent, which should NOT be NULL.
 */
  const char* dn = ((parent!=NULL)?(parent->getDisplayName()):(getDisplayName()));
  pos = formatBuffer_P(buffer,size,pos,Config_form,path,dn,parentPath);
  formatTail(buffer,size,pos);
  svr->send(200,"text/html",buffer); 
}

void SetConfiguration::setup(WebContext* svr) {
   UPnPService::setup(svr);
   char pathBuffer[100];
   formPath(pathBuffer,100);
   svr->on(pathBuffer,[this](WebContext* svr){this->_formHandler(svr);});  
}

void SetConfiguration::formPath(char buffer[], size_t bufferSize) {
  handlerPath(buffer,bufferSize,"configForm");
}

GetConfiguration::GetConfiguration() : UPnPService("urn:LeelanauSoftwareCo-com:service:GetConfiguration:1","getConfiguration") {
  setDisplayName("Get Configuration");
  setHttpHandler([this](WebContext*svr){this->defaultHandler(svr);});
}

GetConfiguration::GetConfiguration(const char* type, const char* target) : UPnPService(type,target) {
  setDisplayName("Get Configuration");
  setHttpHandler([this](WebContext*svr){this->defaultHandler(svr);});
}

void GetConfiguration::defaultHandler(WebContext* svr) {
  UPnPObject* p = getParent();
  char buffer[1000];
  size_t bufferSize = sizeof(buffer);
  int size = bufferSize;
  if( p != NULL ) snprintf_P(buffer,size,Config_template,p->getDisplayName());
  else snprintf_P(buffer,size,Config_template,getDisplayName());                     // Service should always have a parent so this should not happen
  svr->send(200, "text/xml", buffer);
}

} // End of namespace lsc
