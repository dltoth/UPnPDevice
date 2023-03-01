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
 *    CustomControl state management
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