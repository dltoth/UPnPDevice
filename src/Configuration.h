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
    SetConfiguration(const char* target);

    void setFormHandler(HandlerFunction h) {_formHandler = h;}
    
    void defaultHandler(WebContext* svr);
    void defaultFormHandler(WebContext* svr);
    void formPath(char buffer[],size_t size);
    void setup(WebContext* svr);
    
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
    DERIVED_TYPE_CHECK(UPnPService);

    HandlerFunction      _formHandler = [](WebContext* svr){};

/**
 *   Copy construction and destruction are not allowed
 */
     DEFINE_EXCLUSIONS(SetConfiguration);         

};

class GetConfiguration : public UPnPService {
    public:
    GetConfiguration();
    GetConfiguration(const char* target);

    void defaultHandler(WebContext* svr);

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
    DERIVED_TYPE_CHECK(UPnPService);

/**
 *   Copy construction and destruction are not allowed
 */
     DEFINE_EXCLUSIONS(GetConfiguration);         

};

} // End of namespace lsc

 #endif
