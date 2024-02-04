/*
 * drv_gpio_interrupt.h
 *
 *  Created on: 2018. 7. 31.
 *      Author: yoch
 */

#ifndef DRV_GPIO_INTERRUPT_H_
#define DRV_GPIO_INTERRUPT_H_

#include <msp430.h>
#include <type_define.h>

// SW1 = P1.3
#define SW1_PORT_DIR            P1DIR
#define SW1_PORT_OUT            P1OUT
#define SW1_PORT_REN            P1REN
#define SW1_PORT_IES            P1IES
#define SW1_PORT_IE             P1IE
#define SW1_PORT_IFG            P1IFG
#define SW1_BIT                 BIT3

// SW2 = P1.4
#define SW2_PORT_DIR            P1DIR
#define SW2_PORT_OUT            P1OUT
#define SW2_PORT_REN            P1REN
#define SW2_PORT_IES            P1IES
#define SW2_PORT_IE             P1IE
#define SW2_PORT_IFG            P1IFG
#define SW2_BIT                 BIT4


#define PORT_IFG_CLEAR(port, bit)     (port##IFG &= ~bit)


extern uint8_t g_port_int_flag;

inline void PORT1_interrupt_init(uint8_t bits);
inline void PORT2_interrupt_init(uint8_t bits);
void GPIO_SW1SW2_Interrupt_Set(void);


#endif /* DRV_GPIO_INTERRUPT_H_ */
