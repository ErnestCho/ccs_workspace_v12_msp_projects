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


//
// switch related
//
uint8_t switch_status = 0;

// SW1 = P2.0
#define SW1_PORT_DIR            P2DIR
#define SW1_PORT_OUT            P2OUT
#define SW1_PORT_REN            P2REN
#define SW1_PORT_IES            P2IES
#define SW1_PORT_IE             P2IE
#define SW1_PORT_IFG            P2IFG
#define SW1_BIT                 BIT0

// SW2 = P2.1
#define SW2_PORT_DIR            P2DIR
#define SW2_PORT_OUT            P2OUT
#define SW2_PORT_REN            P2REN
#define SW2_PORT_IES            P2IES
#define SW2_PORT_IE             P2IE
#define SW2_PORT_IFG            P2IFG
#define SW2_BIT                 BIT1

///////////////////////////////////////////////////////////////////////
// GPIO Switch interrupt part
///////////////////////////////////////////////////////////////////////

// GPIO_P1InterruptSet() : external interrupt setup for P1
// param : bits - multi-bits setup
void GPIO_SW1_SW2_Interrupt_Set(void)
{
    SW1_PORT_DIR &= ~SW1_BIT;                         // port as Input
    SW1_PORT_OUT |= SW1_BIT;                          // Configure P1 as pulled-up
    SW1_PORT_REN |= SW1_BIT;                          // P1 pull-up register enable
    SW1_PORT_IES &= ~SW1_BIT;                         // P1 edge, 0: low to high edge
    SW1_PORT_IE  |= SW1_BIT;                          // P1 interrupt enabled
    SW1_PORT_IFG &= ~SW1_BIT;                         // P1 IFG cleared

    SW2_PORT_DIR &= ~SW2_BIT;
    SW2_PORT_OUT |= SW2_BIT;                          // Configure P2.x as pulled-up
    SW2_PORT_REN |= SW2_BIT;                          // P2.x pull-up register enable
    SW2_PORT_IES &= ~SW2_BIT;                         // P2.x edge, 0: low to high edge
    SW2_PORT_IE  |= SW2_BIT;                          // P2.x interrupt enabled
    SW2_PORT_IFG &= ~SW2_BIT;                         // P2.x IFG cleared
}



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


    GPIO_SW1_SW2_Interrupt_Set();
    // Disable the GPIO power-on default high-impedance mode
    // to activate previously configured port settings
     PM5CTL0 &= ~LOCKLPM5;
    __delay_cycles(10000);



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


// Port 2 interrupt service routine
#pragma vector=PORT2_VECTOR
__interrupt void Port_2(void)
{
    // switch 1 detection ==> sw1 function : mode change
    if(SW1_PORT_IFG & SW1_BIT)
    {
        SW1_PORT_IFG &= ~SW1_BIT;                         // Clear PORT IFG for SW1

        if(switch_status == BIT1)   // do not operate if another switch pin is on operation
        {
            ;
        }
        else
        {
            switch_status |= BIT0;
        }
    }
    else
    {
        SW1_PORT_IFG &= ~SW1_BIT;
    }

    // switch 2 detection ==> sw2 function : brightness control
    if(SW2_PORT_IFG & SW2_BIT)
    {
        SW2_PORT_IFG &= ~SW2_BIT;                         // Clear PORT IFG for SW1

        if(switch_status == BIT0)   // do not operate if another switch pin is on operation
        {
            ;
        }
        else
        {
            switch_status |= BIT1;
        }
    }
    else
    {
        SW2_PORT_IFG &= ~SW2_BIT;
    }

}


