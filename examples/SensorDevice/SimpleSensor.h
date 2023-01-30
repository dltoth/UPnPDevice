/**
 * 
 */

#ifndef SIMPLESENSOR_H
#define SIMPLESENSOR_H

#include <SensorDevice.h>

/** Leelanau Software Company namespace 
*  
*/
using namespace lsc;

#define BUFF_SIZE 100

/**
 *   Simple Sensor Example with default configuration
**/
class SimpleSensor : public Sensor {

    public:
      SimpleSensor();
      SimpleSensor( const char* displayName, const char* target);

      const char*    getMessage() {return _msg;}

/**
 *   Virtual Functions required for UPnPDevice
 */
      void           setup(WebContext* svr);

/**
 *   Virtual Functions required by Sensor
 */
      void           content(char buffer[], int bufferSize);

/**
 *   Macros to define the following Runtime Type Info:
 *     private: static const ClassType  _classType;             
 *     public:  static const ClassType* classType();   
 *     public:  virtual void*           as(const ClassType* t);
 *     public:  virtual boolean         isClassType( const ClassType* t);
 */
      DEFINE_RTTI;
      DERIVED_TYPE_CHECK(Sensor);
    
    protected:
    void          setMessage(const char* m);

    char          _msg[BUFF_SIZE];

/**
 *   Copy Construction and Deletion are not allowed
 */
    private:
    SimpleSensor(const SimpleSensor&)= delete;
    SimpleSensor& operator=(const SimpleSensor&)= delete;

};

#endif