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

#ifndef SENSORWITHCONFIG_H
#define SENSORWITHCONFIG_H

#include "SimpleSensor.h"

/** Leelanau Software Company namespace 
*  
*/
using namespace lsc;

#define BUFF_SIZE 100

/**
 *   Simple Sensor Example with extended configuration to allow message input
 **/
class SensorWithConfig : public SimpleSensor {

    public:
      SensorWithConfig();
      SensorWithConfig( const char* target);

/**
 *   Virtual Functions required for UPnPDevice
 */
      void           setup(WebContext* svr);

/**
 *    Methods to customize configuration
 */
      void           configForm(WebContext* svr);
      void           getConfiguration(WebContext* svr);
      void           setConfiguration(WebContext* svr);


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
      DERIVED_TYPE_CHECK(Sensor);
    
/**
 *   Copy construction and destruction are not allowed
 */
     DEFINE_EXCLUSIONS(SensorWithConfig);         

};

#endif