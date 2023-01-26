/**
 * 
 */

#include "UPnPService.h"
/** Leelanau Software Company namespace 
*  
*/
namespace lsc {

/**
 *  Static initializers for runtime type identification
 */
int ClassType::_numTypes = 0;
INITIALIZE_STATIC_TYPE(UPnPService);

UPnPObject::UPnPObject() {
  _target[0]      = '\0';
  strlcpy(_displayName," ", sizeof(_displayName));  // Display name defaults to blank
}

void UPnPObject::setType(const char* type) {strlcpy(_type, type, sizeof(_type));}
void UPnPObject::setDisplayName(const char* name) {strlcpy(_displayName, name, sizeof(_displayName));}
void UPnPObject::setTarget(const char* target) {
  if( target[0] == '/' ) strlcpy(_target, target+1, sizeof(_target));
  else strlcpy(_target, target, sizeof(_target));
}

RootDevice* UPnPObject::rootDevice() {
  UPnPObject* p = getParent();
  UPnPObject* result = this;
  while( p != NULL ) {
    result = p;
    p = result->getParent();
  }
  return result->asRootDevice();
}

void UPnPObject::getPath(char buffer[], size_t size) {
  buffer[0] = '\0';
  if( getParent() != NULL ) {
    UPnPObject* d = getParent();
    if( d->getParent() != NULL ) {
       UPnPObject* r = d->getParent();
       snprintf(buffer,size,"/%s/%s/%s",r->getTarget(),d->getTarget(),getTarget());
    }
    else snprintf(buffer,size,"/%s/%s",d->getTarget(),getTarget());
  }
  else snprintf(buffer,size,"/%s",getTarget());
}

void UPnPObject::handlerPath(char buffer[], size_t bufferSize, const char* handlerName) {
  getPath(buffer,bufferSize);
  int len = strlen(buffer);
  int size = bufferSize - len;
  snprintf(buffer+len,size,"/%s",handlerName);
}

/** % encodes ULR string
 *   / encodes to %2F
 *   ? encodes to %3F
 *   = encodes to %3D
 *   & encodes to %26
 */
void UPnPObject::encodePath(char buffer[], size_t bufferSize, const char* path) {
  int pathSize = strlen(path);
  int j = 0;
  for(int i=0; (j<bufferSize)&&(i<pathSize); i++) {
     if( path[i] == '/' )     {buffer[j++]='%';buffer[j++]='2';buffer[j++]='F';}
     else if(path[i] == '?')  {buffer[j++]='%';buffer[j++]='3';buffer[j++]='F';}
     else if(path[i] == '=')  {buffer[j++]='%';buffer[j++]='3';buffer[j++]='D';}     
     else if(path[i] == '&')  {buffer[j++]='%';buffer[j++]='2';buffer[j++]='6';}
     else if(path[i] == '+')  {buffer[j++]='%';buffer[j++]='2';buffer[j++]='0';}
     else buffer[j++] = path[i];
  }
  if( j<bufferSize ) buffer[j] = '\0';
  else buffer[bufferSize-1] = '\0';
}

void UPnPService::location(char buffer[], int buffSize, IPAddress ifc) {
  UPnPObject* p = getParent();
  if( p != NULL ) {
    p->location(buffer,buffSize,ifc);
    int pos = strlen(buffer);
    int size = buffSize-pos;
    snprintf(buffer+pos,size,"/%s",getTarget());
  }
  else snprintf(buffer, buffSize,"/%s",getTarget());
}

void  UPnPService::setup(WebContext* svr) {
  char pathBuffer[100];
  getPath(pathBuffer,100);
  svr->on(pathBuffer,[this](WebContext* svr){this->handleRequest(svr);});
}

} // End of namespace lsc
