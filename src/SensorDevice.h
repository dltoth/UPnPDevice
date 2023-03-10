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

#ifndef SENSOR_DEVICE_H
#define SENSOR_DEVICE_H

/**
 *  Note filename is SensorDevice even though the classname is Sensor, this is to resolve conflicts with ESP32 Sensor.h
 */

#include "UPnPDevice.h"
#include "Configuration.h"

/** Leelanau Software Company namespace 
 *  
 */
namespace lsc {
  

/** A Sensor is a configurable UPnPDevice that provides its Sensor reading as simple HTML.
 *  Configuration is provided by a UPnPService
 */

class Sensor : public UPnPDevice {
    public:
      Sensor();
      Sensor( const char* target );

      GetConfiguration*  getConfiguration() {return &_getConfiguration;}
      SetConfiguration*  setConfiguration() {return &_setConfiguration;}

/** Sensors with complex configutation should implement methods for the following
 *  
 *    configForm(WebContext* svr)             := Presents an HTML form for configuration input. Form submission
 *                                               ultimately calls setConfiguration() on the Sensor Object. Default 
 *                                               action only supplies a form for setting diaplay name.  
 *                                               Note: the form "action" attribute MUST be "setConfiguration".
 *                                               This is set on the constructor with:
 *                                               setConfiguration()->setFormHandler([this](WebContext* svr){this->configForm(svr);});
 *    setSensorconfiguration(WebContext* svr) := Request handler for form submission; called from the UPnPService
 *                                               SetConfiguration::handleRequest(). Default action only sets display name.
 *                                               This is set on the constructor with:
 *                                               setConfiguration()->setHttpHandler([this](WebContext* svr){this->setSensorConfiguration(svr);});
 *    getSensorConfiguration(WebContext* svr) := Request handler that replies with an XML document describing configuration;
 *                                               called from the UPnPService GetConfiguration::handleRequest().
 *                                               There is no formal schema for this document but must start with XML header:
 *                                               <?xml version=\"1.0\" encoding=\"UTF-8\"?>
 *                                               And must include: 
 *                                               <config><displayName>display name</displayName>...</config>                      
 *                                               Default action only supplies the displayName attribute.
 *                                               This is set on the constructor with:
 *                                               getConfiguration()->setHttpHandler([this](WebContext* svr){this->getSensorConfiguration(svr);});
 *    
 */

/** Implementations of Sensor must include the following:
 *  
 *    content()           := Supplies only the Sensor reading as HTML into the supplied
 *                           buffer. Base Sensor class provides implementation for display(),
 *                           which uses content(), so this method must be implemented.
 *    
 */
      virtual void       content(char buffer[], int bufferSize) = 0;
      
      virtual void       display(WebContext* svr);

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
     DEFINE_EXCLUSIONS(Sensor);         

      private:

      GetConfiguration     _getConfiguration;
      SetConfiguration     _setConfiguration;

};

}
#endif
