
#include "LP5012.h"


// Touch key : 3points, (+additional 3points)
// LP5012 : 3x I2C bus, USE EN1,2 per I2C bus, 4x LP5012 per 1x EN
// 1x DISPLAY board have 4x LP5012, 6x DISPLAY board per MCU.


/////////////////////////////////////////////////////////////////////////////////
// I2C GPIO control code
/////////////////////////////////////////////////////////////////////////////////

static void Delay(unsigned int n)
{
    unsigned int i;
    for (i=0; i<n; i++ );
}

void GPIO_I2C_multi_Init(uint8_t ch)
{
    if(ch == LP5012_I2C_CH_A)
    {
        // LP5012
        I2C_CH_A_SCL_1;
        I2C_CH_A_SDA_1;
        I2C_CH_A_SCL_OUT;
        I2C_CH_A_SDA_OUT;
        I2C_CH_A_SCL_PU;
        I2C_CH_A_SDA_PU;

    }
    else if(ch == LP5012_I2C_CH_B)
    {
        // LP5012
        I2C_CH_B_SCL_1;
        I2C_CH_B_SDA_1;
        I2C_CH_B_SCL_OUT;
        I2C_CH_B_SDA_OUT;
        I2C_CH_B_SCL_PU;
        I2C_CH_B_SDA_PU;

    }
    else if(ch == LP5012_I2C_CH_C)
    {
        // LP5012
        I2C_CH_C_SCL_1;
        I2C_CH_C_SDA_1;
        I2C_CH_C_SCL_OUT;
        I2C_CH_C_SDA_OUT;
        I2C_CH_C_SCL_PU;
        I2C_CH_C_SDA_PU;

    }
    else
        ;
}

void GPIO_I2C_Init(void)
{
    // LP5012
    SCL_1;
    SDA_1;
    SCL_OUT;
    SDA_OUT;
    SCL_PU;
    SDA_PU;
}


void multi_I2C_Start(uint8_t ch)
{
    if(ch == LP5012_I2C_CH_A)
    {
        I2C_CH_A_SDA_1;
        Delay(5);
        I2C_CH_A_SCL_1;
        Delay(5);
        I2C_CH_A_SDA_0;
        Delay(5);
        I2C_CH_A_SCL_0;
        Delay(5);
    }
    else if(ch == LP5012_I2C_CH_B)
    {
        I2C_CH_B_SDA_1;
        Delay(5);
        I2C_CH_B_SCL_1;
        Delay(5);
        I2C_CH_B_SDA_0;
        Delay(5);
        I2C_CH_B_SCL_0;
        Delay(5);
    }
    else if(ch == LP5012_I2C_CH_C)
    {
        I2C_CH_C_SDA_1;
        Delay(5);
        I2C_CH_C_SCL_1;
        Delay(5);
        I2C_CH_C_SDA_0;
        Delay(5);
        I2C_CH_C_SCL_0;
        Delay(5);
    }
    else
        ;   // no channel defined, do nothing..
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

void multi_I2C_Stop(uint8_t ch)
{
    if(ch == LP5012_I2C_CH_A)
    {
        I2C_CH_A_SDA_0;
        Delay(5);
        I2C_CH_A_SCL_1;
        Delay(5);
        I2C_CH_A_SDA_1;
        Delay(5);

    }
    else if(ch == LP5012_I2C_CH_B)
    {
        I2C_CH_B_SDA_0;
        Delay(5);
        I2C_CH_B_SCL_1;
        Delay(5);
        I2C_CH_B_SDA_1;
        Delay(5);

    }
    else if (ch == LP5012_I2C_CH_C)
    {
        I2C_CH_C_SDA_0;
        Delay(5);
        I2C_CH_C_SCL_1;
        Delay(5);
        I2C_CH_C_SDA_1;
        Delay(5);

    }
    else
        ;   // no channel defined, do nothing..
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

void multi_I2C_WriteByte(uint8_t ch, uint8_t WriteData)
{
    uint8_t i;
    if(ch == LP5012_I2C_CH_A)
    {
        for (i=0; i<8; i++)
        {
            I2C_CH_A_SCL_0;
            Delay(5);
            if (((WriteData >> 7) & 0x01) == 0x01)
            {
                I2C_CH_A_SDA_1;
            }
            else
            {
                I2C_CH_A_SDA_0;
            }
            Delay(5);
            I2C_CH_A_SCL_1;
            WriteData = WriteData << 1;
            Delay(5);
        }
        // check for ack
        I2C_CH_A_SCL_0;
        I2C_CH_A_SDA_IN;
        Delay(10);
        I2C_CH_A_SCL_1;
        Delay(10);
        I2C_CH_A_SCL_0;
        I2C_CH_A_SDA_OUT;
    }
    else if(ch == LP5012_I2C_CH_B)
    {
        for (i=0; i<8; i++)
        {
            I2C_CH_B_SCL_0;
            Delay(5);
            if (((WriteData >> 7) & 0x01) == 0x01)
            {
                I2C_CH_B_SDA_1;
            }
            else
            {
                I2C_CH_B_SDA_0;
            }
            Delay(5);
            I2C_CH_B_SCL_1;
            WriteData = WriteData << 1;
            Delay(5);
        }
        // check for ack
        I2C_CH_B_SCL_0;
        I2C_CH_B_SDA_IN;
        Delay(10);
        I2C_CH_B_SCL_1;
        Delay(10);
        I2C_CH_B_SCL_0;
        I2C_CH_B_SDA_OUT;

    }
    else if(ch == LP5012_I2C_CH_C)
    {
        for (i=0; i<8; i++)
        {
            I2C_CH_C_SCL_0;
            Delay(5);
            if (((WriteData >> 7) & 0x01) == 0x01)
            {
                I2C_CH_C_SDA_1;
            }
            else
            {
                I2C_CH_C_SDA_0;
            }
            Delay(5);
            I2C_CH_C_SCL_1;
            WriteData = WriteData << 1;
            Delay(5);
        }
        // check for ack
        I2C_CH_C_SCL_0;
        I2C_CH_C_SDA_IN;
        Delay(10);
        I2C_CH_C_SCL_1;
        Delay(10);
        I2C_CH_C_SCL_0;
        I2C_CH_C_SDA_OUT;

    }
    else
        ;   // no channel defined, do nothing..
}

void WriteByte(uint8_t WriteData)
{
    uint8_t i;
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

void multi_I2C_WriteByte_wo_ACK(uint8_t ch, uint8_t WriteData)
{
    uint8_t i;
    if(ch == LP5012_I2C_CH_A)
    {
        for (i=0; i<8; i++)
        {
            I2C_CH_A_SCL_0;
            Delay(5);
            if (((WriteData >> 7) & 0x01) == 0x01)
            {
                I2C_CH_A_SDA_1;
            }
            else
            {
                I2C_CH_A_SDA_0;
            }
            Delay(5);
            I2C_CH_A_SCL_1;
            WriteData = WriteData << 1;
            Delay(5);
        }
    }
    else if(ch == LP5012_I2C_CH_B)
    {
        for (i=0; i<8; i++)
        {
            I2C_CH_B_SCL_0;
            Delay(5);
            if (((WriteData >> 7) & 0x01) == 0x01)
            {
                I2C_CH_B_SDA_1;
            }
            else
            {
                I2C_CH_B_SDA_0;
            }
            Delay(5);
            I2C_CH_B_SCL_1;
            WriteData = WriteData << 1;
            Delay(5);
        }

    }
    else if(ch == LP5012_I2C_CH_C)
    {
        for (i=0; i<8; i++)
        {
            I2C_CH_C_SCL_0;
            Delay(5);
            if (((WriteData >> 7) & 0x01) == 0x01)
            {
                I2C_CH_C_SDA_1;
            }
            else
            {
                I2C_CH_C_SDA_0;
            }
            Delay(5);
            I2C_CH_C_SCL_1;
            WriteData = WriteData << 1;
            Delay(5);
        }

    }
    else
        ;   // no channel defined, do nothing..
}

void WriteByte_wo_ACK(uint8_t WriteData)
{
    uint8_t i;
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

uint8_t multi_I2C_ReadByte(uint8_t ch)
{
    uint8_t i;
    uint8_t TempBit  = 0;
    uint8_t TempData = 0;

    if(ch == LP5012_I2C_CH_A)
    {
        I2C_CH_A_SCL_0;
        Delay(5);
        I2C_CH_A_SDA_1;
        for (i=0; i<8; i++)
        {
            Delay(5);
            I2C_CH_A_SCL_1;
            Delay(5);
            if (I2C_CH_A_SDA_READ == 0x01 )
            {
                TempBit = 1;
            }
            else
            {
                TempBit = 0;
            }
            TempData = (TempData << 1) | TempBit;
            I2C_CH_A_SCL_0;
            Delay(5);
        }
        Delay(5);
        return(TempData);
    }
    else if(ch == LP5012_I2C_CH_B)
    {
        I2C_CH_B_SCL_0;
        Delay(5);
        I2C_CH_B_SDA_1;
        for (i=0; i<8; i++)
        {
            Delay(5);
            I2C_CH_B_SCL_1;
            Delay(5);
            if (I2C_CH_B_SDA_READ == 0x01 )
            {
                TempBit = 1;
            }
            else
            {
                TempBit = 0;
            }
            TempData = (TempData << 1) | TempBit;
            I2C_CH_B_SCL_0;
            Delay(5);
        }
        Delay(5);
        return(TempData);
    }
    else if(ch == LP5012_I2C_CH_C)
    {
        I2C_CH_C_SCL_0;
        Delay(5);
        I2C_CH_C_SDA_1;
        for (i=0; i<8; i++)
        {
            Delay(5);
            I2C_CH_C_SCL_1;
            Delay(5);
            if (I2C_CH_C_SDA_READ == 0x01 )
            {
                TempBit = 1;
            }
            else
            {
                TempBit = 0;
            }
            TempData = (TempData << 1) | TempBit;
            I2C_CH_C_SCL_0;
            Delay(5);
        }
        Delay(5);
        return(TempData);
    }
    else
        return 255;   // no channel defined, do nothing..
}

uint8_t ReadByte(void)
{
    uint8_t i;
    uint8_t TempBit  = 0;
    uint8_t TempData = 0;
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

void multi_I2C_ReceiveAck(uint8_t ch)
{
    uint8_t i = 0;
    if(ch == LP5012_I2C_CH_A)
    {
        I2C_CH_A_SCL_1;
        Delay(5);
        I2C_CH_A_SDA_IN;
        while ((I2C_CH_A_SDA_READ == 0x01) && (i < 255))
        {
            i++;
        }
        I2C_CH_A_SDA_OUT;
        I2C_CH_A_SCL_0;
        Delay(5);
    }
    else if(ch == LP5012_I2C_CH_B)
    {
        I2C_CH_B_SCL_1;
        Delay(5);
        I2C_CH_B_SDA_IN;
        while ((I2C_CH_B_SDA_READ == 0x01) && (i < 255))
        {
            i++;
        }
        I2C_CH_B_SDA_OUT;
        I2C_CH_B_SCL_0;
        Delay(5);
    }
    else if(ch == LP5012_I2C_CH_C)
    {
        I2C_CH_C_SCL_1;
        Delay(5);
        I2C_CH_C_SDA_IN;
        while ((I2C_CH_C_SDA_READ == 0x01) && (i < 255))
        {
            i++;
        }
        I2C_CH_C_SDA_OUT;
        I2C_CH_C_SCL_0;
        Delay(5);
    }
    else
        ;   // no channel defined, do nothing..
}

void ReceiveAck(void)
{
    uint8_t i = 0;
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

void multi_I2C_Acknowledge(uint8_t ch)
{
    if(ch == LP5012_I2C_CH_A)
    {
        I2C_CH_A_SCL_0;
        Delay(5);
        I2C_CH_A_SDA_OUT;
        I2C_CH_A_SDA_0;
        I2C_CH_A_SCL_1;
        Delay(10);
        I2C_CH_A_SCL_0;
    }
    else if(ch == LP5012_I2C_CH_B)
    {
        I2C_CH_B_SCL_0;
        Delay(5);
        I2C_CH_B_SDA_OUT;
        I2C_CH_B_SDA_0;
        I2C_CH_B_SCL_1;
        Delay(10);
        I2C_CH_B_SCL_0;
    }
    else if(ch == LP5012_I2C_CH_C)
    {
        I2C_CH_C_SCL_0;
        Delay(5);
        I2C_CH_C_SDA_OUT;
        I2C_CH_C_SDA_0;
        I2C_CH_C_SCL_1;
        Delay(10);
        I2C_CH_C_SCL_0;
    }
    else
        ;   // no channel defined, do nothing..
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

void multi_I2C_SetDataAsInput(uint8_t ch)
{
    if(ch == LP5012_I2C_CH_A)
    {
        I2C_CH_A_SDA_IN;
    }
    else if(ch == LP5012_I2C_CH_B)
    {
        I2C_CH_B_SDA_IN;
    }
    else if(ch == LP5012_I2C_CH_C)
    {
        I2C_CH_C_SDA_IN;
    }
    else
        ;   // no channel defined, do nothing..
}
void SetDataAsInput(void)
{
    SDA_IN;
}

/////////////////////////////////////////////////////////////////////////////////
// I2C GPIO control HAL code
/////////////////////////////////////////////////////////////////////////////////
#define I2C_WR_BIT      0
#define I2C_RD_BIT      1

void i2c_read_byte_multi_ch(uint8_t ch, uint8_t slave_7bit_addr, uint8_t reg_addr, uint8_t *data)
{
    //begin read command for 0x1A
    multi_I2C_Start(ch);
    multi_I2C_WriteByte(ch, slave_7bit_addr<<1 | I2C_WR_BIT);        // write command
    multi_I2C_WriteByte(ch, reg_addr);     // register address
    multi_I2C_Stop(ch);

    //read 0x1A
    multi_I2C_Start(ch);
    multi_I2C_WriteByte(ch, slave_7bit_addr<<1 | I2C_RD_BIT);        // read command
    multi_I2C_SetDataAsInput(ch);

    //read register with Nack
    *data = multi_I2C_ReadByte(ch);
    multi_I2C_Acknowledge(ch);
    multi_I2C_Stop(ch);

}

void i2c_read_byte(uint8_t slave_7bit_addr, uint8_t reg_addr, uint8_t *data)
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

void i2c_write_byte_multi_ch(uint8_t ch, uint8_t slave_7bit_addr, uint8_t reg_addr, uint8_t data)
{
    multi_I2C_Start(ch);
    multi_I2C_WriteByte(ch, slave_7bit_addr<<1 | I2C_WR_BIT);        // write command
    multi_I2C_WriteByte(ch, reg_addr);     // register address
    multi_I2C_WriteByte(ch, data);        // data to be written
    multi_I2C_Stop(ch);
}

void i2c_write_byte(uint8_t slave_7bit_addr, uint8_t reg_addr, uint8_t data)
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

void LP5012_multi_ch_I2C_chip_enable(uint8_t ch, uint8_t slave_addr, char en)
{
    if(en==1)
    {
        i2c_write_byte_multi_ch(ch, slave_addr, LP5012_DEVICE_CONFIG0, 0x40 );
    }
    else
    {
        i2c_write_byte_multi_ch(ch, slave_addr, LP5012_DEVICE_CONFIG0, 0x00 );
    }
}
void LP5012_I2C_chip_enable(uint8_t slave_addr, char en)
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

void LP5012_multi_ch_I2C_chip_reset(uint8_t ch, uint8_t slave_addr, char en)
{
    if(en == 1)
    {
        i2c_write_byte_multi_ch(ch, slave_addr, LP5012_RESET, 0xFF );
    }
    else
        ;

    delay_ms(10);
}
void LP5012_I2C_chip_reset(uint8_t slave_addr, char en)
{
    if(en == 1)
    {
        i2c_write_byte(slave_addr, LP5012_RESET, 0xFF );
    }
    else
        ;

    delay_ms(10);
}

void LP5012_multi_ch_I2C_IC_init(uint8_t ch, uint8_t slave_addr, uint8_t data)
{
    // <data>
    // Bit 5 : log scale dimming        - linear (0) or log scale (1)
    // Bit 4 : auto power saving mode   - not enabled (0) or enabled(1)
    // Bit 3 : auto increment mode      - not enabled (0) or enabled (1)
    // Bit 2 : PWM dithering mode       - not enabled (0) or enabled (1)
    // Bit 1 : Output max current       - 25.5 mA (0) or 35 mA (1)
    i2c_write_byte_multi_ch(ch, slave_addr, LP5012_DEVICE_CONFIG1, data);
}
void LP5012_I2C_IC_init(uint8_t slave_addr, uint8_t data)
{
    // <data>
    // Bit 5 : log scale dimming        - linear (0) or log scale (1)
    // Bit 4 : auto power saving mode   - not enabled (0) or enabled(1)
    // Bit 3 : auto increment mode      - not enabled (0) or enabled (1)
    // Bit 2 : PWM dithering mode       - not enabled (0) or enabled (1)
    // Bit 1 : Output max current       - 25.5 mA (0) or 35 mA (1)
    i2c_write_byte(slave_addr, LP5012_DEVICE_CONFIG1, data);
}

void LP5012_multi_ch_I2C_LED_global_onoff(uint8_t ch, uint8_t slave_addr, char onoff)
{
    uint8_t temp;
    i2c_read_byte_multi_ch(ch, slave_addr, LP5012_DEVICE_CONFIG1, &temp);

    // when 1 : global off
    if(onoff == 1)
        i2c_write_byte_multi_ch(ch, slave_addr, LP5012_DEVICE_CONFIG1, temp & (~onoff) );
    else
        i2c_write_byte_multi_ch(ch, slave_addr, LP5012_DEVICE_CONFIG1, temp | onoff );
}
void LP5012_I2C_LED_global_onoff(uint8_t slave_addr, char onoff)
{
    uint8_t temp;
    i2c_read_byte(slave_addr, LP5012_DEVICE_CONFIG1, &temp);

    // when 1 : global off
    if(onoff == 1)
        i2c_write_byte(slave_addr, LP5012_DEVICE_CONFIG1, temp & (~onoff) );
    else
        i2c_write_byte(slave_addr, LP5012_DEVICE_CONFIG1, temp | onoff );
}

void LP5012_multi_ch_I2C_bank_control_mode(uint8_t ch, uint8_t slave_addr, char onoff)
{
    if(onoff == 1)
        i2c_write_byte_multi_ch(ch, slave_addr, LP5012_LED_CONFIG0, 0x0F );
    else
        i2c_write_byte_multi_ch(ch, slave_addr, LP5012_LED_CONFIG0, 0x00 );
}
void LP5012_I2C_bank_control_mode(uint8_t slave_addr, char onoff)
{
    if(onoff == 1)
        i2c_write_byte(slave_addr, LP5012_LED_CONFIG0, 0x0F );
    else
        i2c_write_byte(slave_addr, LP5012_LED_CONFIG0, 0x00 );
}

void LP5012_multi_ch_I2C_bank_brightness(uint8_t ch, uint8_t slave_addr, uint8_t brightness)
{
    i2c_write_byte_multi_ch(ch, slave_addr, LP5012_BANK_BRIGHTNESS, brightness );
}
void LP5012_I2C_bank_brightness(uint8_t slave_addr, uint8_t brightness)
{
    i2c_write_byte(slave_addr, LP5012_BANK_BRIGHTNESS, brightness );
}

void LP5012_multi_ch_I2C_bank_color(uint8_t ch, uint8_t slave_addr, uint8_t red, uint8_t green, uint8_t blue)
{
    i2c_write_byte_multi_ch(ch, slave_addr, LP5012_BANK_A_COLOR, red );
    i2c_write_byte_multi_ch(ch, slave_addr, LP5012_BANK_B_COLOR, green );
    i2c_write_byte_multi_ch(ch, slave_addr, LP5012_BANK_C_COLOR, blue );
}
void LP5012_I2C_bank_color(uint8_t slave_addr, uint8_t red, uint8_t green, uint8_t blue)
{
    i2c_write_byte(slave_addr, LP5012_BANK_A_COLOR, red );
    i2c_write_byte(slave_addr, LP5012_BANK_B_COLOR, green );
    i2c_write_byte(slave_addr, LP5012_BANK_C_COLOR, blue );
}

void LP5012_multi_ch_I2C_LEDx_brightness(uint8_t ch, uint8_t slave_addr, uint8_t channel, uint8_t brightness)
{
    i2c_write_byte_multi_ch(ch, slave_addr, LP5012_LED0_BRIGHTNESS + channel, brightness );
}
void LP5012_I2C_LEDx_brightness(uint8_t slave_addr, uint8_t channel, uint8_t brightness)
{
    i2c_write_byte(slave_addr, LP5012_LED0_BRIGHTNESS + channel, brightness );
}

void LP5012_multi_ch_I2C_LEDx_ch_brightness(uint8_t ch, uint8_t slave_addr, uint8_t led0_br, uint8_t led1_br, uint8_t led2_br, uint8_t led3_br)
{
    i2c_write_byte_multi_ch(ch, slave_addr, LP5012_LED0_BRIGHTNESS, led0_br );
    i2c_write_byte_multi_ch(ch, slave_addr, LP5012_LED1_BRIGHTNESS, led1_br );
    i2c_write_byte_multi_ch(ch, slave_addr, LP5012_LED2_BRIGHTNESS, led2_br );
    i2c_write_byte_multi_ch(ch, slave_addr, LP5012_LED3_BRIGHTNESS, led3_br );
}
void LP5012_I2C_LEDx_ch_brightness(uint8_t slave_addr, uint8_t led0_br, uint8_t led1_br, uint8_t led2_br, uint8_t led3_br)
{
    i2c_write_byte(slave_addr, LP5012_LED0_BRIGHTNESS, led0_br );
    i2c_write_byte(slave_addr, LP5012_LED1_BRIGHTNESS, led1_br );
    i2c_write_byte(slave_addr, LP5012_LED2_BRIGHTNESS, led2_br );
    i2c_write_byte(slave_addr, LP5012_LED3_BRIGHTNESS, led3_br );
}

void LP5012_multi_ch_I2C_OUTx_color(uint8_t ch, uint8_t slave_addr, uint8_t channel, uint8_t color)
{
    i2c_write_byte_multi_ch(ch, slave_addr, LP5012_OUT0_COLOR + channel, color );
}
void LP5012_I2C_OUTx_color(uint8_t slave_addr, uint8_t channel, uint8_t color)
{
    i2c_write_byte(slave_addr, LP5012_OUT0_COLOR + channel, color );
}



