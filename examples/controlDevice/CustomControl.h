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
using namespace lsc;

/** CustomControl is a Control that manages 2 states ON, and OFF controlled by a toggle in the HTML interface.
 *  Implementation supplies the following methods:
 *      virtual void  content(WebContext* svr);    // From Control - displays the toggle based on CustomControl state
 *      virtual void  setState(WebContext* svr);   // HttpHandler for retrieving arguments and setting the CustomControl state
 *  Configuration support is provided by Control allowing  deviceName definition 
 */

class CustomControl : public Control {

  public: 
      CustomControl() : Control("customControl") {setDisplayName("Custom Control");}

      CustomControl( const char* target ) : Control(target) {setDisplayName("Custom Control");}

      virtual int     frameHeight()  {return 100;}       // Frame height from Control

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
 
      DEFINE_RTTI;
      DERIVED_TYPE_CHECK(Control);

      protected:
      void                 setControlState(ControlState flag) {_state = flag;}            

/**
 *    Control Variables
 */
      ControlState         _state = OFF;
      
     DEFINE_EXCLUSIONS(CustomControl);         

};

 #endif