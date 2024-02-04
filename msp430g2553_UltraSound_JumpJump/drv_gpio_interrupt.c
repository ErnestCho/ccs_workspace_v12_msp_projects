/*
 * drv_gpio_interrupt.c
 *
 *  Created on: 2018. 7. 31.
 *      Author: yoch
 */

#include <drv_gpio_interrupt.h>

uint8_t g_port_int_flag = 0;

inline void PORT1_interrupt_init(uint8_t bits)
{
    P1OUT |= bits;                            // P1.x set, else reset
    P1REN |= bits;                            // P1.x pullup
    P1IE |= bits;                             // P1.x interrupt enabled
    P1IES |= bits;                            // P1.x Hi/lo edge
    P1IFG &= ~bits;                           // P1.x IFG cleared
}
inline void PORT2_interrupt_init(uint8_t bits)
{
    P2OUT |= bits;                            // P2.x set, else reset
    P2REN |= bits;                            // P2.x pullup
    P2IE |= bits;                             // P2.x interrupt enabled
    P2IES |= bits;                            // P2.x Hi/lo edge
    P2IFG &= ~bits;                           // P2.x IFG cleared
}

// GPIO_P1InterruptSet() : external interrupt setup for P1
// param : bits - multi-bits setup
void GPIO_SW1SW2_Interrupt_Set(void)
{
    SW1_PORT_DIR &= ~SW1_BIT;                         // port as Input
    SW1_PORT_OUT |= SW1_BIT;                          // Configure P1 as pulled-up
    SW1_PORT_REN |= SW1_BIT;                          // P1 pull-up register enable
    SW1_PORT_IES &= ~SW1_BIT;                         // P1 edge, 0: low to high edge
    SW1_PORT_IE  |= SW1_BIT;                          // P1 interrupt enabled
    SW1_PORT_IFG &= ~SW1_BIT;                         // P1 IFG cleared

    SW2_PORT_DIR &= ~SW2_BIT;                         // port as Input
    SW2_PORT_OUT |= SW2_BIT;                          // Configure P1 as pulled-up
    SW2_PORT_REN |= SW2_BIT;                          // P1 pull-up register enable
    SW2_PORT_IES &= ~SW2_BIT;                         // P1 edge, 0: low to high edge
    SW2_PORT_IE  |= SW2_BIT;                          // P1 interrupt enabled
    SW2_PORT_IFG &= ~SW2_BIT;                         // P1 IFG cleared

}

extern uint8_t gf_start_trigger;
// Port 1 interrupt service routine
#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void)
{
    // switch 1 detection ==> sw1 function : mode change
    if(SW1_PORT_IFG & SW1_BIT)
    {
        SW1_PORT_IFG &= ~SW1_BIT;                         // Clear PORT IFG for SW1

        gf_start_trigger = 1;
    }

    if(SW2_PORT_IFG & SW2_BIT)
    {
        SW2_PORT_IFG &= ~SW2_BIT;                         // Clear PORT IFG for SW1

        //gf_start_trigger = 1;
    }

}
