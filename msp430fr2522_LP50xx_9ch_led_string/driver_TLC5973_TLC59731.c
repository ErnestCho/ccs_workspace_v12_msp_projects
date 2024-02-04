
#include "driver_TLC5973_TLC59731.h"

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

