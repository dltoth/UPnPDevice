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

#ifndef UPNP_DEVICE_H
#define UPNP_DEVICE_H
#include <CommonProgmem.h>
#include "UPnPService.h"

/** Leelanau Software Company namespace 
*  
*/
namespace lsc {
  
#define MAX_SERVICES 8
#define MAX_DEVICES  8
#define UUID_SIZE    37
#define DISPLAY_SIZE 1280


 /** UPnPDevice class definition
  *  A UPnPDevice may have up to MAX_SERVICES UPnPServices and can display itself.
  *  Class members are as follows:
  *    numServices()                := Returns the number of UPnPServices
  *    services()                   := Returns an array of MAX_SERVICES UPnPService pointers
  *    display()                    := Responds with an HTML interface for the Object, set on the Web Server as response to target
  *                                    As in: server.on(rootPath,[this,svr]{this->display(svr);});
  *                                    By default, will display a set of buttons for each of the device's UPnPServices. 
  *    setup()                      := Device specific setup, like setting Web Server request handlers for services. Default is to set display()
  *                                    as a request handler for the target path from root e.g. /rootTarget/deviceTarget and to set handleRequest()  
  *                                    as a request handler for /rootTarget/deviceTarget/serviceTarget. Note that all targets must be set prior to 
  *                                    the call to setup().
  *    doDevice()                   := Called in the Arduino loop(); an opportunity to do a unit of work
  *    addService(UPnPService*)     := Adds the next service
  *    addServices(UPnPService*...) := Adds up to MAX_SERVICES UPnPServices
  *    service(int n)               := Returns a pointer to the n'th UPnPService when 0 <= n < numServices() and NULL otherwise
  */

class UPnPDevice : public UPnPObject {

     public:
     UPnPDevice();
     UPnPDevice(const char* target);
  
     const char*    uuid()                         {return _uuid;}
     int            numServices()                  {return _numServices;}
     boolean        isDevice(const char* u)        {return (strcmp(u,uuid()) == 0);} 
     UPnPService**  services()                     {return _services;}
     UPnPService*   service(int i)                 {return (((i<_numServices)&&(i>=0))?(_services[i]):(NULL));}
     boolean        setUUID(String uuid);
     void           addService(UPnPService* svc);
     
     virtual void         doDevice() {}  
     virtual void         display(WebContext* svr);
     virtual void         setup(WebContext* svr);
     virtual void         location(char buffer[], int buffSize, IPAddress ifc);
  
     template<typename T>
     void addServices( T ptr) {addService(ptr);}
     
     template<typename T, typename... Args> 
     void addServices( T ptr, Args... args) {addServices(ptr); addServices(args...);}

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
     DERIVED_TYPE_CHECK(UPnPObject);          

     virtual RootDevice*     asRootDevice() {return NULL;}
     virtual UPnPDevice*     asDevice()     {return this;}
     virtual UPnPService*    asService()    {return NULL;}

/**
 *   Send UPnP info to Serial
 */
     static void             printInfo(UPnPDevice* d);
     
     protected:
     
     UPnPService*       _services[MAX_SERVICES];
     int                _numServices = 0;
     char               _uuid[UUID_SIZE];
     
     friend class RootDevice;

/**
 *   Copy construction and destruction are not allowed
 */
     DEFINE_EXCLUSIONS(UPnPDevice);         

};

/** RootDevice class definition
 *  A RootDevice is a UPnPDevice that can have embedded UPnPDevices.
 *  Class members are as follows:
 *    numDevices()                 := Returns the number of UPnPDevices
 *    devices()                    := Returns an array of MAX_DEVICES UPnPDevice pointers
 *    displayRoot()                := Displays a single HTML Button with the displayName of this RootDevice. Selecting the button
 *                                    will trigger the display() function to be called
 *    setUp()                      := Device specific setup, like setting Web Server request handlers. Default is to set display()
 *                                    as a request handler for target() and to set the CSS styles from styles()
 *    addDevice(UPnPDevice*)       := Adds the next service
 *    addDevices(UPnPDevice*...)   := Adds up to MAX_DEVICES UPnPDevices
 *    service(int)                 := Returns a pointer to the n'th UPnPDevice
 *    styles()                     := Responds with the CSS styles for this RootDevice.
 */
class RootDevice : public UPnPDevice {

     public:
     RootDevice();
     RootDevice(const char* target);

     int               serverPort()                 {return _serverPort;}
     int               numDevices()                 {return _numDevices;}
     UPnPDevice**      devices()                    {return _devices;}
     UPnPDevice*       device(int i)                {return ((i<_numDevices)?(_devices[i]):(NULL));}
     WebContext*       getContext()                 {return _context;}
     
     void              rootLocation(char buffer[], int buffSize, IPAddress ifc);
     void              addDevice(UPnPDevice* dvc);
     UPnPDevice*       getDevice(const ClassType* t);
     UPnPDevice*       getDevice(const char* uuid);

    static UPnPDevice* getDevice(RootDevice* root, const ClassType* type) {return((root!=NULL)?(root->getDevice(type)):(NULL));}

     void              setup(WebContext* svr);
     void              display(WebContext* svr);
     void              doDevice();
     virtual void      location(char buffer[], int buffSize, IPAddress addr);
     virtual void      displayRoot(WebContext* svr);
     virtual void      styles(WebContext* svr); 
  
     template<typename T>
     void addDevices( T ptr) {addDevice(ptr);}
     
     template<typename T, typename... Args> 
     void addDevices( T ptr, Args... args) {addDevices(ptr); addDevices(args...);}

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

     virtual RootDevice*     asRootDevice() {return this;}
     virtual UPnPDevice*     asDevice()     {return this;}
     virtual UPnPService*    asService()    {return NULL;}
     
     protected:
/**
 *   Format content for base URL display, where Sensors insert their HTML and Controls 
 *   are linked with an iFrame, mitigating the need for a large display buffer.
 */
     virtual void            formatContent(char buffer[], int size);
     
     UPnPDevice*             _devices[MAX_DEVICES];
     int                     _numDevices = 0;
     WebContext*             _context = NULL;
     int                     _serverPort = 0;
     
/**
 *   Copy construction and destruction are not allowed
 */
     DEFINE_EXCLUSIONS(RootDevice);         
  
};

} // End of namespace lsc

#endif
