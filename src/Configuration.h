
/**
 * 
 */

#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include "UPnPDevice.h"
#include <WebContext.h>

/** Leelanau Software Company namespace 
*  
*/
namespace lsc {

/**
 *   UPnPServices to get and set configutation for a UPnPDevice. Both Control and Sensor come with default Configuration to set 
 *   and get device display name.
 */
 
class SetConfiguration : public UPnPService {
    public:
    SetConfiguration();
    SetConfiguration(const char* type, const char* target);

    void setFormHandler(HandlerFunction h) {_formHandler = h;}
    
    void defaultHandler(WebContext* svr);
    void defaultFormHandler(WebContext* svr);
    void formPath(char buffer[],size_t size);
    void setup(WebContext* svr);
    
/**
 *   Macros to define the following Runtime Type Info:
 *     private: static const ClassType  _classType;             
 *     public:  static const ClassType* classType();   
 *     public:  virtual void*           as(const ClassType* t);
 *     public:  virtual boolean         isClassType( const ClassType* t);
 */
    DEFINE_RTTI;
    DERIVED_TYPE_CHECK(UPnPService);

    HandlerFunction      _formHandler = [](WebContext* svr){};

};

class GetConfiguration : public UPnPService {
    public:
    GetConfiguration();
    GetConfiguration(const char* type, const char* target);

    void defaultHandler(WebContext* svr);

/**
 *   Macros to define the following Runtime Type Info:
 *     private: static const ClassType  _classType;             
 *     public:  static const ClassType* classType();   
 *     public:  virtual void*           as(const ClassType* t);
 *     public:  virtual boolean         isClassType( const ClassType* t);
 */
    DEFINE_RTTI;
    DERIVED_TYPE_CHECK(UPnPService);
};

} // End of namespace lsc

 #endif
