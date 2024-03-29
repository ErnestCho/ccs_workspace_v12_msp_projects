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
// \version 1.83.00.05
// Released on May 15, 2020
//
//*****************************************************************************

#include <msp430.h>                      // Generic MSP430 Device Include
#include "driverlib.h"                   // MSPWare Driver Library
#include "captivate.h"                   // CapTIvate Touch Software Library
#include "CAPT_App.h"                    // CapTIvate Application Code
#include "CAPT_BSP.h"                    // CapTIvate EVM Board Support Package

#include "stdint.h"
#include "string.h"
#include "ssd1306.h"
#include <i2c.h>
#include <HDC1080_I2C_GPIO.h>

#include "adc.h"




#define LED_ACTIVE_PORT_OUT     P2OUT
#define LED_ACTIVE_PORT_DIR     P2DIR
#define LED_ACTIVE_PORT_PIN     BIT1

#define LED_LOW_BAT_PORT_OUT    P2OUT
#define LED_LOW_BAT_PORT_DIR    P2DIR
#define LED_LOW_BAT_PORT_PIN    BIT0

#define LED_ACTIVE_OFF()     (LED_ACTIVE_PORT_OUT |= LED_ACTIVE_PORT_PIN)
#define LED_ACTIVE_ON()    (LED_ACTIVE_PORT_OUT &= ~LED_ACTIVE_PORT_PIN)

#define LED_LOW_BAT_OFF()     (LED_LOW_BAT_PORT_OUT |= LED_LOW_BAT_PORT_PIN)
#define LED_LOW_BAT_ON()    (LED_LOW_BAT_PORT_OUT &= ~LED_LOW_BAT_PORT_PIN)


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

uint8_t timer_100ms_tick = 0;
// 1msec,  5msec, 10msec continuous timer
void timerA0_init(void)
{
    TA0CTL = TASSEL_2 + MC_2 + ID_2 + TACLR;                   // SMCLK, Cont. mode
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
    //timer.hour = timer.min = timer.sec = timer.msec = 0;

    TA0CTL = TASSEL_2 + MC_2 + + ID_2 + TACLR;                   // SMCLK, DIV4, cont. mode
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
#define SW1_BIT                 BIT2

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

}

typedef struct touch_bit_type {
    unsigned btn0_touch_on : 1;
    unsigned btn0_key_on : 1;
    unsigned change_occur : 1;
}_tag_touch_bit_type;

_tag_touch_bit_type touch_info = {
    .btn0_touch_on = 0,
    .btn0_key_on = 0,
    .change_occur = 0,
};

void touch_info_init(void)
{
    touch_info.btn0_touch_on = 0;
    touch_info.btn0_key_on = 0;
    touch_info.change_occur = 0;

}

#define TOUCH_PRESS_DETECT      1
#define TOUCH_RELEASE_DETECT    2

#define TOUCH_DETECT   TOUCH_RELEASE_DETECT

extern tElement BTN00_E00;

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

}

#define LPM_LEVEL       LPM1_bits

int16_t temperature_x10 = 0;    // uint16_t -> int16_t : consider (-) temperature
uint16_t humidity_x10 = 0;
char disp_buf[24];
char tmp_buf[6];
int16_t tmp_data;//[2];

// ADC Vbat checker
uint8_t g_vbat_decimal_x10 = 0;


CONST char str_Batt_low[] = "Batt Low : ";
CONST char str_____Vbat[] = " Vbat    : ";
CONST char str_dot[] = ".";
CONST char str_V[] = "V";
CONST char str_space[] = " ";
CONST char str__C[] = "'C";
CONST char str_percent_RH[] = " % RH";
CONST char str_Error_to_access[] = "Error to access";
CONST char str_HDC1080[] = "HDC1080..";
CONST char str_Always_on_mode[] = " Always on mode";
CONST char str_10sec_on_mode[] = " 10sec on mode";


void vbat_adc_check_display(void)
{
    // ADC battery check
    g_vbat_decimal_x10 = vbat_level_checker();
    //__delay_cycles(1000);


    ///////////////////////////
    // display battery status
    ///////////////////////////
    if(g_vbat_decimal_x10 <= 34)   // low battery check at Vbat <= 3.4V
    {
        strcpy(disp_buf, str_Batt_low);

        //timerA0_disable();
    }
    else
    {
        strcpy(disp_buf, str_____Vbat);
    }

#if 1
    tmp_data = g_vbat_decimal_x10;   // assume value is "xy" which indicate "x.y"V
#ifdef CCS_VER9
    ltoa(tmp_data, tmp_buf);         // converse integer to string
#else
    ltoa(tmp_data, tmp_buf, 10);         // converse integer to string
#endif
    strncat(disp_buf, tmp_buf, 1);      // copy 1byte string
    strncat(disp_buf, str_dot, 1);
    tmp_buf[0] = tmp_buf[1];
    strncat(disp_buf, tmp_buf, 1);      // copy 1byte string
    strncat(disp_buf, str_V, 1);
#else
    tmp_data[0] = g_vbat_decimal_x10 / 10;
    tmp_data[1] = g_vbat_decimal_x10 % 10;
    ltoa(tmp_data[0], tmp_buf);
    strcat(disp_buf, tmp_buf);
    strcat(disp_buf, ".");
    ltoa(tmp_data[1], tmp_buf);
    strcat(disp_buf, tmp_buf);
    strcat(disp_buf, "V");
#endif
    MAP_CAPT_disableISR(CAPT_TIMER_INTERRUPT);      // disable Captivate ISR during I2C OLED display writing. (to prevent text crack)
//    ssd1306_clearDisplay();
    ssd1306_clear_one_line(0);
    ssd1306_printString(0, 0, disp_buf, FONT_6x8);
    MAP_CAPT_enableISR(CAPT_TIMER_INTERRUPT);       // release disable Captivate ISR during I2C OLED display writing. (to prevent text crack)
}

void hdc1080_read_display(void)
{
    uint8_t num;
    if( HDC1080_I2C_check_device_valid(HDC1080_SLAVE_ADDR) )
    {
        HDC1080_I2C_read_temp_and_humidity(HDC1080_SLAVE_ADDR, &temperature_x10, &humidity_x10);

        ///////////////////////////
        // display current temperature
        ///////////////////////////
#if 1
        // text format : one of xx.x 'C , x.x 'C, -xx.x 'C, -x.x 'C
        tmp_data = temperature_x10;          // assume value is "xyz" which indicate "xy.z" celsius
#ifdef CCS_VER9
        ltoa(tmp_data, tmp_buf);                 // converse integer to string
#else
        ltoa(tmp_data, tmp_buf, 10);                 // converse integer to string
#endif

        if(tmp_data <= -100)             // means '-xxx' -> '-xx.x'
        {
            num = 3;
        }
        else if(tmp_data <= -10)         // means '-xx' -> '-x.x'
        {
            num = 2;
        }
        else if(tmp_data < 0)            // means '-x' -> '-0.x'
        {
            num = 2;

            tmp_buf[2] = tmp_buf[1];
            tmp_buf[1] = '0';
            //tmp_buf[0] = '-';
        }
        else if(tmp_data < 10)           // means 'x' -> '0.x'
        {
            num = 1;

            tmp_buf[1] = tmp_buf[0];
            tmp_buf[0] = '0';
        }
        else if(tmp_data < 100)          // means 'xx' -> 'x.x'
        {
            num = 1;
        }
        else if(tmp_data < 1000)         // means 'xxx' -> 'xx.x'
        {
            num = 2;
        }
        else                                // means 'xxxx' -> 'xxx.x'
        {
            num = 3;
        }

        strcpy(disp_buf, str_space);                // trick of using strncat() from the next
        strncat(disp_buf, tmp_buf,num);             //
        strncat(disp_buf, str_dot, 1);
        tmp_buf[0] = tmp_buf[num];
        strncat(disp_buf, tmp_buf, 1);              // copy 1byte string
        strncat(disp_buf, str__C, 2);
#else
        tmp_data[0] = temperature_x10 / 10;
        tmp_data[1] = temperature_x10 % 10;

        // text format : 00.0 'C
        ltoa(tmp_data[0], tmp_buf);
        strcpy(disp_buf, tmp_buf);
        strncat(disp_buf, ".", 1);
        ltoa(tmp_data[1], tmp_buf);
        strncat(disp_buf, tmp_buf, 1);
        strncat(disp_buf, " 'C", 2);
#endif
        MAP_CAPT_disableISR(CAPT_TIMER_INTERRUPT);      // disable Captivate ISR during I2C OLED display writing. (to prevent text crack)
        ssd1306_clear_one_line(2);
        ssd1306_clear_one_line(3);
        ssd1306_printString(10, 2, disp_buf, FONT_MS_GOTHIC8x16);
        MAP_CAPT_enableISR(CAPT_TIMER_INTERRUPT);       // release disable Captivate ISR during I2C OLED display writing. (to prevent text crack)

        ///////////////////////////
        // display current humidity
        ///////////////////////////
#if 1
        // text format : xx.x % RH
        tmp_data = humidity_x10;          // assume value is "xyz" which indicate "xy.z" % RH
#ifdef CCS_VER9
        ltoa(tmp_data, tmp_buf);             // converse integer to string
#else
        ltoa(tmp_data, tmp_buf, 10);             // converse integer to string
#endif

        if(tmp_data < 0)
        {
            tmp_data = 0;
        }
        if(tmp_data < 10)       // means 'x' -> '0.x'
        {
            num = 1;

            tmp_buf[1] = tmp_buf[0];
            tmp_buf[0] = '0';
        }
        else if(tmp_data < 100)      // means 'xx' -> 'x.x'
        {
            num = 1;
        }
        else if(tmp_data < 1000)     // means 'xxx' -> 'xx.x'
        {
            num = 2;
        }
        else                            // means 'xxxx' -> 'xxx.x'
        {
            num = 3;
        }

        strcpy(disp_buf, str_space);
        strncat(disp_buf, tmp_buf, num);          //
        strncat(disp_buf, str_dot, 1);
        tmp_buf[0] = tmp_buf[num];
        strncat(disp_buf, tmp_buf, 1);          // copy 1byte string
        strncat(disp_buf, str_percent_RH, 5);
#else
        tmp_data[0] = humidity_x10 / 10;
        tmp_data[1] = humidity_x10 % 10;

        // text format : 00.0 % RH
        ltoa(tmp_data[0], tmp_buf);
        strcpy(disp_buf, tmp_buf);
        strncat(disp_buf, ".", 1);
        ltoa(tmp_data[1], tmp_buf);
        strncat(disp_buf, tmp_buf, 1);
        strncat(disp_buf, " % RH", 5);
#endif
        MAP_CAPT_disableISR(CAPT_TIMER_INTERRUPT);      // disable Captivate ISR during I2C OLED display writing. (to prevent text crack)
        ssd1306_clear_one_line(6);
        ssd1306_clear_one_line(5);
        ssd1306_printString(10, 5, disp_buf, FONT_MS_GOTHIC8x16);
        MAP_CAPT_enableISR(CAPT_TIMER_INTERRUPT);       // release disable Captivate ISR during I2C OLED display writing. (to prevent text crack)
    }
    else
    {
        ///////////////////////////
        // display reading error of HDC1080
        ///////////////////////////
        MAP_CAPT_disableISR(CAPT_TIMER_INTERRUPT);      // disable Captivate ISR during I2C OLED display writing. (to prevent text crack)
        ssd1306_clear_one_line(2);
        ssd1306_clear_one_line(3);
        ssd1306_clear_one_line(5);
        ssd1306_clear_one_line(6);
        ssd1306_printString(0, 3, (char*)str_Error_to_access, FONT_6x8);
        ssd1306_printString(0, 5, (char*)str_HDC1080, FONT_6x8);
        MAP_CAPT_enableISR(CAPT_TIMER_INTERRUPT);       // release disable Captivate ISR during I2C OLED display writing. (to prevent text crack)

    }
}












uint8_t active_flag = 0;
uint8_t menu_mode = 1;          // select default : 0: active mode when touch pressed, 1: always active mode

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

	LED_ACTIVE_PORT_DIR |= (LED_ACTIVE_PORT_PIN + LED_LOW_BAT_PORT_PIN);
	LED_ACTIVE_OFF();
	LED_LOW_BAT_OFF();

	// Mechanical switch
    GPIO_SW1_SW2_Interrupt_Set();

    // Disable the GPIO power-on default high-impedance mode
    // to activate previously configured port settings
     PM5CTL0 &= ~LOCKLPM5;
    __delay_cycles(10000);

    // OLED initialization
    i2c_init();                                 // initialize I2C to use with OLED

    // HDC1080 init
    GPIO_I2C_Init();
    __delay_cycles(500000);

    ssd1306_init();                             // Initialize SSD1306 OLED
    ssd1306_clearDisplay();                     // Clear OLED display
    delay_ms(1);

    // Timer
    timerA0_init();

    // ADC

    adc_init_interrupt();

    __bis_SR_register(GIE);

    //
    // Register button call back function
    //
    //MAP_CAPT_registerCallback(&BTN00, &ButtonEventHandler);
    MAP_CAPT_registerCallback(&BTN00, &fasterButtonEventHandler);

	//
	// Start the CapTIvate application
	//
	CAPT_appStart();

    active_flag = 1;

	//
	// Background Loop
	//
	while(1)
	{
		// Run the captivate application handler.
		CAPT_appHandler();
	    // check touch wake on
		if(touch_info.change_occur == 1)
		{
		    touch_info.change_occur = 0;
	        if(touch_info.btn0_touch_on == 1)
	        {
	            active_flag = 1;
	            timer_100ms_tick = 0;
                LED_ACTIVE_ON();
                ssd1306_wake_up();
                ssd1306_clearDisplay();
	        }
		}
		if(switch_status == BIT0)
		{
		    switch_status = 0; // &= ~BIT0;

		    if(menu_mode == 0)
		    {
		        menu_mode = 1;
		    }
		    else
		    {
		        menu_mode = 0;
		    }
            active_flag = 1;
            timer_100ms_tick = 0;
            LED_ACTIVE_ON();
            ssd1306_wake_up();
            ssd1306_clearDisplay();
		}

		if(menu_mode == 0)  // activate when touch button pressed
		{
	        // Touch activate -> turn on the display for 10sec with update temp & humid
	        if(active_flag == 1 )
	        {
	            // check low battery
	            if( g_vbat_decimal_x10 <= 34 )
	            {
	                LED_LOW_BAT_ON();
	            }
	            else
	            {
	                LED_LOW_BAT_OFF();
	            }

	            if(timer_100ms_tick % 10 == 1)
	            {
	                if(timer_100ms_tick < 20)
	                {
	                    ssd1306_printString(0, 7, str_10sec_on_mode, FONT_6x8);
	                }
	                else
	                {
	                    ssd1306_clear_one_line(7);
	                }

	                if(timer_100ms_tick < 50)       // 5sec
	                {
	                    vbat_adc_check_display();
	                    __delay_cycles(1000);

	                    hdc1080_read_display();
	                }
	            }
	            if( timer_100ms_tick == 5)          // 500msec
	            {
	                LED_ACTIVE_OFF();
	            }

	            if( timer_100ms_tick >= 70)
	            {
	                if( timer_100ms_tick % 10 == 1)
	                {
	                    LED_ACTIVE_ON();
	                }
	                if( timer_100ms_tick % 10 == 6)
	                {
	                    LED_ACTIVE_OFF();
	                }
	            }

	            if( timer_100ms_tick >= 100)        // 10sec
	            {
	                active_flag = 0;
	                ssd1306_get_sleep();
	                LED_ACTIVE_OFF();
	                LED_LOW_BAT_OFF();
	            }

	        }
		}
		else if(menu_mode == 1) // always on and checking the humidity & temperature
		{
            // check low battery
            if( g_vbat_decimal_x10 <= 34 )
            {
                LED_LOW_BAT_ON();
            }
            else
            {
                LED_LOW_BAT_OFF();
            }

            if(timer_100ms_tick % 10 == 1)
            {
                if(timer_100ms_tick < 20)
                {
                    ssd1306_printString(0, 7, str_Always_on_mode, FONT_6x8);
                }
                else
                {
                    ssd1306_clear_one_line(7);
                    LED_ACTIVE_OFF();
                }

                    vbat_adc_check_display();
                    __delay_cycles(1000);

                    hdc1080_read_display();
            }

		}



#if 0
        vbat_adc_check_display();
        __delay_cycles(10000);

        hdc1080_read_display();

        //__bis_SR_register(LPM_LEVEL + GIE);       // Enter LPM4 w/interrupt

        //delay_ms(1500);                              // Avoid delay loops in real code, use timer
#endif

		//
		// This is a great place to add in any 
		// background application code.
		//
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

}


/******************************************************************************/
// Timer0_A0 Interrupt Service Routine: Disables the timer and exists LPM3
/******************************************************************************/


// 100ms continuous mode timer
#pragma vector=TIMER0_A0_VECTOR
__interrupt void ISR_Timer0_A0(void)
{
    TA0CCR0 += CONST_TIMER0_CCR0;               // Add Offset to CCR1

    timer_100ms_tick++;
#if 0
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
#endif
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















