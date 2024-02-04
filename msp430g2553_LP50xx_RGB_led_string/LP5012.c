
#include "LP5012.h"



/////////////////////////////////////////////////////////////////////////////////
// I2C GPIO control code
/////////////////////////////////////////////////////////////////////////////////

static void Delay(unsigned int n)
{
    unsigned int i;
    for (i=0; i<n*10; i++ );
}

void Start(void)
{
    SDA_1;
    Delay(5);
    SCL_1;
    Delay(5);
    SDA_0;
    Delay(5);
    SCL_0;
    Delay(5);
}

void Stop(void)
{
    SDA_0;
    Delay(5);
    SCL_1;
    Delay(5);
    SDA_1;
    Delay(5);
}


void WriteByte(unsigned char WriteData)
{
    unsigned char i;
    for (i=0; i<8; i++)
    {
        SCL_0;
        Delay(5);
        if (((WriteData >> 7) & 0x01) == 0x01)
        {
            SDA_1;
        }
        else
        {
            SDA_0;
        }
        Delay(5);
        SCL_1;
        WriteData = WriteData << 1;
        Delay(5);
    }
    // check for ack
    SCL_0;
    SDA_IN;
    Delay(10);
    SCL_1;
    Delay(10);
    SCL_0;
    SDA_OUT;
}

void WriteByte_wo_ACK(unsigned char WriteData)
{
    unsigned char i;
    for (i=0; i<8; i++)
    {
        SCL_0;
        Delay(5);
        if (((WriteData >> 7) & 0x01) == 0x01)
        {
            SDA_1;
        }
        else
        {
            SDA_0;
        }
        Delay(5);
        SCL_1;
        WriteData = WriteData << 1;
        Delay(5);
    }
}

unsigned char ReadByte(void)
{
    unsigned char i;
    unsigned char TempBit  = 0;
    unsigned char TempData = 0;
    SCL_0;
    Delay(5);
    SDA_1;
    for (i=0; i<8; i++)
    {
        Delay(5);
        SCL_1;
        Delay(5);
        if (SDA_READ == 0x01 )
        {
            TempBit = 1;
        }
        else
        {
            TempBit = 0;
        }
        TempData = (TempData << 1) | TempBit;
        SCL_0;
        Delay(5);
    }
    Delay(5);
    return(TempData);
}

void ReceiveAck(void)
{
    unsigned char i = 0;
    SCL_1;
    Delay(5);
    SDA_IN;
    while ((SDA_READ == 0x01) && (i < 255))
    {
        i++;
    }
    SDA_OUT;
    SCL_0;
    Delay(5);
}

void Acknowledge(void)
{
    SCL_0;
    Delay(5);
    SDA_OUT;
    SDA_0;
    SCL_1;
    Delay(10);
    SCL_0;
}

/////////////////////////////////////////////////////////////////////////////////
// I2C GPIO control HAL code
/////////////////////////////////////////////////////////////////////////////////
#define I2C_WR_BIT      0
#define I2C_RD_BIT      1
void i2c_read_byte(unsigned char slave_7bit_addr, unsigned char reg_addr, unsigned char *data)
{
    //begin read command for 0x1A
    Start();
    WriteByte(slave_7bit_addr<<1 | I2C_WR_BIT);        // write command
    WriteByte(reg_addr);     // register address
    Stop();

    //read 0x1A
    Start();
    WriteByte(slave_7bit_addr<<1 | I2C_RD_BIT);        // read command
    SDA_IN;

    //read register with Nack
    *data = ReadByte();
    Acknowledge();
    Stop();

}

void i2c_write_byte(unsigned char slave_7bit_addr, unsigned char reg_addr, unsigned char data)
{
    Start();
    WriteByte(slave_7bit_addr<<1 | I2C_WR_BIT);        // write command
    WriteByte(reg_addr);     // register address
    WriteByte(data);        // data to be written
    Stop();

}


/////////////////////////////////////////////////////////////////////////////////
// LP5012 related I2C function
/////////////////////////////////////////////////////////////////////////////////


void LP5012_I2C_chip_enable(unsigned char slave_addr, char en)
{
    if(en==1)
    {
        i2c_write_byte(slave_addr, LP5012_DEVICE_CONFIG0, 0x40 );
    }
    else
    {
        i2c_write_byte(slave_addr, LP5012_DEVICE_CONFIG0, 0x00 );
    }
}

void LP5012_I2C_chip_reset(unsigned char slave_addr, char en)
{
    if(en == 1)
    {
        i2c_write_byte(slave_addr, LP5012_RESET, 0xFF );
    }
    else
        ;

    delay_ms(10);
}

void LP5012_I2C_IC_init(unsigned char slave_addr, unsigned char data)
{
    // <data>
    // Bit 5 : log scale dimming        - linear (0) or log scale (1)
    // Bit 4 : auto power saving mode   - not enabled (0) or enabled(1)
    // Bit 3 : auto increment mode      - not enabled (0) or enabled (1)
    // Bit 2 : PWM dithering mode       - not enabled (0) or enabled (1)
    // Bit 1 : Output max current       - 25.5 mA (0) or 35 mA (1)
    i2c_write_byte(slave_addr, LP5012_DEVICE_CONFIG1, data);
}

void LP5012_I2C_LED_global_onoff(unsigned char slave_addr, char onoff)
{
    unsigned char temp;
    i2c_read_byte(slave_addr, LP5012_DEVICE_CONFIG1, &temp);

    // when 1 : global off
    if(onoff == 1)
        i2c_write_byte(slave_addr, LP5012_DEVICE_CONFIG1, temp & (~onoff) );
    else
        i2c_write_byte(slave_addr, LP5012_DEVICE_CONFIG1, temp | onoff );
}

void LP5012_I2C_bank_control_mode(unsigned char slave_addr, char onoff)
{
    if(onoff == 1)
        i2c_write_byte(slave_addr, LP5012_LED_CONFIG0, 0x0F );
    else
        i2c_write_byte(slave_addr, LP5012_LED_CONFIG0, 0x00 );
}

void LP5012_I2C_bank_control_mode_single(unsigned char slave_addr, char rgb_led_num)
{
    if( (rgb_led_num >= 0) && (rgb_led_num < 4) )  // should be between 0 and 3
        i2c_write_byte(slave_addr, LP5012_LED_CONFIG0, 0x01<< rgb_led_num);
    else
        ;
}

void LP5012_I2C_bank_brightness(unsigned char slave_addr, unsigned char brightness)
{
    i2c_write_byte(slave_addr, LP5012_BANK_BRIGHTNESS, brightness );
}

void LP5012_I2C_bank_color(unsigned char slave_addr, unsigned char red, unsigned char green, unsigned char blue)
{
    i2c_write_byte(slave_addr, LP5012_BANK_A_COLOR, blue );
    i2c_write_byte(slave_addr, LP5012_BANK_B_COLOR, red );
    i2c_write_byte(slave_addr, LP5012_BANK_C_COLOR, green );
}

void LP5012_I2C_LEDx_brightness(unsigned char slave_addr, unsigned char channel, unsigned char brightness)
{
    i2c_write_byte(slave_addr, LP5012_LED0_BRIGHTNESS + channel, brightness );
}

void LP5012_I2C_LEDx_ch_brightness(unsigned char slave_addr, unsigned char led0_br, unsigned char led1_br, unsigned char led2_br, unsigned char led3_br)
{
    i2c_write_byte(slave_addr, LP5012_LED0_BRIGHTNESS, led0_br );
    i2c_write_byte(slave_addr, LP5012_LED1_BRIGHTNESS, led1_br );
    i2c_write_byte(slave_addr, LP5012_LED2_BRIGHTNESS, led2_br );
    i2c_write_byte(slave_addr, LP5012_LED3_BRIGHTNESS, led3_br );
}

void LP5012_I2C_OUTx_color(unsigned char slave_addr, unsigned char channel, unsigned char color)
{
    i2c_write_byte(slave_addr, LP5012_OUT0_COLOR + channel, color );
}



