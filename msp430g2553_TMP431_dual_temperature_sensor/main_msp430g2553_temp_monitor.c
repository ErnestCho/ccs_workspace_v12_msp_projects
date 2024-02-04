#include <msp430.h> 
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "nokia_5110_lcd.h"
#include "tmp431.h"
#include "adc.h"


unsigned char currXAddr = 0; //TODO this will be used for tracking current addr
unsigned char currYAddr = 0; //not implemented

//char testBlock[8] = {0x01, 0x03, 0x07, 0x0F, 0x1F, 0x3F, 0x7F, 0xFF};
char testBlock[8] = {0x00, 0x7F, 0x7F, 0x33, 0x33, 0x03, 0x03, 0x03};
char testBlock2[8] = {0x00, 0x18, 0x18, 0x18, 0x7E, 0x3C, 0x18, 0x00};

#define UI_TEXT_6LINE       // test done
#define UI_TEXT_3LINE
//#define UI_MATRIX
//#define UI_BAR_GRAPH
//#define UI_DEFAULT_EXAMPLE

short g_tmp431_local_temperature_x10 = 0;
short g_tmp431_remote_temperature_x10 = 0;


// timerA0 constant definition. use SMCLK(125MHz)
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

#ifdef UI_TEXT_6LINE
    int data[7] = {0, 0, 0, 0, 0, 0, 0};
    char disp_data[6][12];
#elif defined UI_MATRIX

#elif defined UI_BAR_GRAPH
#else
#endif

uint8_t f_local_temp_extended = 0;
uint8_t f_remote_temp_extended = 0;
#define TEMP_EXTEND_MODE_OFFSET_x10         640     // = 0x40(=64) *10




uint8_t g_vbat_decimal_x10 = 0;



void main(void)
{
    WDTCTL = WDTPW + WDTHOLD;         // disable WDT
    BCSCTL1 = CALBC1_1MHZ;            // 1MHz clock
    DCOCTL = CALDCO_1MHZ;
    __delay_cycles(1000);

    // pin setting
    P1OUT |= LCD5110_RST_PIN + LCD5110_SCE_PIN + LCD5110_DC_PIN;
    P1DIR |= LCD5110_RST_PIN + LCD5110_SCE_PIN + LCD5110_DC_PIN;

    P1OUT &= ~LCD5110_RST_PIN;
    __delay_cycles(100000);
    P1OUT |= LCD5110_RST_PIN;

    timerA0_init();

    // setup USIx
    P1SEL |= LCD5110_SCLK_PIN + LCD5110_DN_PIN;
    P1SEL2 |= LCD5110_SCLK_PIN + LCD5110_DN_PIN;


    GPIO_I2C_Init();

#ifdef USE_SPI_UCSI_A0
    UCA0CTL0 |= UCCKPH + UCMSB + UCMST + UCSYNC;  // 3-pin, 8-bit SPI master
    UCA0CTL1 |= UCSSEL_2;                     // SMCLK
    UCA0BR0 |= 0x01;                          // 1:1
    UCA0BR1 = 0;
    UCA0CTL1 &= ~UCSWRST;                     // clear SW
#else
    UCB0CTL0 |= UCCKPH + UCMSB + UCMST + UCSYNC;  // 3-pin, 8-bit SPI master
    UCB0CTL1 |= UCSSEL_2;                     // SMCLK
    UCB0BR0 |= 0x01;                          // 1:1
    UCB0BR1 = 0;
    UCB0CTL1 &= ~UCSWRST;                     // clear SW
#endif
    __delay_cycles(500000);
    initLCD();
    clearLCD();

    __delay_cycles(500000);

    // TMP431
    tmp431_i2c_first_dummy_read(TMP431_LOCAL_IC);
    tmp431_i2c_first_dummy_read(TMP431_REMOTE_IC);
    __delay_cycles(5000);

    f_local_temp_extended =  tmp431_i2c_set_extended_mode(TMP431_LOCAL_IC);
    f_remote_temp_extended = tmp431_i2c_set_extended_mode(TMP431_REMOTE_IC);
    //f_local_temp_extended = 1;
    //f_remote_temp_extended = 1;

    __delay_cycles(500000);

    // ADC
    adc_init_interrupt();



    __bis_SR_register(GIE);       // Enter LPM4 w/interrupt

    while(1)
    {
        char tmp_buf[6];

        // local temperature
        g_tmp431_local_temperature_x10 = tmp431_i2c_read_local_temperature(TMP431_LOCAL_IC);

        if(f_local_temp_extended)
        {
            g_tmp431_local_temperature_x10 -= TEMP_EXTEND_MODE_OFFSET_x10;
            strcpy(disp_data[0], "e");
        }
        else
        {
            strcpy(disp_data[0], "s");
        }

        if(g_tmp431_local_temperature_x10 > 0)
        {
            strcat(disp_data[0], "+");
        }
        else
        {
            strcat(disp_data[0], "-");
            g_tmp431_local_temperature_x10 = -g_tmp431_local_temperature_x10;
        }
        data[0] = g_tmp431_local_temperature_x10 / 10;
        data[1] = g_tmp431_local_temperature_x10 % 10;
#ifdef CCS_VER9
        ltoa(data[0], tmp_buf);
#else
        ltoa(data[0], tmp_buf, 10);
#endif
        strcat(disp_data[0], tmp_buf);
        strcat(disp_data[0], ".");
#ifdef CCS_VER9
        ltoa(data[1], tmp_buf);
#else
        ltoa(data[1], tmp_buf, 10);
#endif
        strcat(disp_data[0], tmp_buf);
        //strcat(disp_data[0], 0x7c);

        // remote temperature
        g_tmp431_remote_temperature_x10 = tmp431_i2c_read_local_temperature(TMP431_REMOTE_IC);

        if(f_remote_temp_extended)
        {
            g_tmp431_remote_temperature_x10 -= TEMP_EXTEND_MODE_OFFSET_x10;
            strcpy(disp_data[1], "e");
        }
        else
        {
            strcpy(disp_data[1], "s");
        }

        if(g_tmp431_remote_temperature_x10 > 0)
        {
            strcat(disp_data[1], "+");
        }
        else
        {
            strcat(disp_data[1], "-");
            g_tmp431_remote_temperature_x10 = -g_tmp431_remote_temperature_x10;
        }
        data[2] = g_tmp431_remote_temperature_x10 / 10;
        data[3] = g_tmp431_remote_temperature_x10 % 10;
#ifdef CCS_VER9
        ltoa(data[2], tmp_buf);
#else
        ltoa(data[2], tmp_buf, 10);
#endif
        strcat(disp_data[1], tmp_buf);
        strcat(disp_data[1], ".");
#ifdef CCS_VER9
        ltoa(data[3], tmp_buf);
#else
        ltoa(data[3], tmp_buf, 10);
#endif
        strcat(disp_data[1], tmp_buf);
        //strcat(disp_data[0], 0x7c);

        // ADC battery check
        g_vbat_decimal_x10 = vbat_level_checker();

        strcpy(disp_data[2], "Indoor.  ");
#if 1
        data[4] = g_vbat_decimal_x10 / 10;
        data[5] = g_vbat_decimal_x10 % 10;
#ifdef CCS_VER9
        ltoa(data[4], tmp_buf);
#else
        ltoa(data[4], tmp_buf, 10);
#endif
        strcat(disp_data[2], tmp_buf);
        strcat(disp_data[2], ".");
#ifdef CCS_VER9
        ltoa(data[5], tmp_buf);
#else
        ltoa(data[5], tmp_buf, 10);
#endif
        strcat(disp_data[2], tmp_buf);
        strcat(disp_data[2], "V");
#endif
        writeStringToLCD(0, disp_data[2], FONT_6x8);
        //writeStringToLCD(0, "Indoor Temp.", FONT_6x8);
        writeStringToLCD(1, disp_data[0], FONT_FRANKLIN_GOTHIC_MEDIUM_COND10x15);

        if(g_vbat_decimal_x10 <= 34)   // low battery check at Vbat <= 3.4V
        {
            writeStringToLCD(3, "    Batt Low", FONT_6x8);
            writeStringToLCD(4, disp_data[1], FONT_FRANKLIN_GOTHIC_MEDIUM_COND10x15);

            //timerA0_disable();
        }
        else
        {
            writeStringToLCD(3, "Outdoor Temp.", FONT_6x8);
            writeStringToLCD(4, disp_data[1], FONT_FRANKLIN_GOTHIC_MEDIUM_COND10x15);
        }


        //__delay_cycles(1000000);

        __bis_SR_register(LPM_LEVEL + GIE);       // Enter LPM4 w/interrupt

    }




#if 0
    for(i=0; i<6; i++)
    {
        memset(disp_data[i], 0x00, 12);
    }
    while(1)
    {
        char tmp_buf[6];

        strcpy(disp_data[0], "Volt:");
        ltoa(data[0]++, tmp_buf);
        strcat(disp_data[0], tmp_buf);

        strcpy(disp_data[1], "Curr:");
        ltoa(data[1]++, tmp_buf);
        strcat(disp_data[1], tmp_buf);

        strcpy(disp_data[2], "Temp:");
        ltoa(data[2]++, tmp_buf);
        strcat(disp_data[2], tmp_buf);

        strcpy(disp_data[3], "Humi:");
        ltoa(data[3]++, tmp_buf);
        strcat(disp_data[3], tmp_buf);

        strcpy(disp_data[4], "ADC0:");
        ltoa(data[4]++, tmp_buf);
        strcat(disp_data[4], tmp_buf);

        strcpy(disp_data[5], "ADC1:");
        ltoa(data[5]++, tmp_buf);
        strcat(disp_data[5], tmp_buf);

        clearBank(0);
        writeStringToLCD(0, disp_data[0], FONT_6x8);
        clearBank(1);
        writeStringToLCD(1, disp_data[1], FONT_6x8);
        clearBank(2);
        writeStringToLCD(2, disp_data[2], FONT_6x8);
        clearBank(3);
        writeStringToLCD(3, disp_data[3], FONT_6x8);
        clearBank(4);
        writeStringToLCD(4, disp_data[4], FONT_6x8);
        clearBank(5);
        writeStringToLCD(5, disp_data[5], FONT_6x8);

        _delay_cycles(200000);

    }
#endif


}

unsigned char cnt_t0a0 = 0;

// 1ms continuous mode timer
#pragma vector=TIMER0_A0_VECTOR
__interrupt void ISR_Timer0_A0(void)
{
    TA0CCR0 += CONST_TIMER0_CCR0;               // Add Offset to CCR1
    if(cnt_t0a0++ > 5)
    {
        cnt_t0a0 = 0;
        __bic_SR_register_on_exit(LPM_LEVEL + GIE);
    }

}


// ADC10 interrupt service routine
#pragma vector=ADC10_VECTOR
__interrupt void ADC10_ISR (void)
{
    __bic_SR_register_on_exit(LPM_LEVEL + GIE);          // Clear CPUOFF bit from 0(SR)
}

