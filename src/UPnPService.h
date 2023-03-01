/**
 * 
 */

#ifndef UPNP_SERVICE_H
#define UPNP_SERVICE_H

#include <Arduino.h>
#include <ctype.h>
#include <WebContext.h>

/** Leelanau Software Company namespace 
*  
*/
namespace lsc {

#define TARGET_SIZE    32
#define NAME_SIZE      32

typedef std::function<void(void)> CallbackFunction;

/**
 *   Macro to define Runtime Type Identification and UPnP Device Type
 *   Note that the static upnpType() is tied to the class and virtual getType() is tied to the instance of an Object
 *   the same way that classType() is tied to the class and isClassType() tied to the instance
 */
#define DEFINE_RTTI     private: static const ClassType  _classType;                                                           \
                        public:  static const ClassType* classType()                 {return &_classType;}                     \
                        public:  virtual void*           as(const ClassType* t)      {return((isClassType(t))?(this):(NULL));} \
                        private: static const char*      _upnpType;                                                            \
                        public:  static const char*      upnpType()                  {return _upnpType;}                       \
                        public:  virtual const char*     getType()                   {return upnpType();}                      \
                        public:  virtual boolean         isType(const char* t)       {return(strcmp(t,getType()) == 0);}  

/**
 *   Define type check for classes derived from a single Base Class
 */
#define DERIVED_TYPE_CHECK(BaseClass) public: virtual boolean isClassType( const ClassType* t) {return (_classType.isClassType(t) || BaseClass::isClassType(t));}

/**
 *   Note that isClassType() could be defined for multiple inheritance, however the as() operator will not work correctly when the second (or subsequent) class
 *   has one or more virtual methods. The virtual methods will not properly resolve.  This RTTI subsystem is therefore to be used ONLY in single inheritance
 *   class hierarchys. Multiple inheritance type check would look like:
 *   virtual boolean   isClassType( const ClassType* t) {return (_classType.isClassType(t) || ClassName1::isClassType(t) || ... || ClassNameN::isClassType(t));}
 */

/**
 *   Define type check for base classes
 */
#define BASE_TYPE_CHECK  public: virtual boolean isClassType( const ClassType* t) {return _classType.isClassType(t);}
/**
 *   Note: This should only be necessary for classes that are NOT subclasses of UPnPObject
 */

/**
 *   Define static initializer for ClassName; this should appear at the top of the .cpp file when using DEFINE_RTTI
 */
#define INITIALIZE_STATIC_TYPE(ClassName)  const ClassType ClassName::_classType = ClassType()

/**
*    Define static initializer for device type
*/
#define INITIALIZE_UPnP_TYPE(className,type) const char* className::_upnpType = #type;

/**
*    Copy construction and destruction are not allowed
*/
#define DEFINE_EXCLUSIONS(className)      className(const className&)= delete;      \
                                          className& operator=(const className&)= delete;


/**
 *    Macro to define typesafe cast of parent
 */
#define GET_PARENT_AS(T) ((getParent()!=NULL)?(getParent()->as(T)):(NULL))

class UPnPService;
class UPnPDevice;
class RootDevice;

class ClassType {
  public:
    ClassType() {_typeID=++_numTypes;}

    int         typeID() const               {return _typeID;}
    
    boolean     isClassType( const ClassType* t) const {return ((t!=NULL)?(this->typeID() == t->typeID()):(false));}

  private:
    static int   _numTypes;
    int          _typeID;
};

/** UPnPObject class definition.
 *  UPnPObject Class members are:
 *     _target       := The relative URL for this service. The complete URL can be constructed as "/rootTarget/deviceTarget/serviceTarget"
 *                      or "/rootTarget/serviceTarget"
 *     _parent       := A pointer to the UPnPDevice containing this service
 *     _displayName  := Name of Object for display purposes
 *    
 *  Static Members defined in the macro DEFINE_RTTI 
 *     _classType    := Bespoke RTTI class type and associated methods    
 *                         static const ClassType* classType()                      {return &_classType;}
 *                         virtual boolean         isClassType( const ClassType* t) {return (_classType.isClassType(t) || UPnPDevice::isClassType(t) || ...);}
 *                         virtual void*           as(const ClassType* t)           {return((isClassType(t))?(this):(NULL));}
 *     _upnpType     := UPnP Device (Service) type and associated methods
                           static const char*      upnpType()                  {return _upnpType;} 
                           virtual const char*     getType()                   {return upnpType();}   
                           virtual boolean         isType(const char* t)       {return(strcmp(t,getType()) == 0);}  
*
 *     
 *
 */
 
class UPnPObject {

   public:

     UPnPObject();
     UPnPObject(const char* target) {setTarget(target);}

     void           setTarget(const char* target);
     void           setDisplayName(const char* name);
    
     const char*    getTarget()           {return _target;}
     const char*    getDisplayName()      {return _displayName;}
     UPnPObject*    getParent()           {return _parent;}
     UPnPDevice*    parentAsDevice()      {return ((_parent!=NULL)?(_parent->asDevice()):(NULL));}
     boolean        hasParent()           {return getParent() != NULL;}
     RootDevice*    rootDevice();
     void           getPath(char buffer[], size_t size);                              // Returns a complete target path from root, including this target
     void           handlerPath(char buffer[], size_t size, const char* handlerName); // Concatenate handlerName to path

     static void    encodePath(char buffer[], size_t size, const char* path);         // URL Encode path into buffer. Replaces '/' with "%2F"

     public:
     DEFINE_RTTI;
     BASE_TYPE_CHECK;
     
     virtual RootDevice*     asRootDevice()                        = 0;
     virtual UPnPService*    asService()                           = 0;
     virtual UPnPDevice*     asDevice()                            = 0;
     virtual void            location(char buffer[], int buffSize, IPAddress addr) = 0;
     DEFINE_EXCLUSIONS(UPnPObject);         

   protected:
     char                  _target[TARGET_SIZE];
     char                  _displayName[NAME_SIZE];
     UPnPObject*           _parent = NULL;

     void           setParent(UPnPObject* parent)  {_parent = parent;}

};

/** UPnPService Class Definition
 *  UPnPService is set up to hanele HTTP requests to the target /rootTarget/deviceTarget/serviceTarget
 *  Service implementations can either subclass UPnPService and override handleRequest(), or set a
 *  handler function from the parent UPnPDevice, for example see the GetConfiguration service.
 */

class UPnPService : public UPnPObject {
     public:
     UPnPService() : UPnPObject("service") {setDisplayName("Service");}
     UPnPService(const char* target) : UPnPObject(target) {setDisplayName("Service");};
     
     void            setHttpHandler(HandlerFunction h)    {_handler = h;}
     virtual void    handleRequest(WebContext* svr)       {_handler(svr);}

  
/**
 *   Macro to define the following Runtime and UPnP Type Info:
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

     virtual RootDevice*      asRootDevice()  {return NULL;}
     virtual UPnPDevice*      asDevice()      {return NULL;}
     virtual UPnPService*     asService()     {return this;}
     virtual void             location(char buffer[], int buffSize, IPAddress addr);
     virtual void             setup(WebContext* svr);

     HandlerFunction          _handler = [](WebContext* svr) {};

     friend class             UPnPDevice;

/**
 *   Copy construction and destruction are not allowed
 */
     DEFINE_EXCLUSIONS(UPnPService);         
};

} // End of namespace lsc

#endif
