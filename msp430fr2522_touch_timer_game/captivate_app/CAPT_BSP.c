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
// CAPT_BSP.c
//
// This board support package supports the MSP430FR2522 device.
// 
// *** CLOCK CONFIGURATION ***
// MCLK is 8 MHz, sourced from the DCO
// SMCLK is 2 MHz, sourced from MCLK
// ACLK is 32kHz, sourced from REFO
// FLLREF is 32kHz, sourced from REFO
// NOTE: ACLK and FLLREF can be sourced from XT1, but this will displace
// the UCA0 from P2.0 and P2.1.  UCA0 can also be muxed to P1.4 and P1.5,
// but this requires giving up two CapTIvate pins.
//
// *** SERIAL INTERFACES ***
// UCA0 is muxed in UART mode to P2.0 and P2.1 (secondary mapping)
// UCB0 is muxed in I2C mode to P2.5 and P2.6 (secondary mapping)
//
// \version 1.83.00.05
// Released on May 15, 2020
//
//*****************************************************************************

#include <msp430.h>
#include <stdint.h>

#include "CAPT_BSP.h"

//*****************************************************************************
//
//! This function is configures the MCU Digital IO and CS for operation.
//
//*****************************************************************************
void BSP_configureMCU(void)
{
    uint8_t ui8LFOsc = DEFAULT_OSC_SELECTION;

#if (DEFAULT_OSC_SELECTION == CS_REFOCLK_SELECT)
    //
    // Re-map EUSCI A0 pins to secondary locations
    // This frees up CapTIvate IOs.
    // If XT1 is used (DEFAULT_OSC_SELECTION == CS_XT1CLK_SELECT),
    // Do not remap EUSCI A0, as XT1 needs those pins.
    // Keep the original muxing in that case.
    //
    //SYSCFG3 |= USCIARMP;
#endif

    //
    // Re-map EUSCI B0 pins to secondary locations
    // This frees up CapTIvate IOs
    //
    //SYSCFG2 |= USCIBRMP;

    // P1.0: OUTPUT LOW
    // P1.1: OUTPUT LOW
    // P1.2: OUTPUT LOW
    // P1.3: OUTPUT LOW
    // P1.4: UCA0 UART TXD if (DEFAULT_OSC_SELECTION == CS_XT1CLK_SELECT), else OUTPUT LOW
    // P1.5: UCA0 UART RXD if (DEFAULT_OSC_SELECTION == CS_XT1CLK_SELECT), else OUTPUT LOW
    // P1.6: OUTPUT LOW
    // P1.7: OUTPUT LOW
#if (DEFAULT_OSC_SELECTION == CS_XT1CLK_SELECT)
    P1OUT  = (0);
    P1DIR  = (GPIO_PIN0 | GPIO_PIN1 | GPIO_PIN2 | GPIO_PIN3 | GPIO_PIN6 | GPIO_PIN7);
    P1SEL0 = (GPIO_PIN4 | GPIO_PIN5);
    P1SEL1 = (0);
#else
    P1OUT  = (0);
    P1DIR  = (GPIO_PIN0 | GPIO_PIN1 | GPIO_PIN2 | GPIO_PIN3 | GPIO_PIN4 | GPIO_PIN5 | GPIO_PIN6 | GPIO_PIN7);
    P1SEL0 = GPIO_PIN2 + GPIO_PIN3;
    P1SEL1 = (0);
#endif

    // P2.0: UCA0 UART TXD if (DEFAULT_OSC_SELECTION == CS_REFOCLK_SELECT), else XOUT
    // P2.1: UCA0 UART RXD if (DEFAULT_OSC_SELECTION == CS_REFOCLK_SELECT), else XIN
    // P2.2: SYNC (SET OUTPUT LOW UNLESS USED)
    // P2.3: OUTPUT LOW
    // P2.4: IRQ (OPEN DRAIN), set high to start
    // P2.5: UCB0 I2C SDA
    // P2.6: UCB0 I2C SCL
#if (DEFAULT_OSC_SELECTION == CS_XT1CLK_SELECT)
    P2OUT  =  (GPIO_PIN4);
    P2DIR  =  (GPIO_PIN2 | GPIO_PIN3 | GPIO_PIN4);
    P2SEL0 =  (0);
    P2SEL1 =  (GPIO_PIN0 | GPIO_PIN1 | GPIO_PIN5 | GPIO_PIN6);
#else
    P2OUT  =  (0);
    P2DIR  =  (0);//(GPIO_PIN0 | GPIO_PIN1 | GPIO_PIN2);
    P2SEL0 =  (0);//(GPIO_PIN0 | GPIO_PIN1);              // PIN0,1 as debug UART, remove if you finish captivate debugging
    P2SEL1 =  (0);//(GPIO_PIN5 | GPIO_PIN6);
#endif

    //
    // Clear port lock
    //
    PM5CTL0 &= ~LOCKLPM5;
	
	// Configure FRAM wait state (set to 1 to support 16MHz MCLK)
	MAP_FRAMCtl_configureWaitStateControl(FRAMCTL_ACCESS_TIME_CYCLES_1);

#if (DEFAULT_OSC_SELECTION == CS_XT1CLK_SELECT)
    //
    // Attempt to start the low frequency crystal oscillator
    //
    MAP_CS_setExternalClockSource(XT1_OSC_FREQ);
    if (CS_turnOnXT1LFWithTimeout(CS_XT1_DRIVE_0, XT1_OSC_TIMEOUT) == STATUS_FAIL)
    {
        //
        // If a crystal is not present or is failing, switch the LF
        // clock definition to the internal 32kHz reference oscillator.
        //
        ui8LFOsc = CS_REFOCLK_SELECT;
    }
#endif

    //
    // Initialize Clock Signals
    //
    MAP_CS_initClockSignal(CS_FLLREF, ui8LFOsc, CS_CLOCK_DIVIDER_1);
    MAP_CS_initClockSignal(CS_ACLK, ui8LFOsc, CS_CLOCK_DIVIDER_1);
    MAP_CS_initClockSignal(CS_MCLK, CS_DCOCLKDIV_SELECT, CS_CLOCK_DIVIDER_1);
    MAP_CS_initClockSignal(CS_SMCLK, CS_DCOCLKDIV_SELECT, CS_CLOCK_DIVIDER_8);

    //
    // Tune the DCO parameters
    //
    MAP_CS_initFLL((MCLK_FREQ/1000), FLL_RATIO);
    MAP_CS_clearAllOscFlagsWithTimeout(1000);
}

//*****************************************************************************
//
//! This function disables the watchdog timer during boot, ensuring a WDT
//! reset does not occur during boot before main() is entered.
//
//*****************************************************************************
int _system_pre_init(void)
{
    WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer
    return 1;
}
