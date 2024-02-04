
// pin configuration
//
// P2.1 as CCI0A(CCR0) : Echo pin from UltraSound module
//
// P1.3, P1.4 : External Int.(Push button switch)
// P2.0 : GPIO : Trig pin from UltraSound module
//

// P1.6 : I2C SCL pin for OLED LCD
// P1.7 : I2C SDA pin for OLED LCD

// *****************************************************************************************
// Ultra Sound measured waveform                                                           *
//******************************************************************************************
// TRIG pin   : _____------__________________________________________________
//                   <-A-->
// Sonar TX   : _______________________xxxxxxxx______________________________
//(TX in the module)       <----B-----><---C-->
// ECHO pin   : _______________________________---------------------_________
//                                             <---------D--------->
// A : TRIG duration : MCU output (typ. 10us)
// B : response duration btw MCU and Ultra Sound Tx Sensor
//     A vendor : around 2msec
//     B vendor : around 1msec
// C : 8x pulse, 190usec (8/190us = 42.1kHz), represent ultra sound clock frequency
// D : Pulse indicate distance. It means the time between Tx to Rx sensing of Ultra Sound.
//     For the distance wise, it should be divided by 2 because it is round trip.
//******************************************************************************************

// ********************************
// Test results (measured D)      *
// ********************************
// :: measured and compared between TRIG pin duration and MCU calculated distance..
// :: 5-times trial
// #1 : measured around 8.423ms(=1444.5mm) and MCU calculated 1435mm (9.54mm diff)
// #2 : measured around 2.423ms(=415.5mm) and MCU calculated 412mm (3.54mm diff)
// #3  : measured around 3.189ms(=546.9mm) and MCU calculated 545mm (1.91mm diff)
// #4  : measured around 1.823ms(=312.6mm) and MCU calculated 311mm (1.64mm diff)
// #5  : measured around 9.951ms(=1706.6mm) and MCU calculated 1688mm (18.59mm diff)
// ********************************


#include <msp430.h> 
#include <type_define.h>
#include <drv_clock.h>
#include <drv_gpio.h>
#include <drv_gpio_interrupt.h>
#include <stdint.h>

#include <string.h>
#include <stdlib.h>

#include <ssd1306.h>
#include <i2c.h>

#define CPU_F ((double)16000000)
#define delay_us(x) __delay_cycles((long)(CPU_F*(double)x/1000000.0))
#define delay_ms(x) __delay_cycles((long)(CPU_F*(double)x/1000.0))

#define TIMER_A_SRC_ACLK        TASSEL_1    // timer source from ACLK
#define TIMER_A_SRC_SMCLK       TASSEL_2    // timer source from SMCLK

#define TIMER_A_MODE_UP         MC_1        // timer up mode
#define TIMER_A_MODE_CONTI      MC_2        // timer continuous mode
#define TIMER_A_MODE_UPDN       MC_3        // timer up/down mode

#define TIMER_A_DIV_1           ID_0
#define TIMER_A_DIV_2           ID_1
#define TIMER_A_DIV_4           ID_2
#define TIMER_A_DIV_8           ID_3

#define CAPTURE_NONE            CM_0
#define CAPTURE_RISING          CM_1
#define CAPTURE_FALLING         CM_2
#define CAPTURE_BOTH            CM_3

#define CAPTURE_INPUT_CCIxA     CCIS_0
#define CAPTURE_INPUT_CCIxB     CCIS_1
#define CAPTURE_INPUT_GND       CCIS_2
#define CAPTURE_INPUT_VCC       CCIS_3

#define CAPTURE_SRC_ASYNC       0
#define CAPTURE_SRC_SYNC        SCS

#define CAPTURE_MODE            CAP

#define CAPTURE1_CCR1_
#define CAPTURE1_CCR1_DIR        P2DIR
#define CAPTURE1_CCR1_SEL        P2SEL
#define CAPTURE1_CCR1_BIT        BIT1


// P2.1 as CCI1A(CCR1)
void TIMER1_CAPTURE_CCR1_gpio_init(void)
{
    //P1OUT &= ~BIT1;
    //P1REN |= BIT1;      // P1.1 internal pull down set : P1.1 output low, P1.1 REN high
    CAPTURE1_CCR1_DIR &= ~CAPTURE1_CCR1_BIT;    // P1.1 for TA0.0 CCI0A
    CAPTURE1_CCR1_SEL |= CAPTURE1_CCR1_BIT;
}

void TIMER1_CAPTURE_init(void)
{
    // Timer A0 block : use SMCLK, Timer mode - continuous, divider 1
    TA1CTL = TIMER_A_SRC_SMCLK + TIMER_A_MODE_CONTI + TIMER_A_DIV_1 + TAIE;
    // Timer capture : rising edge, Capture input - CCI0A, Sync capture, Capture mode, Interrupt enable
#if 0   // for 2ch
    TA1CCTL0 = CAPTURE_BOTH + CAPTURE_INPUT_CCIxA + CAPTURE_SRC_SYNC + CAPTURE_MODE;// + CCIE;
    TIMER1_CAPTURE_CCR0_gpio_init();
#endif

    // Timer capture : rising edge, Capture input - CCI0A, Sync capture, Capture mode, Interrupt enable
    TA1CCTL1 = CAPTURE_BOTH + CAPTURE_INPUT_CCIxA + CAPTURE_SRC_SYNC + CAPTURE_MODE;// + CCIE;
    TIMER1_CAPTURE_CCR1_gpio_init();
}

void TIMER1_CAPTURE_interrupt(uint8_t onoff)
{
    if(onoff == 1)
    {
        TA1CCTL1 |= CCIE;
        //TA0CCTL1 |= CCIE;
    }
    else
    {
        TA1CCTL1 &= ~CCIE;
        //TA0CCTL1 &= ~CCIE;
    }
}
// timerA0 constant definition. use SMCLK(1MHz/4 = 125kHz)
#define TIMER0_SMCLK_100usec            25//100
#define TIMER0_SMCLK_500usec            125//500
#define TIMER0_SMCLK_1msec              250//1000
#define TIMER0_SMCLK_1P5msec            375//1500
#define TIMER0_SMCLK_2msec              500//2000
#define TIMER0_SMCLK_2P5msec            625//2500
#define TIMER0_SMCLK_5msec              1250//5000
#define TIMER0_SMCLK_10msec             2500//10000
#define TIMER0_SMCLK_20msec             5000//20000
#define TIMER0_SMCLK_50msec             12500//50000
#define TIMER0_SMCLK_100msec            25000//50000

#define CONST_TIMER0_CCR0               TIMER0_SMCLK_100msec
#define CONST_TIMER0_CCR1               TIMER0_SMCLK_2P5msec
#define CONST_TIMER0_CCR2               TIMER0_SMCLK_500usec

#define CONST_TIMER1_CCR0               TIMER0_SMCLK_100msec
#define CONST_TIMER1_CCR1               TIMER0_SMCLK_2P5msec
#define CONST_TIMER1_CCR2               TIMER0_SMCLK_500usec

// 100msec
void TIMER0_PERIODIC_init(void)
{
    TA0CTL = TASSEL_2 + ID_1 + MC_2;                   // SMCLK, DIV2, Cont. mode
    // CCR0 interrupt enabled
    TA0CCTL0 = CCIE;
    TA0CCR0 = CONST_TIMER0_CCR0;
    // CCR1 interrupt enabled
//    TA0CCTL1 = 0x00;
//    TA0CCR1 = CONST_TIMER0_CCR1;
    // CCR2 interrupt enabled
//    TA0CCTL2 = 0x00;
//    TA0CCR2 = CONST_TIMER0_CCR2;
}

void TIMER0_CCR0_OneTime_start(void)
{
    TA0CTL = TASSEL_2 + MC_1 + + ID_2 + TACLR;                   // SMCLK, DIV4, up mode
    // CCR0 interrupt enabled
    TA0CCTL0 = CCIE;
    TA0CCR0 = CONST_TIMER0_CCR0;
}

void WDT_disable(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
}


#define CAPTURE_START   1
#define CAPTURE_STOP    0
uint8_t gf_start_trigger = 0;       // user button on -> start to trigger to ultra-sonic -> input capture timer on -> count timer
uint8_t gf_capture_done = 0;        // done capture timer
uint8_t gf_wait_trig_rising = 0;
uint8_t gf_20ms_waiting_timer_done = 0;
uint16_t timer_ovf_cnt = 0;
uint16_t timer_ovf_start = 0, timer_ovf_end = 0;
uint16_t timer_capture_start = 0;
uint16_t timer1A0_capture=0;
uint16_t timer1A1_capture=0;
uint16_t timer_ovf_diff = 0;
uint16_t timer_capture_diff = 0;
uint32_t time_delay_us = 0;
uint16_t time_delay_ms = 0;
uint32_t distance_mm = 0;
char disp_buf[24];
char tmp_buf[6];

// cf) VOICE_SPEED : 343 * 0.1s = 34.3m, 343 * 0.01s = 3.43m ==> enough duration 10msec

#define DISTANCE_THRESHOLD_MILI_METER       1000    // 100cm
#define MAX_REPEAT_COUNT                    100
#define MEASURE_SAVE_COUNT                20
uint32_t distance_mm_buf[MEASURE_SAVE_COUNT];
uint32_t min_distance_mm = 0;       // measured minimum distance during a sequence.
int16_t diff_distance_with_previous_mm = 0;
uint16_t repeat_cnt = 0;
uint16_t save_cnt = 0;

#define VOICE_SPEED      343     // voice velocity : 343m/s at room temp., at air
                                // formula : 331.5 + (0.6*temp) m/s

void start_trigger_pin_pulse(void)
{
    PORT_SET_BIT(P2, BIT0);                     // set trigger pin to high in the Ultra-sonic sensor
    __delay_cycles(160); //80);                 // stay high during over 10us (80cycles:10us, 160cycles:20us)
    PORT_CLR_BIT(P2, BIT0);                     // clear trigger pin to low
}

void init_trigger_pin(void)
{
    PORT_DIR_OUTPUT(P2, BIT0);
    PORT_CLR_BIT(P2, BIT0);                     // clear trigger pin to low
    delay_ms(100);
}

void start_timer_input_capture(void)
{
    TIMER1_CAPTURE_interrupt(CAPTURE_START);        // start timer for input capture
    gf_wait_trig_rising = 1;
}

typedef enum _tag_state{
    IDLE = 0,
    START_TRIGGER,
    WAIT_ULTRA_SOUND_RESPONSE,
    CALCULATION,
    DECIDE_NEXT_STATE,
    WAIT_PERIODIC_TIMER_TRIGGER,
    DISPLAY,
    STATE_MAX
}JumpJumpState;

JumpJumpState jumpjump_state = IDLE;

#ifdef _DEBUG
uint8_t i;
uint8_t dbg_buf[0xd0] = {0,};
#endif

void main(void)
{
    uint16_t clk_info;

    WDT_disable();

    init_trigger_pin();     // initialize ultra-sound trigger pin as output low

    clk_info = CLOCK_init(MCLK_8MHZ, SMCLK_DIV_8, ACLK_EXT_32K);

    //PORT_init();
    GPIO_SW1SW2_Interrupt_Set();
    TIMER1_CAPTURE_init();

    // OLED initialization
    i2c_init();                                 // initialize I2C to use with OLED
    delay_ms(1);

    ssd1306_init();                             // Initialize SSD1306 OLED
    ssd1306_clearDisplay();                     // Clear OLED display
    delay_ms(1);

    __bis_SR_register(GIE);                     // global interrupt enable

    ssd1306_clearDisplay();
    ssd1306_printString(0, 0, "Ultra-sonic", FONT_MS_GOTHIC8x16);
    ssd1306_printString(0, 2, " Jump! Jump!", FONT_MS_GOTHIC8x16);
    ssd1306_printString(0, 5, "R U Ready?", FONT_MS_GOTHIC8x16);
    //ssd1306_clearDisplay();

#ifdef _DEBUG
    for(clk_info=0; clk_info<0xd0; clk_info++)
    {
        dbg_buf[clk_info] = 0;
    }
    clk_info = 0;
#endif

    while(1)
    {
        // TODO : process :
        // repeat time period : 10~100ms (100ms at this time)
        // repeat checking ultra-sound distance is < 1000mm or max repeat counts (100~1000 times : ~10sec)  (100 times at this time)
        // once above condition meet, save distance data until max save counts reaches(20~200 times : ~2sec) (20 times at this time)
        // --> measure and calculate each distance and find shortest results.
        //
        switch(jumpjump_state)
        {
            case IDLE :                     // wait push button
            {
                if(gf_start_trigger == 1)       // push the button -> start sensor measuring
                {
                    repeat_cnt = 0;

                    for(save_cnt=0; save_cnt<MEASURE_SAVE_COUNT; save_cnt++)
                    {
                        distance_mm_buf[save_cnt] = 0;
                    }
                    save_cnt = 0;

                    ssd1306_clearDisplay();
                    ssd1306_printString(0, 0, "New Start!!", FONT_MS_GOTHIC8x16);
                    delay_ms(200);

                    jumpjump_state = START_TRIGGER;

#ifdef _DEBUG
                    dbg_buf[clk_info++] = jumpjump_state;     // debug purpose
#endif
                }

            }break;
            case START_TRIGGER :                // make trigger pulse
            {
                timer_ovf_cnt = 0;  // to prevent overflow of this variable which may cause the calculation misses.
                gf_start_trigger = 0;

                start_trigger_pin_pulse();    // set trigger pin and stay over 10us then clear trigger pin

                start_timer_input_capture();        // start timer for input capture
                //TIMER1_CAPTURE_interrupt(CAPTURE_START);        // start timer for input capture
                //gf_start_trigger = 0;
                //gf_wait_trig_rising = 1;

                jumpjump_state = WAIT_ULTRA_SOUND_RESPONSE;

#ifdef _DEBUG
                    dbg_buf[clk_info++] = jumpjump_state;     // debug purpose
#endif
            }break;
            case WAIT_ULTRA_SOUND_RESPONSE :
            {
                if(gf_capture_done == 1)        // once ECHO pulse indicate single capture finished..
                {
                    jumpjump_state = CALCULATION;

#ifdef _DEBUG
                    dbg_buf[clk_info++] = jumpjump_state;     // debug purpose
#endif
                }
            }break;
            case CALCULATION :
            {
                // calculate time to distance
                    // TODO : need to check the clock cycles for below operations
                //calculate_ultra_sound_time_to_distance();
                timer_ovf_diff = timer_ovf_end - timer_ovf_start;
                if(timer1A1_capture > timer_capture_start)
                    timer_capture_diff = timer1A1_capture - timer_capture_start;
                else
                    timer_capture_diff = 65535 - timer_capture_start + timer1A1_capture;

                if(timer_ovf_diff >=2)
                    time_delay_us = (timer_ovf_diff-1) * 65535 + timer_capture_diff;    // ~400cycles
                else
                    time_delay_us = timer_capture_diff;
                //time_delay_ms = time_delay_us / 1000;

                distance_mm = VOICE_SPEED * time_delay_us / 1000;                   // ~500cycles
                distance_mm = distance_mm>>1;                        // ultra sound : go - reflect - return. so distance should divide by 2

                // save first data
                if(save_cnt == 0)
                {
                    distance_mm_buf[0] = distance_mm;  // save to buffer
                    min_distance_mm = distance_mm;
                    save_cnt++;
                }
                else
                {
                    if(distance_mm <= DISTANCE_THRESHOLD_MILI_METER)
                    {
                        diff_distance_with_previous_mm = distance_mm_buf[save_cnt-1] - distance_mm;   // check distance is becoming shorten
                        if(diff_distance_with_previous_mm > 5 || diff_distance_with_previous_mm < -5)          // 0 : same distance, minus : farther, plus : closer
                        {
                            distance_mm_buf[save_cnt++] = distance_mm;  // save to buffer
                            if( min_distance_mm > distance_mm )
                            {
                                min_distance_mm = distance_mm;              // record minimum...
                            }
                        }

                    }
                }

                repeat_cnt++;
                gf_capture_done = 0;        // single capture done

#ifdef CCS_VER9
                ltoa((20-save_cnt), tmp_buf);
#else
                ltoa((20-save_cnt), tmp_buf, 10);
#endif
                strcpy(disp_buf, tmp_buf);
                strcat(disp_buf, ",");
#ifdef CCS_VER9
                ltoa( (100-repeat_cnt), tmp_buf);
#else
                ltoa( (100-repeat_cnt), tmp_buf, 10);
#endif
                strcat(disp_buf, tmp_buf);
                ssd1306_printString(60, 3, disp_buf, FONT_MS_GOTHIC8x16);

#ifdef CCS_VER9
                ltoa( (min_distance_mm/10), tmp_buf);
#else
                ltoa( (min_distance_mm/10), tmp_buf, 10);
#endif
                strcpy(disp_buf, tmp_buf);
                strcat(disp_buf, " cm ");
                ssd1306_printString(60, 5, disp_buf, FONT_MS_GOTHIC8x16);
#if 0
                if(save_cnt % 2)
                {
                    ssd1306_printString(0, 3, "..", FONT_MS_GOTHIC8x16);
                }
                else
                {
                    ssd1306_printString(0, 3, "...", FONT_MS_GOTHIC8x16);
                }
#endif
                jumpjump_state = DECIDE_NEXT_STATE;

#ifdef _DEBUG
                    dbg_buf[clk_info++] = jumpjump_state;     // debug purpose
#endif
            }break;
            case DECIDE_NEXT_STATE :
            {
                if(save_cnt >= MEASURE_SAVE_COUNT || repeat_cnt >= MAX_REPEAT_COUNT)
                {
                    jumpjump_state = DISPLAY;
                }
                else
                {
                    // start one-time timer(20ms)
                    TIMER0_CCR0_OneTime_start();

                    jumpjump_state = WAIT_PERIODIC_TIMER_TRIGGER;
                }
            }break;
            case WAIT_PERIODIC_TIMER_TRIGGER :
            {
                if(gf_20ms_waiting_timer_done == 1)
                {
                    gf_20ms_waiting_timer_done = 0;
                    jumpjump_state = START_TRIGGER;

                }

            }break;
            case DISPLAY :
            {
                ssd1306_clearDisplay();
                ssd1306_printString(0, 0, "Done Sensing", FONT_MS_GOTHIC8x16);
                ssd1306_printString(0, 2, "Record : ", FONT_MS_GOTHIC8x16);

#ifdef CCS_VER9
                ltoa( (min_distance_mm/10), tmp_buf);
#else
                ltoa( (min_distance_mm/10), tmp_buf, 10);
#endif
                strcpy(disp_buf, tmp_buf);
                strcat(disp_buf, ".");
#ifdef CCS_VER9
                ltoa( (min_distance_mm%10), tmp_buf);
#else
                ltoa( (min_distance_mm%10), tmp_buf, 10);
#endif
                strcat(disp_buf, tmp_buf);
                strcat(disp_buf, " cm");
                ssd1306_printString(40, 4, disp_buf, FONT_MS_GOTHIC8x16);

                ssd1306_printString(0, 6, "Start again?", FONT_MS_GOTHIC8x16);

                jumpjump_state = IDLE;

            }break;
        }
    }

}

// Timer0 A0 interrupt service routine
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=TIMER1_A0_VECTOR
__interrupt void ISR_Timer1_A0 (void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(TIMER1_A0_VECTOR))) ISR_Timer1_A0 (void)
#else
#error Compiler not supported!
#endif
{



}

// Timer0 A1 interrupt service routine
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = TIMER1_A1_VECTOR
__interrupt void ISR_TIMER1_A1A2 (void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(TIMER1_A1_VECTOR))) ISR_TIMER1_A1A2 (void)
#else
#error Compiler not supported!
#endif
{
    switch(__even_in_range(TA1IV,TA1IV_TAIFG))
    {
        case  TA1IV_NONE: break;              // Vector  0:  No interrupt
        case  TA1IV_TACCR1:                   // Vector  2:  TACCR1 CCIFG
        {
            if(gf_wait_trig_rising == 1)     // check TRIG pin
            {
                //PORT_SET_BIT(P2, BIT1);   // test port set and clr for checking length

                timer_capture_start = TA1R;
                timer_ovf_start = timer_ovf_cnt;

                gf_wait_trig_rising = 0;
            }
            else
            {
                //PORT_CLR_BIT(P2, BIT1);   // test port set and clr for checking length

                timer1A1_capture = TA1CCR1;
                timer_ovf_end = timer_ovf_cnt;
                TIMER1_CAPTURE_interrupt(CAPTURE_STOP);

                gf_capture_done = 1;
            }
        }
        break;
        case TA0IV_TACCR2:                   // Vector  4:  TACCR2 CCIFG
        {
        } break;
        case TA0IV_6: break;                  // Vector  6:  Reserved CCIFG
        case TA0IV_8: break;                  // Vector  8:  Reserved CCIFG
        case TA0IV_TAIFG:                   // Vector 10:  TAIFG
        {
            timer_ovf_cnt++;
            __no_operation();
        } break;
        default:  break;
    }
}


// Timer0 A0 interrupt service routine
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=TIMER0_A0_VECTOR
__interrupt void ISR_Timer0_A0 (void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(TIMER0_A0_VECTOR))) ISR_Timer0_A0 (void)
#else
#error Compiler not supported!
#endif
{
    //TA0CCR0 += CONST_TIMER0_CCR0;       // add same period (no concern of overflow)
    gf_20ms_waiting_timer_done = 1;
}

// Timer0 A1 interrupt service routine
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = TIMER0_A1_VECTOR
__interrupt void ISR_TIMER0_A1A2 (void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(TIMER0_A1_VECTOR))) ISR_TIMER0_A1A2 (void)
#else
#error Compiler not supported!
#endif
{
    switch(__even_in_range(TA0IV,TA0IV_TAIFG))
    {
        case  TA0IV_NONE: break;              // Vector  0:  No interrupt
        case  TA0IV_TACCR1:                   // Vector  2:  TACCR1 CCIFG
        {
        }
        break;
        case TA0IV_TACCR2:                   // Vector  4:  TACCR2 CCIFG
        {
        } break;
        case TA0IV_6: break;                  // Vector  6:  Reserved CCIFG
        case TA0IV_8: break;                  // Vector  8:  Reserved CCIFG
        case TA0IV_TAIFG:                   // Vector 10:  TAIFG
        {
            __no_operation();
        } break;
        default:  break;
    }
}
