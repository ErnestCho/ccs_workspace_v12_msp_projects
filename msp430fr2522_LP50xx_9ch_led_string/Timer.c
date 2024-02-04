#include <msp430.h>
#include <Timer.h>
#include "Capt_User.h"

void Timer1_Init(void)      // 100ms timer
{
    TA1CTL   = TASSEL_1 + MC_2 + ID_3;   // CONTINUOUE MODE/(ACLK=32786/8(ID_3)= 4096
}

void Timer1_Stop(void)      // 100ms timer
{
    TA1CTL   &= ~(MC_3);           // Timer Stop
    touch_info.long_key_timer_onoff = 0;       // long key timer off-ed

    // protection code for long key counter set to 0 when timer is not operating
    touch_info.button1_long_key_detect_cnt_from_timer1 = 0;
    touch_info.button2_long_key_detect_cnt_from_timer1 = 0;
    touch_info.button3_long_key_detect_cnt_from_timer1 = 0;
    touch_info.button4_long_key_detect_cnt_from_timer1 = 0;
}

void Timer1_Start(void)      // 100ms timer
{
    TA1CCR0  = CONST_TIMER1_CCR0;           //
    TA1CTL   |= (TACLR | MC_2);           // Timer Start
    touch_info.long_key_timer_onoff = 1;       // long key timer on-ed
}

/*************************************************************
 *   Timer1_A0
 *************************************************************/
void Timer1_A0_Int(unsigned char mode)
{
    if(mode == ENABLE)
        TA1CCTL0 |= CCIE;
    else
        TA1CCTL0 &= ~CCIE;
}

void Timer1_A0_Init(void)                   //
{
    TA1CCR0  = CONST_TIMER1_CCR0;           //
    TA1CTL |= TACLR;
}
#pragma vector=TIMER1_A0_VECTOR
__interrupt void Timer1_A0(void)
{
    TA1CCR0  += CONST_TIMER1_CCR0;        //  410  //100ms
    //WAY3_LED_TOGGLE;
    if(touch_info.botton1_long_key_in_progress == 1)
        touch_info.button1_long_key_detect_cnt_from_timer1++;
    if(touch_info.botton2_long_key_in_progress == 1)
        touch_info.button2_long_key_detect_cnt_from_timer1++;
    if(touch_info.botton3_long_key_in_progress == 1)
        touch_info.button3_long_key_detect_cnt_from_timer1++;
    if(touch_info.botton4_long_key_in_progress == 1)
        touch_info.button4_long_key_detect_cnt_from_timer1++;
}

