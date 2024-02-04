#ifndef HDC1080_I2C_GPIO_H
#define HDC1080_I2C_GPIO_H

#include "msp430.h"
#include <common.h>

#define CPU_F ((double)1000000)
#define delay_us(x) __delay_cycles((long)(CPU_F*(double)x/1000000.0))
#define delay_ms(x) __delay_cycles((long)(CPU_F*(double)x/1000.0))




// HDC1080 - SCL : P1.5, SDA : P2.0
#define SCL_OUT     P1DIR |=  BIT5              //SCL Output
#define SCL_1       P1OUT |=  BIT5              //SCL = 1
#define SCL_0       P1OUT &=~ BIT5              //SCL = 0
#define SCL_PU      P1REN |= BIT5;

#define SDA_1       P2OUT |=  BIT0              //SDA = 1
#define SDA_0       P2OUT &=~ BIT0              //SDA = 0
#define SDA_IN      P2DIR &=~ BIT0             //SDA Input
#define SDA_OUT     P2DIR |=  BIT0              //SDA Output
#define SDA_READ    ((P2IN >> 0) & 0x01)        //Read SDA
#define SDA_PU      P2REN |= BIT0;

// Other GPIO define

// Slave address of HDC1080 : 7bit type
#define HDC1080_SLAVE_ADDR     0x40        // b'100 0000

#define I2C_WR_BIT      0
#define I2C_RD_BIT      1

// register address of HDC1080          // 16bit values(reset value)
#define HDC1080_TEMPERATURE_RAW         0x00    // 0x0000(RO)           // 14bit[15:2] read only
#define HDC1080_HUMIDITY_RAW            0x01    // 0x0000(RO)           // 14bit[15:2] read only
#define HDC1080_CONFIG                  0x02    // 0x1000
#define HDC1080_SERIAL_ID_0             0xFB    // device specific
#define HDC1080_SERIAL_ID_1             0xFC    // device specific
#define HDC1080_SERIAL_ID_2             0xFD    // device specific
#define HDC1080_MANUFACTURER_ID         0xFE    // 0x5449
#define HDC1080_DEVICE_ID               0xFF    // 0x1050

#define HDC1080_MANU_ID_VAL             0x5449
#define HDC1080_DEV_ID_VAL              0x1050

// bit definition of configuration register
#define HDC1080_SOFT_RESET                  (0x8000)        // bit15
#define HDC1080_HEAT_DISABLE                (0*0x2000)      // bit13
#define HDC1080_HEAT_ENABLE                 (1*0x2000)
#define HDC1080_MODE_SINGLE_ACQ             (0*0x1000)      // bit12
#define HDC1080_MODE_SEQUENCE_ACQ           (1*0x1000)
#define HDC1080_BAT_STATUS_IS_UNDER_V2P8    (0*0x0800)      // bit11, read only
#define HDC1080_BAT_STATUS_IS_OVER_V2P8     (1*0x0800)      // bit11, read only
#define HDC1080_TEMP_RESOLUTION_14BIT       (0*0x0400)      // bit10
#define HDC1080_TEMP_RESOLUTION_11BIT       (1*0x0400)
#define HDC1080_HUMID_RESOLUTION_14BIT      (0*0x0100)      // bit9, bit8
#define HDC1080_HUMID_RESOLUTION_11BIT      (1*0x0100)
#define HDC1080_HUMID_RESOLUTION_08BIT      (2*0x0100)

/////////////////////////////////////////////////////////////////////////////////
// I2C GPIO : GPIO control code
/////////////////////////////////////////////////////////////////////////////////
void Stop(void);
void WriteByte(unsigned char WriteData);
void WriteByte_wo_ACK(unsigned char WriteData);
unsigned char ReadByte(void);
void ReceiveAck(void);
void Acknowledge(void);

/////////////////////////////////////////////////////////////////////////////////
// I2C GPIO control HAL code
/////////////////////////////////////////////////////////////////////////////////
void i2c_read_byte(unsigned char slave_7bit_addr, unsigned char reg_addr, unsigned char *data);
void i2c_write_byte(unsigned char slave_7bit_addr, unsigned char reg_addr, unsigned char data);

/////////////////////////////////////////////////////////////////////////////////
// HDC1080 related I2C function
/////////////////////////////////////////////////////////////////////////////////

void GPIO_I2C_Init(void);

uint8_t HDC1080_I2C_check_device_valid(uint8_t slave_addr);
uint16_t HDC1080_I2C_read_temperature(uint8_t slave_addr);
uint16_t HDC1080_I2C_read_humidity(uint8_t slave_addr);
void HDC1080_I2C_read_temp_and_humidity(uint8_t slave_addr, int16_t *temp_x10, uint16_t *humid_x10);





#endif /* HDC1080_I2C_GPIO_H */

