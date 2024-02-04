
#include <HDC1080_I2C_GPIO.h>


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

void Nacknowledge(void)
{
    SCL_0;
    Delay(5);
    SDA_OUT;
    SDA_1;
    Delay(5);
    SCL_1;
    Delay(5);
    SCL_0;
    SDA_0;
    Delay(5);
}


/////////////////////////////////////////////////////////////////////////////////
// I2C GPIO control HAL code
/////////////////////////////////////////////////////////////////////////////////
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

void i2c_write_word(unsigned char slave_7bit_addr, unsigned char reg_addr, unsigned short wdata)
{
    unsigned char data[2];
    data[1] = (wdata>>8) & 0xff;
    data[0] = wdata & 0xff;

    Start();
    WriteByte(slave_7bit_addr<<1 | I2C_WR_BIT);            // send device address + WR
    WriteByte(reg_addr);        // send register address
    WriteByte(data[1]);         // send value to write register (higher byte)
    WriteByte(data[0]);         // send value to write register (lower byte)
    Stop();

}
unsigned char ret_char[4]={0,0,0,0};
unsigned short ret_value = 0;
void i2c_read_word(unsigned char slave_7bit_addr, unsigned char reg_Addr, unsigned short *wdata)
{

    //begin the read command for each register
    Start();
    WriteByte(slave_7bit_addr<<1 | I2C_WR_BIT);                        // send device address + WR
    WriteByte(reg_Addr);
    Stop();

    //send the read command for each register
    Start();
    WriteByte(slave_7bit_addr<<1 | I2C_RD_BIT);                        // send device address + RD
    SDA_IN;

    ///read register with Nack
    ret_char[1] = ReadByte();               // read higher byte register
    Acknowledge();

    SDA_IN;
    ret_char[0] = ReadByte();               // read lower byte register
    Acknowledge();

    Stop();

    ret_value = (ret_char[1]<<8) + ret_char[0];
    *wdata = ret_value;
}

void i2c_sequential_read_temp_humid(uint8_t slave_7bit_addr, uint16_t *temp_raw, uint16_t *humid_raw)
{
    //begin the read command for each register
    Start();
    WriteByte(slave_7bit_addr<<1 | I2C_WR_BIT);                        // send device address + WR
    WriteByte(0x00);
    Stop();

    // wait for over 10ms to complete the conversion
    delay_ms(100);



    //read two 16bit data
    Start();
    WriteByte(slave_7bit_addr<<1 | I2C_RD_BIT);                        // send device address + RD
    SDA_IN;

    ///read register with Nack
    ret_char[1] = ReadByte();               // read higher byte register
    Acknowledge();

    SDA_IN;
    ret_char[0] = ReadByte();               // read lower byte register
    Acknowledge();

    delay_ms(5);
    ///read register with Nack
    SDA_IN;
    ret_char[2] = ReadByte();               // read higher byte register
    Acknowledge();

    SDA_IN;
    ret_char[3] = ReadByte();               // read lower byte register
    Nacknowledge();

    Stop();

    ret_value = (ret_char[1]<<8) + ret_char[0];
    *temp_raw = ret_value;

    ret_value = (ret_char[3]<<8) + ret_char[2];
    *humid_raw = ret_value;

}

void i2c_individual_read_temp_humid(uint8_t slave_7bit_addr, uint16_t *temp_raw, uint16_t *humid_raw)
{
    //begin the read command for each register
    Start();
    WriteByte(slave_7bit_addr<<1 | I2C_WR_BIT);                        // send device address + WR
    WriteByte(0x00);                            // 0x00 for temperature
    Stop();

    // wait for over 10ms to complete the conversion
    delay_ms(50);



    //read two 16bit data
    Start();
    WriteByte(slave_7bit_addr<<1 | I2C_RD_BIT);                        // send device address + RD
    SDA_IN;

    ///read register with Nack
    ret_char[1] = ReadByte();               // read higher byte register
    Acknowledge();

    SDA_IN;
    ret_char[0] = ReadByte();               // read lower byte register
    Acknowledge();

    Stop();




    //begin the read command for each register
    Start();
    WriteByte(slave_7bit_addr<<1 | I2C_WR_BIT);                        // send device address + WR
    WriteByte(0x01);                            // 0x00 for temperature
    Stop();

    // wait for over 10ms to complete the conversion
    delay_ms(50);



    //read two 16bit data
    Start();
    WriteByte(slave_7bit_addr<<1 | I2C_RD_BIT);                        // send device address + RD
    SDA_IN;

    ///read register with Nack
    ret_char[3] = ReadByte();               // read higher byte register
    Acknowledge();

    SDA_IN;
    ret_char[2] = ReadByte();               // read lower byte register
    Acknowledge();

    ret_value = (ret_char[1]<<8) + ret_char[0];
    *temp_raw = ret_value;

    ret_value = (ret_char[3]<<8) + ret_char[2];
    *humid_raw = ret_value;

}

/////////////////////////////////////////////////////////////////////////////////
// HDC1080 related I2C function
/////////////////////////////////////////////////////////////////////////////////

void GPIO_I2C_Init(void)
{
    // LP5012
    SCL_1;
    SDA_1;
    SCL_OUT;
    SDA_OUT;
    //SCL_PU;       // if external pull up is existed remove internal pull up setting
    //SDA_PU;       // if external pull up is existed remove internal pull up setting
}

uint8_t HDC1080_I2C_check_device_valid(uint8_t slave_addr)
{
    unsigned short manuf_id = 0, dev_id = 0;

    i2c_read_word(slave_addr, HDC1080_MANUFACTURER_ID, &manuf_id);
    i2c_read_word(slave_addr, HDC1080_DEVICE_ID, &dev_id);

    if(manuf_id == HDC1080_MANU_ID_VAL && dev_id == HDC1080_DEV_ID_VAL)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

uint16_t HDC1080_I2C_read_temperature(uint8_t slave_addr)
{
    unsigned short temperature_raw_data = 0;

    i2c_read_word(slave_addr, HDC1080_TEMPERATURE_RAW, &temperature_raw_data);

    return temperature_raw_data;
}

uint16_t HDC1080_I2C_read_humidity(uint8_t slave_addr)
{
    unsigned short humidity_raw_data = 0;

    i2c_read_word(slave_addr, HDC1080_HUMIDITY_RAW, &humidity_raw_data);

    return humidity_raw_data;
}


void HDC1080_I2C_read_temp_and_humidity(uint8_t slave_addr, int16_t *temp_x10, uint16_t *humid_x10)
{
    uint16_t temperature_raw_data = 0;
    uint16_t humidity_raw_data = 0;
    long l_temp_x10, l_humid_x10;

#if 0   // sequential read
    // sequential read : TEMP then HUMID
    i2c_write_word(slave_addr, HDC1080_CONFIG, (HDC1080_MODE_SEQUENCE_ACQ + HDC1080_TEMP_RESOLUTION_14BIT + HDC1080_HUMID_RESOLUTION_14BIT) );
    delay_ms(5);


    i2c_sequential_read_temp_humid(slave_addr, &temperature_raw_data, &humidity_raw_data);

#else   // individual read
    i2c_write_word(slave_addr, HDC1080_CONFIG, (HDC1080_MODE_SINGLE_ACQ + HDC1080_TEMP_RESOLUTION_14BIT + HDC1080_HUMID_RESOLUTION_14BIT) );
    delay_ms(5);

    i2c_individual_read_temp_humid(slave_addr, &temperature_raw_data, &humidity_raw_data);

#endif
    l_temp_x10 =  (((long)temperature_raw_data * 1650)  >> 16)  - 400 ;   // celsius
    l_humid_x10 = ((long)humidity_raw_data * 1000)  >> 16  ;      // %RH (relative humidity)

    *temp_x10 =  (uint16_t) l_temp_x10;
    *humid_x10 = (uint16_t) l_humid_x10;

}


#if 0
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

void LP5012_I2C_bank_brightness(unsigned char slave_addr, unsigned char brightness)
{
    i2c_write_byte(slave_addr, LP5012_BANK_BRIGHTNESS, brightness );
}

void LP5012_I2C_bank_color(unsigned char slave_addr, unsigned char red, unsigned char green, unsigned char blue)
{
    i2c_write_byte(slave_addr, LP5012_BANK_A_COLOR, red );
    i2c_write_byte(slave_addr, LP5012_BANK_B_COLOR, green );
    i2c_write_byte(slave_addr, LP5012_BANK_C_COLOR, blue );
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
#endif


