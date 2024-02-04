
#include <i2c_gpio.h>

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

