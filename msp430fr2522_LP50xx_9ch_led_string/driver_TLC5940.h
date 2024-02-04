
#ifndef __DRIVER_TLC5973_TLC59731_H__
#define __DRIVER_TLC5973_TLC59731_H__

#include <msp430.h>

typedef unsigned short U16;
typedef unsigned char U08;
typedef short S16;
typedef char S08;

#define TLC5973_CMD		0x03AA		// 12bit TLC5973  CMD
#define TLC59731_CMD	0x3A		//  8bit TLC59731 CMD
//#define T_WH		1
//#define T_WL		1
//#define T_CLK		1
//#define T_SCLK		1
//#define T_EOS		4
//#define T_GSLAT		10

#define SDI_PORT	P2OUT
#define SDI_BIT		BIT2
#define TLC5973_SDI_BIT		BIT2
#define TLC59731_SDI_BIT	BIT3

void delay_twh_twl(void);
void delay_tsdi(void);
void delay_teos(void);
void delay_tgslat(void);
void delay(U16 cnt);
void delay_ms(U16 delay_ms);
#if 0
#define DELAY(delay_cnt)	delay(delay_cnt)

#define SEND_SDI_DATA(bit)		SDI_PORT |= SDI_BIT;		\
								DELAY(T_WH);			\
								SDI_PORT &= (~SDI_BIT);	\
								DELAY(T_WL);			\
								SDI_PORT = (bit==1)? (SDI_PORT | SDI_BIT) : (SDI_PORT & (~SDI_BIT));	\
								DELAY(T_WH);			\
								SDI_PORT &= (~SDI_BIT);	\
								DELAY(T_SCLK)
#endif
void send_tlc5973_sdi_data(U08 bit);
void send_tlc59731_sdi_data(U08 bit);
void send_sdi_data(U08 bit);
// for 12bit data
void TLC5973_SerialComm(U08 target, U16 gs_led0, U16 gs_led1, U16 gs_led2);
// for 8bit data
void TLC59731_SerialComm(U08 target, U08 gs_led0, U08 gs_led1, U08 gs_led2);
#if 0
void TLC5940_SerialData(struct *led_color, U16 led_out_cnt);
#endif

#endif

