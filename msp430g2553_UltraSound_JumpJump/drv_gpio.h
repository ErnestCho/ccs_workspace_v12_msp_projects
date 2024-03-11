/*
 * drv_gpio.h
 *
 *  Created on: 2018. 7. 31.
 *      Author: yoch
 */

#ifndef DRV_GPIO_H_
#define DRV_GPIO_H_

#include <main_definition.h>
#include <msp430.h>

//////////////////////////////////////////////////////////
// port setting for UltraSound JumpJump HW rev.01
// P1.0 : Vbat mon (ADC)
// P1.3 : KEY0
// P1.4 : KEY1
// P1.6 : OLED SCL
// P1.7 : OLED SDA
// P2.0 : Ultra Sound TRIG
// P2.1 : Ultra Sound ECHO
// P2.6 : XIN (32.768kHz)
// P2.7 : XOUT (32.768kHz)
// other : N/C
//////////////////////////////////////////////////////////

#define ALL_BITS        (0xFF)
// Initialization of port register
// P1

#define P1OUT_INIT          (0)
#define P1DIR_INIT          (BIT3 | BIT4)   // KEY0,1            // 0: input, 1: output
#define P1SEL_INIT          (BIT6 | BIT7)   // I2C
#define P1SEL2_INIT         (BIT6 | BIT7)   // I2C
#define P1REN_INIT          (BIT6 | BIT7)   // need to check difference with external pull up.
// P2
#define P2OUT_INIT          (0)
#define P2DIR_INIT          ((BIT0|BIT1|BIT2) | BIT7)   // 0: input, 1: output      // BIT7 for XT1
#define P2SEL_INIT          (BIT6|BIT7)                                 // BIT6,7 for XT1
#define P2SEL2_INIT         (0)
#define P2REN_INIT          (0)
// P3
#define P3OUT_INIT          (0)
#define P3DIR_INIT          (0)   // 0: input, 1: output
#define P3SEL_INIT          (0)
#define P3SEL2_INIT         (0)
#define P3REN_INIT          (0)


// Bit set of output port.
//  ex) PORT_SET_BIT(P1, BIT0)
#define PORT_SET_BIT(out_port, bit_set)     (out_port##OUT |= bit_set)

// Bit clear of output port.
//  ex) PORT_CLR_BIT(P1, BIT0)
#define PORT_CLR_BIT(out_port, bit_clr)     (out_port##OUT &= ~bit_clr)

// Bit toggle of output port.
//  ex) PORT_TOG_BIT(P1, BIT0)
#define PORT_TOG_BIT(out_port, bit_tog)     (out_port##OUT ^= bit_tog)

// Bit sense of input port.
//  ex) if( PORT_SENS_BIT(P1, BIT3) )
#define PORT_SENS_BIT(inp_port, bit_sens)   (inp_port##IN & bit_sens)

#define PORT_DIR_OUTPUT(port, bit)         (port##DIR |= bit)

#define PORT_DIR_INPUT(port, bit)          (port##DIR &= ~bit)


inline void PORT_init(void);


#endif /* DRV_GPIO_H_ */
