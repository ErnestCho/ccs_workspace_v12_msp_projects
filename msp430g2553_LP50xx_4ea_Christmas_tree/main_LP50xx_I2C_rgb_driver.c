#include <msp430.h> 
#include <stdint.h>
#include "LP5012.h"

// SW1 = P1.1
#define SW1_PORT_DIR            P1DIR
#define SW1_PORT_OUT            P1OUT
#define SW1_PORT_REN            P1REN
#define SW1_PORT_IES            P1IES
#define SW1_PORT_IE             P1IE
#define SW1_PORT_IFG            P1IFG
#define SW1_BIT                 BIT1

// SW2 = P1.2
#define SW2_PORT_DIR            P1DIR
#define SW2_PORT_OUT            P1OUT
#define SW2_PORT_REN            P1REN
#define SW2_PORT_IES            P1IES
#define SW2_PORT_IE             P1IE
#define SW2_PORT_IFG            P1IFG
#define SW2_BIT                 BIT2


#define DISABLE     0
#define ENABLE      1

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
    LP5012_I2C_chip_reset(LP5012_DEV1_ADDR, 1);
    LP5012_I2C_chip_reset(LP5012_DEV2_ADDR, 1);
    LP5012_I2C_chip_reset(LP5012_DEV3_ADDR, 1);
    delay_ms(100);

    LP5012_I2C_chip_enable(LP5012_DEV0_ADDR, ENABLE);
    LP5012_I2C_chip_enable(LP5012_DEV1_ADDR, ENABLE);
    LP5012_I2C_chip_enable(LP5012_DEV2_ADDR, ENABLE);
    LP5012_I2C_chip_enable(LP5012_DEV3_ADDR, ENABLE);

    LP5012_I2C_IC_init(LP5012_DEV0_ADDR, LP5012_LOG_SCALE_EN_BIT);
    LP5012_I2C_IC_init(LP5012_DEV1_ADDR, LP5012_LOG_SCALE_EN_BIT);
    LP5012_I2C_IC_init(LP5012_DEV2_ADDR, LP5012_LOG_SCALE_EN_BIT);
    LP5012_I2C_IC_init(LP5012_DEV3_ADDR, LP5012_LOG_SCALE_EN_BIT);


    delay_ms(100);

}

#define MAX_LED_PER_DEV         12      // maximum LED number for LP5012
#define MAX_RGB_LED_PER_DEV     4       // maximum RGB LED number for LP5012

#define NUM_OF_LP5012_DEV       4       // number of LP5012 in the system
#define MAX_LED                 (MAX_LED_PER_DEV*NUM_OF_LP5012_DEV)     // LED max number
#define MAX_RGB_LED_PAIR        (MAX_LED/3)                             // max RGB LED number

// for debug logging
unsigned long data_buf[0x16] = { 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0};

unsigned char dev = 0;                  // LP5012 device number
unsigned char led_bright = 0;           // LED bright
signed char led_ch = 0;               // LED order from 0 to MAX_LED
unsigned char led_per_dev_ch = 0;       // LED order from 0 to MAX_LED_PER_DEV
signed char rgb_led_ch = 0;               // RGB LED order from 0 to MAX_RGB_LED_PAIR
unsigned char rgb_led_per_dev_ch = 0;       // RGB_LED order from 0 to MAX_RGB_LED_PER_DEV
unsigned char color_index = 0;              // color index for certain color array, for _tag_color structure or array
unsigned char bank_bright = 0;              // brightness for bank
unsigned char dir_inc_dec = 0;              // indicate LED direction is inc or dec
unsigned char sequence_cnt = 0;             // sequence when color change after one cycle

#define NUM_OF_WINDOW       8
unsigned char led_bright_window[NUM_OF_WINDOW] = {0xc0, 0xa0, 0x80, 0x60, 0x40, 0x20, 0x10, 0x00};




//#define LED_TEST_MODE           1


uint8_t switch_status = 0;

// LED mode :
// sw1 action :
// - LED shift up(sw2 : All(3delay), B(3delay), G(3delay), R(3delay) ),
// - LED shift down(sw2 : All(3delay), B(3delay), G(3delay), R(3delay) ),
// - LED shift up/down(sw2 : All(3delay), B(3delay), G(3delay), R(3delay) ),
// - LED shift up with tails(sw2 : All(3delay), B(3delay), G(3delay), R(3delay) ),
// - LED shift down with tails(sw2 : All(3delay), B(3delay), G(3delay), R(3delay) ),
// - LED shift up/down with tails(sw2 : All(3delay), B(3delay), G(3delay), R(3delay) ),
#define LED_ACTION_LEDS_SHIFT_UP                1
#define LED_ACTION_LEDS_SHIFT_DOWN              2
#define LED_ACTION_LEDS_SHIFT_UPDN              3
#define LED_ACTION_LEDS_SHIFT_UP_LIKE_WAVE      4
#define LED_ACTION_LEDS_SHIFT_DOWN_LIKE_WAVE    5
//#define LED_ACTION_LEDS_SHIFT_UPDN_LIKE_WAVE    6
#define LED_ACTION_LEDS_COLOR_CHANGE_STRAIGHT   8
#define LED_ACTION_LEDS_COLOR_CHANGE_SMOOTH     9
#define LED_ACTION_LEDS_COLOR_FIX_VARIOUS       6

#define LED_ACTION_LEDS_SHIFT_UP_RAINBOW        7
//#define LED_ACTION_LEDS_SHIFT_DOWN_RAINBOW      11
//#define LED_ACTION_LEDS_SHIFT_UPDN_RAINBOW      12

#define LED_ACTION_MAX                          8

#define LED_ACTION_RGB_EACH_LED_SHIFT_UP        255 //test

#define LED_TEST1               1
#define LED_TEST2               2
#define LED_TEST3               3
#define LED_TEST4               4
#define LED_TEST5               5
#define LED_TEST6               6
#define LED_TEST7               7
#define LED_TEST8               8
#define LED_TEST9               9

#define COLOR_WHITE         {255, 255, 255}
#define COLOR_RED           {255, 0, 0}
#define COLOR_ORANGE        {255, 127, 0}
#define COLOR_YELLOW        {255, 255, 0}
#define COLOR_CHARTREUSE    {127, 255, 0}   // in between Yellow and Green
#define COLOR_GREEN         {0, 255, 0}
#define COLOR_AQUAMARINE    {0, 255, 127}   // in between Green and Cyan
#define COLOR_CYAN          {0, 255, 255}   // Sky blue (?)
#define COLOR_AZURE         {0, 127, 255}   // in between Cyan and Blue
#define COLOR_BLUE          {0, 0, 255}
#define COLOR_VIOLET        {127, 0, 255}
#define COLOR_MAGENTA       {255, 0, 255}
#define COLOR_ROSE          {255, 0, 127}   // in between Magenta and Red

#define WHITE           {255, 255, 255}
#define SILVER          {192, 192, 192}
#define GRAY            {128, 128, 128}
#define BLACK           {0, 0, 0}
#define MAROON          {128, 0, 0}         // near dark red
#define RED             {255, 0, 0}
#define ORANGE          {255, 165, 0}
#define YELLOW          {255, 255, 0}
#define OLIVE           {128, 128, 0}       // near dark yellow
#define LIME            {0, 255, 0}         // near light green
#define GREEN           {0, 128, 0}
#define AQUA            {0, 255, 255}       // near bright blue
#define BLUE            {0, 0, 255}
#define NAVY            {0, 0, 128}         // near dark blue
#define TEAL            {0, 128, 128}       // near dark green
#define FUCHSIA         {255, 0, 255}       // near pink
#define PURPLE          {128, 0, 128}



typedef struct {
    unsigned char r;
    unsigned char g;
    unsigned char b;
}_tag_color;

_tag_color color;

#define NUM_COLOR_RAINBOW   8
_tag_color colors_rainbow[8] = {
    WHITE,
    RED,
    ORANGE,
    YELLOW,
    GREEN,
    BLUE,
    NAVY,
    PURPLE
};

_tag_color colors_mix_16[16] = {
     WHITE,
     SILVER,
     GRAY,
     MAROON,
     RED,
     ORANGE,
     YELLOW,
     OLIVE,
     LIME,
     GREEN,
     AQUA,
     BLUE,
     NAVY,
     TEAL,
     FUCHSIA,
     PURPLE,
};

_tag_color colors_mix_fix[1] = {
     WHITE,
};



uint8_t led_mode = 0;               // mode : 1~8
uint8_t led_disp_color = 0;         // 0 : All, 1 : G, 2 : R, 3 : B
uint8_t brightness_level = 0;       // level : 1(dark)~3(bright)
uint8_t delay_time = 3;             // delay_time : 1(slow)~3(fast)
uint8_t update_once = 0;        // not to do excessive updates of LP5012 registers
uint8_t initial_onetime_activity = 1;
uint8_t cmd_inc_dec = 0;        // 0 : inc, 1 : dec


void main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer

    GPIO_SW1_SW2_Interrupt_Set();
    GPIO_LP5012_IC_enable(0);

    // move this for current consumption
    //GPIO_LP5012_IC_enable(1);
    //GPIO_I2C_Init();
    //G2553_LP5012_board_init();


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
                //G2553_LP5012_board_init();  // reset and init

            }

            G2553_LP5012_board_init();  // reset and init

            update_once = 1;
            led_mode++;
            if(led_mode >= LED_ACTION_MAX)   // end? then repeat first
            {
                led_mode = LED_ACTION_LEDS_SHIFT_UP;
            }

            switch_status &= ~BIT0;
        }

        /////////////////////////////////////////////////
        // check switch buttons are pressed : SW2
        /////////////////////////////////////////////////
        if(switch_status & BIT1)    // switch2 function : brightness or delay time change
        {
            update_once = 1;

            delay_time--;
            if(delay_time == 0)
            {
                delay_time = 3;
                led_disp_color++;
                if(led_disp_color > 3)  // 0:All, 1:G, 2:R, 3:B
                {
                    led_disp_color = 0;
                }
            }

            switch_status &= ~BIT1;
        }

        /////////////////////////////////////////////////
        // actions for case by case of the buttons
        /////////////////////////////////////////////////
        switch (led_mode)
        {
            case LED_ACTION_RGB_EACH_LED_SHIFT_UP : //test for each LED is on or not
            {
                if(update_once == 1)   // update one time when the button pressed
                {
                    LP5012_I2C_LEDx_ch_brightness(LP5012_DEV0_ADDR, 0x40, 0x40, 0x40, 0x40);
                    LP5012_I2C_LEDx_ch_brightness(LP5012_DEV1_ADDR, 0x40, 0x40, 0x40, 0x40);
                    LP5012_I2C_LEDx_ch_brightness(LP5012_DEV2_ADDR, 0x40, 0x40, 0x40, 0x40);
                    LP5012_I2C_LEDx_ch_brightness(LP5012_DEV3_ADDR, 0x40, 0x40, 0x40, 0x40);

                    // seperate color control
                    for(dev=0; dev<NUM_OF_LP5012_DEV; dev++)
                    {
                        for(led_per_dev_ch=0; led_per_dev_ch<MAX_LED_PER_DEV; led_per_dev_ch++)
                        {
                            LP5012_I2C_OUTx_color(LP5012_DEV0_ADDR+dev, led_per_dev_ch, 0x00);
                        }
                    }
                    led_ch = 0;
                }

                // off the previous LED
                if(led_ch == 0)
                    LP5012_I2C_OUTx_color((LP5012_DEV0_ADDR+NUM_OF_LP5012_DEV-1), MAX_LED_PER_DEV-1, 0);
                else
                    LP5012_I2C_OUTx_color(LP5012_DEV0_ADDR+dev, led_per_dev_ch, 0);

                dev = (unsigned char) led_ch / MAX_LED_PER_DEV;
                led_per_dev_ch = (unsigned char) led_ch % MAX_LED_PER_DEV;

                LP5012_I2C_OUTx_color(LP5012_DEV0_ADDR+dev, led_per_dev_ch, 0x30);
                led_ch++;
                if(led_ch == MAX_LED)
                    led_ch = 0;


                if(delay_time == 1)
                    delay_ms(50);
                else if(delay_time == 2)
                    delay_ms(150);
                else if(delay_time == 3)
                    delay_ms(250);
                else
                    delay_ms(250);

                __no_operation();


            }break;

            case LED_ACTION_LEDS_SHIFT_UP :
            {
                if(update_once == 1)   // update one time when the button pressed
                {
                    // separate color control
                    for(dev=0; dev<NUM_OF_LP5012_DEV; dev++)
                    {
                        LP5012_I2C_bank_control_mode(LP5012_DEV0_ADDR+dev, 0);
                        LP5012_I2C_bank_brightness(LP5012_DEV0_ADDR+dev, 0x80);
                        LP5012_I2C_bank_color(LP5012_DEV0_ADDR+dev, 0x80, 0x80, 0x80);
                    }

                    dev = 0;
                    rgb_led_per_dev_ch = 0;
                    rgb_led_ch = 0;
                }

                // off the previous LED
                if(rgb_led_per_dev_ch == MAX_RGB_LED_PER_DEV-1)
                {
                    LP5012_I2C_bank_control_mode((LP5012_DEV0_ADDR+dev), 0);
                }

                dev = (unsigned char) rgb_led_ch / MAX_RGB_LED_PER_DEV;
                rgb_led_per_dev_ch = (unsigned char) rgb_led_ch % MAX_RGB_LED_PER_DEV;

                LP5012_I2C_bank_control_mode_single(LP5012_DEV0_ADDR+dev, rgb_led_per_dev_ch);

                rgb_led_ch++;       // increasing
                if(rgb_led_ch == MAX_RGB_LED_PAIR)
                    rgb_led_ch = 0;

                if(delay_time == 1)
                    delay_ms(50);
                else if(delay_time == 2)
                    delay_ms(150);
                else if(delay_time == 3)
                    delay_ms(250);
                else
                    delay_ms(250);

                __no_operation();

            }break;

//#define LED_ACTION_LEDS_SHIFT_UP_RAINBOW        10
//#define LED_ACTION_LEDS_SHIFT_DOWN_RAINBOW      11
//#define LED_ACTION_LEDS_SHIFT_UPDN_RAINBOW      12
#if 1
            case LED_ACTION_LEDS_SHIFT_UP_RAINBOW :
            {
                if(update_once == 1)   // update one time when the button pressed
                {
                    // separate color control
                    for(dev=0; dev<NUM_OF_LP5012_DEV; dev++)
                    {
                        LP5012_I2C_bank_control_mode(LP5012_DEV0_ADDR+dev, 0);
                        LP5012_I2C_bank_brightness(LP5012_DEV0_ADDR+dev, 0);
                        LP5012_I2C_bank_color(LP5012_DEV0_ADDR+dev, 0, 0, 0);
                        LP5012_I2C_LEDx_ch_brightness(LP5012_DEV0_ADDR+dev, 0x00, 0x00, 0x00, 0x00);
                    }

                    LP5012_I2C_LEDx_ch_brightness(LP5012_DEV0_ADDR, 0x40, 0x40, 0x40, 0x40);
                    LP5012_I2C_LEDx_ch_brightness(LP5012_DEV1_ADDR, 0x40, 0x40, 0x40, 0x40);
                    LP5012_I2C_LEDx_ch_brightness(LP5012_DEV2_ADDR, 0x40, 0x40, 0x40, 0x40);
                    LP5012_I2C_LEDx_ch_brightness(LP5012_DEV3_ADDR, 0x40, 0x40, 0x40, 0x40);

                    dev = 0;
                    rgb_led_per_dev_ch = 0;
                    rgb_led_ch = 0;
                    sequence_cnt = 0;
                }

                //if(led_ch == 0)
                //    LP5012_I2C_OUTx_color((LP5012_DEV0_ADDR+NUM_OF_LP5012_DEV-1), MAX_RGB_LED_PER_DEV-1, 0);
                //else
                //    LP5012_I2C_OUTx_color(LP5012_DEV0_ADDR+dev, led_per_dev_ch, 0);

                // off the previous LED
                if(led_ch == 0)
                {
                //    LP5012_I2C_OUTx_color((LP5012_DEV0_ADDR+NUM_OF_LP5012_DEV-1), rgb_led_per_dev_ch*3+0, 0x00);
                //    LP5012_I2C_OUTx_color((LP5012_DEV0_ADDR+NUM_OF_LP5012_DEV-1), rgb_led_per_dev_ch*3+1, 0x00);
                //    LP5012_I2C_OUTx_color((LP5012_DEV0_ADDR+NUM_OF_LP5012_DEV-1), rgb_led_per_dev_ch*3+2, 0x00);
                }
                else
                {
                //    LP5012_I2C_OUTx_color(LP5012_DEV0_ADDR+dev, rgb_led_per_dev_ch*3+0, 0x00);
                //    LP5012_I2C_OUTx_color(LP5012_DEV0_ADDR+dev, rgb_led_per_dev_ch*3+1, 0x00);
                //    LP5012_I2C_OUTx_color(LP5012_DEV0_ADDR+dev, rgb_led_per_dev_ch*3+2, 0x00);
                }

                dev = (unsigned char) rgb_led_ch / MAX_RGB_LED_PER_DEV;
                rgb_led_per_dev_ch = (unsigned char) rgb_led_ch % MAX_RGB_LED_PER_DEV;

                LP5012_I2C_OUTx_color(LP5012_DEV0_ADDR+dev, rgb_led_per_dev_ch*3+0, colors_rainbow[sequence_cnt].b);
                LP5012_I2C_OUTx_color(LP5012_DEV0_ADDR+dev, rgb_led_per_dev_ch*3+1, colors_rainbow[sequence_cnt].r);
                LP5012_I2C_OUTx_color(LP5012_DEV0_ADDR+dev, rgb_led_per_dev_ch*3+2, colors_rainbow[sequence_cnt].g);

                rgb_led_ch++;       // increasing
                if(rgb_led_ch == MAX_RGB_LED_PAIR)
                {
                    rgb_led_ch = 0;
                    sequence_cnt++;
                    if(sequence_cnt == NUM_COLOR_RAINBOW)
                    {
                        sequence_cnt = 0;
                    }
                }

                if(delay_time == 1)
                    delay_ms(50);
                else if(delay_time == 2)
                    delay_ms(150);
                else if(delay_time == 3)
                    delay_ms(250);
                else
                    delay_ms(250);

                __no_operation();

            }break;
#endif
            case LED_ACTION_LEDS_SHIFT_DOWN :
            {
                if(update_once == 1)   // update one time when the button pressed
                {
                    // separate color control
                    for(dev=0; dev<NUM_OF_LP5012_DEV; dev++)
                    {
                        LP5012_I2C_bank_control_mode(LP5012_DEV0_ADDR+dev, 0);
                        LP5012_I2C_bank_brightness(LP5012_DEV0_ADDR+dev, 0x80);
                        LP5012_I2C_bank_color(LP5012_DEV0_ADDR+dev, 0x80, 0x80, 0x80);
                    }

                    dev = 0;
                    rgb_led_per_dev_ch = 0;
                    rgb_led_ch = MAX_RGB_LED_PAIR-1;
                }

                // off the previous LED
                if(rgb_led_per_dev_ch == 0)
                {
                    LP5012_I2C_bank_control_mode((LP5012_DEV0_ADDR+dev), 0);
                }

                dev = (unsigned char) rgb_led_ch / MAX_RGB_LED_PER_DEV;
                rgb_led_per_dev_ch = (unsigned char) rgb_led_ch % MAX_RGB_LED_PER_DEV;

                LP5012_I2C_bank_control_mode_single(LP5012_DEV0_ADDR+dev, rgb_led_per_dev_ch);

                rgb_led_ch--;       // decreasing
                if(rgb_led_ch == -1)
                    rgb_led_ch = MAX_RGB_LED_PAIR-1;

                if(delay_time == 1)
                    delay_ms(50);
                else if(delay_time == 2)
                    delay_ms(150);
                else if(delay_time == 3)
                    delay_ms(250);
                else
                    delay_ms(250);

                __no_operation();

            }break;
            case LED_ACTION_LEDS_SHIFT_UPDN :
            {
                if(update_once == 1)   // update one time when the button pressed
                {
                    // separate color control
                    for(dev=0; dev<NUM_OF_LP5012_DEV; dev++)
                    {
                        LP5012_I2C_bank_control_mode(LP5012_DEV0_ADDR+dev, 0);
                        LP5012_I2C_bank_brightness(LP5012_DEV0_ADDR+dev, 0x80);
                        LP5012_I2C_bank_color(LP5012_DEV0_ADDR+dev, 0x80, 0x80, 0x80);
                    }

                    dev = 0;
                    rgb_led_per_dev_ch = 0;
                    rgb_led_ch = 0;
                    dir_inc_dec = 0;    // increasing
                }

                if(dir_inc_dec == 0)
                {
                    // off the previous LED
                    if(rgb_led_per_dev_ch == MAX_RGB_LED_PER_DEV-1)
                    {
                        LP5012_I2C_bank_control_mode((LP5012_DEV0_ADDR+dev), 0);
                    }

                    dev = (unsigned char) rgb_led_ch / MAX_RGB_LED_PER_DEV;
                    rgb_led_per_dev_ch = (unsigned char) rgb_led_ch % MAX_RGB_LED_PER_DEV;

                    LP5012_I2C_bank_control_mode_single(LP5012_DEV0_ADDR+dev, rgb_led_per_dev_ch);

                    rgb_led_ch++;       // increasing
                    if(rgb_led_ch == MAX_RGB_LED_PAIR)
                    {
                        rgb_led_ch = MAX_RGB_LED_PAIR-1;;
                        dir_inc_dec = 1;
                    }
                }

                else
                {
                    // off the previous LED
                    if(rgb_led_per_dev_ch == 0)
                    {
                        LP5012_I2C_bank_control_mode((LP5012_DEV0_ADDR+dev), 0);
                    }

                    dev = (unsigned char) rgb_led_ch / MAX_RGB_LED_PER_DEV;
                    rgb_led_per_dev_ch = (unsigned char) rgb_led_ch % MAX_RGB_LED_PER_DEV;

                    LP5012_I2C_bank_control_mode_single(LP5012_DEV0_ADDR+dev, rgb_led_per_dev_ch);

                    rgb_led_ch--;       // decreasing
                    if(rgb_led_ch == -1)
                    {
                        rgb_led_ch = 0;
                        dir_inc_dec = 0;
                    }
                }


                if(delay_time == 1)
                    delay_ms(50);
                else if(delay_time == 2)
                    delay_ms(150);
                else if(delay_time == 3)
                    delay_ms(250);
                else
                    delay_ms(250);

                __no_operation();

            }break;
//////////////////////////////
            case LED_ACTION_LEDS_SHIFT_UP_LIKE_WAVE :
            {
                signed char new_position = 0;

                if(update_once == 1)   // update one time when the button pressed
                {
                    // clean registers
                    for(dev=0; dev<NUM_OF_LP5012_DEV; dev++)
                    {
                        LP5012_I2C_bank_control_mode(LP5012_DEV0_ADDR+dev, 0);
                        LP5012_I2C_bank_brightness(LP5012_DEV0_ADDR+dev, 0);
                        LP5012_I2C_bank_color(LP5012_DEV0_ADDR+dev, 0, 0, 0);
                        LP5012_I2C_LEDx_ch_brightness(LP5012_DEV0_ADDR+dev, 0x00, 0x00, 0x00, 0x00);
                    }

                    // initial registers
                    for(dev=0; dev<NUM_OF_LP5012_DEV; dev++)
                    {
                        for(led_ch=0; led_ch<MAX_LED_PER_DEV; led_ch++)
                        {
                            LP5012_I2C_OUTx_color(LP5012_DEV0_ADDR+dev, led_ch, 0x80);
                        }
                    }

                    dev = 0;
                    rgb_led_per_dev_ch = 0;
                    rgb_led_ch = 0;
                }

                // for some blinking effect
                //for(dev=0; dev<NUM_OF_LP5012_DEV; dev++)
                //    LP5012_I2C_LEDx_ch_brightness(LP5012_DEV0_ADDR+dev, 0x00, 0x00, 0x00, 0x00);

                new_position = rgb_led_ch;
                for(color_index=0; color_index<NUM_OF_WINDOW; color_index++)
                {
                    if( new_position < 0 )
                    {
                        break;      // exit if start points are less than the window size (edge case)
                    }

                    if( new_position > MAX_RGB_LED_PAIR)
                    {
                        //continue;   // skip if window size exceed the end points
                    }
                    else            // normal case
                    {
                        dev = (unsigned char) new_position / MAX_RGB_LED_PER_DEV;
                        rgb_led_per_dev_ch = (unsigned char) new_position % MAX_RGB_LED_PER_DEV;
                        LP5012_I2C_LEDx_brightness(LP5012_DEV0_ADDR+dev, rgb_led_per_dev_ch, led_bright_window[color_index]);
                    }

                    new_position--;
                }

                rgb_led_ch++;       // brightest LED location
                if(rgb_led_ch == (MAX_RGB_LED_PAIR + NUM_OF_WINDOW) )   // do more for the tail of the window
                    rgb_led_ch = 0;

                if(delay_time == 1)
                    delay_ms(50);
                else if(delay_time == 2)
                    delay_ms(150);
                else if(delay_time == 3)
                    delay_ms(250);
                else
                    delay_ms(250);

                __no_operation();
            }break;
#if 1
            case LED_ACTION_LEDS_SHIFT_DOWN_LIKE_WAVE :
            {
                signed char new_position = 0;

                if(update_once == 1)   // update one time when the button pressed
                {
                    // clean registers
                    for(dev=0; dev<NUM_OF_LP5012_DEV; dev++)
                    {
                        LP5012_I2C_bank_control_mode(LP5012_DEV0_ADDR+dev, 0);
                        LP5012_I2C_bank_brightness(LP5012_DEV0_ADDR+dev, 0);
                        LP5012_I2C_bank_color(LP5012_DEV0_ADDR+dev, 0, 0, 0);
                        LP5012_I2C_LEDx_ch_brightness(LP5012_DEV0_ADDR+dev, 0x00, 0x00, 0x00, 0x00);
                    }

                    // initial registers
                    for(dev=0; dev<NUM_OF_LP5012_DEV; dev++)
                    {
                        for(led_ch=0; led_ch<MAX_LED_PER_DEV; led_ch++)
                        {
                            LP5012_I2C_OUTx_color(LP5012_DEV0_ADDR+dev, led_ch, 0x80);
                        }
                    }

                    dev = NUM_OF_LP5012_DEV-1;
                    rgb_led_per_dev_ch = MAX_RGB_LED_PER_DEV;
                    rgb_led_ch = MAX_RGB_LED_PAIR-1;
                }

                // for some blinking effect
                //for(dev=0; dev<NUM_OF_LP5012_DEV; dev++)
                //    LP5012_I2C_LEDx_ch_brightness(LP5012_DEV0_ADDR+dev, 0x00, 0x00, 0x00, 0x00);

                new_position = rgb_led_ch;
                for(color_index=0; color_index<NUM_OF_WINDOW; color_index++)
                {
                    if( new_position >= MAX_RGB_LED_PAIR )
                    {
                        break;      // exit if start points are more than the window size (edge case)
                    }

                    if( new_position < 0)
                    {
                        //continue;   // skip if window size exceed the end points
                    }
                    else            // normal case
                    {
                        dev = (unsigned char) new_position / MAX_RGB_LED_PER_DEV;
                        rgb_led_per_dev_ch = (unsigned char) new_position % MAX_RGB_LED_PER_DEV;
                        LP5012_I2C_LEDx_brightness(LP5012_DEV0_ADDR+dev, rgb_led_per_dev_ch, led_bright_window[color_index]);
                    }

                    new_position++;
                }

                rgb_led_ch--;       // brightest LED location
                if(rgb_led_ch == (0 - NUM_OF_WINDOW) )   // do more for the tail of the window
                    rgb_led_ch = MAX_RGB_LED_PAIR -1;

                if(delay_time == 1)
                    delay_ms(50);
                else if(delay_time == 2)
                    delay_ms(150);
                else if(delay_time == 3)
                    delay_ms(250);
                else
                    delay_ms(250);

                __no_operation();
            }break;
#endif
            case LED_ACTION_LEDS_COLOR_CHANGE_STRAIGHT :
            {
                __no_operation();
            }break;
            case LED_ACTION_LEDS_COLOR_CHANGE_SMOOTH :
            {
                __no_operation();
            }break;
            case LED_ACTION_LEDS_COLOR_FIX_VARIOUS :
            {
                if(update_once == 1)   // update one time when the button pressed
                {
                    // clean registers
                    for(dev=0; dev<NUM_OF_LP5012_DEV; dev++)
                    {
                        LP5012_I2C_bank_control_mode(LP5012_DEV0_ADDR+dev, 0);
                        LP5012_I2C_bank_brightness(LP5012_DEV0_ADDR+dev, 0);
                        LP5012_I2C_bank_color(LP5012_DEV0_ADDR+dev, 0, 0, 0);
                        LP5012_I2C_LEDx_ch_brightness(LP5012_DEV0_ADDR+dev, 0x00, 0x00, 0x00, 0x00);
                    }
#if 1

                    LP5012_I2C_LEDx_ch_brightness(LP5012_DEV0_ADDR, 0x40, 0x40, 0x40, 0x40);
                    LP5012_I2C_LEDx_ch_brightness(LP5012_DEV1_ADDR, 0x40, 0x40, 0x40, 0x40);
                    LP5012_I2C_LEDx_ch_brightness(LP5012_DEV2_ADDR, 0x40, 0x40, 0x40, 0x40);
                    LP5012_I2C_LEDx_ch_brightness(LP5012_DEV3_ADDR, 0x40, 0x40, 0x40, 0x40);

                    for(dev=0; dev<NUM_OF_LP5012_DEV; dev++)
                    {
                        for(led_ch=0; led_ch<MAX_LED_PER_DEV; led_ch++)
                        {
                            LP5012_I2C_OUTx_color(LP5012_DEV0_ADDR+dev, led_ch, 0x80);
                        }
                    }
#else
                    // initial registers
                    for(dev=0; dev<NUM_OF_LP5012_DEV; dev++)
                    {
                        for(led_ch=0; led_ch<MAX_LED_PER_DEV; led_ch++)
                        {
                            LP5012_I2C_OUTx_color(LP5012_DEV0_ADDR+dev, led_ch, colors_mix_16[dev*4+led_ch/3].);
                        }
                    }
#endif

                    dev = 0;
                    rgb_led_per_dev_ch = 0;
                    rgb_led_ch = 0;
                }

                // for some blinking effect
                //for(dev=0; dev<NUM_OF_LP5012_DEV; dev++)
                //    LP5012_I2C_LEDx_ch_brightness(LP5012_DEV0_ADDR+dev, 0x00, 0x00, 0x00, 0x00);


                if(delay_time == 1)
                {
                    LP5012_I2C_LEDx_ch_brightness(LP5012_DEV0_ADDR, 0xa0, 0xa0, 0xa0, 0xa0);
                    LP5012_I2C_LEDx_ch_brightness(LP5012_DEV1_ADDR, 0xa0, 0xa0, 0xa0, 0xa0);
                    LP5012_I2C_LEDx_ch_brightness(LP5012_DEV2_ADDR, 0xa0, 0xa0, 0xa0, 0xa0);
                    LP5012_I2C_LEDx_ch_brightness(LP5012_DEV3_ADDR, 0xa0, 0xa0, 0xa0, 0xa0);
                }
                else if(delay_time == 2)
                {
                    LP5012_I2C_LEDx_ch_brightness(LP5012_DEV0_ADDR, 0x80, 0x80, 0x80, 0x80);
                    LP5012_I2C_LEDx_ch_brightness(LP5012_DEV1_ADDR, 0x80, 0x80, 0x80, 0x80);
                    LP5012_I2C_LEDx_ch_brightness(LP5012_DEV2_ADDR, 0x80, 0x80, 0x80, 0x80);
                    LP5012_I2C_LEDx_ch_brightness(LP5012_DEV3_ADDR, 0x80, 0x80, 0x80, 0x80);
                }
                else if(delay_time == 3)
                {
                    LP5012_I2C_LEDx_ch_brightness(LP5012_DEV0_ADDR, 0x40, 0x40, 0x40, 0x40);
                    LP5012_I2C_LEDx_ch_brightness(LP5012_DEV1_ADDR, 0x40, 0x40, 0x40, 0x40);
                    LP5012_I2C_LEDx_ch_brightness(LP5012_DEV2_ADDR, 0x40, 0x40, 0x40, 0x40);
                    LP5012_I2C_LEDx_ch_brightness(LP5012_DEV3_ADDR, 0x40, 0x40, 0x40, 0x40);
                }
                else
                {
                    LP5012_I2C_LEDx_ch_brightness(LP5012_DEV0_ADDR, 0x40, 0x40, 0x40, 0x40);
                    LP5012_I2C_LEDx_ch_brightness(LP5012_DEV1_ADDR, 0x40, 0x40, 0x40, 0x40);
                    LP5012_I2C_LEDx_ch_brightness(LP5012_DEV2_ADDR, 0x40, 0x40, 0x40, 0x40);
                    LP5012_I2C_LEDx_ch_brightness(LP5012_DEV3_ADDR, 0x40, 0x40, 0x40, 0x40);
                }

                __no_operation();
            }break;
            default : break;
        }
        update_once = 0;

#if 0
        /////////////////////////////////////////////////
        // actions for case by case of the buttons
        /////////////////////////////////////////////////

        LP5012_I2C_bank_control_mode(LP5012_DEV0_ADDR, 1);
        LP5012_I2C_bank_control_mode(LP5012_DEV1_ADDR, 1);
        LP5012_I2C_bank_control_mode(LP5012_DEV2_ADDR, 1);
        LP5012_I2C_bank_control_mode(LP5012_DEV3_ADDR, 1);

        LP5012_I2C_bank_brightness(LP5012_DEV0_ADDR, 0x7F);
        LP5012_I2C_bank_brightness(LP5012_DEV1_ADDR, 0x7F);
        LP5012_I2C_bank_brightness(LP5012_DEV2_ADDR, 0x7F);
        LP5012_I2C_bank_brightness(LP5012_DEV3_ADDR, 0x7F);

        for(dev=0; dev<NUM_OF_LP5012_DEV; dev++)
        {
            for(led_bright = 0; led_bright < 0xd0; led_bright += 0x20)
            {
                LP5012_I2C_bank_color(LP5012_DEV0_ADDR+dev, led_bright, 0, 0);
                delay_ms(200);
            }
            for(led_bright = 0; led_bright < 0xd0; led_bright += 0x20)
            {
                LP5012_I2C_bank_color(LP5012_DEV0_ADDR+dev, 0, led_bright, 0);
                delay_ms(200);
            }
            for(led_bright = 0; led_bright < 0xd0; led_bright += 0x20)
            {
                LP5012_I2C_bank_color(LP5012_DEV0_ADDR+dev, 0, 0, led_bright);
                delay_ms(200);
            }
        }


        update_once = 0;
        //for(i=0; i<0x16; i++)
        //{
        //    i2c_read_byte(LP5012_DEV0_ADDR, i, &data);
        //    data_buf[i] = data;
        //}
#endif
    }
}


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
        }
    }

    else
    {
        SW2_PORT_IFG &= ~SW2_BIT;
    }

}




