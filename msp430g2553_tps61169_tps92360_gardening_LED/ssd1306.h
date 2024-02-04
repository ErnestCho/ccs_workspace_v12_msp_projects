/*
 * ssd1306.h
 */

#ifndef SSD1306_H_
#define SSD1306_H_

#include <msp430.h>
#include <stdint.h>
#include <string.h>

#define LINE_BUF_SIZ     17                 // command + 32
#define LCDWIDTH_PER_LINE_BUF_SIZ   9       // 128 / 32 = 4
unsigned char buffer[LINE_BUF_SIZ];                                                     // buffer for data transmission to screen

/* ====================================================================
 * Horizontal Centering Number Array
 * ==================================================================== */
#define HCENTERUL_OFF   0
#define HCENTERUL_ON    1

/* ====================================================================
 * SSD1306 OLED Settings and Command Definitions
 * ==================================================================== */

#define SSD1306_I2C_ADDRESS             0x3C//0x3D

// SSD1306 PARAMETERS
#define SSD1306_LCDWIDTH                128
#define SSD1306_LCDHEIGHT               64
#define SSD1306_128_64
        #define SSD1306_MAXROWS                 7
        #define SSD1306_MAXCONTRAST             0xFF

// command table
#define SSD1306_SETCONTRAST             0x81
#define SSD1306_DISPLAYALLON_RESUME     0xA4
#define SSD1306_DISPLAYALLON            0xA5
#define SSD1306_NORMALDISPLAY           0xA6
#define SSD1306_INVERTDISPLAY           0xA7
#define SSD1306_DISPLAYOFF              0xAE
#define SSD1306_DISPLAYON               0xAF

        // scrolling commands
        #define SSD1306_SCROLL_RIGHT            0x26
        #define SSD1306_SCROLL_LEFT             0X27
        #define SSD1306_SCROLL_VERT_RIGHT       0x29
        #define SSD1306_SCROLL_VERT_LEFT        0x2A
        #define SSD1306_SET_VERTICAL            0xA3

        // address setting
        #define SSD1306_SETLOWCOLUMN            0x00
        #define SSD1306_SETHIGHCOLUMN           0x10
        #define SSD1306_MEMORYMODE              0x20
        #define SSD1306_COLUMNADDRESS           0x21
        #define SSD1306_COLUMNADDRESS_MSB       0x00
        #define SSD1306_COLUMNADDRESS_LSB       0x7F
        #define SSD1306_PAGEADDRESS             0x22
        #define SSD1306_PAGE_START_ADDRESS      0xB0

        // hardware configuration
        #define SSD1306_SETSTARTLINE            0x40
        #define SSD1306_SEGREMAP                0xA1
        #define SSD1306_SETMULTIPLEX            0xA8
        #define SSD1306_COMSCANINC              0xC0
        #define SSD1306_COMSCANDEC              0xC8
        #define SSD1306_SETDISPLAYOFFSET        0xD3
        #define SSD1306_SETCOMPINS              0xDA

        // timing and driving
        #define SSD1306_SETDISPLAYCLOCKDIV      0xD5
        #define SSD1306_SETPRECHARGE            0xD9
        #define SSD1306_SETVCOMDETECT           0xDB
        #define SSD1306_NOP                     0xE3

        // power supply configuration
        #define SSD1306_CHARGEPUMP              0x8D
        #define SSD1306_EXTERNALVCC             0x10
        #define SSD1306_SWITCHCAPVCC            0x20


#define SSD1306_SETDISPLAYOFFSET        0xD3
#define SSD1306_SETCOMPINS              0xDA

#define SSD1306_SETVCOMDETECT           0xDB

#define SSD1306_SETDISPLAYCLOCKDIV      0xD5
#define SSD1306_SETPRECHARGE            0xD9

#define SSD1306_SETMULTIPLEX            0xA8

#define SSD1306_SETLOWCOLUMN            0x00
#define SSD1306_SETHIGHCOLUMN           0x10

#define SSD1306_SETSTARTLINE            0x40

#define SSD1306_MEMORYMODE              0x20
#define SSD1306_COLUMNADDR              0x21
#define SSD1306_PAGEADDR                0x22

#define SSD1306_COMSCANINC              0xC0
#define SSD1306_COMSCANDEC              0xC8

//#define SSD1306_SEGREMAP                0xA0

#define SSD1306_CHARGEPUMP              0x8D

//#define SSD1306_EXTERNALVCC             0x1
//#define SSD1306_SWITCHCAPVCC            0x2

// currently no scroll functionality, left for possible future use
#define SSD1306_ACTIVATE_SCROLL                         0x2F
#define SSD1306_DEACTIVATE_SCROLL                       0x2E
#define SSD1306_SET_VERTICAL_SCROLL_AREA                0xA3
#define SSD1306_RIGHT_HORIZONTAL_SCROLL                 0x26
#define SSD1306_LEFT_HORIZONTAL_SCROLL                  0x27
#define SSD1306_VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL    0x29
#define SSD1306_VERTICAL_AND_LEFT_HORIZONTAL_SCROLL     0x2A

typedef enum {
    FONT_5x7 = 0,
    FONT_6x8,
    FONT_14x16,
    FONT_FRANKLIN_GOTHIC_MEDIUM_COND10x15,
    FONT_MS_GOTHIC8x16,
    FONT_MAX
}font_idx;

typedef struct {
    unsigned char width;
    unsigned char height;
    const char *data;

}font;


/* ====================================================================
 * SSD1306 OLED Prototype Definitions
 * ==================================================================== */
void ssd1306_init(void);
void ssd1306_command(unsigned char);
void ssd1306_clear_one_line(uint8_t y);
void ssd1306_clearDisplay(void);
void ssd1306_setPosition(uint8_t, uint8_t);
void ssd1306_printText(uint8_t, uint8_t, char *);
void ssd1306_printString(uint8_t x, uint8_t y, char *ptString, font_idx font_id);

void ssd1306_printTextBlock(uint8_t, uint8_t, char *);
void ssd1306_printUI32(uint8_t, uint8_t, uint32_t, uint8_t);

uint8_t digits(uint32_t);
void ultoa(uint32_t, char *);
void reverse(char *);

#endif /* SSD1306_H_ */
