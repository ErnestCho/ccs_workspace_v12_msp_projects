
#ifndef NOKIA_5110_LCD_H_
#define NOKIA_5110_LCD_H_

//////////////////////////////// include files ///////////////////////////////////////////////

#include "PCD8544.h"

/////////////////////////////////define region //////////////////////////////////////////////

/* For utilizing serial communication for MSP430G2xx3,
 * we need to select the SPI port : UCSI_A0 or UCSI_B0
 * This serial port supports functions as below :
 *  - UCSI_A0 : UART, LIN, IrDA, SPI
 *  - UCSI_B0 : SPI, I2C
 * If you need to acquire input(display) data from I2C,
 * you should check USE_SPI_UCSI_A0 for this LCD display.
 */
//#define USE_SPI_UCSI_A0
#define USE_SPI_UCSI_B0


#if defined USE_SPI_UCSI_A0
#define LCD5110_SCLK_PIN        BIT4    // P1.4 : UCA0CLK
#define LCD5110_DN_PIN          BIT2    // P1.2 : UCA0SIMO
#elif defined USE_SPI_UCSI_B0
#define LCD5110_SCLK_PIN        BIT5    // P1.5 : UCB0CLK
#define LCD5110_DN_PIN          BIT7    // P1.7 : UCB0SIMO

#endif

//#define LCD5110_SCLK_PIN        BIT5
//#define LCD5110_DN_PIN          BIT7
#define LCD5110_RST_PIN         BIT0
#define LCD5110_SCE_PIN         BIT1
#define LCD5110_DC_PIN          BIT3 // BIT3 // bit3 is not conflict to UCSI_A0 setting
#define LCD5110_SELECT          P1OUT &= ~LCD5110_SCE_PIN
#define LCD5110_DESELECT        P1OUT |= LCD5110_SCE_PIN
#define LCD5110_SET_COMMAND     P1OUT &= ~LCD5110_DC_PIN
#define LCD5110_SET_DATA        P1OUT |= LCD5110_DC_PIN
#define LCD5110_COMMAND         0
#define LCD5110_DATA            1

#if defined USE_SPI_UCSI_A0
#define SPI_MSB_FIRST           UCA0CTL0 |= UCMSB // or UCA0CTL0 |= UCMSB (USCIA) or USICTL0 &= ~USILSB (USI)
#define SPI_LSB_FIRST           UCA0CTL0 &= ~UCMSB // or UCA0CTL0 &= ~UCMSB or USICTL0 |= USILSB (USI)
#elif defined USE_SPI_UCSI_B0
#define SPI_MSB_FIRST           UCB0CTL0 |= UCMSB // or UCA0CTL0 |= UCMSB (USCIA) or USICTL0 &= ~USILSB (USI)
#define SPI_LSB_FIRST           UCB0CTL0 &= ~UCMSB // or UCA0CTL0 &= ~UCMSB or USICTL0 |= USILSB (USI)
#endif



/////////////////////////////////variable region //////////////////////////////////////////////

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





/////////////////////////////////extern region //////////////////////////////////////////////

extern void writeStringToLCD(char y, const char *string, font_idx font_id);
extern void writeCharToLCD(char x, char y, char c, font_idx font_id);
#if 0
extern void writeStringToLCD(const char *string);
extern void writeCharToLCD(char c);
extern void writeStringToLCD2(char y, const char *string);
extern void writeCharToLCD2(char x, char y, char c);
#endif
extern void writeBlockToLCD(char *byte, unsigned char length);
extern void writeGraphicToLCD(char *byte, unsigned char transform);
extern void writeToLCD(unsigned char dataCommand, unsigned char data);
extern void clearLCD(void);
extern void clearBank(unsigned char bank);
extern void setAddr(unsigned char xAddr, unsigned char yAddr);
extern void initLCD(void);




#endif // NOKIA_5110_LCD_H_
