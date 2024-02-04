

/**
 * main.c
 *
 * 1MHz main clock & sub main clock (MCLK & SMCLK)
 * 1kHz PWM cycle with 1000 duty rate
 *
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

#define CPU_F ((double)1000000)
#define delay_us(x) __delay_cycles((long)(CPU_F*(double)x/1000000.0))
#define delay_ms(x) __delay_cycles((long)(CPU_F*(double)x/1000.0))

/////////////////////////////////////////////////////////////
//                      EXT INT
/////////////////////////////////////////////////////////////
// SW1 = P1.3
#define SW1_PORT_DIR            P1DIR
#define SW1_PORT_OUT            P1OUT
#define SW1_PORT_REN            P1REN
#define SW1_PORT_IES            P1IES
#define SW1_PORT_IE             P1IE
#define SW1_PORT_IFG            P1IFG
#define SW1_BIT                 BIT3

// SW2 = P1.4
#define SW2_PORT_DIR            P1DIR
#define SW2_PORT_OUT            P1OUT
#define SW2_PORT_REN            P1REN
#define SW2_PORT_IES            P1IES
#define SW2_PORT_IE             P1IE
#define SW2_PORT_IFG            P1IFG
#define SW2_BIT                 BIT4

uint8_t switch_status = 0;

// GPIO_P1InterruptSet() : external interrupt setup for P1
// param : bits - multi-bits setup
void GPIO_SWs_Interrupt_Set(void)
{
    SW1_PORT_DIR &= ~SW1_BIT;                         // port as Input
    SW1_PORT_OUT |=  SW1_BIT;                          // Configure P1 as pulled-up
    SW1_PORT_REN |=  SW1_BIT;                          // P1 pull-up register enable
    SW1_PORT_IES &= ~SW1_BIT;                         // P1 edge, 0: low to high edge
    SW1_PORT_IE  |=  SW1_BIT;                          // P1 interrupt enabled
    SW1_PORT_IFG &= ~SW1_BIT;                         // P1 IFG cleared

    SW2_PORT_DIR &= ~SW2_BIT;
    SW2_PORT_OUT |=  SW2_BIT;                          // Configure P2.x as pulled-up
    SW2_PORT_REN |=  SW2_BIT;                          // P2.x pull-up register enable
    SW2_PORT_IES &= ~SW2_BIT;                         // P2.x edge, 0: low to high edge
    SW2_PORT_IE  |=  SW2_BIT;                          // P2.x interrupt enabled
    SW2_PORT_IFG &= ~SW2_BIT;                         // P2.x IFG cleared
}
/////////////////////////////////////////////////////////////
//                      CLOCK
/////////////////////////////////////////////////////////////
#define CLOCK_1MHZ      1
#define CLOCK_4MHZ      2
#define CLOCK_8MHZ      3
#define CLOCK_10MHZ     4
#define CLOCK_12MHZ     5
#define CLOCK_16MHZ     6
#define CLOCK_20MHZ     7
#define CLOCK_24MHZ     8
#define CLOCK_25MHZ     9


void clock_init(uint8_t mclk_spd)
{
    // check if TLV SegmentA is erased or not,
    // especially check clock related data...

    // check if calibration constant erased or not
    if (CALBC1_1MHZ==0xFF ||
        CALBC1_8MHZ==0xFF ||
        CALBC1_12MHZ==0xFF ||
        CALBC1_16MHZ==0xFF)
    {
        // values are calibrated during device manufacturing..
        // default value of CALDCO_xMHZ and CALBC1_xMHZ
        // 1MHz:86, BA, 8MHz: 8D, 82, 12MHz: 8E, 92, 16MHz: 8F, 8E

        // if it is erased, use below 1MHz
        BCSCTL1 = 0x86;
        DCOCTL = 0xBA;
    }

    // setup for MCLK
    switch(mclk_spd)
    {
        case CLOCK_16MHZ :
            BCSCTL1 = CALBC1_16MHZ;                   // Set range
            DCOCTL = CALDCO_16MHZ;                    // Set DCO step + modulation*/
        break;
        case CLOCK_12MHZ :
            BCSCTL1 = CALBC1_12MHZ;                   // Set range
            DCOCTL = CALDCO_12MHZ;                    // Set DCO step + modulation*/
        break;
        case CLOCK_8MHZ :
            BCSCTL1 = CALBC1_8MHZ;                    // Set range
            DCOCTL = CALDCO_8MHZ;                     // Set DCO step + modulation */
        break;
        case CLOCK_1MHZ :
            BCSCTL1 = CALBC1_1MHZ;                    // Set range
            DCOCTL = CALDCO_1MHZ;                     // Set DCO step + modulation */
        break;
        case CLOCK_20MHZ :
        case CLOCK_24MHZ :
        case CLOCK_25MHZ :
            BCSCTL1 = 0x0f;                    // Set range
            DCOCTL = 0xe0;                     // Set DCO step + modulation */
        break;
        default :
            BCSCTL1 = CALBC1_1MHZ;
            DCOCTL = CALDCO_1MHZ;
            break;
    }

    // setup for SMCLK division rate
    if(mclk_spd >= CLOCK_8MHZ)
        BCSCTL2 |= DIVS_3;                    // divide SMCLK by 8(=1/8) if main clock is >= 8MHz
    else
        BCSCTL2 |= DIVS_0;                    // no divide//divide SMCLK by 2(=1/2) if main clock is < 8MHz

    // setup AclK source
    BCSCTL3 |= LFXT1S_0;                  // ACLK : LFXT1 = 32768Hz external Xtal
    //BCSCTL3 |= LFXT1S_2;                  // ACLK : LFXT1 = Internal VLO clk(4~20kHz, typ 12KHz)
}

/////////////////////////////////////////////////////////////
//                      PWM
/////////////////////////////////////////////////////////////

#define TIMER0_SMCLK_100usec            100
#define TIMER0_SMCLK_500usec            500
#define TIMER0_SMCLK_1msec              1000
#define TIMER0_SMCLK_5msec              5000
#define TIMER0_SMCLK_10msec             10000//2500        // = 1/250KHz * 2500 = 10msec
#define TIMER0_SMCLK_50msec             50000
//#define TIMER0_SMCLK_100msec            25000
//#define TIMER0_SMCLK_200msec            50000

#define CONST_TIMER0_CCR0               TIMER0_SMCLK_5msec
//#define CONST_TIMER0_CCR1               TIMER0_SMCLK_500usec
//#define CONST_TIMER0_CCR2               TIMER0_SMCLK_50msec


#define PWM_OUT0_PORT_DIR       P2DIR
#define PWM_OUT0_PORT_SEL0      P2SEL
#define PWM_OUT0_PORT_SEL2      P2SEL2
#define PWM_OUT0_BIT            BIT1

#define PWM_OUT1_PORT_DIR       P2DIR
#define PWM_OUT1_PORT_SEL0      P2SEL
#define PWM_OUT1_PORT_SEL2      P2SEL2
#define PWM_OUT1_BIT            BIT4

#define PWM_SMCLK_2MHZ_PERIOD_100HZ     20000
#define PWM_SMCLK_2MHZ_PERIOD_500HZ     10000
#define PWM_SMCLK_2MHZ_PERIOD_1KHZ      2000
#define PWM_SMCLK_2MHZ_PERIOD_5KHZ      1000
#define PWM_SMCLK_2MHZ_PERIOD_10KHZ     200
#define PWM_SMCLK_2MHZ_PERIOD_50KHZ     100
#define PWM_SMCLK_2MHZ_PERIOD_100KHZ    20

#define PWM_SMCLK_1MHZ_PERIOD_100HZ     10000
#define PWM_SMCLK_1MHZ_PERIOD_500HZ     5000
#define PWM_SMCLK_1MHZ_PERIOD_1KHZ      1000
#define PWM_SMCLK_1MHZ_PERIOD_5KHZ      500
#define PWM_SMCLK_1MHZ_PERIOD_10KHZ     100
#define PWM_SMCLK_1MHZ_PERIOD_50KHZ     50
#define PWM_SMCLK_1MHZ_PERIOD_100KHZ    10

#define PWM_PERIOD_COUNT                7       // 100, 500, 1k, 5k, 10k, 50k, 100kHz
#define PWM_DUTY_MIN                    0       // 0%
#define PWM_DUTY_MAX                    100     // 100%
#define PWM_DUTY_STEP_UNDER_10KHZ       1       // 1% step
#define PWM_DUTY_STEP_OVER_10KHZ        10      // 10% step

uint16_t pwm_period = PWM_SMCLK_1MHZ_PERIOD_1KHZ;
uint16_t pwm_duty = 0;

// P2.1, P2.4 as pwm source
void timer1_PWM_init(void)
{
    // PWM port init
    PWM_OUT0_PORT_DIR |= PWM_OUT0_BIT;                     // P1.6 and P1.7 output
    PWM_OUT0_PORT_SEL0 |= PWM_OUT0_BIT;                    // P1.6 and P1.7 options select
    PWM_OUT0_PORT_SEL2 &= ~(PWM_OUT0_BIT);
    // PWM port init
    PWM_OUT1_PORT_DIR |= PWM_OUT1_BIT;                     // P1.6 and P1.7 output
    PWM_OUT1_PORT_SEL0 |= PWM_OUT1_BIT;                    // P1.6 and P1.7 options select
    PWM_OUT1_PORT_SEL2 &= ~(PWM_OUT1_BIT);

    TA1CCR0 = pwm_period;                         // PWM Period 1MHz

    // CCR1 for P2.1
    TA1CCTL1 = OUTMOD_7;                      // CCR1 reset/set
    TA1CCR1 = 0;                            // CCR1 PWM duty cycle

    // CCR2 for P2.4
    TA1CCTL2 = OUTMOD_7;                      // CCR2 reset/set
    TA1CCR2 = 0;                            // CCR2 PWM duty cycle

    TA1CTL = TASSEL_2 | MC_0 | TACLR;  // SMCLK, STOP, clear TAR

    __no_operation();                         // For debugger
}

void timer1_PWM_stop(void)      // 1ms timer
{
    TA1CTL = TASSEL_2 + MC_0 + ID_0;                 // CONTINUOUE MODE/(SMCLK=1,000,000/1(ID_0)= 1MHz
}

void timer1_PWM_start(void)      // 1ms timer
{
    TA1CTL = TASSEL_2 + MC_1 + ID_0;                 // CONTINUOUE MODE/(SMCLK=1,000,000/1(ID_0)= 1MHz
}

// Period : 800
// increase/decrease : 80=>10%, 40=>5%, 4 ==> 0.5%
#define SPEED_MAX   (800-4)
#define SPEED_STEP  8
#define SPEED_START_POINT   392

#define SPEED_START_ON   80
#define SPEED_START_OFF  0

short speed_level = 0;        // 600/800 = 75% duty :: starting duty

void PWM_Generating(short level)
{
    unsigned short duty = 0;
    if(level == SPEED_START_OFF)
    {
        TA0CCTL1 = OUTMOD_5;                      // CCR1 reset/set
        __delay_cycles(1000);
        TA0CTL = MC_0;                          // stop

    }
    else
    {
        duty = (level+1);
        TA0CCR1 = duty;

        TA0CCTL1 = OUTMOD_7;                      // CCR1 reset/set
        TA0CTL = TASSEL_2 | MC_1 | TACLR;  // SMCLK, up mode, clear TAR
    }
}

void pwm_start_stop(uint8_t trigger_onoff)
{
    uint16_t read_period = 0, read_duty = 0;

    read_period = TA0CCR0;
    read_duty = TA0CCR1;

    if(read_period != 0 && read_duty != 0)
    {
        if(trigger_onoff == 1)
        {
            TA0CCTL1 = OUTMOD_7;                        // CCR1 reset/set
            TA0CTL = TASSEL_2 | MC_1 | TACLR;           // SMCLK, up mode, clear TAR
        }
        else
        {
            TA0CCTL1 = OUTMOD_5;                        // CCR1 reset/set
            __delay_cycles(1000);
            TA0CTL = MC_0;                              // stop
        }
    }

}

// 10msec,  50msec, 200msec continuous timer
void timerA0_init(void)
{
    TA0CTL = TASSEL_2 + MC_2 + ID_2;                   // SMCLK, Cont. mode, 1/4 = 1000/4=250kHz
    // CCR0 interrupt enabled : 10msec
    TA0CCTL0 = CCIE;
    TA0CCR0 = CONST_TIMER0_CCR0;
}

#define SIMPLE_GUI       1
#ifdef SIMPLE_GUI
#define MENU_IDLE                       0
#define MENU_PWM0_SETTING               1           // PWM0 duty increase (10~100%)
#define MENU_PWM1_SETTING               2           // PWM1 duty increase (10~100%)
#define MENU_DISP_STATUS                3           // display status (Temperature, Humidity, Vbus voltage, PWM duty increase rate..)
#define MENU_MAX                        4
#else
#define MENU_IDLE                       0
#define MENU_PWM0_SETTING               1           // PWM0 duty increase (10~100%)
#define MENU_PWM1_SETTING               2           // PWM1 duty increase (10~100%)
//#define MENU_LED_CHANGE                 3           // PWM duty change rate
#define MENU_PWM_DUTY_INC_RATE          3           // PWM duty increase rate (5%, 10% or 20%)
#define MENU_PWM_CHANGE_STEP_LINAR      4           // Set PWM duty increase as STEP or LINEAR
#define MENU_LED_ON_TIME_SET            5           // LED ON duration time set
#define MENU_BEEP_ON_OFF                6
#define MENU_DISP_STATUS                7           // display status (Temperature, Humidity, Vbus voltage, PWM duty increase rate..)
#define MENU_MAX                        8
#endif

#define PWM_DEFAULT_DUTY                100         // 10%
#define PWM_MAX_DUTY                    1000        // 100%
uint8_t menu = MENU_PWM0_SETTING;
uint16_t pwm0_duty_target = PWM_DEFAULT_DUTY;
uint16_t pwm1_duty_target = PWM_DEFAULT_DUTY;
uint16_t pwm0_duty = PWM_DEFAULT_DUTY;
uint16_t pwm1_duty = PWM_DEFAULT_DUTY;

#define LED_DIMMING_CHANGE_STEP        1
#define LED_DIMMING_CHANGE_LINEAR      2
uint8_t led_dimming_change_method = LED_DIMMING_CHANGE_STEP;

#define PWM_DUTY_INC_5_PERCENT          50      // 50 = 5%
#define PWM_DUTY_INC_10_PERCENT         100     // 100 = 10%
#define PWM_DUTY_INC_20_PERCENT         200     // 200 = 20%
uint8_t pwm_duty_increase_rate = PWM_DUTY_INC_10_PERCENT;

#define LED_ON_TIME_1HR         1
#define LED_ON_TIME_MAX         12
uint16_t led_on_time_hr = LED_ON_TIME_1HR;                     // led on duration in hour

#define ON                  1
#define OFF                 0
uint8_t beep_on_off = ON;
uint8_t tick_5ms = 0;


int16_t temperature_x10 = 0;    // uint16_t -> int16_t : consider (-) temperature
uint16_t humidity_x10 = 0;
char disp_buf[24];
char tmp_buf[6];
int16_t tmp_data[2];
// ADC Vbat checker
uint8_t g_vbat_decimal_x10 = 0;


const char str_Batt_low[] = "Batt Low : ";
const char str_____Vbat[] = " Vbat    : ";
const char str_dot[] = ".";
const char str_V[] = "V";
const char str_space[] = " ";
const char str__C[] = "'C";
const char str_percent_RH[] = " % RH";
const char str_Error_to_access[] = "Error to access";
const char str_HDC1080[] = "HDC1080..";
const char str_Always_on_mode[] = " Always on mode";
const char str_10sec_on_mode[] = " 10sec on mode";

void vbat_adc_check_display(void)
{
    // ADC battery check
    g_vbat_decimal_x10 = vbat_level_checker();
    //__delay_cycles(1000);


    ///////////////////////////
    // display battery status
    ///////////////////////////
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

}


void hdc1080_read_display(void)
{

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
        ssd1306_printString(0, 2, disp_buf, FONT_MS_GOTHIC8x16);

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
        ssd1306_printString(0, 4, disp_buf, FONT_MS_GOTHIC8x16);
    }
    else
    {
        ssd1306_printString(0, 2, "Error to access", FONT_MS_GOTHIC8x16);

        ssd1306_printString(0, 4, "HDC1080..", FONT_MS_GOTHIC8x16);

    }
}

void display_pwm0H_pwm1L_info(void)
{
    strcpy(disp_buf, "1.PWM0: ");
#ifdef CCS_VER9
    ltoa( (pwm0_duty_target/10), tmp_buf);
#else
    ltoa( (pwm0_duty_target/10), tmp_buf, 10);
#endif
    strcat(disp_buf, tmp_buf);
    strcat(disp_buf, "%  ");
    ssd1306_printString(0, 0, disp_buf, FONT_MS_GOTHIC8x16);          // 0 -> ... -> 100%

    strcpy(disp_buf, "   PWM1 : ");
#ifdef CCS_VER9
    ltoa( (pwm1_duty_target/10), tmp_buf);
#else
    ltoa( (pwm1_duty_target/10), tmp_buf, 10);
#endif
    strcat(disp_buf, tmp_buf);
    strcat(disp_buf, "%  ");
    ssd1306_printString(0, 2, disp_buf, FONT_6x8);
}
void display_pwm1H_pwm0L_info(void)
{
    strcpy(disp_buf, "   PWM0 : ");
#ifdef CCS_VER9
    ltoa( (pwm0_duty_target/10), tmp_buf);
#else
    ltoa( (pwm0_duty_target/10), tmp_buf, 10);
#endif
    strcat(disp_buf, tmp_buf);
    strcat(disp_buf, "%  ");
    ssd1306_printString(0, 0, disp_buf, FONT_6x8);

    strcpy(disp_buf, "2.PWM1: ");
#ifdef CCS_VER9
    ltoa( (pwm1_duty_target/10), tmp_buf);
#else
    ltoa( (pwm1_duty_target/10), tmp_buf, 10);
#endif
    strcat(disp_buf, tmp_buf);
    strcat(disp_buf, "%  ");
    ssd1306_printString(0, 1, disp_buf, FONT_MS_GOTHIC8x16);          // 0 -> ... -> 100%
}
void display_pwm0L_pwm1L_info(void)
{
    strcpy(disp_buf, " PWM0 : ");
#ifdef CCS_VER9
    ltoa( (pwm0_duty_target/10), tmp_buf);
#else
    ltoa( (pwm0_duty_target/10), tmp_buf, 10);
#endif
    strcat(disp_buf, tmp_buf);
    strcat(disp_buf, "%  ");
    ssd1306_printString(0, 0, disp_buf, FONT_6x8);

    strcpy(disp_buf, " PWM1 : ");
#ifdef CCS_VER9
    ltoa( (pwm1_duty_target/10), tmp_buf);
#else
    ltoa( (pwm1_duty_target/10), tmp_buf, 10);
#endif
    strcat(disp_buf, tmp_buf);
    strcat(disp_buf, "%  ");
    ssd1306_printString(0, 1, disp_buf, FONT_6x8);          // 0 -> ... -> 100%
}

uint16_t *Flash_ptr = 0x0000;                          // Flash pointer

void read_infoFlash(void)
{
    Flash_ptr = (uint16_t *) 0x1040;              // Initialize Flash pointer   // information region

    pwm0_duty = pwm0_duty_target = *(Flash_ptr);
    pwm1_duty = pwm1_duty_target = *(Flash_ptr+1);
}

void write_infoFlash(void)
{
    FCTL2 = FWKEY + FSSEL0 + FN1;             // MCLK/3 for Flash Timing Generator

    Flash_ptr = (uint16_t *) 0x1040;              // Initialize Flash pointer   // information region
    FCTL1 = FWKEY + ERASE;                    // Set Erase bit
    FCTL3 = FWKEY;                            // Clear Lock bit
    *Flash_ptr = 0;                           // Dummy write to erase Flash segment

    FCTL1 = FWKEY + WRT;                      // Set WRT bit for write operation

    *Flash_ptr++ = (uint16_t)pwm0_duty;                   // Write value to flash
    *Flash_ptr++ = (uint16_t)pwm1_duty;                   // Write value to flash

    FCTL1 = FWKEY;                            // Clear WRT bit
    FCTL3 = FWKEY + LOCK;                     // Set LOCK bit

}

void main(void)
{
    WDTCTL = WDTPW | WDTHOLD;                   // stop watchdog timer

    clock_init(CLOCK_8MHZ);
    delay_ms(1);

    timer1_PWM_init();

    GPIO_SWs_Interrupt_Set();      // Set P1.2 and P2.6 as interrupt source setup

    // OLED initialization
    i2c_init();                                 // initialize I2C to use with OLED
    delay_ms(1);

    ssd1306_init();                             // Initialize SSD1306 OLED
    ssd1306_clearDisplay();                     // Clear OLED display
    delay_ms(1);

    // HDC1080 init
    GPIO_I2C_Init();

    SW1_PORT_IFG &= ~SW1_BIT;                         // Clear SW1 IFG
    SW2_PORT_IFG &= ~SW2_BIT;                         // Clear SW2 IFG

    timerA0_init();
    timer1_PWM_start();
    __bis_SR_register(/*LPM0_bits +*/ GIE);     // let MCU as always on

    // animation effect
    ssd1306_printString(0, 0, " Gardening", FONT_MS_GOTHIC8x16);
    ssd1306_printString(0, 2, " LED v1.0", FONT_MS_GOTHIC8x16);
    ssd1306_printString(0, 4, " Made by", FONT_MS_GOTHIC8x16);
    ssd1306_printString(0, 6, " Ernest Cho", FONT_MS_GOTHIC8x16);
    delay_ms(1000);

    //ssd1306_clearDisplay();
    ssd1306_clear_one_line(0);
    ssd1306_clear_one_line(1);
    delay_ms(100);
    ssd1306_clear_one_line(2);
    ssd1306_clear_one_line(3);
    delay_ms(100);
    ssd1306_clear_one_line(4);
    ssd1306_clear_one_line(5);
    delay_ms(100);
    ssd1306_clear_one_line(6);
    ssd1306_clear_one_line(7);
    delay_ms(100);

    read_infoFlash();
    TA1CCR1 = pwm0_duty = pwm0_duty_target;
    TA1CCR2 = pwm1_duty = pwm1_duty_target;
    tick_5ms = 0;

    switch_status |= BIT0;

    menu = MENU_IDLE;
    while(1)
    {
        ///////////////////////////////////////////////////
        // check switch buttons are pressed : SW1
        ///////////////////////////////////////////////////
        if(switch_status & BIT0) // switch1 function : menu change
        {
            menu++;
            if(menu >= MENU_MAX)   // end? then repeat first
            {
                menu = MENU_PWM0_SETTING;
            }

            ssd1306_clearDisplay();
            if(menu == MENU_PWM0_SETTING)
            {
                display_pwm0H_pwm1L_info();
            }
            else if(menu == MENU_PWM1_SETTING)
            {
                display_pwm1H_pwm0L_info();
            }

#if !defined (SIMPLE_GUI)
            // this part is a kind of minor setting
            else if(menu == MENU_PWM_DUTY_INC_RATE || menu == MENU_PWM_CHANGE_STEP_LINAR || menu == MENU_LED_ON_TIME_SET || menu == MENU_BEEP_ON_OFF)
            {
                // display PWM0, PWM1 info
                display_pwm0L_pwm1L_info();

                if(menu == MENU_PWM_DUTY_INC_RATE)
                {
                    ssd1306_printString(0, 3, "3.increase %:", FONT_6x8);      // 5%, 10%, 20%

                    //display_pwm_incr_rate_info();
                    if(pwm_duty_increase_rate == PWM_DUTY_INC_5_PERCENT)
                        strcpy(disp_buf, "+ 5%");
                    else if(pwm_duty_increase_rate == PWM_DUTY_INC_10_PERCENT)
                        strcpy(disp_buf, "+10%");
                    else
                        strcpy(disp_buf, "+20%");
                    ssd1306_printString(84, 3, disp_buf, FONT_MS_GOTHIC8x16);      // 5%, 10%, 20%

                }
                else if(menu == MENU_PWM_CHANGE_STEP_LINAR)
                {
                    ssd1306_printString(0, 4, "4.Dim:", FONT_6x8);         // instant(step), linear

                    if(led_dimming_change_method == LED_DIMMING_CHANGE_STEP)
                        strcpy(disp_buf, "Step  ");
                    else
                        strcpy(disp_buf, "Linear");
                    ssd1306_printString(56, 4, disp_buf, FONT_MS_GOTHIC8x16);
                }
                else if(menu == MENU_LED_ON_TIME_SET)
                {
                    ssd1306_printString(0, 5, "5.Timer:", FONT_6x8);         // 1hr -> 10hr

                    strcpy(disp_buf, " ");
                    ltoa(led_on_time_hr, tmp_buf);
                    strcat(disp_buf, tmp_buf);
                    strcat(disp_buf, "H");
                    ssd1306_printString(70, 5, disp_buf, FONT_MS_GOTHIC8x16);
                }
                else if(menu == MENU_BEEP_ON_OFF)
                {
                    ssd1306_printString(0, 6, "6.BEEP:", FONT_6x8);      // on, off

                    if(beep_on_off == ON)
                        strcpy(disp_buf, " On");
                    else
                        strcpy(disp_buf, " Off");
                    ssd1306_printString(64, 6, disp_buf, FONT_MS_GOTHIC8x16);
                }
                else
                {
                    ssd1306_printString(0, 0, "Nothing to do1..", FONT_6x8);

                }
            }
#endif // (SIMPLE_GUI)

            else if(menu == MENU_DISP_STATUS)
            {
                ssd1306_printString(0, 0, "====Status====", FONT_6x8);          // temp, humidity

                hdc1080_read_display();

                strcpy(disp_buf, " ");
#ifdef CCS_VER9
                ltoa(led_on_time_hr, tmp_buf);
#else
                ltoa(led_on_time_hr, tmp_buf, 10);
#endif
                strcat(disp_buf, tmp_buf);
                strcat(disp_buf, "Hr");
                ssd1306_printString(0, 6, disp_buf, FONT_MS_GOTHIC8x16);
            }
            else
            {
                ssd1306_printString(0, 0, "Nothing to do2..", FONT_6x8);

            }


            switch_status &= ~BIT0;
        }

        /////////////////////////////////////////////////
        // check switch buttons are pressed : SW2
        /////////////////////////////////////////////////
        if(switch_status & BIT1)    // switch2 function : status change
        {
            if(menu == MENU_PWM0_SETTING)
            {
                pwm0_duty_target += pwm_duty_increase_rate;
                if(pwm0_duty_target > PWM_MAX_DUTY)
                {
                    pwm0_duty_target = 0;
                }

                display_pwm0H_pwm1L_info();

            }
            else if(menu == MENU_PWM1_SETTING)
            {
                pwm1_duty_target += pwm_duty_increase_rate;
                if(pwm1_duty_target > PWM_MAX_DUTY)
                {
                    pwm1_duty_target = 0;
                }

                display_pwm1H_pwm0L_info();
            }

#if !defined (SIMPLE_GUI)
            // this part is a kind of minor setting
            else if(menu == MENU_PWM_DUTY_INC_RATE || MENU_PWM_CHANGE_STEP_LINAR || MENU_LED_ON_TIME_SET || MENU_BEEP_ON_OFF)
            {
                // display PWM0, PWM1 info
                display_pwm0L_pwm1L_info();

                if(menu == MENU_PWM_DUTY_INC_RATE)
                {
                    if(pwm_duty_increase_rate == PWM_DUTY_INC_5_PERCENT)
                        pwm_duty_increase_rate = PWM_DUTY_INC_10_PERCENT;
                    else if(pwm_duty_increase_rate == PWM_DUTY_INC_10_PERCENT)
                        pwm_duty_increase_rate = PWM_DUTY_INC_20_PERCENT;
                    else
                        pwm_duty_increase_rate = PWM_DUTY_INC_5_PERCENT;

                    //display_pwm_incr_rate_info();
                    if(pwm_duty_increase_rate == PWM_DUTY_INC_5_PERCENT)
                        strcpy(disp_buf, "+5%");
                    else if(pwm_duty_increase_rate == PWM_DUTY_INC_10_PERCENT)
                        strcpy(disp_buf, "+10%");
                    else
                        strcpy(disp_buf, "+20%");
                    ssd1306_printString(86, 3, disp_buf, FONT_MS_GOTHIC8x16);      // 5%, 10%, 20%
                }
                else if(menu == MENU_PWM_CHANGE_STEP_LINAR)
                {
                    if(led_dimming_change_method == LED_DIMMING_CHANGE_STEP)
                        led_dimming_change_method = LED_DIMMING_CHANGE_LINEAR;
                    else
                        led_dimming_change_method = LED_DIMMING_CHANGE_STEP;


                    if(led_dimming_change_method == LED_DIMMING_CHANGE_STEP)
                        strcpy(disp_buf, "Step");
                    else
                        strcpy(disp_buf, "Linear");
                    ssd1306_printString(56, 4, disp_buf, FONT_MS_GOTHIC8x16);
                }
                else if(menu == MENU_LED_ON_TIME_SET)
                {
                    led_on_time_hr += LED_ON_TIME_1HR;
                    if(led_on_time_hr > LED_ON_TIME_MAX)
                    {
                        led_on_time_hr = LED_ON_TIME_1HR;
                    }

                    strcpy(disp_buf, " ");
                    ltoa(led_on_time_hr, tmp_buf);
                    strcat(disp_buf, tmp_buf);
                    strcat(disp_buf, "H");
                    ssd1306_printString(76, 5, disp_buf, FONT_MS_GOTHIC8x16);
                }
                else if(menu == MENU_BEEP_ON_OFF)
                {
                    if(beep_on_off == ON)
                        beep_on_off = OFF;
                    else
                        beep_on_off = ON;


                    if(beep_on_off == ON)
                        strcpy(disp_buf, " On");
                    else
                        strcpy(disp_buf, " Off");
                    ssd1306_printString(64, 6, disp_buf, FONT_MS_GOTHIC8x16);
                }
            }
#endif
            else if(menu == MENU_DISP_STATUS)
            {

            }


            switch_status &= ~BIT1;
        }
        /////////////////////////////////////////////////
        // actions for case by case of the buttons
        /////////////////////////////////////////////////
        switch (menu)
        {
            case MENU_PWM0_SETTING :
            {
                if(led_dimming_change_method == LED_DIMMING_CHANGE_STEP)
                {
                    pwm0_duty = pwm0_duty_target;
                    if(pwm0_duty > 1000)
                    {
                        pwm0_duty = 0;
                    }
                    TA1CCR1 = pwm0_duty;
                }
                else if(led_dimming_change_method == LED_DIMMING_CHANGE_LINEAR)
                {
                    if(tick_5ms == 1)
                    {
                        if(pwm0_duty_target > pwm0_duty)
                        {
                            if(pwm0_duty <= 990)
                                pwm0_duty += 10;
                            else
                                pwm0_duty = 1000;

                            strcpy(disp_buf, " ");
#ifdef CCS_VER9
                            ltoa( pwm0_duty, tmp_buf);
#else
                            ltoa( pwm0_duty, tmp_buf, 10);
#endif
                            strcat(disp_buf, tmp_buf);
                            strcat(disp_buf, "   ");
                            ssd1306_printString(0, 5, disp_buf, FONT_6x8);

                        }
                        else if(pwm0_duty_target < pwm0_duty)
                        {
                            if(pwm0_duty >= 10)
                                pwm0_duty -= 10;
                            else
                                pwm0_duty = 0;

                            strcpy(disp_buf, " ");
#ifdef CCS_VER9
                            ltoa( pwm0_duty, tmp_buf);
#else
                            ltoa( pwm0_duty, tmp_buf, 10);
#endif
                            strcat(disp_buf, tmp_buf);
                            strcat(disp_buf, "   ");
                            ssd1306_printString(0, 5, disp_buf, FONT_6x8);

                        }
                        TA1CCR1 = pwm0_duty;
                        tick_5ms = 0;

                    }
                }
            }break;
            case MENU_PWM1_SETTING :
            {
                if(led_dimming_change_method == LED_DIMMING_CHANGE_STEP)
                {
                    pwm1_duty = pwm1_duty_target;
                    if(pwm1_duty > 1000)
                    {
                        pwm1_duty = 0;
                    }
                    TA1CCR2 = pwm1_duty;
                }
                else if(led_dimming_change_method == LED_DIMMING_CHANGE_LINEAR)
                {
                    if(tick_5ms == 1)
                    {
                        if(pwm1_duty_target > pwm1_duty)
                        {
                            if(pwm1_duty <= 990)
                                pwm1_duty += 10;
                            else
                                pwm1_duty = 1000;

                            strcpy(disp_buf, " ");
#ifdef CCS_VER9
                            ltoa( pwm1_duty, tmp_buf);
#else
                            ltoa( pwm1_duty, tmp_buf, 10);
#endif
                            strcat(disp_buf, tmp_buf);
                            strcat(disp_buf, "   ");
                            ssd1306_printString(0, 5, disp_buf, FONT_6x8);

                        }
                        else if(pwm1_duty_target < pwm1_duty)
                        {
                            if(pwm1_duty >= 10)
                                pwm1_duty -= 10;
                            else
                                pwm1_duty = 0;

                            strcpy(disp_buf, " ");
#ifdef CCS_VER9
                            ltoa( pwm1_duty, tmp_buf);
#else
                            ltoa( pwm1_duty, tmp_buf, 10);
#endif
                            strcat(disp_buf, tmp_buf);
                            strcat(disp_buf, "   ");
                            ssd1306_printString(0, 5, disp_buf, FONT_6x8);
                        }
                        TA1CCR2 = pwm1_duty;
                        tick_5ms = 0;
                    }
                }
            }break;
#if !defined (SIMPLE_GUI)
            case MENU_PWM_DUTY_INC_RATE :
            {

            }break;
            case MENU_PWM_CHANGE_STEP_LINAR :
            {
            }break;
            case MENU_LED_ON_TIME_SET :
            {
            }break;
            case MENU_BEEP_ON_OFF :
            {
            }break;
#endif // SIMPLE_GUI
            case MENU_DISP_STATUS :
            {
                write_infoFlash();
            }break;
            default : break;
        }


    }


}

///////////////////////////////////////////////////////////////////////
// Timer Interrupt handler part
///////////////////////////////////////////////////////////////////////
// 5ms continuous mode timer
#pragma vector=TIMER0_A0_VECTOR
__interrupt void ISR_Timer0_A0(void)
{
    TA0CCR0 += CONST_TIMER0_CCR0;               // Add Offset to CCR1

    tick_5ms = 1;
//    timer_cnt++;
//    if(timer_cnt >= ONE_HOUR_FROM_50ms_Tick)        //ONE_MIN_FROM_50ms_Tick : test for shortening the time
//    {
//        timer_cnt = 0;

//        led_mode = LED_ACTION_MAX;          // will generate sleep at the main loop
//        switch_status |= BIT0;              // generate emulated signal of button press..
//    }
}


#pragma vector=TIMER0_A1_VECTOR
__interrupt void Timer0_A1A2(void)
{
    switch( TA0IV )
    {
        case  2:                                        // CCR1
        {
//            TA0CCR1 += CONST_TIMER0_CCR1;               // Add Offset to CCR1

        }
        break;
        case  4:                                        // CCR2
        {
//            TA0CCR2 += CONST_TIMER0_CCR2;               // Add Offset to CCR2

        } break;
        case 10:                                        // TA0 Overflow
        {
            //P1OUT ^= LED_TA0_OVF;
        } break;
    }
}

///////////////////////////////////////////////////////////////////////
// Port External Interrupt handler part
///////////////////////////////////////////////////////////////////////

// Port 1 interrupt service routine
#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void)
{
    // switch 1 detection ==> sw1 function : mode change
    if(SW1_PORT_IFG & SW1_BIT)
    {
        SW1_PORT_IFG &= ~SW1_BIT;                         // Clear PORT IFG for SW1

        if(switch_status == BIT1)   // do not operate if another switch pin is on operation
        {
            ;
        }
        else
        {
            switch_status |= BIT0;

            //timer_cnt = 0;
        }
    }

    else
    {
        SW1_PORT_IFG &= ~SW1_BIT;
    }

    // switch 2 detection ==> sw2 function : brightness control
    if(SW2_PORT_IFG & SW2_BIT)
    {
        SW2_PORT_IFG &= ~SW2_BIT;                         // Clear PORT IFG for SW1

        if(switch_status == BIT0)   // do not operate if another switch pin is on operation
        {
            ;
        }
        else
        {
            switch_status |= BIT1;

            //timer_cnt = 0;
        }
    }

    else
    {
        SW2_PORT_IFG &= ~SW2_BIT;
    }

}





