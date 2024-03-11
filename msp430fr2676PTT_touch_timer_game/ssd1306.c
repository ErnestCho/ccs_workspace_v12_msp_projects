/*
 * ssd1306.c
 */

#include <msp430.h>
#include <stdint.h>

#include "ssd1306.h"
#if 0
#include "font_5x7.h"
#include "font_6x8.h"
#include "font_14x16.h"
#include "font_Franklin_Gothic_Medium_Cond10x15.h"
#include "font_MS_Gothic8x16.h"
#else
#include "font_6x8.h"
#include "font_5x7_zero_enc.h"
#include "font_6x8_zero_enc.h"
#include "font_14x16_zero_enc.h"
#include "font_Franklin_Gothic_Medium_Cond10x15_zero_enc.h"
#include "font_MS_Gothic8x16_zero_enc.h"
#endif

#include "i2c.h"
#include "common.h"


CONST font fonts[FONT_MAX] =
{
#if 0   // memory size limit
    //FONT_5x7
    {
        .width = 5,
        .height = 7,
        .data = &font_5x7_raw[0],
    },
#endif
    //FONT_6x8
    {
        .width = 6,
        .height = 8,
        .data_type = DATA_TYPE_RAW, //DATA_TYPE_ZERO_ENC
        .data = &font_6x8_raw[0][0], //&font_6x8__zero_enc_raw[0],
        .offset = NULL, //&font_6x8_raw_offset[0],
    },
#if 0   // memory size limit
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
#endif
    //FONT_MS_GOTHIC8x16
    {
        .width = 8,
        .height = 16,
        .data_type = DATA_TYPE_ZERO_ENC, //DATA_TYPE_RAW
        .data = &MS_Gothic8x16[0],
#if 1
        .offset = (uint16_t*)INFOMATION_FRAM_BASE,
#else
        .offset = &MS_Gothic8x16_offset[0],
#endif
    },
};

#if 0
/* ====================================================================
 * Horizontal Centering Number Array
 * ==================================================================== */
const unsigned char HcenterUL[] = {                                           // Horizontally center number with separators on screen
                               0,                                       // 0 digits, not used but included to size array correctly
                               61,                                      // 1 digit
                               58,                                      // 2 digits
                               55,                                      // 3 digits
                               49,                                      // 4 digits and 1 separator
                               46,                                      // 5 digits and 1 separator
                               43,                                      // 6 digits and 1 separator
                               37,                                      // 7 digits and 2 separators
                               34,                                      // 8 digits and 2 separators
                               31,                                      // 9 digits and 2 separators
                               25                                       // 10 digits and 3 separators
};
#endif
CONST unsigned char init_cmd_array[]=
{
     0xAE,          // DISPLAY OFF
     0xD5,          // SET OSC FREQUENY
         0x80,          // divide ratio = 1 (bit 3-0), OSC (bit 7-4)
     0xA8,          // SET MUX RATIO
         0x3F,          // 64MUX
     0xD3,          // SET DISPLAY OFFSET
         0x00,          // offset = 0
     0x40,          // set display start line, start line = 0
     0x8D,          // ENABLE CHARGE PUMP REGULATOR
         0x14,          //
     0x20,          // SET MEMORY ADDRESSING MODE
         0x02,          // horizontal addressing mode
     0xA1,          // set segment re-map, column address 127 is mapped to SEG0
     0xC8,          // set COM/Output scan direction, remapped mode (COM[N-1] to COM0)
     0xDA,          // SET COM PINS HARDWARE CONFIGURATION
         0x12,          // alternative COM pin configuration
     0x81,          // SET CONTRAST CONTROL
         0xCF,          //
     0xD9,          // SET PRE CHARGE PERIOD
         0xF1,          //
     0xDB,          // SET V_COMH DESELECT LEVEL
         0x40,          //
     0xA4,          // DISABLE ENTIRE DISPLAY ON
     0xA6,          // NORMAL MODE (A7 for inverse display)
     0xAF           // DISPLAY ON
};

void ssd1306_init(void) {
#if 1
    unsigned char cnt;

    for(cnt=0; cnt<sizeof(init_cmd_array); cnt++)
    {
        ssd1306_command(init_cmd_array[cnt]);
    }
#elif AAA
    ssd1306_command(0xAE);          // DISPLAY OFF);
    ssd1306_command(0xD5);          // SET OSC FREQUENY);
        ssd1306_command(0x80);          // divide ratio = 1 (bit 3-0), OSC (bit 7-4));
    ssd1306_command(0xA8);          // SET MUX RATIO);
        ssd1306_command(0x3F);          // 64MUX);
    ssd1306_command(0xD3);          // SET DISPLAY OFFSET);
        ssd1306_command(0x00);          // offset = 0);
    ssd1306_command(0x40);          // set display start line, range : 0x40~0x7F
    ssd1306_command(0x8D);          // ENABLE CHARGE PUMP REGULATOR);
        ssd1306_command(0x14);          // );

    ssd1306_command(0x20);          // SET MEMORY ADDRESSING MODE);
    // select one of the three
        //ssd1306_command(0x00);          // horizontal addressing mode);
        //ssd1306_command(0x01);          // vertical addressing mode);
        ssd1306_command(0x02);          // page addressing mode);

    ssd1306_command(0xA1);          // set segment re-map, column address 127 is mapped to SEG0);
    ssd1306_command(0xC8);          // set COM/Output scan direction, remapped mode (COM[N-1] to COM0));
    ssd1306_command(0xDA);          // SET COM PINS HARDWARE CONFIGURATION);
        ssd1306_command(0x12);          // alternative COM pin configuration );
    ssd1306_command(0x81);          // SET CONTRAST CONTROL);
    ssd1306_command(0xCF);          // );
    ssd1306_command(0xD9);          // SET PRE CHARGE PERIOD);
    ssd1306_command(0xF1);          // );
    ssd1306_command(0xDB);          // SET V_COMH DESELECT LEVEL);
    ssd1306_command(0x40);          // );
    ssd1306_command(0xA4);          // DISABLE ENTIRE DISPLAY ON);
    ssd1306_command(0xA6);          // NORMAL MODE (A7 for inverse display));
    ssd1306_command(0xAF);          // DISPLAY ON);

#elif TEST_USE
    ssd1306_command(SSD1306_DISPLAYOFF);            // 0xAE : Display off
    ssd1306_command(SSD1306_SETLOWCOLUMN);          // 0x00
    ssd1306_command(SSD1306_SETHIGHCOLUMN);         // 0x10
    ssd1306_command(SSD1306_SETSTARTLINE);          // 0x40
    ssd1306_command(SSD1306_SETCONTRAST);           // 0x81
    ssd1306_command(0x7f);                            // data : contrast value
    ssd1306_command(SSD1306_SEGREMAP);              // 0xA1
    ssd1306_command(SSD1306_NORMALDISPLAY);         // 0xA6 : Normal, 0xA7 : Inverse display
    ssd1306_command(SSD1306_SETMULTIPLEX);          // 0xA8 : multiplex ratio
    ssd1306_command(0x3f);                            // data : 0x3F
    ssd1306_command(SSD1306_SETDISPLAYOFFSET);      // 0xD3
    ssd1306_command(0x00);                            // data : 0x00
    ssd1306_command(SSD1306_SETDISPLAYCLOCKDIV);    // 0xD5
    ssd1306_command(0x80);                            // data : 0x80
    ssd1306_command(SSD1306_SETPRECHARGE);          // 0xD9
    ssd1306_command(0xf1);                            // data : 0xf1
    ssd1306_command(SSD1306_SETCOMPINS);            // 0xDA
    ssd1306_command(0x12);                            // data : 0x12
    ssd1306_command(SSD1306_SETVCOMDETECT);         // 0xDB
    ssd1306_command(0x40);                            // data : 0x40
    ssd1306_command(SSD1306_CHARGEPUMP);            // 0x8D
    ssd1306_command(0x14);                            // data : 0x14
    ssd1306_command(SSD1306_DISPLAYON);             // 0xAF


#else
    // SSD1306 init sequence
    ssd1306_command(SSD1306_DISPLAYOFF);                                // 0xAE
//    ssd1306_command(SSD1306_SETDISPLAYCLOCKDIV);                        // 0xD5
//    ssd1306_command(0x80);                                              // the suggested ratio 0x80

    ssd1306_command(SSD1306_SETMULTIPLEX);                              // 0xA8
    ssd1306_command(SSD1306_LCDHEIGHT - 1);

    ssd1306_command(SSD1306_SETDISPLAYOFFSET);                          // 0xD3
    ssd1306_command(0x0);                                               // no offset
    ssd1306_command(SSD1306_SETSTARTLINE | 0x0);                        // line #0
    ssd1306_command(SSD1306_CHARGEPUMP);                                // 0x8D
    ssd1306_command(0x14);                                              // generate high voltage from 3.3v line internally
    ssd1306_command(SSD1306_MEMORYMODE);                                // 0x20
    ssd1306_command(0x00);                                              // 0x0 : horizontal
    ssd1306_command(SSD1306_SEGREMAP | 0x1);                        // SEG setting
    ssd1306_command(SSD1306_COMSCANDEC);                            // COM setting

    ssd1306_command(SSD1306_SETCOMPINS);                                // 0xDA
    ssd1306_command(0x12);
    ssd1306_command(SSD1306_SETCONTRAST);                               // 0x81
    ssd1306_command(0xCF);

    ssd1306_command(SSD1306_SETPRECHARGE);                              // 0xd9
    ssd1306_command(0xF1);
    ssd1306_command(SSD1306_SETVCOMDETECT);                             // 0xDB
    ssd1306_command(0x40);
    ssd1306_command(SSD1306_DISPLAYALLON_RESUME);                       // 0xA4
    ssd1306_command(SSD1306_NORMALDISPLAY);                             // 0xA6

    ssd1306_command(SSD1306_DEACTIVATE_SCROLL);

    ssd1306_command(SSD1306_DISPLAYON);                                 //--turn on oled panel
#endif

} // end ssd1306_init
uint8_t cmd_buf[2] = {0, 0};
void ssd1306_command(unsigned char command)
{
    cmd_buf[0] = 0x80;
    cmd_buf[1] = command;

    i2c_write(SSD1306_I2C_ADDRESS, cmd_buf, 2);
} // end ssd1306_command

void ssd1306_get_sleep(void)
{
    ssd1306_command(0xAE);
}

void ssd1306_wake_up(void)
{
    ssd1306_command(0xAF);
}

void setAddress( char page, char column )
{

    unsigned char pageAddress[1];// = {SSD1306_PAGE_START_ADDRESS};
    unsigned char columnAddress[3];// = { SSD1306_COLUMNADDRESS, SSD1306_COLUMNADDRESS_MSB, SSD1306_COLUMNADDRESS_LSB };

    if (page > SSD1306_MAXROWS)
    {
        page = SSD1306_MAXROWS;
    }

    if (column > SSD1306_LCDWIDTH)
    {
        column = SSD1306_LCDWIDTH;
    }

    pageAddress[0] = SSD1306_PAGE_START_ADDRESS | (SSD1306_MAXROWS - page);

    columnAddress[0] = SSD1306_COLUMNADDRESS;
    columnAddress[1] = SSD1306_COLUMNADDRESS_MSB | column;
    columnAddress[2] = SSD1306_COLUMNADDRESS_LSB;

    //__no_operation();

    ssd1306_command(pageAddress[0]);
    ssd1306_command(columnAddress[0]);
    ssd1306_command(columnAddress[1]);
    ssd1306_command(columnAddress[2]);

}

void ssd1306_clear_one_line(uint8_t y)
{
    uint8_t x;
    for(x=0; x<LINE_BUF_SIZ; x++)
        buffer[x] = 0x00;

    ssd1306_setPosition(0, y);

    for(x=0; x<LCDWIDTH_PER_LINE_BUF_SIZ; x++)
    {
        buffer[0] = 0x40;       // 1st i2c data for data transmission
        i2c_write(SSD1306_I2C_ADDRESS, buffer, LINE_BUF_SIZ);
    }

    ssd1306_setPosition(0, 0);

}

void ssd1306_clearDisplay(void)
{
    uint8_t x, y;
    for(x=0; x<LINE_BUF_SIZ; x++)
        buffer[x] = 0x00;

    for(y=0; y<SSD1306_LCDHEIGHT>>3/*/8*/; y++)
    {
        ssd1306_setPosition(0, y);

        for(x=0; x<LCDWIDTH_PER_LINE_BUF_SIZ; x++)
        {
            buffer[0] = 0x40;       // 1st i2c data for data transmission
            i2c_write(SSD1306_I2C_ADDRESS, buffer, LINE_BUF_SIZ);
        }

    }
    ssd1306_setPosition(0, 0);

} // end ssd1306_clearDisplay

void ssd1306_setPosition(uint8_t column, uint8_t page)
{
#if 1
    uint8_t nibble_h, nibble_l;

    nibble_h = (column & 0xF0) >> 4;
    nibble_l = (column & 0x0F);
    ssd1306_command(0xB0 + page);
    ssd1306_command(0x10 + nibble_h);
    ssd1306_command(0x00 + nibble_l);
#else
    if (column > 128) {
        column = 0;                                                     // constrain column to upper limit
    }

    if (page > 8) {
        page = 0;                                                       // constrain page to upper limit
    }

    ssd1306_command(SSD1306_COLUMNADDR);
    ssd1306_command(column);                                            // Column start address (0 = reset)
    ssd1306_command(SSD1306_LCDWIDTH-1);                                // Column end address (127 = reset)

    ssd1306_command(SSD1306_PAGEADDR);
    ssd1306_command(page);                                              // Page start address (0 = reset)
    ssd1306_command(7);                                                 // Page end address
#endif
} // end ssd1306_setPosition

uint8_t dec_data[20];

void ssd1306_printString(uint8_t x, uint8_t y, const char *ptString, font_idx font_id)
{
    uint8_t i;
    uint8_t font_width = fonts[font_id].width;
    uint8_t font_height = fonts[font_id].height;
    uint8_t font_data_type = fonts[font_id].data_type;
    //const uint8_t *font_data = fonts[font_id].data;
    //const unsigned short *font_offset = fonts[font_id].offset;
    //font *cur_font = &fonts[font_id];
    //ssd1306_setPosition(x+2, y); // due to the position of x is lead 2pixel

    uint8_t *ptr_enc_data = fonts[font_id].data;
    uint16_t *ptr_enc_data_offset;
    uint8_t *ptr_dec_data = &dec_data[0];
    uint8_t enc_data_size;
    uint16_t enc_offset_pos, enc_offset_pos2;

    // for every 1 character
    while (*ptString != '\0')
    {
        ssd1306_setPosition(x+2, y);            // due to the position of x is lead 2pixel

        if ((x + font_width) >= 127) {                  // exception if x is over the range then position will go to the next line(y)
            x = 0;

            y++;
            if((x + font_height) > 8)
                y++;
            if((x + font_height) > 16)
                y++;

            ssd1306_setPosition(x+2, y);
        }

        if(font_data_type == DATA_TYPE_RAW)
        {
            ;
            //enc_offset_pos = 0;
            //enc_offset_pos2 = 0;
            //enc_data_size = enc_offset_pos2 - enc_offset_pos;
            //ptr_enc_data_offset = fonts[font_id].offset[position];
            //decompress_font_one_char_array(COMPRESS_METHOD_ZERO_ENC_DEC_v3, enc_offset_pos, ptr_enc_data, ptr_dec_data, enc_data_size);
        }
        else if(font_data_type == DATA_TYPE_ZERO_ENC)
        {
            enc_offset_pos = fonts[font_id].offset[*ptString-0x20];
            enc_offset_pos2 = fonts[font_id].offset[*ptString-0x20+1];
            enc_data_size = enc_offset_pos2 - enc_offset_pos;
            //ptr_enc_data_offset = fonts[font_id].offset[position];
            decompress_font_one_char_array(COMPRESS_METHOD_ZERO_ENC_DEC_v3, enc_offset_pos, ptr_enc_data, ptr_dec_data, enc_data_size);
        }

        // control by text size(height)
        if(font_height <= 8)
        {
            buffer[0] = 0x40;

            for(i = 0; i< font_width; i++)
            {
                if(font_data_type == DATA_TYPE_RAW)
                {
                    //buffer[i+1] = font_5x7_raw[*ptString - ' '][i];     // ' ' means space bar and ASCII code is 0x20
                    //buffer[i+1] = *(fonts[font_id].data + (*ptString-0x20)*font_width +i);     // ' ' means space bar and ASCII code is 0x20

                    // we only use font_6x8 small characters and space
                    if(*ptString == ' ')
                    {
                        buffer[i+1] = *(fonts[font_id].data +i);     // ' ' means space bar and ASCII code is 0x20

                    }
                    else
                    {
                        buffer[i+1] = *(fonts[font_id].data + (*ptString-'a'+1)*font_width +i);     // first array has space and then small capital is located..
                    }
                }
                else if(font_data_type == DATA_TYPE_ZERO_ENC)
                {
                    buffer[i+1] = *(ptr_dec_data + i);
                }
            }

            buffer[font_width + 1] = 0x0;    // at the end of buffer = 0x00

            i2c_write(SSD1306_I2C_ADDRESS, buffer, 7);
            x += font_width+2;                // x position

        }
        else if( (font_height > 8) && (font_height <= 16) )
        {
            buffer[0] = 0x40;
            for(i = 0; i< font_width; i++)
            {
                if(font_data_type == DATA_TYPE_RAW)
                {
                    //buffer[i+1] = fonts[font_id].data[*ptString - ' '][2*i];     // ' ' means space bar and ASCII code is 0x20
                    buffer[i+1] = *(fonts[font_id].data+(*ptString-0x20)*font_width*2 +(i*2) );     // ' ' means space bar and ASCII code is 0x20
                }
                else if(font_data_type == DATA_TYPE_ZERO_ENC)
                {
                    buffer[i+1] = *(ptr_dec_data + i*2);
                }
            }
            buffer[font_width + 1] = 0x0;     // at the end of buffer = 0x00
            i2c_write(SSD1306_I2C_ADDRESS, buffer, font_height+1);

            buffer[0] = 0x40;
            ssd1306_setPosition(x+2, y+1);                                  // y+1 position
            for(i = 0; i< font_width; i++)
            {
                if(font_data_type == DATA_TYPE_RAW)
                {
                    //buffer[i+1] = fonts[font_id].data[*ptString - ' '][2*i+1];
                    buffer[i+1] = *(fonts[font_id].data+(*ptString-0x20)*font_width*2 +(i*2+1) );
                }
                else if(font_data_type == DATA_TYPE_ZERO_ENC)
                {
                    buffer[i+1] = *(ptr_dec_data + (i*2+1) );
                }
            }
            buffer[font_width + 1] = 0x0;     // at the end of buffer = 0x00
            i2c_write(SSD1306_I2C_ADDRESS, buffer, font_height+1);

            x += font_width+2;
        }
        else if( (font_height > 16) && (font_height <= 24) )
        {
        }
        else
        {

        }
        ptString++;
    }
#if 0   // previous code
    // for every 1 character
    while (*ptString != '\0')
    {
        buffer[0] = 0x40;
        ssd1306_setPosition(x+2, y);            // y+0 position

        if ((x + 14) >= 127) {                  // exception if x is over the range then position will go to the next line(y)
            x = 0;
            y++;
            ssd1306_setPosition(x+2, y);
        }

        for(i = 0; i< 14; i++) {
            buffer[i+1] = font_14x16_raw[*ptString - ' '][2*i];     // ' ' means space bar and ASCII code is 0x20
        }
        buffer[15] = 0x0;
        i2c_write(SSD1306_I2C_ADDRESS, buffer, 16);

        buffer[0] = 0x40;
        ssd1306_setPosition(x+2, y+1);                                  // y+1 position
        for(i = 0; i< 14; i++) {
            buffer[i+1] = font_14x16_raw[*ptString - ' '][2*i+1];
        }
        buffer[15] = 0x0;
        i2c_write(SSD1306_I2C_ADDRESS, buffer, 16);

        x+=15;
        ptString++;
    }
#endif
} // end ssd1306_printString
#if 0
void ssd1306_printText(uint8_t x, uint8_t y, char *ptString)
{
    ssd1306_clear_one_line(y);
    ssd1306_setPosition(x+2, y);

    while (*ptString != '\0') {
        buffer[0] = 0x40;

        if ((x + 5) >= 127) {                                           // char will run off screen
            x = 0;                                                      // set column to 0
            y++;                                                        // jump to next page
            ssd1306_setPosition(x+2, y);                                  // send position change to oled
        }

        uint8_t i;
        for(i = 0; i< 5; i++) {
            buffer[i+1] = font_5x7_raw[*ptString - ' '][i];
        }

        buffer[6] = 0x0;

        i2c_write(SSD1306_I2C_ADDRESS, buffer, 7);
        ptString++;
        x+=6;
    }
} // end ssd1306_printText

void ssd1306_printTextBlock(uint8_t x, uint8_t y, char *ptString)
{
    char word[12];
    uint8_t i;
    uint8_t endX = x;
    while (*ptString != '\0'){
        i = 0;
        while ((*ptString != ' ') && (*ptString != '\0')) {
            word[i] = *ptString;
            ptString++;
            i++;
            endX += 6;
        }

        word[i++] = '\0';

        if (endX >= 127) {
            x = 0;
            y++;
            ssd1306_printText(x, y, word);
            endX = (i * 6);
            x = endX;
        } else {
            ssd1306_printText(x, y, word);
            endX += 6;
            x = endX;
        }
        ptString++;
    }

}


void ssd1306_printUI32( uint8_t x, uint8_t y, uint32_t val, uint8_t Hcenter )
{
    char text[14];

    ultoa(val, text);
    if (Hcenter) {
        ssd1306_printText(HcenterUL[digits(val)], y, text);
    } else {
        ssd1306_printText(x, y, text);
    }
} // end ssd1306_printUI32
#endif
uint8_t digits(uint32_t n)
{
    if (n < 10) {
        return 1;
    } else if (n < 100) {
        return 2;
    } else if (n < 1000) {
        return 3;
    } else if (n < 10000) {
        return 4;
    } else if (n < 100000) {
        return 5;
    } else if (n < 1000000) {
        return 6;
    } else if (n < 10000000) {
        return 7;
    } else if (n < 100000000) {
        return 8;
    } else if (n < 1000000000) {
        return 9;
    } else {
        return 10;
    }
} // end digits

void ultoa(uint32_t val, char *string)
{
    uint8_t i = 0;
    uint8_t j = 0;
                                                                        // use do loop to convert val to string
    do {
        if (j==3) {                                                     // we have reached a separator position
            string[i++] = ',';                                          // add a separator to the number string
            j=0;                                                        // reset separator indexer thingy
        }
            string[i++] = val%10 + '0';                                 // add the ith digit to the number string
            j++;                                                        // increment counter to keep track of separator placement
    } while ((val/=10) > 0);

    string[i++] = '\0';                                                 // add termination to string
    reverse(string);                                                    // string was built in reverse, fix that
} // end ultoa

void reverse(char *s)
{
    uint8_t i, j;
    uint8_t c;

    for (i = 0, j = strlen(s)-1; i<j; i++, j--) {
        c = s[i];
        s[i] = s[j];
        s[j] = c;
    }
} // end reverse
