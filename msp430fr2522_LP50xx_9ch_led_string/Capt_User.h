/*
 * Capt_User.h
 *
 *  Created on: 2019. 5. 27.
 *      Author: yoch
 */

#ifndef CAPT_USER_H_
#define CAPT_USER_H_

#include "CAPT_Type.h"

typedef struct button_bit_t {
    unsigned short button1_long_key_detect_cnt_from_timer2;     // counter for long key detect
    unsigned button1_touch_on : 1;
    unsigned button1_key_on : 1;
    unsigned button1_long_key_on : 1;
    unsigned botton1_long_key_in_progress : 1;      // flag for long key is in progress
    unsigned button1_long_key_one_time : 1;          // long key want to execute only one time at a(one) long press
    unsigned dummy : 3; // total 8bit
}_tag_button;

typedef struct touch_bit_t {
    _tag_button button[4];
    unsigned change_occur : 1;                      // any update needed at the main loop
    unsigned long_key_timer_onoff : 1;              // flag for long key timer is on or off-ed

}_tag_touch_button_type;

extern _tag_touch_button_type capt;

typedef struct touch_bit_type {
    unsigned short button1_long_key_detect_cnt_from_timer1;     // counter for long key detect
    unsigned short button2_long_key_detect_cnt_from_timer1;     // counter for long key detect
    unsigned short button3_long_key_detect_cnt_from_timer1;     // counter for long key detect
    unsigned short button4_long_key_detect_cnt_from_timer1;     // counter for long key detect
    unsigned button1_touch_on : 1;
    unsigned button2_touch_on : 1;
    unsigned button3_touch_on : 1;
    unsigned button4_touch_on : 1;
    unsigned button1_key_on : 1;
    unsigned button2_key_on : 1;
    unsigned button3_key_on : 1;
    unsigned button4_key_on : 1;
    // below 4 flag is not useful
//    unsigned button1_long_touch_on : 1;
//    unsigned button2_long_touch_on : 1;
//    unsigned button3_long_touch_on : 1;
//    unsigned button4_long_touch_on : 1;
    unsigned button1_long_key_on : 1;
    unsigned button2_long_key_on : 1;
    unsigned button3_long_key_on : 1;
    unsigned button4_long_key_on : 1;
    unsigned change_occur : 1;                      // any update needed at the main loop
    unsigned botton1_long_key_in_progress : 1;      // flag for long key is in progress
    unsigned botton2_long_key_in_progress : 1;      // flag for long key is in progress
    unsigned botton3_long_key_in_progress : 1;      // flag for long key is in progress
    unsigned botton4_long_key_in_progress : 1;      // flag for long key is in progress
    unsigned button1_long_key_one_time : 1;          // long key want to execute only one time at a(one) long press
    unsigned button2_long_key_one_time : 1;          // long key want to execute only one time at a(one) long press
    unsigned button3_long_key_one_time : 1;          // long key want to execute only one time at a(one) long press
    unsigned button4_long_key_one_time : 1;          // long key want to execute only one time at a(one) long press
    unsigned long_key_timer_onoff : 1;      // flag for long key timer is on or off-ed
    // compatible for dominant single long key detect
    unsigned botton_long_key_in_progress : 1;     // flag for long key is in progress
    unsigned button_long_key_one_time : 1;          // long key want to execute only one time at a(one) long press
}_tag_touch_bit_type;

extern _tag_touch_bit_type touch_info;

#if LONGKEY_USE_EXTERNAL_TIMER
#define BUTTON_TOUCH_LONG_KEY_500ms     5     //  100ms*5
#define BUTTON_TOUCH_LONG_KEY_1SEC      10     // 100ms*10
#define BUTTON_TOUCH_LONG_KEY_3SEC      30    //  100ms*50
#define BUTTON_TOUCH_LONG_KEY_5SEC      50    //  100ms*50
#else
#define BUTTON_TOUCH_LONG_KEY_500ms     15     // 32*3(=100ms)*5
#define BUTTON_TOUCH_LONG_KEY_1SEC      30     // 32*3(=100ms)*10
#define BUTTON_TOUCH_LONG_KEY_3SEC      90    //  32*3(=100ms)*30
#define BUTTON_TOUCH_LONG_KEY_5SEC      150    // 32*3(=100ms)*50
#endif
// time = 32ms(==CAPT_MS_TO_CYCLES) * {this_value}
#define TOUCH_LONG_KEY_TIME             BUTTON_TOUCH_LONG_KEY_3SEC




void ToggleButtonEventHandler(tSensor *pSensor);
void PushButtonEventHandler(tSensor *pSensor);



#endif /* CAPT_USER_H_ */
