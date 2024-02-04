#include <msp430.h> 
#include <stdint.h>
#include "LP5012.h"



#define DISABLE     0
#define ENABLE      1


// SW1 = P1.0
#define SW1_PORT_DIR            P1DIR
#define SW1_PORT_OUT            P1OUT
#define SW1_PORT_REN            P1REN
#define SW1_PORT_IES            P1IES
#define SW1_PORT_IE             P1IE
#define SW1_PORT_IFG            P1IFG
#define SW1_BIT                 BIT0

// SW2 = P1.1
#define SW2_PORT_DIR            P1DIR
#define SW2_PORT_OUT            P1OUT
#define SW2_PORT_REN            P1REN
#define SW2_PORT_IES            P1IES
#define SW2_PORT_IE             P1IE
#define SW2_PORT_IFG            P1IFG
#define SW2_BIT                 BIT1


// timerA0 constant definition. use SMCLK(2MHz)
#define TIMER0_10SEC_VERSUS_100msec     100

#define TIMER0_SMCLK_100usec            100
#define TIMER0_SMCLK_500usec            500
#define TIMER0_SMCLK_1msec              1000
#define TIMER0_SMCLK_5msec              5000
#define TIMER0_SMCLK_10msec             10000//2500        // = 1/250KHz * 2500 = 10msec
#define TIMER0_SMCLK_50msec             50000
//#define TIMER0_SMCLK_100msec            25000
//#define TIMER0_SMCLK_200msec            50000

#define CONST_TIMER0_CCR0               TIMER0_SMCLK_50msec//TIMER0_SMCLK_1msec//TIMER0_SMCLK_10msec
#define CONST_TIMER0_CCR1               TIMER0_SMCLK_500usec
#define CONST_TIMER0_CCR2               TIMER0_SMCLK_50msec

// 10msec,  50msec, 200msec continuous timer
void timerA0_init(void)
{
    TA0CTL = TASSEL_2 + MC_2;                   // SMCLK, Cont. mode
    // CCR0 interrupt enabled : 10msec
    TA0CCTL0 = CCIE;
    TA0CCR0 = CONST_TIMER0_CCR0;
    // CCR1 interrupt enabled : 100msec
//    TA0CCTL1 = CCIE;
//    TA0CCR1 = CONST_TIMER0_CCR1;
    // CCR2 interrupt enabled : 200msec
//    TA0CCTL2 = CCIE;
//    TA0CCR2 = CONST_TIMER0_CCR2;
}

///////////////////////////////////////////////////////////////////////
// GPIO Switch interrupt part
///////////////////////////////////////////////////////////////////////

// GPIO_P1InterruptSet() : external interrupt setup for P1
// param : bits - multi-bits setup
void GPIO_SW1_SW2_Interrupt_Set(void)
{
    SW1_PORT_DIR &= ~SW1_BIT;                         // port as Input
    SW1_PORT_OUT |= SW1_BIT;                          // Configure P1 as pulled-up
    SW1_PORT_REN |= SW1_BIT;                          // P1 pull-up register enable
    SW1_PORT_IES &= ~SW1_BIT;                         // P1 edge, 0: low to high edge
    SW1_PORT_IE  |= SW1_BIT;                          // P1 interrupt enabled
    SW1_PORT_IFG &= ~SW1_BIT;                         // P1 IFG cleared

    SW2_PORT_DIR &= ~SW2_BIT;
    SW2_PORT_OUT |= SW2_BIT;                          // Configure P2.x as pulled-up
    SW2_PORT_REN |= SW2_BIT;                          // P2.x pull-up register enable
    SW2_PORT_IES &= ~SW2_BIT;                         // P2.x edge, 0: low to high edge
    SW2_PORT_IE  |= SW2_BIT;                          // P2.x interrupt enabled
    SW2_PORT_IFG &= ~SW2_BIT;                         // P2.x IFG cleared
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

void GPIO_LP5012_IC_enable(unsigned char en)
{
    LP5012_EN_DIR_OUTPUT;

    if(en == 1)
        LP5012_EN_PIN_HIGH;
    else
        LP5012_EN_PIN_LOW;
    delay_ms(10);
}

void G2553_LP5012_board_init()
{
    LP5012_I2C_chip_reset(LP5012_DEV0_ADDR, 1);
    delay_ms(100);

    LP5012_I2C_chip_enable(LP5012_DEV0_ADDR, ENABLE);

    delay_ms(100);

}

// for debugging
uint8_t data_buf[0x16] = { 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0};

uint8_t brightness = 0, led_ch = 0;

#define NUM_OF_LP5012_DEV   1
#define MAX_LED     9

uint8_t switch_status = 0;

// LED mode :
// sw1 action :
// - LED on(sw2 : brightness),
// - LED odd on(sw2 : brightness),
// - LED 3ea on(sw2 : brightness),
// - brightness inc(sw2 : delay time)
// - brightness dec(sw2 : delay time)
// - brightness up(sw2 : delay time)
// - brightness down(sw2 : delay time)
// - brightness up/down(sw2 : delay time)
#define LED_ACTION_SLEEP                        0
#define LED_ACTION_ALL_ON                       1
#define LED_ACTION_ODD_ON                       2
#define LED_ACTION_3EA_ON                       3
#define LED_ACTION_BRIGHTNESS_INC               4
#define LED_ACTION_BRIGHTNESS_INC_DEC           5
#define LED_ACTION_BRIGHTNESS_SHIFT_UP          6
#define LED_ACTION_BRIGHTNESS_SHIFT_DOWN        7
#define LED_ACTION_BRIGHTNESS_SHIFT_UPDN        8
#define LED_ACTION_MAX                          9

uint8_t led_mode = 0;               // mode : 1~8
uint8_t brightness_level = 0;       // level : 1(dark)~5(bright)
uint8_t delay_time = 0;             // delay_time : 1(slow)~5(fast)
uint8_t update_once = 0;        // not to do excessive updates of LP5012 registers
uint8_t initial_onetime_activity = 1;
uint8_t cmd_inc_dec = 0;        // 0 : inc, 1 : dec

void main(void)
{
//    unsigned char i, data;

    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
    //
    // Clear port lock
    //
    PM5CTL0 &= ~LOCKLPM5;

    GPIO_SW1_SW2_Interrupt_Set();
    timerA0_init();

    // move this for current consumption
    //GPIO_LP5012_IC_enable(1);
    //GPIO_I2C_Init();
    //G2553_LP5012_board_init();

    //i2c_write_byte(LP5012_DEV0_ADDR, 0x01, 0x00);       // set config1 register

    SW1_PORT_IFG &= ~SW1_BIT;                         // Clear PORT IFG for SW1
    SW2_PORT_IFG &= ~SW2_BIT;                         // Clear PORT IFG for SW1
    __bis_SR_register(GIE);

    while(1)
    {
        ///////////////////////////////////////////////////
        // check switch buttons are pressed : SW1
        ///////////////////////////////////////////////////
        if(switch_status & BIT0) // switch1 function : action change
        {
            if(initial_onetime_activity == 1)   // moved here for current consumption
            {
                initial_onetime_activity = 0;
                GPIO_LP5012_IC_enable(1);
                GPIO_I2C_Init();
                G2553_LP5012_board_init();

                //i2c_write_byte(LP5012_DEV0_ADDR, LP5012_DEVICE_CONFIG1, BIT5);       // set config1 register
                i2c_write_byte(LP5012_DEV0_ADDR, LP5012_DEVICE_CONFIG1, 0x00);       // set config1 register
            }

            update_once = 1;
            led_mode++;
            if(led_mode >= LED_ACTION_MAX)   // end? then repeat first
            {
                led_mode = LED_ACTION_SLEEP;
            }

            if(led_mode == LED_ACTION_ALL_ON || led_mode == LED_ACTION_ODD_ON || led_mode == LED_ACTION_3EA_ON)
            {
                brightness = 1;
                delay_time = 1;   // no change
            }
            else    // case of LED_ACTION_BRIGHTNESS_INC, LED_ACTION_BRIGHTNESS_INC_DEC, LED_ACTION_BRIGHTNESS_SHIFT_UP, LED_ACTION_BRIGHTNESS_SHIFT_DOWN, LED_ACTION_BRIGHTNESS_SHIFT_UPDN
            {
                brightness = 1;     // no change
                delay_time = 1;
            }

            //G2553_LP5012_board_init();
            //i2c_write_byte(LP5012_DEV0_ADDR, 0x01, 0x00);       // set config1 register

            switch_status &= ~BIT0;
        }

        /////////////////////////////////////////////////
        // check switch buttons are pressed : SW2
        /////////////////////////////////////////////////
        if(switch_status & BIT1)    // switch2 function : brightness or delay time change
        {
            // initial operation can be run for both pins..
            if(initial_onetime_activity == 1)   // moved here for current consumption
            {
                initial_onetime_activity = 0;
                GPIO_LP5012_IC_enable(1);
                GPIO_I2C_Init();
                G2553_LP5012_board_init();

                //i2c_write_byte(LP5012_DEV0_ADDR, LP5012_DEVICE_CONFIG1, BIT5);       // set config1 register
                i2c_write_byte(LP5012_DEV0_ADDR, LP5012_DEVICE_CONFIG1, 0x00);       // set config1 register

                led_mode++;
                brightness = 1;
                delay_time = 1;   // no change

            }

            update_once = 1;

            if(led_mode == LED_ACTION_ALL_ON || led_mode == LED_ACTION_ODD_ON || led_mode == LED_ACTION_3EA_ON)
            {
                brightness++;
                if(brightness > 8)      // change brightness steps from 5(5*50=250) to 8(8*31 = 248)
                {
                    brightness = 1;
                }
                delay_time = 0;   // no change
            }
            else    // case of LED_ACTION_BRIGHTNESS_INC, LED_ACTION_BRIGHTNESS_INC_DEC, LED_ACTION_BRIGHTNESS_SHIFT_UP, LED_ACTION_BRIGHTNESS_SHIFT_DOWN, LED_ACTION_BRIGHTNESS_SHIFT_UPDN
            {
                delay_time--;
                if(delay_time == 0)
                {
                    delay_time = 5;
                }
                brightness = 1;     // no change
            }

            switch_status &= ~BIT1;
        }

        /////////////////////////////////////////////////
        // actions for case by case of the buttons
        /////////////////////////////////////////////////
        switch (led_mode)
        {
            case LED_ACTION_SLEEP :
            {
                if(update_once == 1)   // update one time when the button pressed
                {
                    for(led_ch=0; led_ch<MAX_LED; led_ch++)
                    {
                        LP5012_I2C_OUTx_color(LP5012_DEV0_ADDR, led_ch, 0x00);      // color level set from 0x80 to 0xc0
                    }
                    LP5012_I2C_LEDx_ch_brightness(LP5012_DEV0_ADDR, 0x00, 0x00, 0x00, 0x00);

                    GPIO_LP5012_IC_enable(0);
                    initial_onetime_activity = 1;
                }
            }break;
            case LED_ACTION_ALL_ON :
            {
                if(update_once == 1)   // update one time when the button pressed
                {
                    GPIO_LP5012_IC_enable(1);

                    brightness_level = brightness * 31;         // brightness : 1~5 --> 1~8
                    // seperate color control
                    for(led_ch=0; led_ch<MAX_LED; led_ch++)
                    {
                        LP5012_I2C_OUTx_color(LP5012_DEV0_ADDR, led_ch, 0xc0);      // color level set from 0x80 to 0xc0
                    }
                    LP5012_I2C_LEDx_ch_brightness(LP5012_DEV0_ADDR, brightness_level, brightness_level, brightness_level, brightness_level);
                    delay_ms(200);
                }

                __no_operation();


            }break;
            case LED_ACTION_ODD_ON :
            {
                if(update_once == 1)   // update one time when the button pressed
                {
                    brightness_level = brightness * 31;         // brightness : 1~5 --> 1~8
                    // seperate color control
                    for(led_ch=1; led_ch<MAX_LED; led_ch+=2)    // even led = 0
                    {
                        LP5012_I2C_OUTx_color(LP5012_DEV0_ADDR, led_ch, 0x00);
                    }
                    for(led_ch=0; led_ch<MAX_LED; led_ch+=2)
                    {
                        LP5012_I2C_OUTx_color(LP5012_DEV0_ADDR, led_ch, 0xc0);      // color level set from 0x80 to 0xc0
                    }
                    LP5012_I2C_LEDx_ch_brightness(LP5012_DEV0_ADDR, brightness_level, brightness_level, brightness_level, brightness_level);

                    delay_ms(200);
                }
                __no_operation();

            }break;
            case LED_ACTION_3EA_ON :
            {
                if(update_once == 1)   // update one time when the button pressed
                {
                    brightness_level = brightness * 31;         // brightness : 1~5 --> 1~8
                    // seperate color control
                    for(led_ch=1; led_ch<MAX_LED; led_ch++)
                    {
                        LP5012_I2C_OUTx_color(LP5012_DEV0_ADDR, led_ch, 0x00);
                    }
                    LP5012_I2C_OUTx_color(LP5012_DEV0_ADDR, 0, 0xc0);      // color level set from 0x80 to 0xc0
                    LP5012_I2C_OUTx_color(LP5012_DEV0_ADDR, 4, 0xc0);
                    LP5012_I2C_OUTx_color(LP5012_DEV0_ADDR, 8, 0xc0);

                    LP5012_I2C_LEDx_ch_brightness(LP5012_DEV0_ADDR, brightness_level, brightness_level, brightness_level, brightness_level);

                    delay_ms(200);
                }

                __no_operation();

            }break;
            case LED_ACTION_BRIGHTNESS_INC :
            {
                if(update_once == 1)   // update one time when the button pressed
                {
                    // seperate color control
                    for(led_ch=0; led_ch<MAX_LED; led_ch++)
                    {
                        LP5012_I2C_OUTx_color(LP5012_DEV0_ADDR, led_ch, 0x40);
                    }
                    brightness_level = 0;
                }
                brightness_level += 5;
                if(brightness_level >= 200)
                {
                    brightness_level = 0;
                }

                LP5012_I2C_LEDx_ch_brightness(LP5012_DEV0_ADDR, brightness_level, brightness_level, brightness_level, brightness_level);

                if(delay_time == 1)
                    delay_ms(25);
                else if(delay_time == 2)
                    delay_ms(50);
                else if(delay_time == 3)
                    delay_ms(75);
                else if(delay_time == 4)
                    delay_ms(100);
                else if(delay_time == 5)
                    delay_ms(125);
                else
                    delay_ms(25);

                __no_operation();
            }break;
            case LED_ACTION_BRIGHTNESS_INC_DEC :
            {

                if(update_once == 1)   // update one time when the button pressed
                {
                    // seperate color control
                    for(led_ch=0; led_ch<MAX_LED; led_ch++)
                    {
                        LP5012_I2C_OUTx_color(LP5012_DEV0_ADDR, led_ch, 0x40);
                    }
                    brightness_level = 0;
                    cmd_inc_dec = 0;    // increase
                }

                if(cmd_inc_dec == 0)
                {
                    brightness_level += 5;
                    if(brightness_level >= 200)
                    {
                        cmd_inc_dec = 1;    // decrease
                    }
                }
                else
                {
                    brightness_level -= 5;
                    if(brightness_level == 0)
                    {
                        cmd_inc_dec = 0;    // increase
                    }

                }
                LP5012_I2C_LEDx_ch_brightness(LP5012_DEV0_ADDR, brightness_level, brightness_level, brightness_level, brightness_level);

                if(delay_time == 1)
                    delay_ms(25);
                else if(delay_time == 2)
                    delay_ms(50);
                else if(delay_time == 3)
                    delay_ms(75);
                else if(delay_time == 4)
                    delay_ms(100);
                else if(delay_time == 5)
                    delay_ms(125);
                else
                    delay_ms(25);

                __no_operation();
            }break;
            case LED_ACTION_BRIGHTNESS_SHIFT_UP :
            {
                if(update_once == 1)   // update one time when the button pressed
                {
                    LP5012_I2C_LEDx_ch_brightness(LP5012_DEV0_ADDR, 0x80, 0x80, 0x80, 0x80);

                    // seperate color control
                    for(led_ch=0; led_ch<MAX_LED; led_ch++)
                    {
                        LP5012_I2C_OUTx_color(LP5012_DEV0_ADDR, led_ch, 0x00);
                    }
                    led_ch = 0;
                }

                // off the previous LED
                if(led_ch == 0)
                    LP5012_I2C_OUTx_color(LP5012_DEV0_ADDR, 8, 0);
                else
                    LP5012_I2C_OUTx_color(LP5012_DEV0_ADDR, led_ch-1, 0);

                LP5012_I2C_OUTx_color(LP5012_DEV0_ADDR, led_ch++, 0x80);
                if(led_ch == 9)
                    led_ch = 0;


                if(delay_time == 1)
                    delay_ms(25);
                else if(delay_time == 2)
                    delay_ms(50);
                else if(delay_time == 3)
                    delay_ms(75);
                else if(delay_time == 4)
                    delay_ms(100);
                else if(delay_time == 5)
                    delay_ms(125);
                else
                    delay_ms(25);

                __no_operation();
            }break;
            case LED_ACTION_BRIGHTNESS_SHIFT_DOWN :
            {
                if(update_once == 1)   // update one time when the button pressed
                {
                    LP5012_I2C_LEDx_ch_brightness(LP5012_DEV0_ADDR, 0x80, 0x80, 0x80, 0x80);

                    // seperate color control
                    for(led_ch=0; led_ch<MAX_LED; led_ch++)
                    {
                        LP5012_I2C_OUTx_color(LP5012_DEV0_ADDR, led_ch, 0x00);
                    }
                    led_ch = 8;
                }

                // off the previous LED
                if(led_ch == 8)
                    LP5012_I2C_OUTx_color(LP5012_DEV0_ADDR, 0, 0);
                else
                    LP5012_I2C_OUTx_color(LP5012_DEV0_ADDR, led_ch+1, 0);

                LP5012_I2C_OUTx_color(LP5012_DEV0_ADDR, led_ch--, 0x80);
                if(led_ch == 255)
                    led_ch = 8;

                if(delay_time == 1)
                    delay_ms(25);
                else if(delay_time == 2)
                    delay_ms(50);
                else if(delay_time == 3)
                    delay_ms(75);
                else if(delay_time == 4)
                    delay_ms(100);
                else if(delay_time == 5)
                    delay_ms(125);
                else
                    delay_ms(25);

                __no_operation();
            }break;
            case LED_ACTION_BRIGHTNESS_SHIFT_UPDN :
            {
                if(update_once == 1)   // update one time when the button pressed
                {
                    LP5012_I2C_LEDx_ch_brightness(LP5012_DEV0_ADDR, 0x80, 0x80, 0x80, 0x80);

                    // seperate color control
                    for(led_ch=0; led_ch<MAX_LED; led_ch++)
                    {
                        LP5012_I2C_OUTx_color(LP5012_DEV0_ADDR, led_ch, 0x00);
                    }
                    led_ch = 0;

                    cmd_inc_dec = 0;    // up

                }

                if(cmd_inc_dec == 0)    // up
                {
                    // off the previous LED
                    if(led_ch == 0)
                        LP5012_I2C_OUTx_color(LP5012_DEV0_ADDR, 8, 0);
                    else
                        LP5012_I2C_OUTx_color(LP5012_DEV0_ADDR, led_ch-1, 0);

                    LP5012_I2C_OUTx_color(LP5012_DEV0_ADDR, led_ch++, 0x80);
                    if(led_ch == 8)
                        cmd_inc_dec = 1;
                }
                else                // down
                {
                    // off the previous LED
                    if(led_ch == 8)
                        LP5012_I2C_OUTx_color(LP5012_DEV0_ADDR, 0, 0);
                    else
                        LP5012_I2C_OUTx_color(LP5012_DEV0_ADDR, led_ch+1, 0);

                    LP5012_I2C_OUTx_color(LP5012_DEV0_ADDR, led_ch--, 0x80);
                    if(led_ch == 0)
                        cmd_inc_dec = 0;
                }


                if(delay_time == 1)
                    delay_ms(25);
                else if(delay_time == 2)
                    delay_ms(50);
                else if(delay_time == 3)
                    delay_ms(75);
                else if(delay_time == 4)
                    delay_ms(100);
                else if(delay_time == 5)
                    delay_ms(125);
                else
                    delay_ms(25);

                __no_operation();
            }break;
            default : break;
        }

        update_once = 0;
        //for(i=0; i<0x16; i++)
        //{
        //    i2c_read_byte(LP5012_DEV0_ADDR, i, &data);
        //    data_buf[i] = data;
        //}

    }

}


#define ONE_MIN_FROM_50ms_Tick              1200        //20*60
#define ONE_HOUR_FROM_50ms_Tick             72000       //20*60*60
#define TWO_HOUR_FROM_50ms_Tick             144000      //20*60*60*2

uint32_t timer_cnt = 0;



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

            timer_cnt = 0;
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

            timer_cnt = 0;
        }
    }

    else
    {
        SW2_PORT_IFG &= ~SW2_BIT;
    }

}


/******************************************************************************/
// Timer0_A0 Interrupt Service Routine: Disables the timer and exists LPM3
/******************************************************************************/

// 50ms continuous mode timer
#pragma vector=TIMER0_A0_VECTOR
__interrupt void ISR_Timer0_A0(void)
{
    TA0CCR0 += CONST_TIMER0_CCR0;               // Add Offset to CCR1

    timer_cnt++;
    if(timer_cnt >= ONE_HOUR_FROM_50ms_Tick)        //ONE_MIN_FROM_50ms_Tick : test for shortening the time
    {
        timer_cnt = 0;

        led_mode = LED_ACTION_MAX;          // will generate sleep at the main loop
        switch_status |= BIT0;              // generate emulated signal of button press..
    }
}


#pragma vector=TIMER0_A1_VECTOR
__interrupt void Timer0_A1A2(void)
{
    switch( TA0IV )
    {
        case  2:                                        // CCR1
        {
            TA0CCR1 += CONST_TIMER0_CCR1;               // Add Offset to CCR1

        }
        break;
        case  4:                                        // CCR2
        {
            TA0CCR2 += CONST_TIMER0_CCR2;               // Add Offset to CCR2

        } break;
        case 10:                                        // TA0 Overflow
        {
            //P1OUT ^= LED_TA0_OVF;
        } break;
    }
}
