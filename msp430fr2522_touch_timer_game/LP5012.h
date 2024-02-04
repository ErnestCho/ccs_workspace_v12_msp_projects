#ifndef LP5012_H
#define LP5012_H

#include "msp430.h"


#define CPU_F ((double)16000000)
#define delay_us(x) __delay_cycles((long)(CPU_F*(double)x/1000000.0))
#define delay_ms(x) __delay_cycles((long)(CPU_F*(double)x/1000.0))


/* LP5012 registers
*  http://www.ti.com/lit/gpn/LP5012.pdf
*/
#define LP5012_DEVICE_CONFIG0    0x00       // R/W
#define LP5012_DEVICE_CONFIG1    0x01       // R/W
#define LP5012_LED_CONFIG0       0x02       // R/W
#define LP5012_BANK_BRIGHTNESS   0x03       // R/W
#define LP5012_BANK_A_COLOR      0x04       // R/W
#define LP5012_BANK_B_COLOR      0x05       // R/W
#define LP5012_BANK_C_COLOR      0x06       // R/W
#define LP5012_LED0_BRIGHTNESS   0x07       // R/W
#define LP5012_LED1_BRIGHTNESS   0x08       // R/W
#define LP5012_LED2_BRIGHTNESS   0x09       // R/W
#define LP5012_LED3_BRIGHTNESS   0x0a       // R/W

#define LP5012_OUT0_COLOR       0x0b        // R/W
#define LP5012_OUT1_COLOR       0x0c        // R/W
#define LP5012_OUT2_COLOR       0x0d        // R/W
#define LP5012_OUT3_COLOR       0x0e        // R/W
#define LP5012_OUT4_COLOR       0x0f        // R/W
#define LP5012_OUT5_COLOR       0x10        // R/W
#define LP5012_OUT6_COLOR       0x11        // R/W
#define LP5012_OUT7_COLOR       0x12        // R/W
#define LP5012_OUT8_COLOR       0x13        // R/W
#define LP5012_OUT9_COLOR       0x14        // R/W
#define LP5012_OUT10_COLOR      0x15        // R/W
#define LP5012_OUT11_COLOR      0x16        // R/W
#define LP5012_RESET            0x17        // Write only

//
// bit configurations of LP5012 registers
//
// DEVICE_CONFIG0
#define LP5012_CHIP_EN_BIT                  BIT6
#define LP5012_CHIP_EN_SHIFT                6
// DEVICE_CONFIG1
#define LP5012_LOG_SCALE_EN_BIT             BIT5
#define LP5012_POWER_SAVE_EN_BIT            BIT4
#define LP5012_AUTO_INCR_EN_BIT             BIT3
#define LP5012_PWM_DITHERING_EN_BIT         BIT2
#define LP5012_MAX_CURRENT_OPTION_BIT       BIT1
#define LP5012_LED_GLOBAL_OFF_BIT           BIT0
// LED_CONFIG0
#define LP5012_LED3_BANK_EN_BIT             BIT3
#define LP5012_LED2_BANK_EN_BIT             BIT2
#define LP5012_LED1_BANK_EN_BIT             BIT1
#define LP5012_LED0_BANK_EN_BIT             BIT0



// Independent Slave address of LP5012 : 7bit type
#define LP5012_DEV0_ADDR     0x14        // pin [ADDR1,ADDR0] = [0,0]    // 0x14
#define LP5012_DEV1_ADDR     0x15        // pin [ADDR1,ADDR0] = [0,1]    // 0x15
#define LP5012_DEV2_ADDR     0x16        // pin [ADDR1,ADDR0] = [1,0]    // 0x16
#define LP5012_DEV3_ADDR     0x17        // pin [ADDR1,ADDR0] = [1,1]    // 0x17

#define LP5012_DEV0_WR_ADDR     0x28        // pin [ADDR1,ADDR0] = [0,0]    // 0x14<<1 + 0(WR)
#define LP5012_DEV0_RD_ADDR     0x29        // pin [ADDR1,ADDR0] = [0,0]    // 0x14<<1 + 1(RD)
#define LP5012_DEV1_WR_ADDR     0x2A        // pin [ADDR1,ADDR0] = [0,1]    // 0x15<<1 + 0(WR)
#define LP5012_DEV1_RD_ADDR     0x2B        // pin [ADDR1,ADDR0] = [0,1]    // 0x15<<1 + 1(RD)
#define LP5012_DEV2_WR_ADDR     0x2C        // pin [ADDR1,ADDR0] = [1,0]    // 0x16<<1 + 0(WR)
#define LP5012_DEV2_RD_ADDR     0x2D        // pin [ADDR1,ADDR0] = [1,0]    // 0x16<<1 + 1(RD)
#define LP5012_DEV3_WR_ADDR     0x2E        // pin [ADDR1,ADDR0] = [1,1]    // 0x17<<1 + 0(WR)
#define LP5012_DEV3_RD_ADDR     0x2F        // pin [ADDR1,ADDR0] = [1,1]    // 0x17<<1 + 1(RD)

// Broadcast(All LP5012 in the same I2C bus) Slave address of LP5012 :
#define LP5012_BROAD_ADDR    0x0C        // pin [ADDR1,ADDR0] = [x,x]    // 0x0C
/*
#define LP5012_BROAD_WR_ADDR    0x18        // pin [ADDR1,ADDR0] = [x,x]    // 0x0C<<1 + 0(WR)
#define LP5012_BROAD_RD_ADDR    0x19        // pin [ADDR1,ADDR0] = [x,x]    // 0x0C<<1 + 1(RD)
*/

// LP5012 - SCL : P1.7, SDA : P1.6
#define SCL_OUT     P1DIR |=  BIT7              //SCL Output
#define SCL_1       P1OUT |=  BIT7              //SCL = 1
#define SCL_0       P1OUT &=~ BIT7              //SCL = 0
#define SCL_PU      P1REN |= BIT7;
#define SDA_1       P1OUT |=  BIT6              //SDA = 1
#define SDA_0       P1OUT &=~ BIT6              //SDA = 0
#define SDA_IN      P1DIR &=~ BIT6;             //SDA Input
#define SDA_OUT     P1DIR |=  BIT6              //SDA Output
#define SDA_READ    ((P1IN >> 6) & 0x01)        //Read SDA
#define SDA_PU      P1REN |= BIT6;

// LP5012 - EN --> Hard wired pull up
//#define LP5012_EN_DIR_OUTPUT        P1DIR |= BIT7
//#define LP5012_EN_PIN_HIGH          P1OUT |= BIT7
//#define LP5012_EN_PIN_LOW           P1OUT &= ~BIT7


void Stop(void);
void WriteByte(unsigned char WriteData);
void WriteByte_wo_ACK(unsigned char WriteData);
unsigned char ReadByte(void);
void ReceiveAck(void);
void Acknowledge(void);

/////////////////////////////////////////////////////////////////////////////////
// I2C GPIO control HAL code
/////////////////////////////////////////////////////////////////////////////////
//void i2c_read_byte(unsigned char slave_7bit_addr, unsigned char reg_addr, unsigned char *data);
//void i2c_write_byte(unsigned char slave_7bit_addr, unsigned char reg_addr, unsigned char data);

// to save memory.. remove slave_addr parameter, .._1 means 1x LP5012 is used..
void i2c_read_byte_1(unsigned char reg_addr, unsigned char *data);
void i2c_write_byte_1(unsigned char reg_addr, unsigned char data);

/////////////////////////////////////////////////////////////////////////////////
// LP5012 related I2C function
/////////////////////////////////////////////////////////////////////////////////
void LP5012_I2C_chip_enable(unsigned char slave_addr, char en);
void LP5012_I2C_chip_reset(unsigned char slave_addr, char en);
void LP5012_I2C_IC_init(unsigned char slave_addr, unsigned char data);
void LP5012_I2C_LED_global_onoff(unsigned char slave_addr, char onoff);
void LP5012_I2C_bank_control_mode(unsigned char slave_addr, char onoff);
void LP5012_I2C_bank_brightness(unsigned char slave_addr, unsigned char brightness);
void LP5012_I2C_LEDx_ch_brightness(unsigned char slave_addr, unsigned char led0_br, unsigned char led1_br, unsigned char led2_br, unsigned char led3_br);
void LP5012_I2C_LEDx_single_brightness(unsigned char slave_addr, unsigned char ledx_br);
void LP5012_I2C_bank_color(unsigned char slave_addr, unsigned char red, unsigned char green, unsigned char blue);
void LP5012_I2C_LEDx_brightness(unsigned char slave_addr, unsigned char channel, unsigned char brightness);
void LP5012_I2C_OUTx_color(unsigned char slave_addr, unsigned char channel, unsigned char color);

// to save memory.. remove slave_addr parameter, .._1 means 1x LP5012 is used..
void LP5012_I2C_chip_enable_1(/*unsigned char slave_addr, */char en);
void LP5012_I2C_chip_reset_1(/*unsigned char slave_addr, */char en);
void LP5012_I2C_IC_init_1(/*unsigned char slave_addr, */unsigned char data);
void LP5012_I2C_LED_global_onoff_1(/*unsigned char slave_addr, */char onoff);
void LP5012_I2C_bank_control_mode_1(/*unsigned char slave_addr, */char onoff);
void LP5012_I2C_bank_brightness_1(/*unsigned char slave_addr, */unsigned char brightness);
void LP5012_I2C_LEDx_ch_brightness_1(/*unsigned char slave_addr, */unsigned char led0_br, unsigned char led1_br, unsigned char led2_br, unsigned char led3_br);
void LP5012_I2C_LEDx_single_brightness_1(/*unsigned char slave_addr, */unsigned char ledx_br);
void LP5012_I2C_bank_color_1(/*unsigned char slave_addr, */unsigned char red, unsigned char green, unsigned char blue);
void LP5012_I2C_LEDx_brightness_1(/*unsigned char slave_addr, */unsigned char channel, unsigned char brightness);
void LP5012_I2C_OUTx_color_1(/*unsigned char slave_addr,*/ unsigned char channel, unsigned char color);





#endif /* LP5012_H */

