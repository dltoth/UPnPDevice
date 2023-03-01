
/**
 * 
 */

#ifndef CONTROL_H
#define CONTROL_H

#include "UPnPDevice.h"
#include "Configuration.h"

/** Leelanau Software Company namespace 
*  
*/
namespace lsc {

/** A Control is a Configurable Device that provides its Control UI thru an iFrame.
 */
class Control : public UPnPDevice  {
    public:
      Control();
      Control( const char* target);

/**
*   Root based url to the request handler for iFrame content display, registered with the Web server 
*   at setup()
*/
      void               contentPath(char buffer[], size_t size);

/** Controls with complex configutation should implement methods for the following
 *  
 *    configForm(WebContext* svr)              := Presents an HTML form for configuration input. Form submission
 *                                                ultimately calls setConfiguration() on the Control Object. Default 
 *                                                action only supplies a form for setting diaplay name.  
 *                                                Note: the form "action" attribute MUST be "setConfiguration".
 *                                                This is set on the constructor with:
 *                                                setConfiguration()->setFormHandler([this](WebContext* svr){this->configForm(svr);});
 *    setControlconfiguration(WebContext* svr) := Request handler for form submission; called from the UPnPService
 *                                                SetConfiguration::handleRequest(). Default action only sets display name.
 *                                                This is set on the constructor with:
 *                                                setConfiguration()->setHttpHandler([this](WebContext* svr){this->setControlConfiguration(svr);});
 *    getControlConfiguration(WebContext* svr) := Request handler that replies with an XML document describing configuration;
 *                                                called from the UPnPService GetConfiguration::handleRequest().
 *                                                here is no formal schema for this document but must start with XML header:
 *                                                <?xml version=\"1.0\" encoding=\"UTF-8\"?>
 *                                                And must include: 
 *                                                <config><displayName>display name</displayName>...</config>                      
 *                                                Default action only supplies the displayName attribute.
 *                                                This is set on the constructor with:
 *                                                getConfiguration()->setHttpHandler([this](WebContext* svr){this->getControlConfiguration(svr);});
 *    
 */

/** Control is an abstract class, so implementations must include the following:
 *  
 *    content(char buffer[], int buffSize)     := Supplies only the Control HTML into the buffer provided. Base Control class implements 
 *                                                the request handlers for display, which use this method, so implementation is mandatory.   
 *    frameHeight()                            := Height of iFrame (defaults to 75)
 *    frameWidth()                             := Width of iFrame (defaults to 300);
 *    
 */
      virtual void       content(char buffer[], int buffSize) = 0;
      virtual int        frameHeight()      {return 75;}
      virtual int        frameWidth()       {return 300;}
      
      GetConfiguration*  getConfiguration() {return &_getConfiguration;}
      SetConfiguration*  setConfiguration() {return &_setConfiguration;}
      
      void               display(WebContext* svr);
      void               setup(WebContext* svr);

/**
 *   Display Control content, intended for the endpoint of an iFrame link and
 *   when the Control refreshes its display
 */
      virtual void       displayControl(WebContext* svr);

/**
 *   Macros to define the following Runtime and UPnP Type Info:
 *     private: static const ClassType  _classType;             
 *     public:  static const ClassType* classType();   
 *     public:  virtual void*           as(const ClassType* t);
 *     public:  virtual boolean         isClassType( const ClassType* t);
 *     private: static const char*      _upnpType;                                      
 *     public:  static const char*      upnpType()                  
 *     public:  virtual const char*     getType()                   
 *     public:  virtual boolean         isType(const char* t)       
 */
      DEFINE_RTTI;
      DERIVED_TYPE_CHECK(UPnPDevice);

/**
 *   Copy construction and destruction are not allowed
 */
     DEFINE_EXCLUSIONS(Control);         

      private:
      GetConfiguration     _getConfiguration;
      SetConfiguration     _setConfiguration;
};

} // End of namespace lsc

 #endif
