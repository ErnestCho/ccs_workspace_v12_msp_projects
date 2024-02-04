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
// Development main.c for MSP430FR2522 and MSP430FR2512 CapTIvate devices.
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
// \version 1.80.00.27
// Released on March 05, 2019
//
//*****************************************************************************

#include <msp430.h>                      // Generic MSP430 Device Include
#include "driverlib.h"                   // MSPWare Driver Library
#include "captivate.h"                   // CapTIvate Touch Software Library
#include "CAPT_App.h"                    // CapTIvate Application Code
#include "CAPT_BSP.h"                    // CapTIvate EVM Board Support Package
#include "CAPT_UserConfig.h"
#include "Capt_User.h"

#include "driver_TLC5973_TLC59731.h"

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
	// Start the CapTIvate application
	//
    MAP_CAPT_registerCallback(&BTN00, &PushButtonEventHandler);
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
        if(touch_info.change_occur == 1)
        {
            touch_info.change_occur = 0;

            // short key check
            if(touch_info.button1_key_on)
            {
                __no_operation();
                TLC5973_SerialComm(0, 0x0100, 0x0100, 0x0100);
                __delay_cycles(10000);

                //mode_change(MOVE_UP);
            }
            if(touch_info.button2_key_on)
            {
                __no_operation();
                TLC5973_SerialComm(0, 0x03ff, 0x0000, 0x03ff);
                __delay_cycles(10000);

                //mode_change(MOVE_DN);
            }
            if(touch_info.button3_key_on)
            {
                __no_operation();
                TLC5973_SerialComm(0, 0x0000, 0x03ff, 0x0000);
                __delay_cycles(10000);

                //attribute_change(MOVE_RIGHT);
            }
            if(touch_info.button4_key_on)
            {
                __no_operation();
                TLC5973_SerialComm(0, 0x0000, 0x0000, 0x0000);
                __delay_cycles(10000);
                //attribute_change(MOVE_LEFT);
            }

#if 0
            // long key process ==[
            // long key check
            if(touch_info.button1_long_key_on)
            {
                touch_info.button1_long_key_on = 0;
                touch_info.button1_long_key_one_time = 0;
                ALL_LED_ON;
                __delay_cycles(1000000);
                ALL_LED_OFF;
                __delay_cycles(1000000);
                ALL_LED_ON;
                __delay_cycles(1000000);
                ALL_LED_OFF;
                __delay_cycles(1000000);
                ALL_LED_ON;
                __delay_cycles(1000000);
                ALL_LED_OFF;
                __delay_cycles(1000000);
            }
            if(touch_info.button2_long_key_on)
            {
                touch_info.button2_long_key_on = 0;
                touch_info.button2_long_key_one_time = 0;
                ALL_LED_ON;
                __delay_cycles(1000000);
                ALL_LED_OFF;
                __delay_cycles(1000000);
                ALL_LED_ON;
                __delay_cycles(1000000);
                ALL_LED_OFF;
                __delay_cycles(1000000);
                ALL_LED_ON;
                __delay_cycles(1000000);
                ALL_LED_OFF;
                __delay_cycles(1000000);

            }
            if(touch_info.button3_long_key_on)
            {
                touch_info.button3_long_key_on = 0;
                touch_info.button3_long_key_one_time = 0;
                ALL_LED_ON;
                __delay_cycles(1000000);
                ALL_LED_OFF;
                __delay_cycles(1000000);
                ALL_LED_ON;
                __delay_cycles(1000000);
                ALL_LED_OFF;
                __delay_cycles(1000000);
                ALL_LED_ON;
                __delay_cycles(1000000);
                ALL_LED_OFF;
                __delay_cycles(1000000);

            }
            if(touch_info.button4_long_key_on)
            {
                touch_info.button4_long_key_on = 0;
                touch_info.button4_long_key_one_time = 0;
                ALL_LED_ON;
                __delay_cycles(1000000);
                ALL_LED_OFF;
                __delay_cycles(1000000);
                ALL_LED_ON;
                __delay_cycles(1000000);
                ALL_LED_OFF;
                __delay_cycles(1000000);
                ALL_LED_ON;
                __delay_cycles(1000000);
                ALL_LED_OFF;
                __delay_cycles(1000000);

            }
            // ]== long key
#endif

        }
		//
		// End of background loop iteration
		// Go to sleep if there is nothing left to do
		//
		CAPT_appSleep();
		
	} // End background loop
} // End main()
