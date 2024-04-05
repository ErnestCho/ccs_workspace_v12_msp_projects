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
#include "CAPT_UserConfig.h"
#include <LP5012.h>
#include <common.h>

#define DEMO_WHEEL_BTN  (0)
#define DEMO_0_0_BTN    (1)
#define DEMO_0_1_BTN    (2)
#define DEMO_0_2_BTN    (3)
#define DEMO_0_3_BTN    (4)
#define DEMO_0_4_BTN    (5)
#define DEMO_1_0_BTN    (6)
#define DEMO_1_1_BTN    (7)
#define DEMO_1_2_BTN    (8)
#define DEMO_1_3_BTN    (9)
#define DEMO_1_4_BTN    (10)
#define DEMO_2_0_BTN    (11)
#define DEMO_2_1_BTN    (12)
#define DEMO_2_2_BTN    (13)
#define DEMO_2_3_BTN    (14)
#define DEMO_2_4_BTN    (15)

void Capt_keyButtonHandler(tSensor* pSensor)
{
    // If the sensor has a new touch, process it based on the key
    if ((pSensor->bSensorTouch == true)
            && (pSensor->bSensorPrevTouch == false))
    {
        // Identify the dominant element in the sensor.
        // Of the four buttons, only one is allowed to be touched at a time,
        // and the button with the strongest response is selected.
        switch (CAPT_getDominantButton(pSensor))
        {
            // If the mute button was pressed, fire a double click.
            case DEMO_WHEEL_BTN:
                break;

            case DEMO_0_0_BTN:
                break;
            case DEMO_0_1_BTN:
                break;
            case DEMO_0_2_BTN:
                break;
            case DEMO_0_3_BTN:
                break;
            case DEMO_0_4_BTN:
                break;

            case DEMO_1_0_BTN:
                break;
            case DEMO_1_1_BTN:
                break;
            case DEMO_1_2_BTN:
                break;
            case DEMO_1_3_BTN:
                break;
            case DEMO_1_4_BTN:
                break;

            case DEMO_2_0_BTN:
                break;
            case DEMO_2_1_BTN:
                break;
            case DEMO_2_2_BTN:
                break;
            case DEMO_2_3_BTN:
                break;
            case DEMO_2_4_BTN:
                break;

            default:
                break;
        }
    }
}
uint16_t slider_position[4] = {0,0,0,0};
void Capt_slider1_Handler(tSensor* pSensor)
{
    // If the sensor has a new touch, fire a "sharp pulsing" effect.
    if ((pSensor->bSensorTouch == true) && (pSensor->bSensorPrevTouch == false))
    {
        slider_position[0] = CAPT_getSensorPosition (pSensor);
    }
}
void Capt_slider2_Handler(tSensor* pSensor)
{
    // If the sensor has a new touch, fire a "sharp pulsing" effect.
    if ((pSensor->bSensorTouch == true) && (pSensor->bSensorPrevTouch == false))
    {
        slider_position[1] = CAPT_getSensorPosition (pSensor);
    }

}
void Capt_slider3_Handler(tSensor* pSensor)
{
    // If the sensor has a new touch, fire a "sharp pulsing" effect.
    if ((pSensor->bSensorTouch == true) && (pSensor->bSensorPrevTouch == false))
    {
        slider_position[2] = CAPT_getSensorPosition (pSensor);
    }

}
void Capt_slider4_Handler(tSensor* pSensor)
{
    // If the sensor has a new touch, fire a "sharp pulsing" effect.
    if ((pSensor->bSensorTouch == true) && (pSensor->bSensorPrevTouch == false))
    {
        slider_position[3] = CAPT_getSensorPosition (pSensor);
    }

}
void Capt_Wheel_Handler(tSensor* pSensor)
{
    // If the sensor has a new touch, fire a "sharp pulsing" effect.
    if ((pSensor->bSensorTouch == true) && (pSensor->bSensorPrevTouch == false))
    {
        ;
    }

}

void device_init(void)
{
    SYSCFG3 |= USCIB1RMP; //Re-map P4.3 and P4.4 as I2C


    //
    // Associate the capacitive touch callback handlers with sensors.
    //
    MAP_CAPT_registerCallback( &BTN00,  &Capt_keyButtonHandler);
    MAP_CAPT_registerCallback( &SLD00,  &Capt_slider1_Handler);
    MAP_CAPT_registerCallback( &SLD01,  &Capt_slider2_Handler);
    MAP_CAPT_registerCallback( &SLD02,  &Capt_slider3_Handler);
    MAP_CAPT_registerCallback( &SLD03,  &Capt_slider4_Handler);
    MAP_CAPT_registerCallback( &WHL00,  &Capt_Wheel_Handler);
}

#define MAX_LED                     12      // number of LED in the board
#define LED_OUT_X_LOW_LEVEL         25      // brightness level : low
#define LED_OUT_X_MID_LEVEL         25      // brightness level : mid
#define LED_OUT_X_MAX_LEVEL         25      // brightness level : high

uint8_t led_ch;

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

#if 1       // EN pin is hard wired pulled up
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
    LP5012_I2C_chip_reset(LP5012_DEV0_ADDR, 1);
    delay_ms(100);

    LP5012_I2C_chip_enable(LP5012_DEV0_ADDR, ENABLE);

    delay_ms(100);

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

    PM5CTL0 &= ~LOCKLPM5;
   __delay_cycles(10000);

	__bis_SR_register(GIE);

	device_init();

	//
	// Start the CapTIvate application
	//
	CAPT_appStart();

    GPIO_LP5012_IC_enable(1);             // EN pin is hard wired pulled up at this solution
    GPIO_I2C_Init();
    FR2676_LP5012_board_init();

    //i2c_write_byte(LP5012_DEV0_ADDR, LP5012_DEVICE_CONFIG1, BIT5);       // set config1 register
    //[to save mem]i2c_write_byte(LP5012_DEV0_ADDR, LP5012_DEVICE_CONFIG1, 0x00);       // set config1 register
    i2c_write_byte(LP5012_DEV0_ADDR, LP5012_DEVICE_CONFIG1, 0x00);
    //LP5012_I2C_chip_enable(LP5012_DEV0_ADDR, 1);

    // seperate color control
    for(led_ch=0; led_ch<MAX_LED; led_ch++)
    {
        LP5012_I2C_OUTx_color(LP5012_DEV0_ADDR, led_ch, LED_OUT_X_MID_LEVEL);      // color level set from 0x80 to 0xc0
        delay_ms(50);
    }
    for(led_ch=0; led_ch<MAX_LED; led_ch++)
    {
        LP5012_I2C_OUTx_color(LP5012_DEV0_ADDR, led_ch, 0x00);      // color level set from 0x80 to 0xc0
        delay_ms(50);
    }

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

		//
		// End of background loop iteration
		// Go to sleep if there is nothing left to do
		//
		CAPT_appSleep();
		
	} // End background loop
} // End main()
