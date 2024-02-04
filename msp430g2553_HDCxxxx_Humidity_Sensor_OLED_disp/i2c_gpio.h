#ifndef I2C_GPIO_H_
#define I2C_GPIO_H_

#include <msp430.h>

#if 0
// 128x64 OLED - SCL : P1.6, SDA : P1.7
#define SCL_OUT     P1DIR |=  BIT6              //SCL Output
#define SCL_1       P1OUT |=  BIT6              //SCL = 1
#define SCL_0       P1OUT &=~ BIT6              //SCL = 0
#define SCL_PU      P1REN |= BIT6;
#define SDA_1       P1OUT |=  BIT7              //SDA = 1
#define SDA_0       P1OUT &=~ BIT7              //SDA = 0
#define SDA_IN      P1DIR &=~ BIT7;             //SDA Input
#define SDA_OUT     P1DIR |=  BIT7              //SDA Output
#define SDA_READ    ((P1IN >> 7) & 0x01)        //Read SDA
#define SDA_PU      P1REN |= BIT7;
#endif

void i2c_read_byte(unsigned char slave_7bit_addr, unsigned char reg_addr, unsigned char *data);
void i2c_write_byte(unsigned char slave_7bit_addr, unsigned char reg_addr, unsigned char data);


#endif /* I2C_GPIO_H_ */
