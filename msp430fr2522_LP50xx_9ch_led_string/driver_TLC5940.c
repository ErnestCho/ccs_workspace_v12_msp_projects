
#include "driver_TLC5940.h"


void delay_twh_twl(void)
{
	_nop();
}
void delay_tsdi(void)
{
	_nop();
	_nop();
	_nop();
	_nop();
}

void delay_teos(void)
{
	delay_twh_twl();
	delay_twh_twl();
	delay_twh_twl();
	delay_tsdi();

	delay_twh_twl();
	delay_twh_twl();
	delay_twh_twl();
	delay_tsdi();

	delay_twh_twl();
	delay_twh_twl();
	delay_twh_twl();
	delay_tsdi();

	delay_twh_twl();
	delay_twh_twl();
	delay_twh_twl();
	delay_tsdi();

	delay_twh_twl();
	delay_twh_twl();
	delay_twh_twl();
	delay_tsdi();
}

void delay_tgslat(void)
{
	delay_teos();
	delay_teos();
	delay_teos();
}
void delay(U16 cnt)
{
	while(cnt--)
		_nop();
}

void delay_ms(U16 delay_ms)
{
	U16 cnt = delay_ms;
	U16 cnt2 = 2200;
	U16 i;
	while(cnt--)
	{
		//while(cnt2--)
		for(i=0; i<cnt2; i++)
		{
			_nop();
		}
	}
}

void send_tlc5973_sdi_data(U08 bit)
{
	SDI_PORT |= TLC5973_SDI_BIT;
	delay_twh_twl();
	SDI_PORT &= (~TLC5973_SDI_BIT);
	delay_twh_twl();
	SDI_PORT = (bit==1) ? (SDI_PORT | TLC5973_SDI_BIT) : (SDI_PORT & (~TLC5973_SDI_BIT));
	delay_twh_twl();
	SDI_PORT &= (~TLC5973_SDI_BIT);
	delay_tsdi();
}

void send_tlc59731_sdi_data(U08 bit)
{
	SDI_PORT |= TLC59731_SDI_BIT;
	delay_twh_twl();
	SDI_PORT &= (~TLC59731_SDI_BIT);
	delay_twh_twl();
	SDI_PORT = (bit==1) ? (SDI_PORT | TLC59731_SDI_BIT) : (SDI_PORT & (~TLC59731_SDI_BIT));
	delay_twh_twl();
	SDI_PORT &= (~TLC59731_SDI_BIT);
	delay_tsdi();

}

void send_sdi_data(U08 bit)
{
	SDI_PORT |= SDI_BIT;
	delay_twh_twl();
	SDI_PORT &= (~SDI_BIT);
	delay_twh_twl();
	SDI_PORT = (bit==1) ? (SDI_PORT | SDI_BIT) : (SDI_PORT & (~SDI_BIT));
	delay_twh_twl();
	SDI_PORT &= (~SDI_BIT);
	delay_tsdi();

}

// 12bit data
U16 temp_12bit[4];
// 8bit data
U08 temp_8bit[4];
U08 bit;
void TLC5973_SerialComm(U08 target, U16 gs_led0, U16 gs_led1, U16 gs_led2)
{
	S08 i, j;
	//U16 temp_12bit[4];
	//U08 bit;

	temp_12bit[0] = TLC5973_CMD;		// Write Cmd
	temp_12bit[1] = gs_led0;			// LED0
	temp_12bit[2] = gs_led1;			// LED1
	temp_12bit[3] = gs_led2;			// LED2

	for(i=0; i<4; i++)
	{
		for(j=12; j>0; j--)
		{
			bit = (temp_12bit[i] >> (j-1)) & 0x01;
			//SEND_SDI_DATA(bit);
			//send_sdi_data(bit);
			send_tlc5973_sdi_data(bit);
		}
	}
	if(target == 1)
		delay_teos();
	else
		delay_tgslat();
}

void TLC59731_SerialComm(U08 target, U08 gs_led0, U08 gs_led1, U08 gs_led2)
{
	U08 i, j;
//	U08 temp_8bit[4];
//	U08 bit;

	temp_8bit[0] = TLC59731_CMD;		// Write Cmd
	temp_8bit[1] = gs_led0;			// LED0
	temp_8bit[2] = gs_led1;			// LED1
	temp_8bit[3] = gs_led2;			// LED2

	for(i=0; i<4; i++)
	{
		for(j=8; j>0; j--)
		{
			bit = (temp_8bit[i] >> (j-1)) & 0x01;
			//SEND_SDI_DATA(bit);
			//send_sdi_data(bit);
			send_tlc59731_sdi_data(bit);
		}
	}
	if(target == 1)
		delay_teos();
	else
		delay_tgslat();
}
typedef struct {
	unsigned short r;
	unsigned short g;
	unsigned short b;
}_tag_colorLed;

_tag_colorLed led_color;
unsigned short gs_led;		// gray scaled Led brightness.
#define GRAYSCALE_BIT	12
#if 0
void TLC5940_rgb_alignData(_tag_colorLed *led_color, U16 *target)
{
}
void TLC5940_led2serialData(U16 gs_led, U16 *target)
{

}
void TLC5940_SerialData(_tag_colorLed *led_color, U16 led_out_cnt)
{
	U16 cnt, i;

	// clear SIN, SCLK, BLANK, GSCLK

	// XLAT high
	// delay t_wh2
	// XLAT low
	// delay t_h3

	// VPRG low

	for(cnt=0; cnt<led_out_cnt; cnt++)
	{
#if 1//def LED_RGB_TYPE
		for(i=GRAYSCALE_BIT; i>0; i--)
		{
			bit = (led_color[cnt]->R >> (i-1)) & 0x01;
			// SIN = bit
			// SCLK = 1
			// delay t_wh0
			// SCLK = 0
			// no delay // delay t_wl0
		}
#elif defined LED_GRAY_TYPE
#endif
		if(cnt & 0x0f)
		{
			;// XLAT high
			// delay t_wh2
			// XLAT low
		}
	}
}
#endif
