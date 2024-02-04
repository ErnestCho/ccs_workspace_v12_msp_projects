/*
 * tmp431.h
 *
 *  Created on: 2021. 3. 18.
 *      Author: yoch
 */

#ifndef TMP431_H_
#define TMP431_H_

#include <stdint.h>

#define TMP431_LOCAL_IC             0
#define TMP431_REMOTE_IC            1

#define TMP431C_I2C_SLAVE_ADDR_RD   0x99        // 0x4C<<1 + 1(RD)
#define TMP431C_I2C_SLAVE_ADDR_WR   0x98        // 0x4C<<1 + 0(WR)

// (Local_SDA = P2.3), (Local_SCL = P2.2)
#define LOCAL_SDA_1             P2OUT |=  BIT3              //SDA = 1
#define LOCAL_SDA_0             P2OUT &=~ BIT3              //SDA = 0
#define LOCAL_SDA_DIR_IN        P2DIR &=~ BIT3              //SDA Input
#define LOCAL_SDA_DIR_OUT       P2DIR |=  BIT3              //SDA Output
#define LOCAL_SDA_READ          ((P2IN >> 3) & 0x01)        //Read SDA
#define LOCAL_SCL_1             P2OUT |=  BIT2              //SCL = 1
#define LOCAL_SCL_0             P2OUT &=~ BIT2              //SCL = 0
#define LOCAL_SCL_DIR_OUT       P2DIR |=  BIT2              //SCL Output
#define LOCAL_SDA_PU            P2REN |=  BIT3
#define LOCAL_SCL_PU            P2REN |=  BIT2

// (Remote_SDA = P2.1), (Remote_SCL = P2.0)
#define REMOTE_SDA_1            P2OUT |=  BIT1              //SDA = 1
#define REMOTE_SDA_0            P2OUT &=~ BIT1              //SDA = 0
#define REMOTE_SDA_DIR_IN       P2DIR &=~ BIT1              //SDA Input
#define REMOTE_SDA_DIR_OUT      P2DIR |=  BIT1              //SDA Output
#define REMOTE_SDA_READ         ((P2IN >> 1) & 0x01)        //Read SDA
#define REMOTE_SCL_1            P2OUT |=  BIT0              //SCL = 1
#define REMOTE_SCL_0            P2OUT &=~ BIT0              //SCL = 0
#define REMOTE_SCL_DIR_OUT      P2DIR |=  BIT0               //SCL Output
#define REMOTE_SDA_PU           P2REN |=  BIT1
#define REMOTE_SCL_PU           P2REN |=  BIT0


extern void GPIO_I2C_Init(void);

extern short tmp431_i2c_read_local_temperature(uint8_t local_remote);
extern void tmp431_i2c_first_dummy_read(uint8_t local_remote);
extern uint8_t tmp431_i2c_set_extended_mode(uint8_t local_remote);


#endif /* TMP431_H_ */
