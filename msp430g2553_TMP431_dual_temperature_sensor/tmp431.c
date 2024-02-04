
#include <msp430.h>
#include <stdint.h>
#include "tmp431.h"




/////////////////////////////////////////////////////////////////////////////////
// I2C GPIO control code
/////////////////////////////////////////////////////////////////////////////////

static void Delay(unsigned int n)
{
    unsigned int i;
    for (i=0; i<n*10; i++ );        //check n*2
}

void Start(int8_t local_remote)
{
    if(local_remote == TMP431_LOCAL_IC)
    {
        LOCAL_SDA_1;    Delay(5);
        LOCAL_SCL_1;    Delay(5);
        LOCAL_SDA_0;    Delay(5);
        LOCAL_SCL_0;    Delay(5);
    }
    else if(local_remote == TMP431_REMOTE_IC)
    {
        REMOTE_SDA_1;    Delay(5);
        REMOTE_SCL_1;    Delay(5);
        REMOTE_SDA_0;    Delay(5);
        REMOTE_SCL_0;    Delay(5);
    }
    else
    {
        ;
    }
}


// I2C init
void GPIO_I2C_Init(void)
{
    // Local TMP431
    LOCAL_SCL_1;
    LOCAL_SDA_1;
    LOCAL_SCL_DIR_OUT;
    LOCAL_SDA_DIR_OUT;


    REMOTE_SCL_1;
    REMOTE_SDA_1;
    REMOTE_SCL_DIR_OUT;
    REMOTE_SDA_DIR_OUT;

}


void Stop(int8_t local_remote)
{
    if(local_remote == TMP431_LOCAL_IC)
    {
        LOCAL_SDA_0;    Delay(5);
        LOCAL_SCL_1;    Delay(5);
        LOCAL_SDA_1;    Delay(5);
    }
    else if(local_remote == TMP431_REMOTE_IC)
    {
        REMOTE_SDA_0;    Delay(5);
        REMOTE_SCL_1;    Delay(5);
        REMOTE_SDA_1;    Delay(5);
    }
    else
    {
        ;
    }
}


void WriteByte(int8_t local_remote, unsigned char WriteData)
{
    unsigned char i;

    if(local_remote == TMP431_LOCAL_IC)
    {
        for (i=0; i<8; i++)
        {
            LOCAL_SCL_0;        Delay(5);
            if (((WriteData >> 7) & 0x01) == 0x01)
            {
                LOCAL_SDA_1;
            }
            else
            {
                LOCAL_SDA_0;
            }
            Delay(5);
            WriteData = WriteData << 1;
            LOCAL_SCL_1;        Delay(5);
        }
        // check for ack
        LOCAL_SCL_0;
        LOCAL_SDA_DIR_IN;     Delay(10);
        LOCAL_SCL_1;          Delay(10);
        LOCAL_SCL_0;
        LOCAL_SDA_DIR_OUT;
    }
    else if(local_remote == TMP431_REMOTE_IC)
    {
        for (i=0; i<8; i++)
        {
            REMOTE_SCL_0;        Delay(5);
            if (((WriteData >> 7) & 0x01) == 0x01)
            {
                REMOTE_SDA_1;
            }
            else
            {
                REMOTE_SDA_0;
            }
            Delay(5);
            WriteData = WriteData << 1;
            REMOTE_SCL_1;        Delay(5);
        }
        // check for ack
        REMOTE_SCL_0;
        REMOTE_SDA_DIR_IN;     Delay(10);
        REMOTE_SCL_1;          Delay(10);
        REMOTE_SCL_0;
        REMOTE_SDA_DIR_OUT;
    }
    else
    {
        ;
    }
}

void WriteByte_wo_ACK(int8_t local_remote, unsigned char WriteData)
{
    unsigned char i;

    if(local_remote == TMP431_LOCAL_IC)
    {
        for (i=0; i<8; i++)
        {
            LOCAL_SCL_0;        Delay(5);
            if (((WriteData >> 7) & 0x01) == 0x01)
            {
                LOCAL_SDA_1;
            }
            else
            {
                LOCAL_SDA_0;
            }
            Delay(5);
            WriteData = WriteData << 1;
            LOCAL_SCL_1;        Delay(5);
        }
    }
    else if(local_remote == TMP431_REMOTE_IC)
    {
        for (i=0; i<8; i++)
        {
            REMOTE_SCL_0;        Delay(5);
            if (((WriteData >> 7) & 0x01) == 0x01)
            {
                REMOTE_SDA_1;
            }
            else
            {
                REMOTE_SDA_0;
            }
            Delay(5);
            WriteData = WriteData << 1;
            REMOTE_SCL_1;        Delay(5);
        }
    }
    else
    {
        ;
    }

}

unsigned char ReadByte(int8_t local_remote)
{
    unsigned char i;
    unsigned char TempBit  = 0;
    unsigned char TempData = 0;


    if(local_remote == TMP431_LOCAL_IC)
    {
        LOCAL_SDA_DIR_IN;
        LOCAL_SCL_0;    Delay(5);
        LOCAL_SDA_1;
        for (i=0; i<8; i++)
        {
            Delay(5);
            LOCAL_SCL_1;        Delay(5);
            if (LOCAL_SDA_READ == 0x01 )
            {
                TempBit = 1;
            }
            else
            {
                TempBit = 0;
            }
            TempData = (TempData << 1) | TempBit;
            LOCAL_SCL_0;            Delay(5);
        }
        Delay(5);
    }
    else if(local_remote == TMP431_REMOTE_IC)
    {
        REMOTE_SDA_DIR_IN;
        REMOTE_SCL_0;    Delay(5);
        REMOTE_SDA_1;
        for (i=0; i<8; i++)
        {
            Delay(5);
            REMOTE_SCL_1;        Delay(5);
            if (REMOTE_SDA_READ == 0x01 )
            {
                TempBit = 1;
            }
            else
            {
                TempBit = 0;
            }
            TempData = (TempData << 1) | TempBit;
            REMOTE_SCL_0;            Delay(5);
        }
        Delay(5);
    }
    else
    {
        ;
    }

    return(TempData);

}

void ReceiveAck(int8_t local_remote)
{
    unsigned char i = 0;

    if(local_remote == TMP431_LOCAL_IC)
    {
        LOCAL_SCL_1;    Delay(5);
        LOCAL_SDA_DIR_IN;
        while ((LOCAL_SDA_READ == 0x01) && (i < 255))
        {
            i++;
        }
        LOCAL_SDA_DIR_OUT;
        LOCAL_SCL_0;    Delay(5);
    }
    else if(local_remote == TMP431_REMOTE_IC)
    {
        REMOTE_SCL_1;    Delay(5);
        REMOTE_SDA_DIR_IN;
        while ((REMOTE_SDA_READ == 0x01) && (i < 255))
        {
            i++;
        }
        REMOTE_SDA_DIR_OUT;
        REMOTE_SCL_0;    Delay(5);
    }
    else
    {
        ;
    }
}

void Acknowledge(int8_t local_remote)
{
    if(local_remote == TMP431_LOCAL_IC)
    {
        LOCAL_SCL_0;    Delay(5);
        LOCAL_SDA_DIR_OUT;
        LOCAL_SDA_0;
        LOCAL_SCL_1;    Delay(10);
        LOCAL_SCL_0;
    }
    else if(local_remote == TMP431_REMOTE_IC)
    {
        REMOTE_SCL_0;    Delay(5);
        REMOTE_SDA_DIR_OUT;
        REMOTE_SDA_0;
        REMOTE_SCL_1;    Delay(10);
        REMOTE_SCL_0;
    }
    else
    {
        ;
    }
}


/////////////////////////////////////////////////////////////////////////////////
// I2C GPIO control HAL code
/////////////////////////////////////////////////////////////////////////////////

void i2c_read_word(int8_t local_remote, unsigned char addr, unsigned short *s_data)
{
    unsigned char data[2] = {0,0};
    Start(local_remote);
    WriteByte(local_remote, TMP431C_I2C_SLAVE_ADDR_WR);                    //
    WriteByte(local_remote, addr);                    //
//why comment?    Stop(local_remote);

    Start(local_remote);
    WriteByte_wo_ACK(local_remote, TMP431C_I2C_SLAVE_ADDR_RD);                    //
    //P1OUT &=~BIT6;
    //P1REN &=~BIT6;
    //SDA_DIR_IN;

    data[1] = ReadByte(local_remote); // read command
    Acknowledge(local_remote);

    //SDA_DIR_IN;
    data[0] = ReadByte(local_remote); // read command
    Acknowledge(local_remote);
    Stop(local_remote);

    *s_data = data[1]<<8 + data[0];
}

void i2c_read_byte(int8_t local_remote, unsigned char addr, unsigned char *data)
{
    //begin read command for 0x1A
    Start(local_remote);
    WriteByte(local_remote, TMP431C_I2C_SLAVE_ADDR_WR);        // write command
    WriteByte(local_remote, addr);     // register address
    Stop(local_remote);

    //read 0x1A
    Start(local_remote);
    WriteByte(local_remote, TMP431C_I2C_SLAVE_ADDR_RD);        // read command
    //SDA_DIR_IN;
    P1OUT &=~BIT6;
    P1REN &=~BIT6;

    //read register with Nack
    *data = ReadByte(local_remote);
    Acknowledge(local_remote);
    Stop(local_remote);

}

void i2c_write_word(int8_t local_remote, unsigned char addr, unsigned short s_data)
{
    unsigned char data[2] = {0,0};
    data[1] = (s_data>>8) & 0xff;
    data[0] = s_data & 0xff;
    Start(local_remote);
    WriteByte(local_remote, TMP431C_I2C_SLAVE_ADDR_WR);                    //
    WriteByte(local_remote, addr);                    //
    WriteByte(local_remote, data[1]);                    //
    WriteByte(local_remote, data[0]);                    //
    Stop(local_remote);

}

void i2c_write_byte(int8_t local_remote, unsigned char addr, unsigned char data)
{
    Start(local_remote);
    WriteByte(local_remote, TMP431C_I2C_SLAVE_ADDR_WR);       // write command
    WriteByte(local_remote, addr);                            // register address
    WriteByte(local_remote, data);                            // data to be written
    Stop(local_remote);

}

#define TMP431_REG_ADDR_LOCAL_TEMP_HIGH_BYTE     0x00
#define TMP431_REG_ADDR_LOCAL_TEMP_LOW_BYTE     0x15


unsigned char temperature_high = 0, temperature_low = 0;
unsigned char reg[255], test;

short tmp431_i2c_read_local_temperature(uint8_t local_remote)
{
    short tmp431_local_temperature_x10 = 0;
    unsigned short calc_temp_low = 0;


    i2c_read_byte(local_remote, TMP431_REG_ADDR_LOCAL_TEMP_HIGH_BYTE, &temperature_high);
    __delay_cycles(1000);
    i2c_read_byte(local_remote, TMP431_REG_ADDR_LOCAL_TEMP_LOW_BYTE, &temperature_low);
    __delay_cycles(1000);


    calc_temp_low = (temperature_low >>4) & 0x0F;
    calc_temp_low = calc_temp_low * 625 + 500;
    calc_temp_low = calc_temp_low / 1000;

    tmp431_local_temperature_x10 = temperature_high*10 + calc_temp_low;

    return tmp431_local_temperature_x10;

}

unsigned char decimal_text[4];      // format '3' '6' . '5' 'C'
void temperature_digit_to_decimal_buf(unsigned short temp_x10, unsigned char *buf)
{
    unsigned short l_tmp431_local_temperature_x10 = temp_x10;

    buf[0] = l_tmp431_local_temperature_x10 / 100;
    buf[1] = (l_tmp431_local_temperature_x10 - (buf[0]*100) ) /10;
    buf[2] = ( (l_tmp431_local_temperature_x10 - (buf[0]*100) ) - buf[1]*10 );
}

void tmp431_i2c_first_dummy_read(uint8_t local_remote)
{
    i2c_read_byte(local_remote, 0x00, &temperature_high);
    i2c_read_byte(local_remote, 0x15, &temperature_low);

}

// check CONFIG register bit3 (CONFIG reg addr : 0x09(at write), 0x03(at read)
uint8_t tmp431_i2c_set_extended_mode(uint8_t local_remote)
{
    uint8_t read_to_check = 0;
    uint8_t try_cnt = 10;
    uint8_t return_value = 1;
    i2c_write_byte(local_remote, 0x09, 0x04);     // bit3 : 0 => 0~127'C / bit3 : 1 => -64~191'C

    do
    {
        i2c_read_byte(local_remote, 0x03, &read_to_check);
        //__no_operation();
        __delay_cycles(100);

        if(try_cnt-- == 0)
        {
            return_value = 0;
            break;
        }

    } while(!(read_to_check & 0x04));

    return return_value;
}


