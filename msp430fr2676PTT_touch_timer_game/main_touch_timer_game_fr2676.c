/* --COPYRIGHT--,BSD
 * Copyright (c) 2017, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * --/COPYRIGHT--*/
//*****************************************************************************
// Development main.c for MSP430FR2676 CapTIvate devices.
//
// This starter application initializes the CapTIvate touch library
// for the touch panel specified by CAPT_UserConfig.c/.h via a call to
// CAPT_appStart(), which initializes and calibrates all sensors in the
// application, and starts the CapTIvate interval timer.
//
// Then, the capacitive touch interface is driven by calling the CapTIvate
// application handler, CAPT_appHandler().  The application handler manages
// whether the user interface (UI) is running in full active scan mode, or
// in a low-power wake-on-proximity mode.
//
// \version 1.83.00.05
// Released on May 15, 2020
//
//*****************************************************************************

#include <msp430.h>                      // Generic MSP430 Device Include
#include "driverlib.h"                   // MSPWare Driver Library
#include "captivate.h"                   // CapTIvate Touch Software Library
#include "CAPT_App.h"                    // CapTIvate Application Code
#include "CAPT_BSP.h"                    // CapTIvate EVM Board Support Package


typedef struct touch_bit_type {
    unsigned btn0_touch_on : 1;
    unsigned btn0_key_on : 1;
    unsigned btn1_touch_on : 1;
    unsigned btn1_key_on : 1;
    unsigned btn2_touch_on : 1;
    unsigned btn2_key_on : 1;
    unsigned change_occur : 1;
}_tag_touch_bit_type;

_tag_touch_bit_type touch_info = {
    .btn0_touch_on = 0,
    .btn0_key_on = 0,
    .btn1_touch_on = 0,
    .btn1_key_on = 0,
    .btn2_touch_on = 0,
    .btn2_key_on = 0,
    .change_occur = 0,
};

void touch_info_init(void)
{
    touch_info.btn0_touch_on = 0;
    touch_info.btn0_key_on = 0;
    touch_info.btn1_touch_on = 0;
    touch_info.btn1_key_on = 0;
    touch_info.btn2_touch_on = 0;
    touch_info.btn2_key_on = 0;
    touch_info.change_occur = 0;

}

#define TOUCH_PRESS_DETECT      1
#define TOUCH_RELEASE_DETECT    2

#define TOUCH_DETECT   TOUCH_RELEASE_DETECT

extern tElement BTN00_E00;
extern tElement BTN00_E01;

// make faster touch detection response
void fasterButtonEventHandler(tSensor *pSensor)
{
    // bTouch is set to 1 when it pressed
    // ==> bDetect is faster than bTouch when pressed, bTouch is faster than bDetect when released
    // so we would like to make a flag(button_touch when it turned released
    // check BTN00_E00 button
    if (BTN00_E00.bDetect == 1)
    {
        if(touch_info.btn0_touch_on == 0)    // set bit one time
        {
            touch_info.btn0_touch_on = 1;

            touch_info.change_occur = 1;        // update needed at main loop
            touch_info.btn0_key_on = 1;     // user use this variable
        }
        else
        {
            __no_operation();
        }

        if(touch_info.change_occur == 0)    // if touch detection captured from main?
        {
            touch_info.btn0_key_on = 0;
        }
    }
    else
    {
        if(touch_info.btn0_touch_on == 1)        // normal case of touch
        {
            touch_info.btn0_touch_on = 0;

            touch_info.change_occur = 1;        // update needed at main loop
            touch_info.btn0_key_on = 0;
        }
    }
}

void ButtonEventHandler(tSensor *pSensor)
{
    // bTouch is set to 1 when it pressed
    // so we would like to make a flag(button_touch when it turned released
    // check BTN00_E00 button
    if (BTN00_E00.bTouch == 1)
    {
        if(touch_info.btn0_touch_on == 0)    // set bit one time
        {
            touch_info.btn0_touch_on = 1;
        }
#if (TOUCH_DETECT == TOUCH_PRESS_DETECT)
            touch_info.change_occur = 1;        // update needed at main loop
            if(touch_info.btn0_key_on == 1)      // (key_on == 1) : button touched and released
                touch_info.btn0_key_on = 0;
            else
                touch_info.btn0_key_on = 1;
        }
#endif
    }
    else
    {
        if(touch_info.btn0_touch_on == 1)        // normal case of touch
        {
            touch_info.btn0_touch_on = 0;
#if (TOUCH_DETECT == TOUCH_RELEASE_DETECT)
            touch_info.change_occur = 1;        // update needed at main loop
            if(touch_info.btn0_key_on == 1)      // (key_on == 1) : button touched and released
                touch_info.btn0_key_on = 0;
            else
                touch_info.btn0_key_on = 1;
        }
#endif
    }
    // check BTN00_E01 button
    if (BTN00_E01.bTouch == 1)
    {
        if(touch_info.btn1_touch_on == 0)    // set bit one time
        {
            touch_info.btn1_touch_on = 1;
        }
#if (TOUCH_DETECT == TOUCH_PRESS_DETECT)
            touch_info.change_occur = 1;        // update needed at main loop
            if(touch_info.btn1_key_on == 1)      // (key_on == 1) : button touched and released
                touch_info.btn1_key_on = 0;
            else
                touch_info.btn1_key_on = 1;
        }
#endif
    }
    else
    {
        if(touch_info.btn1_touch_on == 1)        // normal case of touch
        {
            touch_info.btn1_touch_on = 0;
#if (TOUCH_DETECT == TOUCH_RELEASE_DETECT)
            touch_info.change_occur = 1;        // update needed at main loop
            if(touch_info.btn1_key_on == 1)      // (key_on == 1) : button touched and released
                touch_info.btn1_key_on = 0;
            else
                touch_info.btn1_key_on = 1;
        }
#endif

    }
}



void main(void)
{
	//
	// Initialize the MCU
	// BSP_configureMCU() sets up the device IO and clocking
	// The global interrupt enable is set to allow peripherals
	// to wake the MCU.
	//
	WDTCTL = WDTPW | WDTHOLD;
	BSP_configureMCU();
	__bis_SR_register(GIE);

    //
    // Register button call back function
    //
//    MAP_CAPT_registerCallback(&BTN00, &ButtonEventHandler);
    MAP_CAPT_registerCallback(&BTN00, &fasterButtonEventHandler);

	//
	// Start the CapTIvate application
	//
	CAPT_appStart();

	//
	// Background Loop
	//
	while(1)
	{
		//
		// Run the captivate application handler.
		//
		CAPT_appHandler();

		//
		// This is a great place to add in any 
		// background application code.
		//
		__no_operation();
        __no_operation();

		//
		// End of background loop iteration
		// Go to sleep if there is nothing left to do
		//
		CAPT_appSleep();
		
	} // End background loop
} // End main()
