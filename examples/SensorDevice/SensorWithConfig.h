/**
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
      SensorWithConfig( const char* displayName, const char* target);

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
 *   Macros to define the following Runtime Type Info:
 *     private: static const ClassType  _classType;             
 *     public:  static const ClassType* classType();   
 *     public:  virtual void*           as(const ClassType* t);
 *     public:  virtual boolean         isClassType( const ClassType* t);
 */
      DEFINE_RTTI;
      DERIVED_TYPE_CHECK(Sensor);
    
/**
 *   Copy Construction and Deletion are not allowed
 */
    private:
      SensorWithConfig(const SensorWithConfig&)= delete;
      SensorWithConfig& operator=(const SensorWithConfig&)= delete;

};

#endif