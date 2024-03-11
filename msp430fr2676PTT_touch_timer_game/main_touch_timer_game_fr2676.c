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

#include "common.h"
#include "stdint.h"
#include "ssd1306.h"
#include "lp5012.h"
#include "string.h"


#define DISABLE     0
#define ENABLE      1

void GPIO_I2C_Init(void)
{
    // LP5012
    SCL_1;
    SDA_1;
    SCL_OUT;
    SDA_OUT;
    SCL_PU;
    SDA_PU;
}

#if 0       // EN pin is hard wired pulled up
void GPIO_LP5012_IC_enable(unsigned char en)
{
    LP5012_EN_DIR_OUTPUT;

    if(en == 1)
        LP5012_EN_PIN_HIGH;
    else
        LP5012_EN_PIN_LOW;
    delay_ms(10);
}
#endif

void FR2676_LP5012_board_init()
{
    LP5012_I2C_chip_reset_1(1);
    delay_ms(100);

    LP5012_I2C_chip_enable_1(ENABLE);

    delay_ms(100);

}


//
// Timer related
//
#define LIMIT_MILI_SEC      1000
#define LIMIT_SECOND        60
#define LIMIT_MINUTE        60

typedef struct tag_timer_type
{
    unsigned short msec;
    unsigned char sec;
    unsigned char min;
    unsigned char hour;
}_tag_timer_type;

_tag_timer_type timer =
{
    .msec = 0,
    .sec = 0,
    .min = 0,
    .hour = 0
};
long timer_comparison = 0;
_tag_timer_type record_timer =
{
    .msec = 0,
    .sec = 0,
    .min = 0,
    .hour = 0
};
long record_timer_comparison = 0;
_tag_timer_type previous_timer =
{
    .msec = 0,
    .sec = 0,
    .min = 0,
    .hour = 0
};

uint8_t new_record_flag = 0;

//
// Periodic timer (TA0 source = SMCLK/4 = 2M/4 = 500kHz)
//
#define TIMER0_SMCLK_1msec              500
#define TIMER0_SMCLK_5msec              2500
#define TIMER0_SMCLK_10msec             5000
#define TIMER0_SMCLK_100msec            50000

#define CONST_TIMER0_CCR0               TIMER0_SMCLK_1msec
#define CONST_TIMER0_CCR1               TIMER0_SMCLK_5msec
#define CONST_TIMER0_CCR2               TIMER0_SMCLK_10msec

// 1msec,  5msec, 10msec continuous timer
void timerA0_init(void)
{
    TA0CTL = TASSEL_2 + MC_2;                   // SMCLK, Cont. mode
    // CCR0 interrupt enabled : 10msec
    TA0CCTL0 = CCIE;
    TA0CCR0 = CONST_TIMER0_CCR0;
    // CCR1 interrupt enabled : 100msec
//    TA0CCTL1 = CCIE;
//    TA0CCR1 = CONST_TIMER0_CCR1;
    // CCR2 interrupt enabled : 200msec
//    TA0CCTL2 = CCIE;
//    TA0CCR2 = CONST_TIMER0_CCR2;
}

void timerA0_start(void)
{
    timer.hour = timer.min = timer.sec = timer.msec = 0;

    TA0CTL = TASSEL_2 + MC_2 + + ID_2 + TACLR;                   // SMCLK, DIV4, up mode
    // CCR0 interrupt enabled
    TA0CCTL0 = CCIE;
    TA0CCR0 = CONST_TIMER0_CCR0;

}

void timerA0_stop(void)
{
    TA0CTL = TASSEL_2 + MC_0;
    // CCR0 interrupt enabled
    TA0CCTL0 = 0;
    TA0CCR0 = 0;

}



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


#define NUM_OF_LP5012_DEV   1
#define MAX_LED     12
#define LED_OUT_X_MID_LEVEL         0x20
#define LED_OUT_X_HIGH_LEVEL        0x60

uint8_t toggle = 0;
uint8_t brightness_level = 128;       // level : 0~255
uint8_t brightness = 2, led_ch = 0;
uint8_t data_buf[0x16];
uint8_t i, data;



//#define CONST   //const

CONST char str_mode[] = "game";               //5 : including null
CONST char str_switch_info1[] = "game       count";    //17 : including null
CONST char str_switch_info2[] = "game     execute";    //17 : including null
CONST char str_main_dp1[] = "Touch Timer ";            //12 : including null
CONST char str_main_dp2[] = "    running!";           //13 : including null
CONST char str_main_dp3[] = "Want to run?";           //13 : including null

CONST char str_game_mode[]  = "Select Game:";          // 12 : including null
CONST char str_game[] = "Game :";                     // 7 : including null
CONST char str_game_mode0[] = "1.Touch&Run";          // 12 : including null
CONST char str_game_mode1[] = "2.Stop watch";           // 11 : including null
CONST char str_game_cnt[] = "Game Cnt:";            // 12 : including null
CONST char str_push_execute[] = "Push Execute";           // 13 : including null

CONST char str_stop_stop[] = "stop        stop";        // 17
CONST char str_executing[] = "Executing";                  // 10
CONST char str_more[] = " more..";
CONST char str_result[] =     "::Results::";                  // 10
CONST char str_new_record[] = "New Record!";                  // 10
CONST char str_top[] = "Top:";                  // 10
CONST char str_now[] = "Now:";                  // 10
CONST char str_previous[] = "Prv:";                  // 10

CONST char str_touch_to_start[] = "touch to start";          // 14 : including null
CONST char str_touch_to_stop[] = "touch & stop";          // 14 : including null


void main(void)
{
	//
	// Initialize the MCU
	// BSP_configureMCU() sets up the device IO and clocking
	// The global interrupt enable is set to allow peripherals
	// to wake the MCU.
	//
	WDTCTL = WDTPW | WDTHOLD;
	BSP_configureMCU();             // need to initialize/modify by HW pin map.


    GPIO_SW1_SW2_Interrupt_Set();
    // Disable the GPIO power-on default high-impedance mode
    // to activate previously configured port settings
     PM5CTL0 &= ~LOCKLPM5;
    __delay_cycles(10000);

    // OLED initialization
    i2c_init();                                 // initialize I2C to use with OLED

    ssd1306_init();                             // Initialize SSD1306 OLED
    ssd1306_clearDisplay();                     // Clear OLED display
    delay_ms(1);


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


    //GPIO_LP5012_IC_enable(1);             // EN pin is hard wired pulled up at this solution
    GPIO_I2C_Init();
    FR2676_LP5012_board_init();

    //i2c_write_byte(LP5012_DEV0_ADDR, LP5012_DEVICE_CONFIG1, BIT5);       // set config1 register
    //[to save mem]i2c_write_byte(LP5012_DEV0_ADDR, LP5012_DEVICE_CONFIG1, 0x00);       // set config1 register
    i2c_write_byte_1(LP5012_DEVICE_CONFIG1, 0x00);
    //LP5012_I2C_chip_enable(LP5012_DEV0_ADDR, 1);

    //
    // LED animation of shifting LED
    //
    //brightness_level = 128;         // brightness :
    //LP5012_I2C_LEDx_ch_brightness(LP5012_DEV0_ADDR, brightness_level, brightness_level, brightness_level, brightness_level);
    LP5012_I2C_LEDx_single_brightness_1(brightness_level);

    // seperate color control
    for(led_ch=0; led_ch<MAX_LED; led_ch++)
    {
        LP5012_I2C_OUTx_color_1(led_ch, LED_OUT_X_MID_LEVEL);      // color level set from 0x80 to 0xc0
        delay_ms(25);
    }
    for(led_ch=0; led_ch<MAX_LED; led_ch++)
    {
        LP5012_I2C_OUTx_color_1(led_ch, 0x00);      // color level set from 0x80 to 0xc0
        delay_ms(25);
    }


    MAP_CAPT_disableISR(CAPT_TIMER_INTERRUPT);      // disable Captivate ISR during I2C OLED display writing. (to prevent text crack)
    ssd1306_clearDisplay();
    ssd1306_printString(0, 0, str_mode, FONT_6x8);
    ssd1306_printString(0, 1, str_main_dp1, FONT_MS_GOTHIC8x16);
    ssd1306_printString(0, 3, str_main_dp2, FONT_MS_GOTHIC8x16);
    ssd1306_printString(0, 5, str_main_dp3, FONT_MS_GOTHIC8x16);
    ssd1306_printString(0, 7, str_touch_to_start, FONT_6x8);
    MAP_CAPT_enableISR(CAPT_TIMER_INTERRUPT);       // release disable Captivate ISR during I2C OLED display writing. (to prevent text crack)

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


/******************************************************************************/
// Timer0_A0 Interrupt Service Routine: Disables the timer and exists LPM3
/******************************************************************************/


// 1ms continuous mode timer
#pragma vector=TIMER0_A0_VECTOR
__interrupt void ISR_Timer0_A0(void)
{
    TA0CCR0 += CONST_TIMER0_CCR0;               // Add Offset to CCR1

    timer.msec++;
    if(timer.msec >= LIMIT_MILI_SEC)
    {
        timer.msec = 0;
        timer.sec++;
        if(timer.sec >= LIMIT_SECOND)
        {
            timer.sec = 0;
            timer.min++;
            if(timer.min >= LIMIT_MINUTE)
            {
                timer.min = 0;
                timer.hour++;
            }
        }

    }
}


#pragma vector=TIMER0_A1_VECTOR
__interrupt void Timer0_A1A2(void)
{
    switch( TA0IV )
    {
        case  2:                                        // CCR1
        {
            TA0CCR1 += CONST_TIMER0_CCR1;               // Add Offset to CCR1

        }
        break;
        case  4:                                        // CCR2
        {
            TA0CCR2 += CONST_TIMER0_CCR2;               // Add Offset to CCR2

        } break;
        case 10:                                        // TA0 Overflow
        {
            //P1OUT ^= LED_TA0_OVF;
        } break;
    }
}


