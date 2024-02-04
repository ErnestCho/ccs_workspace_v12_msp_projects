

/**
 * main.c
 *
 * Example usage of SSD1306 library created for MSP-EXP430G2 TI LaunchPad
 * Complied using TI v18.12.2.LTS
 *
 * SSD1306 library based on Adafuit's wonderful Arduino library with modifications
 *  and less features to fit the small amount of RAM available on the MSP430G2553 *
 *
 */

#include <msp430.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <SSD1306.h>
#include <i2c.h>
#include <HDC1080_I2C_GPIO.h>

#include <adc.h>

#define OLED_PWR    BIT0                        // OLED power connect/disconnect on pin P1.0

#define ANUMBER     19052019
#define MAX_COUNT   4.2e9

#define LONG_DELAY  __delay_cycles(3000000)
#define SHORT_DELAY __delay_cycles(50000)

uint16_t temperature_x10 = 0, humidity_x10 = 0;
char disp_buf[24];
char tmp_buf[6];
uint16_t tmp_data[2];

// ADC Vbat checker
uint8_t g_vbat_decimal_x10 = 0;


// timerA0 constant definition. use SMCLK(125kHz)
#define TIMER0_10SEC_VERSUS_100msec     100

#define TIMER0_SMCLK_1msec              250          //31.25
#define TIMER0_SMCLK_5msec              1250        // 31.25*5 = 156.25
#define TIMER0_SMCLK_10msec             2500        // 312.5
#define TIMER0_SMCLK_50msec             12500       // 312.5*5 = 1562.5
#define TIMER0_SMCLK_100msec            25000
#define TIMER0_SMCLK_200msec            50000


#define CONST_TIMER0_CCR0               TIMER0_SMCLK_200msec//TIMER0_SMCLK_1msec//TIMER0_SMCLK_10msec
#define CONST_TIMER0_CCR1               TIMER0_SMCLK_50msec
#define CONST_TIMER0_CCR2               TIMER0_SMCLK_50msec

// 10msec,  50msec, 200msec continuous timer
void timerA0_init(void)
{
    TA0CTL = TASSEL_2 + MC_2 + ID_2;                   // SMCLK, Cont. mode, 1/4 = 1000/4=250kHz
    // CCR0 interrupt enabled : 10msec
    TA0CCTL0 = CCIE;
    TA0CCR0 = CONST_TIMER0_CCR0;
}

void timerA0_disable(void)
{
    TA0CTL |= MC_0;
    TA0CCTL0 = 0;
}

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;                   // stop watchdog timer
    //BCSCTL1=CALBC1_16MHZ;
    //DCOCTL=CALDCO_16MHZ;
    i2c_init();                                 // initialize I2C to use with OLED

    // HDC1080 init
    GPIO_I2C_Init();
    __delay_cycles(500000);

    ssd1306_init();                             // Initialize SSD1306 OLED
    __delay_cycles(500000);

    // Timer
    timerA0_init();
    // ADC
    adc_init_interrupt();

    __bis_SR_register(GIE);       // Enter LPM4 w/interrupt

    ssd1306_clearDisplay();
    while(1) {
        // =========================================
        // Display a Long Text Block
        // =========================================
        // There seems to be a bug in the ssd1306_printTextBlock code,
        //  long text blocks like below need 1 extra character in their array
        //  so the code does not read outside of the array. The below 91 character
        //  text block should only need a array of 92 but to fix the bug use an array of 93.
        //  !!!! MUST FIX BUG IN THE FUTURE !!!!
        //char txtBlock[93] =   "ABCDEFGHIJKLMNOPQRSTUVWXYZ,.";//abcdefghijklmnopqrstuvwxyz1234567890123456789012345678901234567";
        //char txtBlock2[] = "This is a long multiline text block. The code will automatically add extra lines as needed.";
//        char txtBlock[] = "abcdefghijklmn";
//        char txtBlock2[] = "ABCDEFGHIJKLMN";
//        char txtBlock3[] = "1234567890';/.,";
//        ssd1306_printTextBlock(0, 1, txtBlock);
//        ssd1306_printTextBlock(1, 2, txtBlock2);
//        ssd1306_printTextBlock(2, 3, txtBlock3);
//        LONG_DELAY;                             // Avoid delay loops in real code, use timer
//        ssd1306_clearDisplay();

        //ssd1306_clearDisplay();
        //ssd1306_clear_one_line(uint8_t y);


        // ADC battery check
        g_vbat_decimal_x10 = vbat_level_checker();
        if(g_vbat_decimal_x10 <= 34)   // low battery check at Vbat <= 3.4V
        {
            strcpy(disp_buf, "Batt Low : ");

            //timerA0_disable();
        }
        else
        {
            strcpy(disp_buf, "    Vbat : ");
        }

        tmp_data[0] = g_vbat_decimal_x10 / 10;
        tmp_data[1] = g_vbat_decimal_x10 % 10;
#ifdef CCS_VER9
        ltoa(tmp_data[0], tmp_buf);
#else
        ltoa(tmp_data[0], tmp_buf, 10);
#endif
        strcat(disp_buf, tmp_buf);
        strcat(disp_buf, ".");
#ifdef CCS_VER9
        ltoa(tmp_data[1], tmp_buf);
#else
        ltoa(tmp_data[1], tmp_buf, 10);
#endif
        strcat(disp_buf, tmp_buf);
        strcat(disp_buf, "V");
        ssd1306_printString(0, 0, disp_buf, FONT_6x8);                      // display line 0 : Vbat voltage



        if( HDC1080_I2C_check_device_valid(HDC1080_SLAVE_ADDR) )
        {
            HDC1080_I2C_read_temp_and_humidity(HDC1080_SLAVE_ADDR, &temperature_x10, &humidity_x10);

            tmp_data[0] = temperature_x10 / 10;
            tmp_data[1] = temperature_x10 % 10;

#ifdef CCS_VER9
            ltoa(tmp_data[0], tmp_buf);
#else
            ltoa(tmp_data[0], tmp_buf, 10);
#endif
            strcpy(disp_buf, tmp_buf);
            strcat(disp_buf, ".");
#ifdef CCS_VER9
            ltoa(tmp_data[1], tmp_buf);
#else
            ltoa(tmp_data[1], tmp_buf, 10);
#endif
            strcat(disp_buf, tmp_buf);
            strcat(disp_buf, " 'C");
            ssd1306_printString(0, 3, disp_buf, FONT_MS_GOTHIC8x16);

            tmp_data[0] = humidity_x10 / 10;
            tmp_data[1] = humidity_x10 % 10;

#ifdef CCS_VER9
            ltoa(tmp_data[0], tmp_buf);
#else
            ltoa(tmp_data[0], tmp_buf, 10);
#endif
            strcpy(disp_buf, tmp_buf);
            strcat(disp_buf, ".");
#ifdef CCS_VER9
            ltoa(tmp_data[1], tmp_buf);
#else
            ltoa(tmp_data[1], tmp_buf, 10);
#endif
            strcat(disp_buf, tmp_buf);
            strcat(disp_buf, " % RH");
            ssd1306_printString(0, 5, disp_buf, FONT_MS_GOTHIC8x16);
        }
        else
        {
            ssd1306_printString(0, 3, "Error to access", FONT_MS_GOTHIC8x16);

            ssd1306_printString(0, 5, "HDC1080..", FONT_MS_GOTHIC8x16);

        }
        __bis_SR_register(LPM_LEVEL + GIE);       // Enter LPM4 w/interrupt

        //delay_ms(1500);                              // Avoid delay loops in real code, use timer



    }


}

unsigned char cnt_t0a0 = 0;

// 1ms continuous mode timer
#pragma vector=TIMER0_A0_VECTOR
__interrupt void ISR_Timer0_A0(void)
{
    TA0CCR0 += CONST_TIMER0_CCR0;               // Add Offset to CCR1
    if(cnt_t0a0++ > 10)
    {
        cnt_t0a0 = 0;
        __bic_SR_register_on_exit(LPM_LEVEL + GIE);
    }

}


