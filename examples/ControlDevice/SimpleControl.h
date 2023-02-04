/**
 * 
 */

#ifndef CONTROLTEST_H
#define CONTROLTEST_H

#include <CommonProgmem.h>
#include "Control.h"

/** Leelanau Software Company namespace 
*  
*/
namespace lsc {

/** SimpleControl is a Control for managing a relay, which in turn could control an outlet. SimpleControl has 2 states ON, and OFF controlled by a toggle 
 *  in the HTML interface. Switch ControlState is read from the relay itself rather than managing an internal variable.
 *  Implementers of this class must provide:
 *      virtual void  content(WebContext* svr);    // From Control - displays the device based on SimpleControl state
 *      virtual void  setState(WebContext* svr);   // HttpHandler for retrieving arguments and setting the SimpleControl state
 *  The actual electronics for  ON/OFF are implemented in the method setOutletState(ControlState)
 *  SimpleControl relay uses WeMOS pin D5 (GPIO pin 14), but pin can be set via:
 *      void setPin(int pin);
 *  Configuration support is provided by Control allowing  deviceName definition 
 */

class SimpleControl : public Control {

  public: 
      SimpleControl();
      SimpleControl( const char* type, const char* target );

      virtual int     frameHeight()  {return 100;}                                                      // Frame height from Control

/**
 *    Relay state management
 */
      void            setState(WebContext* svr);                                                        // HttpHandler for setting ControlState
      
      boolean         isON()                      {return(getControlState() == ON);}                    // Returns TRUE if the relay is ON
      boolean         isOFF()                     {return(getControlState() == OFF);}                   // Returns TRUE if the relay is OFF
      ControlState    getControlState()           {return(_state);}                                     // Returns ControlState ON/OFF
      const char*     controlState()              {return((isON())?("ON"):("OFF"));}                    // Returns char* representation of ControlState

/**
 *    Display this Control
 */
      void             content(char buffer[], int size);
      void             setup(WebContext* svr);
 
/**
 *   Macros to define the following Runtime Type Info:
 *     private: static const ClassType  _classType;             
 *     public:  static const ClassType* classType();   
 *     public:  virtual void*           as(const ClassType* t);
 *     public:  virtual boolean         isClassType( const ClassType* t);
 */
      DEFINE_RTTI;
      DERIVED_TYPE_CHECK(Control);

      protected:
      void                 setControlState(ControlState flag) {_state = flag;}            

/**
 *    Control Variables
 */
      ControlState         _state;
      
      SimpleControl(const SimpleControl&)= delete;
      SimpleControl& operator=(const SimpleControl&)= delete;

};

} // End of namespace lsc

 #endif