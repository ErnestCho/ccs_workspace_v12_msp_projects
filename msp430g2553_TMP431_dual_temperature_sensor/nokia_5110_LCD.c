#include <msp430.h>

#include "nokia_5110_LCD.h"

#include "font_5x7.h"
#include "font_6x8.h"
#include "font_14x16.h"
#include "font_Franklin_Gothic_Medium_Cond10x15.h"
#include "font_MS_Gothic8x16.h"


const font fonts[FONT_MAX] =
{
    //FONT_5x7
    {
        .width = 5,
        .height = 7,
        .data = &font_5x7_raw[0],
    },
    //FONT_6x8
    {
        .width = 6,
        .height = 8,
        .data = &font_6x8_raw[0],
    },
    //FONT_14x16
    {
        .width = 14,
        .height = 16,
        .data = &font_14x16_raw[0],
    },
    //FONT_FRANKLIN_GOTHIC_MEDIUM_COND10x15
    {
        .width = 10,
        .height = 15,
        .data = &Franklin_Gothic_Medium_Cond10x15[0],
    },
    //FONT_MS_GOTHIC8x16
    {
        .width = 8,
        .height = 16,
        .data = &MS_Gothic8x16[0],
    },
};

#if 0
// font_5x7.h
const font font5x7 =
{
    .width = 5,
    .height = 7,
    .data = &font_5x7_raw[0][0],
};
// font_6x8.h
const font font6x8 =
{
    .width = 6,
    .height = 8,
    .data = &font_6x8_raw[0][0],
};
// font_14x16.h
const font font14x16 =
{
    .width = 14,
    .height = 16,
    .data = &font_14x16_raw[0][0],
};
#endif

#if 1
void writeStringToLCD(char y, const char *string, font_idx font_id)
{
    char x = 0;
    clearBank(y);

    while(*string)
    {
        writeCharToLCD(x, y, *string++, font_id);
        x++;
    }
}

void writeCharToLCD(char x, char y, char c, font_idx font_id)
{
    unsigned char i;
    if(fonts[font_id].height <= 8)
    {
        setAddr(x*fonts[font_id].width, y);
        for(i = 0; i < fonts[font_id].width; i++)
        {
            writeToLCD(LCD5110_DATA, *(fonts[font_id].data + (c-0x20)*fonts[font_id].width +i) );
        }
        writeToLCD(LCD5110_DATA, 0);
    }
    else if( (fonts[font_id].height > 8) && (fonts[font_id].height <= 16) )
    {
        setAddr(x*fonts[font_id].width, y);
        for(i = 0; i < fonts[font_id].width; i++)
        {
            writeToLCD(LCD5110_DATA, *(fonts[font_id].data+(c-0x20)*fonts[font_id].width*2 +(i*2) ) );
        }

        setAddr(x*fonts[font_id].width, y+1);
        for(i = 0; i < fonts[font_id].width; i++)
        {
            writeToLCD(LCD5110_DATA, *(fonts[font_id].data+(c-0x20)*fonts[font_id].width*2 +(i*2+1) ) );
        }
        writeToLCD(LCD5110_DATA, 0);
    }
    else
    {
        setAddr(x*fonts[font_id].width, y);
        for(i = 0; i < fonts[font_id].width; i++)
        {
            writeToLCD(LCD5110_DATA, (*(fonts[font_id].data+(c-0x20) )+i) );
        }

        writeToLCD(LCD5110_DATA, 0);
    }
}

#else
void writeStringToLCD2(char y, const char *string)
{
    char x = 0;
    while(*string)
    {
        writeCharToLCD2(x, y, *string++);
        x++;
    }
}
void writeStringToLCD(const char *string)
{
    while(*string)
    {
        writeCharToLCD(*string++);
    }
}
void writeCharToLCD_fontsize(unsigned char font_xy, char c)
{
    unsigned char i;
    if(font_xy == FONT_5x7)
    {
        for(i = 0; i < 5; i++)
        {
            writeToLCD(LCD5110_DATA, font_5x7[c - 0x20][i]);
        }
    }
    else if(font_xy == FONT_6x8)
    {
        for(i = 0; i < 6; i++)
        {
            writeToLCD(LCD5110_DATA, font_6x8[c - 0x20][i]);
        }
    }
    writeToLCD(LCD5110_DATA, 0);
}
void writeCharToLCD2(char x, char y, char c)
{
    unsigned char i;
    setAddr(x*14, y);
    for(i = 0; i < 14; i++)
    {
        writeToLCD(LCD5110_DATA, font_14x16[c - 0x20][i*2]);
    }
    setAddr(x*14, y+1);
    for(i = 0; i < 14; i++)
    {
        writeToLCD(LCD5110_DATA, font_14x16[c - 0x20][i*2+1]);
    }
    writeToLCD(LCD5110_DATA, 0);
}


void writeCharToLCD(char c)
{
    unsigned char i;
    for(i = 0; i < 6; i++)
    {
        writeToLCD(LCD5110_DATA, font_6x8[c - 0x20][i]);
    }
    writeToLCD(LCD5110_DATA, 0);
}
#endif


void writeBlockToLCD(char *byte, unsigned char length)
{
    unsigned char c = 0;
    while(c < length)
    {
        writeToLCD(LCD5110_DATA, *byte++);
        c++;
    }
}

void writeGraphicToLCD(char *byte, unsigned char transform)
{
    int c = 0;
    char block[8];

    if(transform & FLIP_V)
    {
        SPI_LSB_FIRST;
    }
    if(transform & ROTATE)
    {
        c = 1;
        while(c != 0)
        {
            (*byte & 0x01) ? (block[7] |= c) : (block[7] &= ~c);
            (*byte & 0x02) ? (block[6] |= c) : (block[6] &= ~c);
            (*byte & 0x04) ? (block[5] |= c) : (block[5] &= ~c);
            (*byte & 0x08) ? (block[4] |= c) : (block[4] &= ~c);
            (*byte & 0x10) ? (block[3] |= c) : (block[3] &= ~c);
            (*byte & 0x20) ? (block[2] |= c) : (block[2] &= ~c);
            (*byte & 0x40) ? (block[1] |= c) : (block[1] &= ~c);
            (*byte & 0x80) ? (block[0] |= c) : (block[0] &= ~c);
            *byte++;
            c <<= 1;
        }
    }
    else
    {
        while(c < 8)
        {
            block[c++] = *byte++;
        }
    }

    if(transform & FLIP_H)
    {
        c = 7;
        while(c > -1)
        {
            writeToLCD(LCD5110_DATA, block[c--]);
        }
    }
    else
    {
        c = 0;
        while(c < 8)
        {
            writeToLCD(LCD5110_DATA, block[c++]);
        }
    }
    SPI_MSB_FIRST;
}

void writeToLCD(unsigned char dataCommand, unsigned char data)
{
    LCD5110_SELECT;
    if(dataCommand)
    {
        LCD5110_SET_DATA;
    }
    else
    {
        LCD5110_SET_COMMAND;
    }
#if defined USE_SPI_UCSI_A0
    UCA0TXBUF = data;
    while(!(IFG2 & UCA0TXIFG))
        ;
#else
    UCB0TXBUF = data;
    while(!(IFG2 & UCB0TXIFG))
        ;
#endif
    LCD5110_DESELECT;
}

void clearLCD(void)
{
    setAddr(0, 0);
    int c = 0;
    while(c < PCD8544_MAXBYTES)
    {
        writeToLCD(LCD5110_DATA, 0);
        c++;
    }
    setAddr(0, 0);
}

void clearBank(unsigned char bank)
{
    setAddr(0, bank);
    int c = 0;
    while(c < PCD8544_HPIXELS)
    {
        writeToLCD(LCD5110_DATA, 0);
        c++;
    }
    setAddr(0, bank);
}

void setAddr(unsigned char xAddr, unsigned char yAddr)
{
    writeToLCD(LCD5110_COMMAND, PCD8544_SETXADDR | xAddr);
    writeToLCD(LCD5110_COMMAND, PCD8544_SETYADDR | yAddr);
}

void initLCD(void)
{
    writeToLCD(LCD5110_COMMAND, PCD8544_FUNCTIONSET | PCD8544_EXTENDEDINSTRUCTION); // horizontal addressing
    writeToLCD(LCD5110_COMMAND, PCD8544_SETVOP | 0x3F);
    writeToLCD(LCD5110_COMMAND, PCD8544_SETTEMP | 0x02);
    writeToLCD(LCD5110_COMMAND, PCD8544_SETBIAS | 0x03);
    writeToLCD(LCD5110_COMMAND, PCD8544_FUNCTIONSET);
    writeToLCD(LCD5110_COMMAND, PCD8544_DISPLAYCONTROL | PCD8544_DISPLAYNORMAL);
}
