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
// Development main.c for MSP430FR2633, MSP430FR2533, MSP430FR2632, and
// MSP430FR2532.
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
// The CapTIvate application handler will return true if proximity was
// detected on any of the sensors in the application, which is used here
// to control the state of LED2. LED1 is set while the background loop enters
// the handler, and is cleared when the background loop leaves the handler.
//
// \version 1.80.00.27
// Released on March 05, 2019
//
//*****************************************************************************

// Touch key : 3points, (+additional 3points)
// LP5012 : 3x I2C bus, USE EN1,2 per I2C bus, 4x LP5012 per 1x EN
// 1x DISPLAY board have 4x LP5012, 6x DISPLAY board per MCU.

// MCU port description
// LP5012 CH_A : 2x4 LP5012
//    --> (1x4 LP5012) --> (EN0 = P2.3(18), SDA = P3.0(16), SCL = P1.0(11), 4x HW I2C slave ADDR)
//    --> (1x4 LP5012) --> (EN1 = P3.1(20), SDA = P3.0(16), SCL = P1.0(11), 4x HW I2C slave ADDR)
// LP5012 CH_B : 2x4 LP5012
//    --> (1x4 LP5012) --> (EN0 = P2.4(21), SDA = P2.2(15), SCL = P1.1(12), 4x HW I2C slave ADDR)
//    --> (1x4 LP5012) --> (EN1 = P2.5(22), SDA = P2.2(15), SCL = P1.1(12), 4x HW I2C slave ADDR)
// LP5012 CH_C :2x4 LP5012
//    --> (1x4 LP5012) --> (EN0 = P1.7(10), SDA = P1.3(14), SCL = P1.2(13), 4x HW I2C slave ADDR)
//    --> (1x4 LP5012) --> (EN1 = P1.6(9),  SDA = P1.3(14), SCL = P1.2(13), 4x HW I2C slave ADDR)



#include <msp430.h>                      // Generic MSP430 Device Include
#include "driverlib.h"                   // MSPWare Driver Library
#include "captivate.h"                   // CapTIvate Touch Software Library
#include "CAPT_App.h"                    // CapTIvate Application Code
#include "CAPT_BSP.h"                    // CapTIvate EVM Board Support Package

#include "LP5012.h"
#include "stdint.h"

typedef struct touch_bit_type {
    unsigned btn1_pressed : 1;
    unsigned btn2_pressed : 1;
    unsigned btn3_pressed : 1;
    unsigned btn4_pressed : 1;
    unsigned btn1_released : 1;
    unsigned btn2_released : 1;
    unsigned btn3_released : 1;
    unsigned btn4_released : 1;
}_tag_touch_bit_type;

_tag_touch_bit_type touch_info = {
    .btn1_pressed = 0,
    .btn2_pressed = 0,
    .btn3_pressed = 0,
    .btn4_pressed = 0,
    .btn1_released = 0,
    .btn2_released = 0,
    .btn3_released = 0,
    .btn4_released = 0,
};

void touch_info_init(void)
{
    touch_info.btn1_pressed = 0;
    touch_info.btn2_pressed = 0;
    touch_info.btn3_pressed = 0;
    touch_info.btn4_pressed = 0;
    touch_info.btn1_released = 0;
    touch_info.btn2_released = 0;
    touch_info.btn3_released = 0;
    touch_info.btn4_released = 0;

}

extern tElement BTN00_E00;
extern tElement BTN00_E01;
extern tElement BTN00_E02;

void ButtonEventHandler(tSensor *pSensor)
{
    // bTouch is set to 1 when it pressed
    // so we would like to make a flag(button_touch when it turned released
    // check BTN00_E00 button
    if (BTN00_E00.bTouch == 1)
    {
        touch_info.btn1_released = 0;
        if(touch_info.btn1_pressed == 0)    // set bit one time
        {
            touch_info.btn1_pressed = 1;
        }
    }
    else
    {
        if(touch_info.btn1_pressed == 1)
        {
            if(touch_info.btn1_released == 0)
            {
                touch_info.btn1_released = 1;
            }
            touch_info.btn1_pressed = 0;
        }
    }

    // check BTN00_E01 button
    if (BTN00_E01.bTouch == 1)
    {
        touch_info.btn2_released = 0;
        if(touch_info.btn2_pressed == 0)    // set bit one time
        {
            touch_info.btn2_pressed = 1;
        }
    }
    else
    {
        if(touch_info.btn2_pressed == 1)
        {
            if(touch_info.btn2_released == 0)
            {
                touch_info.btn2_released = 1;
            }
            touch_info.btn2_pressed = 0;
        }
    }
    // check BTN00_E02 button
    if (BTN00_E02.bTouch == 1)
    {
        touch_info.btn3_released = 0;
        if(touch_info.btn3_pressed == 0)    // set bit one time
        {
            touch_info.btn3_pressed = 1;
        }
    }
    else
    {
        if(touch_info.btn3_pressed == 1)
        {
            if(touch_info.btn3_released == 0)
            {
                touch_info.btn3_released = 1;
            }
            touch_info.btn3_pressed = 0;
        }
    }
}


//
// Periodic timer (TA0 source = SMCLK/4 = 2M/4 = 500kHz)
//
#define TIMER0_SMCLK_1msec              500
#define TIMER0_SMCLK_5msec              2500
#define TIMER0_SMCLK_10msec             5000
#define TIMER0_SMCLK_100msec            50000

#define CONST_TIMER0_CCR0               TIMER0_SMCLK_100msec
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


//////////////////////////////////////
// LP5012 structure
// 3x I2C bus exist in a system (CH1, CH2, CH3)
// One of I2C channel has 8x LP5012 with two EN pin(EN0, EN1)
//   4x LP5012 with I2C Address as 00, 01, 10, 11
//
// (BOARD0) I2C CH1 --> EN0 : IC1, IC2, IC3, IC4
// (BOARD1)         --> EN1 : IC1, IC2, IC3, IC4
// (BOARD2) I2C CH2 --> EN0 : IC1, IC2, IC3, IC4
// (BOARD3)         --> EN1 : IC1, IC2, IC3, IC4
// (BOARD4) I2C CH3 --> EN0 : IC1, IC2, IC3, IC4
// (BOARD5)         --> EN1 : IC1, IC2, IC3, IC4
//
//
//////////////////////////////////////

#define LED_BOARD0                  0       // I2C CH1, EN0
#define LED_BOARD1                  1       // I2C CH1, EN1
#define LED_BOARD2                  2       // I2C CH2, EN0
#define LED_BOARD3                  3       // I2C CH2, EN1
#define LED_BOARD4                  4       // I2C CH3, EN0
#define LED_BOARD5                  5       // I2C CH3, EN1
#define LED_BOARD_ALL_INIT          100

#define EN_0_HIGH    1
#define EN_1_HIGH    2
#define EN_0_1_HIGH  3
#define EN_0_1_LOW   4

// ALL channels of EN0,EN1 pins set to OUTPUT LOW
void GPIO_LP5012_multi_IC_enable_init(void)
{
    //LP5012_EN_DIR_OUTPUT;
    LP5012_CH_A_EN0_PIN_LOW;
    LP5012_CH_A_EN1_PIN_LOW;
    LP5012_CH_B_EN0_PIN_LOW;
    LP5012_CH_B_EN1_PIN_LOW;
    LP5012_CH_C_EN0_PIN_LOW;
    LP5012_CH_C_EN1_PIN_LOW;

    LP5012_CH_A_EN0_DIR_OUTPUT;
    LP5012_CH_A_EN1_DIR_OUTPUT;
    LP5012_CH_B_EN0_DIR_OUTPUT;
    LP5012_CH_B_EN1_DIR_OUTPUT;
    LP5012_CH_C_EN0_DIR_OUTPUT;
    LP5012_CH_C_EN1_DIR_OUTPUT;
}

void GPIO_LP5012_multi_IC_enable(uint8_t i2c_ch, uint8_t en0_en1)
{
    if(i2c_ch == LP5012_I2C_CH_A)
    {
        if(en0_en1 == EN_0_HIGH)
        {
            LP5012_CH_A_EN0_PIN_HIGH;
            LP5012_CH_A_EN1_PIN_LOW;
        }
        else if(en0_en1 == EN_1_HIGH)
        {
            LP5012_CH_A_EN0_PIN_LOW;
            LP5012_CH_A_EN1_PIN_HIGH;
        }
        else if(en0_en1 == EN_0_1_HIGH)
        {
            LP5012_CH_A_EN0_PIN_HIGH;
            LP5012_CH_A_EN1_PIN_HIGH;
        }
        else if(en0_en1 == EN_0_1_LOW)
        {
            LP5012_CH_A_EN0_PIN_LOW;
            LP5012_CH_A_EN1_PIN_LOW;
        }
    }
    else if(i2c_ch == LP5012_I2C_CH_B)
    {
        if(en0_en1 == EN_0_HIGH)
        {
            LP5012_CH_B_EN0_PIN_HIGH;
            LP5012_CH_B_EN1_PIN_LOW;
        }
        else if(en0_en1 == EN_1_HIGH)
        {
            LP5012_CH_B_EN0_PIN_LOW;
            LP5012_CH_B_EN1_PIN_HIGH;
        }
        else if(en0_en1 == EN_0_1_HIGH)
        {
            LP5012_CH_B_EN0_PIN_HIGH;
            LP5012_CH_B_EN1_PIN_HIGH;
        }
        else if(en0_en1 == EN_0_1_LOW)
        {
            LP5012_CH_B_EN0_PIN_LOW;
            LP5012_CH_B_EN1_PIN_LOW;
        }
    }
    else if(i2c_ch == LP5012_I2C_CH_C)
    {
        if(en0_en1 == EN_0_HIGH)
        {
            LP5012_CH_C_EN0_PIN_HIGH;
            LP5012_CH_C_EN1_PIN_LOW;
        }
        else if(en0_en1 == EN_1_HIGH)
        {
            LP5012_CH_C_EN0_PIN_LOW;
            LP5012_CH_C_EN1_PIN_HIGH;
        }
        else if(en0_en1 == EN_0_1_HIGH)
        {
            LP5012_CH_C_EN0_PIN_HIGH;
            LP5012_CH_C_EN1_PIN_HIGH;
        }
        else if(en0_en1 == EN_0_1_LOW)
        {
            LP5012_CH_C_EN0_PIN_LOW;
            LP5012_CH_C_EN1_PIN_LOW;
        }
    }
}



void GPIO_LP5012_multi_I2C_board_init(void)
{
    uint8_t ch, addr_idx;

    for(ch=LP5012_I2C_CH_A; ch<=LP5012_I2C_CH_C; ch++)
    {
        //GPIO_LP5012_multi_IC_enable(ch, EN_0_1_HIGH);
        GPIO_LP5012_multi_IC_enable(ch, EN_0_HIGH);

        for(addr_idx = 0; addr_idx < 4; addr_idx++)
        {
            LP5012_multi_ch_I2C_chip_reset(ch, LP5012_DEV0_ADDR+addr_idx, ENABLE);
            delay_ms(10);
            LP5012_multi_ch_I2C_chip_enable(ch, LP5012_DEV0_ADDR+addr_idx, ENABLE);
            delay_ms(10);

            LP5012_multi_ch_I2C_IC_init(ch, LP5012_DEV0_ADDR+addr_idx, LP5012_LOG_SCALE_EN_BIT);
            delay_ms(10);
        }
    }
    delay_ms(20);

}

// LED mode :
// sw1 action :
// - LED on(sw2 : brightness),
// - LED odd on(sw2 : brightness),
// - LED 3ea on(sw2 : brightness),
// - brightness inc(sw2 : delay time)
// - brightness dec(sw2 : delay time)
// - brightness up(sw2 : delay time)
// - brightness down(sw2 : delay time)
// - brightness up/down(sw2 : delay time)
#define LED_ACTION_SLEEP                        0
#define LED_ACTION_ALL_ON                       1
#define LED_ACTION_LOWER_ON                     2
#define LED_ACTION_UPPER_ON                     3
#define LED_ACTION_SINGLE_DIRECTION             4
#define LED_ACTION_CHAR_INDICATION              5
#define LED_ACTION_BRIGHTNESS_SHIFT_UP          6
#define LED_ACTION_BRIGHTNESS_SHIFT_DOWN        7
#define LED_ACTION_BRIGHTNESS_SHIFT_UPDN        8
#define LED_ACTION_MAX                          LED_ACTION_CHAR_INDICATION


uint8_t led_mode = 0;               // mode : 1~8
uint8_t brightness_level = 0;       // level : 1(dark)~5(bright)
uint8_t delay_time = 0;             // delay_time : 1(slow)~5(fast)
uint8_t f_update_once = 0;        // not to do excessive updates of LP5012 registers
uint8_t f_brightness_change = 0;
uint8_t cmd_inc_dec = 0;        // 0 : inc, 1 : dec

uint8_t brightness = 0x30, led_ch = 0;
uint8_t brightness_change_step = 5;
uint8_t group_brightness = 0xC0;

typedef enum {
    ID_NO_DISPLAY = 0,
    ID_CHO_JAE_YOON = 1,
    ID_CHO_SU_AH,
    ID_LEE_YEON_JUNG,
    ID_CHO_YOUNG_CHUNG,
    ID_MAX
}display_ID;

display_ID display_chars = ID_NO_DISPLAY;
uint8_t dir_idx = 0;

#define NUM_OF_LP5012_DEV   1
#define MAX_LED     12

#define GLOBAL_BRIGHTNESS_MAX   0xF0
//#define BRIGHTNESS_STEP         5

uint8_t switch_status = 0;
uint8_t cnt = 0;
uint8_t addr_idx = 0;
uint8_t i2c_ch = 0;

uint8_t data_buf[3][4][0x16];

// Hangul font text display data
//uint8_t font_6x8_hangul_cho[6]       = {0x92, 0x8A, 0xE6, 0x8A, 0x92, 0x12};      // Code for char " 조
//uint8_t font_6x8_hangul_jae[6]       = {0x32, 0x0E, 0x32, 0x7F, 0x08, 0x7F};      // Code for char # 재
//uint8_t font_6x8_hangul_yoon[6]      = {0xD0, 0x92, 0xB5, 0x95, 0xB5, 0x92};      // Code for char $ 윤
//uint8_t font_6x8_hangul_su[6]        = {0x28, 0x24, 0xE3, 0x26, 0x24, 0x28};      // Code for char % 수
//uint8_t font_6x8_hangul_ah[6]        = {0x3C, 0x42, 0x3C, 0x00, 0xFE, 0x10};      // Code for char & 아
//uint8_t font_6x8_hangul_lee[6]       = {0x1C, 0x22, 0x22, 0x1C, 0x00, 0xFF};      // Code for char ' 이
//uint8_t font_6x8_hangul_yeon[6]      = {0xCE, 0x91, 0x8E, 0x80, 0x8A, 0xBF};      // Code for char ( 연
//uint8_t font_6x8_hangul_jeong[6]     = {0x49, 0xA7, 0xA9, 0xA1, 0x44, 0x1F};      // Code for char ) 정
//uint8_t font_6x8_hangul_young[6]     = {0x46, 0xA9, 0xA6, 0xA0, 0x4A, 0x1F};      // Code for char * 영
//uint8_t font_6x8_hangul_chung_c[6]   = {0xCE, 0x0A, 0xDF, 0xAA, 0x8E, 0xA0};      // Code for char + 충성충
//uint8_t font_6x8_hangul_chung[6]     = {0x10, 0xDA, 0xF7, 0xDA, 0x10, 0x10};      // Code for char , 충
uint8_t font_6x8_hangul_cho[6]       = {0x12, 0x92, 0x8A, 0xE6, 0x8A, 0x92};      // Code for char " 조
uint8_t font_6x8_hangul_jae[6]       = {0x7F, 0x08, 0x7F, 0x32, 0x0E, 0x32};      // Code for char # 재
uint8_t font_6x8_hangul_yoon[6]      = {0x92, 0xB5, 0x95, 0xB5, 0x92, 0xD0};      // Code for char $ 윤
uint8_t font_6x8_hangul_su[6]        = {0x28, 0x24, 0x26, 0xE3, 0x24, 0x28};      // Code for char % 수
uint8_t font_6x8_hangul_ah[6]        = {0x10, 0xFE, 0x00, 0x3C, 0x42, 0x3C};      // Code for char & 아
uint8_t font_6x8_hangul_lee[6]       = {0xFF, 0x00, 0x1C, 0x22, 0x22, 0x1C};      // Code for char ' 이
uint8_t font_6x8_hangul_yeon[6]      = {0xBF, 0x8A, 0x80, 0x8E, 0x91, 0xCE};      // Code for char ( 연
uint8_t font_6x8_hangul_jeong[6]     = {0x1F, 0x44, 0xA1, 0xA9, 0xA7, 0x49};      // Code for char ) 정
uint8_t font_6x8_hangul_young[6]     = {0x1F, 0x4A, 0xA0, 0xA6, 0xA9, 0x46};      // Code for char * 영
uint8_t font_6x8_hangul_chung_c[6]   = {0xA0, 0x8E, 0xAA, 0xDF, 0x0A, 0xCE};      // Code for char + 충성충
uint8_t font_6x8_hangul_chung[6]     = {0x10, 0x10, 0xDA, 0xF7, 0xDA, 0x10};      // Code for char , 충

uint8_t *pt_dp_window_loc[6];   // data points being displayed on the LED location

uint8_t rework_cnt = 0;     // while loop is repeating every 33ms
uint8_t cycle_cnt = 0;

void fn_action_all_on(uint8_t brightness);
void fn_action_all_off(void);
void fn_action_sleep(void);
void fn_action_lower_on(uint8_t brightness);
void fn_action_upper_on(uint8_t brightness);
void fn_action_single_direction(uint8_t dir_idx, uint8_t brightness);
void fn_action_chars_display(uint8_t display_chars);

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

	// LP5012 EN pin set to OUTPUT LOW (totally 6 x EN pins)
    GPIO_LP5012_multi_IC_enable_init();

    PM5CTL0 &= ~LOCKLPM5;


	//GPIO_LP5012_multiple_I2C_IC_enable(0);

    timerA0_init();
	__bis_SR_register(GIE);

    //
    // Register button call back function
    //
    MAP_CAPT_registerCallback(&BTN00, &ButtonEventHandler);
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
		// Set LED1 while the app handler is running,
		// and set LED2 if proximity is detected
		// on any sensor.
		//
		CAPT_appHandler();
		
		//
		// This is a great place to add in any 
		// background application code.
		//
		__no_operation();

        if(brightness <= 0x10)
            brightness_change_step = 1;
		else if(brightness > 0x10 && brightness <= 0x40)
		    brightness_change_step = 3;
        else if(brightness > 0x40)
            brightness_change_step = 5;

        //////////////////////////////////////////////////////////////////////////////////////////////////////
        // check switch pads are touched : SW1
        //////////////////////////////////////////////////////////////////////////////////////////////////////
        if(touch_info.btn1_released == 1)        // button1 : action change
        {
            __no_operation();

            led_mode++;
            f_update_once = 1;
            if(led_mode > LED_ACTION_MAX)
            {
                led_mode = 0;
            }


            touch_info.btn1_released = 0;
        }
        ///////////////////////////////////////////////////
        // check switch pads are touched : SW2
        ///////////////////////////////////////////////////
        if(touch_info.btn2_released == 1)        // button2 : param up
        {
            __no_operation();
            f_brightness_change = 1;

            switch(led_mode)
            {
                // do nothing
                case LED_ACTION_SLEEP :
                {

                }break;
                // change brightness : increase
                case LED_ACTION_ALL_ON :
                case LED_ACTION_LOWER_ON :
                case LED_ACTION_UPPER_ON :
                case LED_ACTION_CHAR_INDICATION :
                {
                    brightness += brightness_change_step;
                    if(brightness >= GLOBAL_BRIGHTNESS_MAX)
                        brightness = GLOBAL_BRIGHTNESS_MAX;
                }break;
                // change direction : left
                case LED_ACTION_SINGLE_DIRECTION :
                {
                    dir_idx++;
                    if(dir_idx >= 6)
                    {
                        dir_idx = 0;
                    }
                }break;
            }


            touch_info.btn2_released = 0;

        }
        ///////////////////////////////////////////////////
        // check switch pads are touched : SW3
        ///////////////////////////////////////////////////
        if(touch_info.btn3_released == 1)        // button3 : param down
        {
            __no_operation();
            f_brightness_change = 1;

            switch(led_mode)
            {
                // do nothing
                case LED_ACTION_SLEEP :
                {

                }break;
                // change brightness : decrease
                case LED_ACTION_ALL_ON :
                case LED_ACTION_LOWER_ON :
                case LED_ACTION_UPPER_ON :
                case LED_ACTION_CHAR_INDICATION :
                {
                    if(brightness == 2)
                        ;
                    else
                    {
                        brightness -= brightness_change_step;
                    }

                }break;
                // change direction : right
                case LED_ACTION_SINGLE_DIRECTION :
                {
                    dir_idx--;
                    if(dir_idx == 255)
                    {
                        dir_idx = 5;
                    }

                }break;
            }

            touch_info.btn3_released = 0;

        }

        /////////////////////////////////////////////////////////////////////////////////////
        // actions for case by case of the buttons
        /////////////////////////////////////////////////////////////////////////////////////
        switch (led_mode)
        {
            case LED_ACTION_SLEEP :
            {
                if(f_update_once == 1)
                {
                    fn_action_sleep();
                }
                __no_operation();
            }break;
            case LED_ACTION_ALL_ON :
            {
                if(f_update_once == 1)
                {
                    fn_action_all_off();
                    fn_action_all_on(brightness);
                }
                if(f_brightness_change == 1)
                {
                    fn_action_all_on(brightness);
                }
                __no_operation();

            }break;
            case LED_ACTION_LOWER_ON :
            {
                if(f_update_once == 1)
                {
                    fn_action_all_off();
                    fn_action_lower_on(brightness);
                }
                if(f_brightness_change == 1)
                {
                    fn_action_lower_on(brightness);
                }
                __no_operation();

            }break;
            case LED_ACTION_UPPER_ON :
            {
                if(f_update_once == 1)
                {
                    fn_action_all_off();
                    fn_action_upper_on(brightness);
                }
                if(f_brightness_change == 1)
                {
                    fn_action_upper_on(brightness);
                }
                __no_operation();

            }break;

            // display single direction
            case LED_ACTION_SINGLE_DIRECTION :
            {
                if(f_update_once == 1 || f_brightness_change == 1)
                {
                    fn_action_all_off();
                    fn_action_single_direction(dir_idx, brightness);
                }
                __no_operation();

            }break;
            case LED_ACTION_CHAR_INDICATION :
            {
                if(f_update_once == 1) // 조 재 윤
                {
                    pt_dp_window_loc[0] = &font_6x8_hangul_cho[0];
                    pt_dp_window_loc[1] = &font_6x8_hangul_cho[0];
                    pt_dp_window_loc[2] = &font_6x8_hangul_cho[0];
                    pt_dp_window_loc[3] = &font_6x8_hangul_cho[0];
                    pt_dp_window_loc[4] = &font_6x8_hangul_cho[0];
                    pt_dp_window_loc[5] = &font_6x8_hangul_cho[0];

                    fn_action_all_off();
                    fn_action_chars_display(display_chars);

                    rework_cnt = 0;
                }
                if(f_brightness_change == 1)
                {
                    fn_action_chars_display(brightness);
                }


                if(rework_cnt > 40)    // 1 rework_cnt : 100ms
                {
                    rework_cnt = 0;

                    cycle_cnt++;
                    if(cycle_cnt >= 12)
                        cycle_cnt = 0;

                    switch(cycle_cnt)
                    {
                        case 0 : // for Cho Jae Yoon
                        {
                            pt_dp_window_loc[0] = &font_6x8_hangul_cho[0];
                            pt_dp_window_loc[1] = &font_6x8_hangul_cho[0];
                            pt_dp_window_loc[2] = &font_6x8_hangul_cho[0];
                            pt_dp_window_loc[3] = &font_6x8_hangul_cho[0];
                            pt_dp_window_loc[4] = &font_6x8_hangul_cho[0];
                            pt_dp_window_loc[5] = &font_6x8_hangul_cho[0];

                        }break;
                        case 1 : // for Cho Jae Yoon
                        {
                            pt_dp_window_loc[0] = &font_6x8_hangul_jae[0];
                            pt_dp_window_loc[1] = &font_6x8_hangul_jae[0];
                            pt_dp_window_loc[2] = &font_6x8_hangul_jae[0];
                            pt_dp_window_loc[3] = &font_6x8_hangul_jae[0];
                            pt_dp_window_loc[4] = &font_6x8_hangul_jae[0];
                            pt_dp_window_loc[5] = &font_6x8_hangul_jae[0];
                        }break;
                        case 2 : // for Cho Jae Yoon
                        {
                            pt_dp_window_loc[0] = &font_6x8_hangul_yoon[0];
                            pt_dp_window_loc[1] = &font_6x8_hangul_yoon[0];
                            pt_dp_window_loc[2] = &font_6x8_hangul_yoon[0];
                            pt_dp_window_loc[3] = &font_6x8_hangul_yoon[0];
                            pt_dp_window_loc[4] = &font_6x8_hangul_yoon[0];
                            pt_dp_window_loc[5] = &font_6x8_hangul_yoon[0];

                        }break;
                        case 3 : // for Cho Su Ah
                        {
                            pt_dp_window_loc[0] = &font_6x8_hangul_cho[0];
                            pt_dp_window_loc[1] = &font_6x8_hangul_cho[0];
                            pt_dp_window_loc[2] = &font_6x8_hangul_cho[0];
                            pt_dp_window_loc[3] = &font_6x8_hangul_cho[0];
                            pt_dp_window_loc[4] = &font_6x8_hangul_cho[0];
                            pt_dp_window_loc[5] = &font_6x8_hangul_cho[0];
                        }break;
                        case 4 : // for Cho Su Ah
                        {
                            pt_dp_window_loc[0] = &font_6x8_hangul_su[0];
                            pt_dp_window_loc[1] = &font_6x8_hangul_su[0];
                            pt_dp_window_loc[2] = &font_6x8_hangul_su[0];
                            pt_dp_window_loc[3] = &font_6x8_hangul_su[0];
                            pt_dp_window_loc[4] = &font_6x8_hangul_su[0];
                            pt_dp_window_loc[5] = &font_6x8_hangul_su[0];
                        }break;
                        case 5 : // for Cho Su Ah
                        {
                            pt_dp_window_loc[0] = &font_6x8_hangul_ah[0];
                            pt_dp_window_loc[1] = &font_6x8_hangul_ah[0];
                            pt_dp_window_loc[2] = &font_6x8_hangul_ah[0];
                            pt_dp_window_loc[3] = &font_6x8_hangul_ah[0];
                            pt_dp_window_loc[4] = &font_6x8_hangul_ah[0];
                            pt_dp_window_loc[5] = &font_6x8_hangul_ah[0];
                        }break;
                        case 6 : // for Lee Yeon Jeong
                        {
                            pt_dp_window_loc[0] = &font_6x8_hangul_lee[0];
                            pt_dp_window_loc[1] = &font_6x8_hangul_lee[0];
                            pt_dp_window_loc[2] = &font_6x8_hangul_lee[0];
                            pt_dp_window_loc[3] = &font_6x8_hangul_lee[0];
                            pt_dp_window_loc[4] = &font_6x8_hangul_lee[0];
                            pt_dp_window_loc[5] = &font_6x8_hangul_lee[0];
                        }break;
                        case 7 : // for Lee Yeon Jeong
                        {
                            pt_dp_window_loc[0] = &font_6x8_hangul_yeon[0];
                            pt_dp_window_loc[1] = &font_6x8_hangul_yeon[0];
                            pt_dp_window_loc[2] = &font_6x8_hangul_yeon[0];
                            pt_dp_window_loc[3] = &font_6x8_hangul_yeon[0];
                            pt_dp_window_loc[4] = &font_6x8_hangul_yeon[0];
                            pt_dp_window_loc[5] = &font_6x8_hangul_yeon[0];
                        }break;
                        case 8 : // for Lee Yeon Jeong
                        {
                            pt_dp_window_loc[0] = &font_6x8_hangul_jeong[0];
                            pt_dp_window_loc[1] = &font_6x8_hangul_jeong[0];
                            pt_dp_window_loc[2] = &font_6x8_hangul_jeong[0];
                            pt_dp_window_loc[3] = &font_6x8_hangul_jeong[0];
                            pt_dp_window_loc[4] = &font_6x8_hangul_jeong[0];
                            pt_dp_window_loc[5] = &font_6x8_hangul_jeong[0];
                        }break;
                        case 9 : // for Cho Young Chung
                        {
                            pt_dp_window_loc[0] = &font_6x8_hangul_cho[0];
                            pt_dp_window_loc[1] = &font_6x8_hangul_cho[0];
                            pt_dp_window_loc[2] = &font_6x8_hangul_cho[0];
                            pt_dp_window_loc[3] = &font_6x8_hangul_cho[0];
                            pt_dp_window_loc[4] = &font_6x8_hangul_cho[0];
                            pt_dp_window_loc[5] = &font_6x8_hangul_cho[0];
                        }break;
                        case 10 : // for Cho Young Chung
                        {
                            pt_dp_window_loc[0] = &font_6x8_hangul_young[0];
                            pt_dp_window_loc[1] = &font_6x8_hangul_young[0];
                            pt_dp_window_loc[2] = &font_6x8_hangul_young[0];
                            pt_dp_window_loc[3] = &font_6x8_hangul_young[0];
                            pt_dp_window_loc[4] = &font_6x8_hangul_young[0];
                            pt_dp_window_loc[5] = &font_6x8_hangul_young[0];
                        }break;
                        case 11 : // for Cho Young Chung
                        {
                            pt_dp_window_loc[0] = &font_6x8_hangul_chung_c[0];
                            pt_dp_window_loc[1] = &font_6x8_hangul_chung_c[0];
                            pt_dp_window_loc[2] = &font_6x8_hangul_chung_c[0];
                            pt_dp_window_loc[3] = &font_6x8_hangul_chung_c[0];
                            pt_dp_window_loc[4] = &font_6x8_hangul_chung_c[0];
                            pt_dp_window_loc[5] = &font_6x8_hangul_chung_c[0];
                        }break;
                        default :
                            break;
                    }

                    fn_action_all_off();
                    fn_action_chars_display(display_chars);

                }
                __no_operation();

            }break;

            default :
            {
                __no_operation();
            }break;
        }

        f_update_once = 0;
        f_brightness_change = 0;


		//
		// End of background loop iteration
		// Go to sleep if there is nothing left to do
		//
		CAPT_appSleep();

	} // End background loop

} // End main()



void fn_action_all_off(void)
{
    for(i2c_ch = LP5012_I2C_CH_A; i2c_ch <= LP5012_I2C_CH_C; i2c_ch++)
    {
        //
        // Enable CH_A - EN0, EN1
        //
        GPIO_LP5012_multi_IC_enable(i2c_ch, EN_0_1_HIGH);
        for(addr_idx = 0; addr_idx < 4; addr_idx++)
        {
            LP5012_multi_ch_I2C_LEDx_ch_brightness(i2c_ch, LP5012_DEV0_ADDR + addr_idx, 0, 0, 0, 0);
            for(led_ch=0; led_ch<MAX_LED; led_ch++)
            {
                LP5012_multi_ch_I2C_OUTx_color(i2c_ch, LP5012_DEV0_ADDR + addr_idx, led_ch, 0);      // color level set from 0x80 to 0xc0
            }
        }

    }

}

void fn_action_sleep(void)
{
    for(i2c_ch = LP5012_I2C_CH_A; i2c_ch <= LP5012_I2C_CH_C; i2c_ch++)
    {
        //
        // Enable CH_A - EN0, EN1
        //
        GPIO_LP5012_multi_IC_enable(i2c_ch, EN_0_1_HIGH);
        for(addr_idx = 0; addr_idx < 4; addr_idx++)
        {
            LP5012_multi_ch_I2C_LEDx_ch_brightness(i2c_ch, LP5012_DEV0_ADDR + addr_idx, 0, 0, 0, 0);
        }
        GPIO_LP5012_multi_IC_enable(i2c_ch, EN_0_1_LOW);

    }

}


void fn_action_all_on(uint8_t brightness)
{
    //
    // currents consumption test
    //
    for(i2c_ch = LP5012_I2C_CH_A; i2c_ch <= LP5012_I2C_CH_C; i2c_ch++)
    {
        //
        // Enable CH_A - EN0, EN1
        //
        GPIO_LP5012_multi_IC_enable(i2c_ch, EN_0_1_HIGH);

        GPIO_I2C_multi_Init(i2c_ch);
        // initialize the 1st panel(CH_A EN0)
        for(addr_idx = 0; addr_idx < 4; addr_idx++)
        {
            //LP5012_multi_ch_I2C_chip_reset(i2c_ch, LP5012_DEV0_ADDR+addr_idx, ENABLE);
            LP5012_multi_ch_I2C_chip_enable(i2c_ch, LP5012_DEV0_ADDR+addr_idx, ENABLE);
//            LP5012_multi_ch_I2C_IC_init(i2c_ch, LP5012_DEV0_ADDR+addr_idx, 0x00/*LP5012_LOG_SCALE_EN_BIT*/);
            LP5012_multi_ch_I2C_IC_init(i2c_ch, LP5012_DEV0_ADDR+addr_idx, LP5012_LOG_SCALE_EN_BIT);
        }

        for(addr_idx = 0; addr_idx < 4; addr_idx++)
        {
            // CH_A, EN_0, DEV0+addr_idx
            LP5012_multi_ch_I2C_LEDx_ch_brightness(i2c_ch, LP5012_DEV0_ADDR + addr_idx, group_brightness, group_brightness, group_brightness, group_brightness);
            for(led_ch=0; led_ch<MAX_LED; led_ch++)
            {
                LP5012_multi_ch_I2C_OUTx_color(i2c_ch, LP5012_DEV0_ADDR + addr_idx, led_ch, brightness);      // color level set from 0x80 to 0xc0
            }
        }
    }
#if 0
    for(i2c_ch = LP5012_I2C_CH_A; i2c_ch <= LP5012_I2C_CH_C; i2c_ch++)
    {
        //
        // Enable CH_A - EN0, EN1
        //
        GPIO_LP5012_multi_IC_enable(i2c_ch, EN_0_1_HIGH);
        for(addr_idx = 0; addr_idx < 4; addr_idx++)
        {
            LP5012_multi_ch_I2C_LEDx_ch_brightness(i2c_ch, LP5012_DEV0_ADDR + addr_idx, 0, 0, 0, 0);
        }
        GPIO_LP5012_multi_IC_enable(i2c_ch, EN_0_1_LOW);

    }
#endif
}


void fn_action_lower_on(uint8_t brightness)
{
    //
    // currents consumption test
    //
    for(i2c_ch = LP5012_I2C_CH_A; i2c_ch <= LP5012_I2C_CH_C; i2c_ch++)
    {
        //
        // Enable CH_A - EN0, EN1
        //
        GPIO_LP5012_multi_IC_enable(i2c_ch, EN_0_1_HIGH);

        GPIO_I2C_multi_Init(i2c_ch);
        // initialize the 1st panel(CH_A EN0)
        for(addr_idx = 1; addr_idx < 4; addr_idx +=2)
        {
            //LP5012_multi_ch_I2C_chip_reset(i2c_ch, LP5012_DEV0_ADDR+addr_idx, ENABLE);
            LP5012_multi_ch_I2C_chip_enable(i2c_ch, LP5012_DEV0_ADDR+addr_idx, ENABLE);
            //LP5012_multi_ch_I2C_IC_init(i2c_ch, LP5012_DEV0_ADDR+addr_idx, 0x00/*LP5012_LOG_SCALE_EN_BIT*/);
            LP5012_multi_ch_I2C_IC_init(i2c_ch, LP5012_DEV0_ADDR+addr_idx, LP5012_LOG_SCALE_EN_BIT);
        }

        for(addr_idx = 1; addr_idx < 4; addr_idx +=2)
        {
            // CH_A, EN_0, DEV0+addr_idx
            LP5012_multi_ch_I2C_LEDx_ch_brightness(i2c_ch, LP5012_DEV0_ADDR + addr_idx, group_brightness, group_brightness, group_brightness, group_brightness);
            for(led_ch=0; led_ch<MAX_LED; led_ch++)
            {
                LP5012_multi_ch_I2C_OUTx_color(i2c_ch, LP5012_DEV0_ADDR + addr_idx, led_ch, brightness);      // color level set from 0x80 to 0xc0
            }
        }
    }

}

void fn_action_upper_on(uint8_t brightness)
{
    //
    // currents consumption test
    //
    for(i2c_ch = LP5012_I2C_CH_A; i2c_ch <= LP5012_I2C_CH_C; i2c_ch++)
    {
        //
        // Enable CH_A - EN0, EN1
        //
        GPIO_LP5012_multi_IC_enable(i2c_ch, EN_0_1_HIGH);

        GPIO_I2C_multi_Init(i2c_ch);
        // initialize the 1st panel(CH_A EN0)
        for(addr_idx = 0; addr_idx < 4; addr_idx +=2)
        {
            //LP5012_multi_ch_I2C_chip_reset(i2c_ch, LP5012_DEV0_ADDR+addr_idx, ENABLE);
            LP5012_multi_ch_I2C_chip_enable(i2c_ch, LP5012_DEV0_ADDR+addr_idx, ENABLE);
            //LP5012_multi_ch_I2C_IC_init(i2c_ch, LP5012_DEV0_ADDR+addr_idx, 0x00/*LP5012_LOG_SCALE_EN_BIT*/);
            LP5012_multi_ch_I2C_IC_init(i2c_ch, LP5012_DEV0_ADDR+addr_idx, LP5012_LOG_SCALE_EN_BIT);
        }

        for(addr_idx = 0; addr_idx < 4; addr_idx +=2)
        {
            // CH_A, EN_0, DEV0+addr_idx
            LP5012_multi_ch_I2C_LEDx_ch_brightness(i2c_ch, LP5012_DEV0_ADDR + addr_idx, group_brightness, group_brightness, group_brightness, group_brightness);
            for(led_ch=0; led_ch<MAX_LED; led_ch++)
            {
                LP5012_multi_ch_I2C_OUTx_color(i2c_ch, LP5012_DEV0_ADDR + addr_idx, led_ch, brightness);      // color level set from 0x80 to 0xc0
            }
        }
    }

}

void fn_action_single_direction(uint8_t dir_idx, uint8_t brightness)
{
    uint8_t en0_en1;
    //
    // currents consumption test
    //
    switch(dir_idx)
    {
        case 0 :
        {
            i2c_ch = LP5012_I2C_CH_A;
            en0_en1 = EN_0_HIGH;
        }break;
        case 1 :
        {
            i2c_ch = LP5012_I2C_CH_A;
            en0_en1 = EN_1_HIGH;

        }break;
        case 2 :
        {
            i2c_ch = LP5012_I2C_CH_B;
            en0_en1 = EN_0_HIGH;

        }break;
        case 3 :
        {
            i2c_ch = LP5012_I2C_CH_B;
            en0_en1 = EN_1_HIGH;

        }break;
        case 4 :
        {
            i2c_ch = LP5012_I2C_CH_C;
            en0_en1 = EN_0_HIGH;

        }break;
        case 5 :
        {
            i2c_ch = LP5012_I2C_CH_C;
            en0_en1 = EN_1_HIGH;

        }break;
        case 6 :
        {
            i2c_ch = LP5012_I2C_CH_A;
            en0_en1 = EN_0_HIGH;

        }break;
        default :
        {
            i2c_ch = LP5012_I2C_CH_A;
            en0_en1 = EN_0_HIGH;

        }break;
    }

    //for(i2c_ch = LP5012_I2C_CH_A; i2c_ch <= LP5012_I2C_CH_C; i2c_ch++)
    {
        //
        // Enable CH_A - EN0, EN1
        //
        GPIO_LP5012_multi_IC_enable(i2c_ch, en0_en1);

        GPIO_I2C_multi_Init(i2c_ch);
        // initialize the 1st panel(CH_A EN0)
        for(addr_idx = 0; addr_idx < 4; addr_idx++)
        {
            //LP5012_multi_ch_I2C_chip_reset(i2c_ch, LP5012_DEV0_ADDR+addr_idx, ENABLE);
            LP5012_multi_ch_I2C_chip_enable(i2c_ch, LP5012_DEV0_ADDR+addr_idx, ENABLE);
            //LP5012_multi_ch_I2C_IC_init(i2c_ch, LP5012_DEV0_ADDR+addr_idx, 0x00/*LP5012_LOG_SCALE_EN_BIT*/);
            LP5012_multi_ch_I2C_IC_init(i2c_ch, LP5012_DEV0_ADDR+addr_idx, LP5012_LOG_SCALE_EN_BIT);
        }

        for(addr_idx = 0; addr_idx < 4; addr_idx++)
        {
            // CH_A, EN_0, DEV0+addr_idx
            LP5012_multi_ch_I2C_LEDx_ch_brightness(i2c_ch, LP5012_DEV0_ADDR + addr_idx, group_brightness, group_brightness, group_brightness, group_brightness);
            for(led_ch=0; led_ch<MAX_LED; led_ch++)
            {
                LP5012_multi_ch_I2C_OUTx_color(i2c_ch, LP5012_DEV0_ADDR + addr_idx, led_ch, brightness);      // color level set from 0x80 to 0xc0
            }
        }
    }

}

typedef struct {
    union {
        unsigned short word;
        struct {
            unsigned bit0 : 1;
            unsigned bit1 : 1;
            unsigned bit2 : 1;
            unsigned bit3 : 1;
            unsigned bit4 : 1;
            unsigned bit5 : 1;
            unsigned bit6 : 1;
            unsigned bit7 : 1;
            unsigned bit8 : 1;
            unsigned bit9 : 1;
            unsigned bit10 : 1;
            unsigned bit11 : 1;
            unsigned dummy : 4;
        }bits;
    };
}_tag_bits;

_tag_bits LP5012_disp_data_transform[6][4];



void fn_action_chars_display(uint8_t display_chars)
{

    for(cnt=0; cnt<6; cnt++)
    {
        LP5012_disp_data_transform[cnt][0].bits.bit0  = ( *(pt_dp_window_loc[cnt])   & BIT0);
        LP5012_disp_data_transform[cnt][0].bits.bit1  = ( *(pt_dp_window_loc[cnt]+1) & BIT0);
        LP5012_disp_data_transform[cnt][0].bits.bit2  = ( *(pt_dp_window_loc[cnt]+2) & BIT0);
        LP5012_disp_data_transform[cnt][0].bits.bit3  = ( *(pt_dp_window_loc[cnt])   & BIT1) >>1;
        LP5012_disp_data_transform[cnt][0].bits.bit4  = ( *(pt_dp_window_loc[cnt]+1) & BIT1) >>1;
        LP5012_disp_data_transform[cnt][0].bits.bit5  = ( *(pt_dp_window_loc[cnt]+2) & BIT1) >>1;
        LP5012_disp_data_transform[cnt][0].bits.bit6  = ( *(pt_dp_window_loc[cnt])   & BIT2) >>2;
        LP5012_disp_data_transform[cnt][0].bits.bit7  = ( *(pt_dp_window_loc[cnt]+1) & BIT2) >>2;
        LP5012_disp_data_transform[cnt][0].bits.bit8  = ( *(pt_dp_window_loc[cnt]+2) & BIT2) >>2;
        LP5012_disp_data_transform[cnt][0].bits.bit9  = ( *(pt_dp_window_loc[cnt])   & BIT3) >>3;
        LP5012_disp_data_transform[cnt][0].bits.bit10 = ( *(pt_dp_window_loc[cnt]+1) & BIT3) >>3;
        LP5012_disp_data_transform[cnt][0].bits.bit11 = ( *(pt_dp_window_loc[cnt]+2) & BIT3) >>3;

        LP5012_disp_data_transform[cnt][1].bits.bit0  = ( *(pt_dp_window_loc[cnt])   & BIT4) >>4;
        LP5012_disp_data_transform[cnt][1].bits.bit1  = ( *(pt_dp_window_loc[cnt]+1) & BIT4) >>4;
        LP5012_disp_data_transform[cnt][1].bits.bit2  = ( *(pt_dp_window_loc[cnt]+2) & BIT4) >>4;
        LP5012_disp_data_transform[cnt][1].bits.bit3  = ( *(pt_dp_window_loc[cnt])   & BIT5) >>5;
        LP5012_disp_data_transform[cnt][1].bits.bit4  = ( *(pt_dp_window_loc[cnt]+1) & BIT5) >>5;
        LP5012_disp_data_transform[cnt][1].bits.bit5  = ( *(pt_dp_window_loc[cnt]+2) & BIT5) >>5;
        LP5012_disp_data_transform[cnt][1].bits.bit6  = ( *(pt_dp_window_loc[cnt])   & BIT6) >>6;
        LP5012_disp_data_transform[cnt][1].bits.bit7  = ( *(pt_dp_window_loc[cnt]+1) & BIT6) >>6;
        LP5012_disp_data_transform[cnt][1].bits.bit8  = ( *(pt_dp_window_loc[cnt]+2) & BIT6) >>6;
        LP5012_disp_data_transform[cnt][1].bits.bit9  = ( *(pt_dp_window_loc[cnt])   & BIT7) >>7;
        LP5012_disp_data_transform[cnt][1].bits.bit10 = ( *(pt_dp_window_loc[cnt]+1) & BIT7) >>7;
        LP5012_disp_data_transform[cnt][1].bits.bit11 = ( *(pt_dp_window_loc[cnt]+2) & BIT7) >>7;

        LP5012_disp_data_transform[cnt][2].bits.bit0  = ( *(pt_dp_window_loc[cnt]+3) & BIT0) >>0;
        LP5012_disp_data_transform[cnt][2].bits.bit1  = ( *(pt_dp_window_loc[cnt]+4) & BIT0) >>0;
        LP5012_disp_data_transform[cnt][2].bits.bit2  = ( *(pt_dp_window_loc[cnt]+5) & BIT0) >>0;
        LP5012_disp_data_transform[cnt][2].bits.bit3  = ( *(pt_dp_window_loc[cnt]+3) & BIT1) >>1;
        LP5012_disp_data_transform[cnt][2].bits.bit4  = ( *(pt_dp_window_loc[cnt]+4) & BIT1) >>1;
        LP5012_disp_data_transform[cnt][2].bits.bit5  = ( *(pt_dp_window_loc[cnt]+5) & BIT1) >>1;
        LP5012_disp_data_transform[cnt][2].bits.bit6  = ( *(pt_dp_window_loc[cnt]+3) & BIT2) >>2;
        LP5012_disp_data_transform[cnt][2].bits.bit7  = ( *(pt_dp_window_loc[cnt]+4) & BIT2) >>2;
        LP5012_disp_data_transform[cnt][2].bits.bit8  = ( *(pt_dp_window_loc[cnt]+5) & BIT2) >>2;
        LP5012_disp_data_transform[cnt][2].bits.bit9  = ( *(pt_dp_window_loc[cnt]+3) & BIT3) >>3;
        LP5012_disp_data_transform[cnt][2].bits.bit10 = ( *(pt_dp_window_loc[cnt]+4) & BIT3) >>3;
        LP5012_disp_data_transform[cnt][2].bits.bit11 = ( *(pt_dp_window_loc[cnt]+5) & BIT3) >>3;

        LP5012_disp_data_transform[cnt][3].bits.bit0  = ( *(pt_dp_window_loc[cnt]+3) & BIT4) >>4;
        LP5012_disp_data_transform[cnt][3].bits.bit1  = ( *(pt_dp_window_loc[cnt]+4) & BIT4) >>4;
        LP5012_disp_data_transform[cnt][3].bits.bit2  = ( *(pt_dp_window_loc[cnt]+5) & BIT4) >>4;
        LP5012_disp_data_transform[cnt][3].bits.bit3  = ( *(pt_dp_window_loc[cnt]+3) & BIT5) >>5;
        LP5012_disp_data_transform[cnt][3].bits.bit4  = ( *(pt_dp_window_loc[cnt]+4) & BIT5) >>5;
        LP5012_disp_data_transform[cnt][3].bits.bit5  = ( *(pt_dp_window_loc[cnt]+5) & BIT5) >>5;
        LP5012_disp_data_transform[cnt][3].bits.bit6  = ( *(pt_dp_window_loc[cnt]+3) & BIT6) >>6;
        LP5012_disp_data_transform[cnt][3].bits.bit7  = ( *(pt_dp_window_loc[cnt]+4) & BIT6) >>6;
        LP5012_disp_data_transform[cnt][3].bits.bit8  = ( *(pt_dp_window_loc[cnt]+5) & BIT6) >>6;
        LP5012_disp_data_transform[cnt][3].bits.bit9  = ( *(pt_dp_window_loc[cnt]+3) & BIT7) >>7;
        LP5012_disp_data_transform[cnt][3].bits.bit10 = ( *(pt_dp_window_loc[cnt]+4) & BIT7) >>7;
        LP5012_disp_data_transform[cnt][3].bits.bit11 = ( *(pt_dp_window_loc[cnt]+5) & BIT7) >>7;
    }




    for(i2c_ch = LP5012_I2C_CH_A; i2c_ch <= LP5012_I2C_CH_C; i2c_ch++)
    {
        //
        // Init CH_A - EN0, EN1
        //
        GPIO_LP5012_multi_IC_enable(i2c_ch, EN_0_1_HIGH);

        GPIO_I2C_multi_Init(i2c_ch);
        // initialize the 1st panel(CH_A EN0)
        for(addr_idx = 0; addr_idx < 4; addr_idx++)
        {
            //LP5012_multi_ch_I2C_chip_reset(i2c_ch, LP5012_DEV0_ADDR+addr_idx, ENABLE);
            LP5012_multi_ch_I2C_chip_enable(i2c_ch, LP5012_DEV0_ADDR+addr_idx, ENABLE);
            //LP5012_multi_ch_I2C_IC_init(i2c_ch, LP5012_DEV0_ADDR+addr_idx, 0x00/*LP5012_LOG_SCALE_EN_BIT*/);
            LP5012_multi_ch_I2C_IC_init(i2c_ch, LP5012_DEV0_ADDR+addr_idx, LP5012_LOG_SCALE_EN_BIT);
        }

        //
        // Register Set : CH_A - EN0
        //
        for(addr_idx = 0; addr_idx < 4; addr_idx++)
        {
            // CH_A, EN_0, DEV0+addr_idx
            LP5012_multi_ch_I2C_LEDx_ch_brightness(i2c_ch, LP5012_DEV0_ADDR + addr_idx, group_brightness, group_brightness, group_brightness, group_brightness);
            for(led_ch=0; led_ch<MAX_LED; led_ch++)
            {
                if((LP5012_disp_data_transform[i2c_ch*2][addr_idx].word>>led_ch) & 0x0001 )
                    LP5012_multi_ch_I2C_OUTx_color(i2c_ch, LP5012_DEV0_ADDR + addr_idx, led_ch, brightness);      // color level set from 0x80 to 0xc0
                else
                    LP5012_multi_ch_I2C_OUTx_color(i2c_ch, LP5012_DEV0_ADDR + addr_idx, led_ch, 0x00);      // color level set from 0x80 to 0xc0
            }
        }
        // disable CHIP_EN register to make IC for STANDBY mode..
        LP5012_multi_ch_I2C_chip_enable(i2c_ch, LP5012_DEV0_ADDR+addr_idx, DISABLE);



        //
        // Register Set : CH_A - EN1
        //
        for(addr_idx = 0; addr_idx < 4; addr_idx++)
        {
            // CH_A, EN_0, DEV0+addr_idx
            LP5012_multi_ch_I2C_LEDx_ch_brightness(i2c_ch, LP5012_DEV0_ADDR + addr_idx, group_brightness, group_brightness, group_brightness, group_brightness);
            for(led_ch=0; led_ch<MAX_LED; led_ch++)
            {
                if((LP5012_disp_data_transform[i2c_ch*2+1][addr_idx].word>>led_ch) & 0x0001 )
                    LP5012_multi_ch_I2C_OUTx_color(i2c_ch, LP5012_DEV0_ADDR + addr_idx, led_ch, brightness);      // color level set from 0x80 to 0xc0
                else
                    LP5012_multi_ch_I2C_OUTx_color(i2c_ch, LP5012_DEV0_ADDR + addr_idx, led_ch, 0x00);      // color level set from 0x80 to 0xc0
            }
        }

        LP5012_multi_ch_I2C_chip_enable(i2c_ch, LP5012_DEV0_ADDR+addr_idx, ENABLE);


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

    rework_cnt++;
}


















