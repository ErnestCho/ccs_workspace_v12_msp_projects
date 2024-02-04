/*
 * drv_gpio.c
 *
 *  Created on: 2018. 7. 31.
 *      Author: yoch
 */

#include <drv_gpio.h>

// Port initialization is differ from design to design...
// And driver-library-typed coding is not good for memory..
// So register direct driven code is best for MSP430G2xx..

// PORT related register description
//
// PxIN : Port Input value
// PxOUT : Port Output value
// PxDIR : Port Direction setting (0 : Input, 1 : Output)
// PxSEL, PxSEL2 : Port Pin I/O function setting.. --> see device-specific datasheet
// PxREN : Port internal pull up/down resister setting
//          (0 : disabled, 1 : Pull up(when PxOUT corresponding bit is 1) or Pull down (PxOUT is 0)
/////////////
inline void PORT_init(void)
{
    P1OUT = P1OUT_INIT;
    P1DIR = P1DIR_INIT;
    P1SEL = P1SEL_INIT;
    P1SEL2 = P1SEL2_INIT;
    P1REN = P1REN_INIT;

    P2OUT = P2OUT_INIT;
    P2DIR = P2DIR_INIT;
    P2SEL = P2SEL_INIT;
    P2SEL2 = P2SEL2_INIT;
    P2REN = P2REN_INIT;

#if 0
    P3OUT = P3OUT_INIT;
    P3DIR = P3DIR_INIT;
    P3SEL = P3SEL_INIT;
    P3SEL2 = P3SEL2_INIT;
    P3REN = P3REN_INIT;
#endif
}

