#ifndef LP5012_H
#define LP5012_H

#include "msp430.h"
#include <stdint.h>



#define CPU_F ((double)16000000)
#define delay_us(x) __delay_cycles((long)(CPU_F*(double)x/1000000.0))
#define delay_ms(x) __delay_cycles((long)(CPU_F*(double)x/1000.0))

#define ENABLE      1
#define DISABLE     0

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
/*
#define LP5012_DEV0_WR_ADDR     0x38        // pin [ADDR1,ADDR0] = [0,0]    // 0x14<<1 + 0(WR)
#define LP5012_DEV0_RD_ADDR     0x39        // pin [ADDR1,ADDR0] = [0,0]    // 0x14<<1 + 1(RD)
#define LP5012_DEV1_WR_ADDR     0x3A        // pin [ADDR1,ADDR0] = [0,1]    // 0x15<<1 + 0(WR)
#define LP5012_DEV1_RD_ADDR     0x3B        // pin [ADDR1,ADDR0] = [0,1]    // 0x15<<1 + 1(RD)
#define LP5012_DEV0_WR_ADDR     0x3C        // pin [ADDR1,ADDR0] = [1,0]    // 0x16<<1 + 0(WR)
#define LP5012_DEV0_RD_ADDR     0x3D        // pin [ADDR1,ADDR0] = [1,0]    // 0x16<<1 + 1(RD)
#define LP5012_DEV1_WR_ADDR     0x3E        // pin [ADDR1,ADDR0] = [1,1]    // 0x17<<1 + 0(WR)
#define LP5012_DEV1_RD_ADDR     0x3F        // pin [ADDR1,ADDR0] = [1,1]    // 0x17<<1 + 1(RD)
*/
// Broadcast(All LP5012 in the same I2C bus) Slave address of LP5012 :
#define LP5012_BROAD_ADDR    0x0C        // pin [ADDR1,ADDR0] = [x,x]    // 0x0C
/*
#define LP5012_BROAD_WR_ADDR    0x18        // pin [ADDR1,ADDR0] = [x,x]    // 0x0C<<1 + 0(WR)
#define LP5012_BROAD_RD_ADDR    0x19        // pin [ADDR1,ADDR0] = [x,x]    // 0x0C<<1 + 1(RD)
*/
//#define I2C_MULTI_CHANNEL


#if 1//ndef I2C_MULTI_CHANNEL
// LP5012 - SCL : P1.6, SDA : P1.5
#define SCL_OUT     P1DIR |=  BIT6              //SCL Output
#define SCL_1       P1OUT |=  BIT6              //SCL = 1
#define SCL_0       P1OUT &=~ BIT6              //SCL = 0
#define SCL_PU      P1REN |= BIT6;
#define SDA_1       P1OUT |=  BIT5              //SDA = 1
#define SDA_0       P1OUT &=~ BIT5              //SDA = 0
#define SDA_IN      P1DIR &=~ BIT5;             //SDA Input
#define SDA_OUT     P1DIR |=  BIT5              //SDA Output
#define SDA_READ    ((P1IN >> 5) & 0x01)        //Read SDA
#define SDA_PU      P1REN |= BIT7;

// LP5012 - EN : P1.6,
#define LP5012_EN_DIR_OUTPUT        P1DIR |= BIT7
#define LP5012_EN_PIN_HIGH          P1OUT |= BIT7
#define LP5012_EN_PIN_LOW           P1OUT &= ~BIT7

//#else

// multiple LP5012 usage for character display
// MSP430FR2633 -- 3x I2C -- 2x EN -- 4x I2C slave addr ==> 24x LP5012 = 6 x 4 LP5012
//
//          SDA    SCL    EN0    EN1
// I2C_A : P3.0   P1.2   P2.3   P3.1
// I2C_B : P2.2   P1.1   P2.4   P2.5
// I2C_C : P1.3   P1.0   P1.7   P1.6



////////////////////////////////
//// multi I2C channel .. /////

#define LP5012_I2C_CH_A      0
#define LP5012_I2C_CH_B      1
#define LP5012_I2C_CH_C      2

// LP5012 CH_A - SCL : P1.2, SDA : P3.0
#define I2C_CH_A_SCL_OUT     (P1DIR |=  BIT2)              //SCL Output
#define I2C_CH_A_SCL_1       (P1OUT |=  BIT2)              //SCL = 1
#define I2C_CH_A_SCL_0       (P1OUT &=~ BIT2)              //SCL = 0
#define I2C_CH_A_SCL_PU      (P1REN |= BIT2)
#define I2C_CH_A_SDA_1       (P3OUT |=  BIT0)              //SDA = 1
#define I2C_CH_A_SDA_0       (P3OUT &=~ BIT0)              //SDA = 0
#define I2C_CH_A_SDA_IN      (P3DIR &=~ BIT0)             //SDA Input
#define I2C_CH_A_SDA_OUT     (P3DIR |=  BIT0)              //SDA Output
#define I2C_CH_A_SDA_READ    ((P3IN >> 0) & 0x01)        //Read SDA
#define I2C_CH_A_SDA_PU      (P3REN |= BIT0)

// LP5012 CH_B - SCL : P1.1, SDA : P2.2
#define I2C_CH_B_SCL_OUT     (P1DIR |=  BIT1)              //SCL Output
#define I2C_CH_B_SCL_1       (P1OUT |=  BIT1)              //SCL = 1
#define I2C_CH_B_SCL_0       (P1OUT &=~ BIT1)              //SCL = 0
#define I2C_CH_B_SCL_PU      (P1REN |= BIT1)
#define I2C_CH_B_SDA_1       (P2OUT |=  BIT2)              //SDA = 1
#define I2C_CH_B_SDA_0       (P2OUT &=~ BIT2)              //SDA = 0
#define I2C_CH_B_SDA_IN      (P2DIR &=~ BIT2)             //SDA Input
#define I2C_CH_B_SDA_OUT     (P2DIR |=  BIT2)              //SDA Output
#define I2C_CH_B_SDA_READ    ((P2IN >> 2) & 0x01)        //Read SDA
#define I2C_CH_B_SDA_PU      (P2REN |= BIT2)

// LP5012 CH_C - SCL : P1.0, SDA : P1.3
#define I2C_CH_C_SCL_OUT     (P1DIR |=  BIT0)              //SCL Output
#define I2C_CH_C_SCL_1       (P1OUT |=  BIT0)              //SCL = 1
#define I2C_CH_C_SCL_0       (P1OUT &=~ BIT0)              //SCL = 0
#define I2C_CH_C_SCL_PU      (P1REN |= BIT0)
#define I2C_CH_C_SDA_1       (P1OUT |=  BIT3)              //SDA = 1
#define I2C_CH_C_SDA_0       (P1OUT &=~ BIT3)              //SDA = 0
#define I2C_CH_C_SDA_IN      (P1DIR &=~ BIT3)             //SDA Input
#define I2C_CH_C_SDA_OUT     (P1DIR |=  BIT3)              //SDA Output
#define I2C_CH_C_SDA_READ    ((P1IN >> 3) & 0x01)        //Read SDA
#define I2C_CH_C_SDA_PU      (P1REN |= BIT3)

// LP5012 CH_A - EN0 : P2.3, EN1 : P3.1
#define LP5012_CH_A_EN0_DIR_OUTPUT        (P2DIR |= BIT3)
#define LP5012_CH_A_EN0_PIN_HIGH          (P2OUT |= BIT3)
#define LP5012_CH_A_EN0_PIN_LOW           (P2OUT &= ~BIT3)

#define LP5012_CH_A_EN1_DIR_OUTPUT        (P3DIR |= BIT1)
#define LP5012_CH_A_EN1_PIN_HIGH          (P3OUT |= BIT1)
#define LP5012_CH_A_EN1_PIN_LOW           (P3OUT &= ~BIT1)

// LP5012 CH_B - EN0 : P2.4, EN1 : 2.5
#define LP5012_CH_B_EN0_DIR_OUTPUT        (P2DIR |= BIT4)
#define LP5012_CH_B_EN0_PIN_HIGH          (P2OUT |= BIT4)
#define LP5012_CH_B_EN0_PIN_LOW           (P2OUT &= ~BIT4)

#define LP5012_CH_B_EN1_DIR_OUTPUT        (P2DIR |= BIT5)
#define LP5012_CH_B_EN1_PIN_HIGH          (P2OUT |= BIT5)
#define LP5012_CH_B_EN1_PIN_LOW           (P2OUT &= ~BIT5)

// LP5012 CH_C - EN0 : P1.7, EN1 : 1.6
#define LP5012_CH_C_EN0_DIR_OUTPUT        (P1DIR |= BIT7)
#define LP5012_CH_C_EN0_PIN_HIGH          (P1OUT |= BIT7)
#define LP5012_CH_C_EN0_PIN_LOW           (P1OUT &= ~BIT7)

#define LP5012_CH_C_EN1_DIR_OUTPUT        (P1DIR |= BIT6)
#define LP5012_CH_C_EN1_PIN_HIGH          (P1OUT |= BIT6)
#define LP5012_CH_C_EN1_PIN_LOW           (P1OUT &= ~BIT6)


////////////////////////////////
#endif



void GPIO_I2C_Init(void);

void Stop(void);
void WriteByte(uint8_t WriteData);
void WriteByte_wo_ACK(uint8_t WriteData);
uint8_t ReadByte(void);
void ReceiveAck(void);
void Acknowledge(void);
void SetDataAsInput(void);

void GPIO_I2C_multi_Init(uint8_t ch);
//void multi_GPIO_I2C_Init(uint8_t ch);
void multi_I2C_Start(uint8_t ch);
void multi_I2C_Stop(uint8_t ch);
void multi_I2C_WriteByte(uint8_t ch, uint8_t WriteData);
void multi_I2C_WriteByte_wo_ACK(uint8_t ch, uint8_t WriteData);
uint8_t multi_I2C_ReadByte(uint8_t ch);
void multi_I2C_ReceiveAck(uint8_t ch);
void multi_I2C_Acknowledge(uint8_t ch);
void multi_I2C_SetDataAsInput(uint8_t ch);

/////////////////////////////////////////////////////////////////////////////////
// I2C GPIO control HAL code
/////////////////////////////////////////////////////////////////////////////////
void i2c_read_byte(uint8_t slave_7bit_addr, uint8_t reg_addr, uint8_t *data);
void i2c_write_byte(uint8_t slave_7bit_addr, uint8_t reg_addr, uint8_t data);

void i2c_read_byte_multi_ch(uint8_t ch, uint8_t slave_7bit_addr, uint8_t reg_addr, uint8_t *data);
void i2c_write_byte_multi_ch(uint8_t ch, uint8_t slave_7bit_addr, uint8_t reg_addr, uint8_t data);
/////////////////////////////////////////////////////////////////////////////////
// LP5012 related I2C function
/////////////////////////////////////////////////////////////////////////////////
void LP5012_I2C_chip_enable(uint8_t slave_addr, char en);
void LP5012_I2C_chip_reset(uint8_t slave_addr, char en);
void LP5012_I2C_IC_init(uint8_t slave_addr, uint8_t data);
void LP5012_I2C_LED_global_onoff(uint8_t slave_addr, char onoff);
void LP5012_I2C_bank_control_mode(uint8_t slave_addr, char onoff);
void LP5012_I2C_bank_brightness(uint8_t slave_addr, uint8_t brightness);
void LP5012_I2C_LEDx_ch_brightness(uint8_t slave_addr, uint8_t led0_br, uint8_t led1_br, uint8_t led2_br, uint8_t led3_br);
void LP5012_I2C_bank_color(uint8_t slave_addr, uint8_t red, uint8_t green, uint8_t blue);
void LP5012_I2C_LEDx_brightness(uint8_t slave_addr, uint8_t channel, uint8_t brightness);
void LP5012_I2C_OUTx_color(uint8_t slave_addr, uint8_t channel, uint8_t color);

void LP5012_multi_ch_I2C_chip_enable(uint8_t ch, uint8_t slave_addr, char en);
void LP5012_multi_ch_I2C_chip_reset(uint8_t ch, uint8_t slave_addr, char en);
void LP5012_multi_ch_I2C_IC_init(uint8_t ch, uint8_t slave_addr, uint8_t data);
void LP5012_multi_ch_I2C_LED_global_onoff(uint8_t ch, uint8_t slave_addr, char onoff);
void LP5012_multi_ch_I2C_bank_control_mode(uint8_t ch, uint8_t slave_addr, char onoff);
void LP5012_multi_ch_I2C_bank_brightness(uint8_t ch, uint8_t slave_addr, uint8_t brightness);
void LP5012_multi_ch_I2C_bank_color(uint8_t ch, uint8_t slave_addr, uint8_t red, uint8_t green, uint8_t blue);
void LP5012_multi_ch_I2C_LEDx_brightness(uint8_t ch, uint8_t slave_addr, uint8_t channel, uint8_t brightness);
void LP5012_multi_ch_I2C_LEDx_ch_brightness(uint8_t ch, uint8_t slave_addr, uint8_t led0_br, uint8_t led1_br, uint8_t led2_br, uint8_t led3_br);
void LP5012_multi_ch_I2C_OUTx_color(uint8_t ch, uint8_t slave_addr, uint8_t channel, uint8_t color);



#endif /* LP5012_H */

