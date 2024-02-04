/*
 * Capt_User.c
 *
 *  Created on: 2019. 5. 27.
 *      Author: yoch
 */

#include <Timer.h>
#include "captivate.h"                   // CapTIvate Touch Software Library
#include "Capt_User.h"

#define     false       0
#define     true        1

// touch information struct
_tag_touch_bit_type touch_info = {
    .button1_touch_on = 0,
    .button2_touch_on = 0,
    .button3_touch_on = 0,
    .button4_touch_on = 0,
    .button1_key_on = 0,
    .button2_key_on = 0,
    .button3_key_on = 0,
    .button4_key_on = 0,
//    .button1_long_touch_on = 0,
//    .button2_long_touch_on = 0,
//    .button3_long_touch_on = 0,
//    .button4_long_touch_on = 0,
    .button1_long_key_on = 0,
    .button2_long_key_on = 0,
    .button3_long_key_on = 0,
    .button4_long_key_on = 0,
    .change_occur = 0,                      // any update needed at the main loop
    .botton1_long_key_in_progress = 0,      // flag for long key is in progress
    .botton2_long_key_in_progress = 0,      // flag for long key is in progress
    .botton3_long_key_in_progress = 0,      // flag for long key is in progress
    .botton4_long_key_in_progress = 0,      // flag for long key is in progress
    .button1_long_key_one_time = 0,         // long key want to execute only one time at a(one) long press
    .button2_long_key_one_time = 0,         // long key want to execute only one time at a(one) long press
    .button3_long_key_one_time = 0,         // long key want to execute only one time at a(one) long press
    .button4_long_key_one_time = 0,         // long key want to execute only one time at a(one) long press
    // compatible for dominant single long key detect
    .botton_long_key_in_progress = 0,   // flag for long key is in progress
    .button_long_key_one_time = 0,      // long key want to execute only one time at a(one) long press
};

_tag_touch_button_type capt;
//*****************************************************************************
// Function Implementations
//*****************************************************************************
#if 0 // delete, initialization is done at variable declaration
void touch_info_init(void)
{
    touch_info.button1_touch_on = 0;
    touch_info.button2_touch_on = 0;
    touch_info.button3_touch_on = 0;
    touch_info.button4_touch_on = 0;
    touch_info.button1_key_on = 0;
    touch_info.button2_key_on = 0;
    touch_info.button3_key_on = 0;
    touch_info.button4_key_on = 0;
//    touch_info.button1_long_touch_on = 0;
//    touch_info.button2_long_touch_on = 0;
//    touch_info.button3_long_touch_on = 0;
//    touch_info.button4_long_touch_on = 0;
    touch_info.button1_long_key_on = 0;
    touch_info.button2_long_key_on = 0;
    touch_info.button3_long_key_on = 0;
    touch_info.button4_long_key_on = 0;
    touch_info.button_long_key_occur = 0;
}
#endif

//*****************************************************************************
// Custom Extern Variables
//*****************************************************************************
extern tElement BTN00_E00;
extern tElement BTN00_E01;
extern tElement BTN00_E02;
extern tElement BTN00_E03;


// touch is sensing every [g_uiApp.ui16ActiveModeScanPeriod] msec.
void ToggleButtonEventHandler(tSensor *pSensor)
{
#if 1                       // multiple touch recognition
    __no_operation();                       // for break point

    ///////////////////////////////
    // check BTN00_E00 button
    ///////////////////////////////
    if (BTN00_E00.bTouch == 1)
    {
        if(touch_info.button1_touch_on == 0)    // set bit one time
        {
            // long key process ==[
            if(touch_info.botton1_long_key_in_progress == 0)
            {
            // ]== long key
                touch_info.button1_touch_on = 1;
            // long key process ==[
            }
            // ]== long key
        }

        // long key process ==[
        if(touch_info.button1_long_key_on == 0)
        {
            // start to count timer
            if(touch_info.botton1_long_key_in_progress == 0)
            {
                touch_info.botton1_long_key_in_progress = 1;
                touch_info.button1_long_key_detect_cnt_from_timer1 = 0;
                // check timer2_start is already in progress.
                if(touch_info.long_key_timer_onoff == 0)
                    Timer1_Start();
            }

            // condition of setting of long key flag
            if(touch_info.button1_long_key_detect_cnt_from_timer1 == TOUCH_LONG_KEY_TIME)
            {
                if(touch_info.button1_long_key_one_time == 0)
                {
                    if(touch_info.button1_touch_on == 1)
                    {
                        touch_info.button1_touch_on = 0;

                        touch_info.change_occur = 1;
                        // long key have to execute one time, even the user press the button longer than TOUCH_LONG_KEY_TIME
                        touch_info.button1_long_key_one_time = 1;  // long key want to execute only one time at a(one) long press
                        touch_info.button1_long_key_on = 1;

                        touch_info.button1_long_key_detect_cnt_from_timer1 = 0;
                        // timer2 stop condition == other buttons are not in progress
                        if( (touch_info.botton2_long_key_in_progress == 0)
                          && (touch_info.botton3_long_key_in_progress == 0)
                          && (touch_info.botton4_long_key_in_progress == 0)
                          )
                        {
                            if(touch_info.long_key_timer_onoff == 1)
                                Timer1_Stop();
                        }
                    }
                    // need else? No -> long key is effective only at button1_touch_on == 1
                }
                // need else? (long_key_one_time == 1) && (long_key_detect_cnt == TOUCH_LONG_KEY_TIME) ????
            }
            // need else? --> protection code for (long_key_detect_cnt > TOUCH_LONG_KEY_TIME)
            else if(touch_info.button1_long_key_detect_cnt_from_timer1 > TOUCH_LONG_KEY_TIME)
            {
                touch_info.button1_long_key_detect_cnt_from_timer1 = 0;
            }
        }
        // need else? (long_key_on == 1) ??
        // ]== long key
    }
    // case of BTN00_E00.bTouch == 0
    else
    {
        if(touch_info.button1_touch_on == 1)        // normal case of touch
        {
            touch_info.button1_touch_on = 0;
            touch_info.change_occur = 1;        // update needed at main loop
            if(touch_info.button1_key_on == 1)      // (key_on == 1) : button touched and released
                touch_info.button1_key_on = 0;
            else
            {
                touch_info.button1_key_on = 1;
            }

            // long key process ==[
            // short key released then initial long key process too.
            touch_info.button1_long_key_detect_cnt_from_timer1 = 0;

            // timer2 stop condition == other buttons are not in progress
            if( (touch_info.botton2_long_key_in_progress == 0)
              && (touch_info.botton3_long_key_in_progress == 0)
              && (touch_info.botton4_long_key_in_progress == 0)
              )
            {
                Timer1_Stop();
            }
            // ]== long key
        }
        // need else?

        // long key process ==[
        touch_info.botton1_long_key_in_progress = 0;
        // ]== long key
    }

    ///////////////////////////////
    // check BTN00_E01 button
    ///////////////////////////////
    if (BTN00_E01.bTouch == 1)
    {
        if(touch_info.button2_touch_on == 0)    // set bit one time
        {
            // long key process ==[
            if(touch_info.botton2_long_key_in_progress == 0)
            {
            // ]== long key
                touch_info.button2_touch_on = 1;
            // long key process ==[
            }
            // ]== long key
        }

        // long key process ==[
        if(touch_info.button2_long_key_on == 0)
        {
            // start to count timer
            if(touch_info.botton2_long_key_in_progress == 0)
            {
                touch_info.botton2_long_key_in_progress = 1;
                touch_info.button2_long_key_detect_cnt_from_timer1 = 0;
                // check timer2_start is already in progress.
                if(touch_info.long_key_timer_onoff == 0)
                    Timer1_Start();
            }

            // condition of setting of long key flag
            if(touch_info.button2_long_key_detect_cnt_from_timer1 == TOUCH_LONG_KEY_TIME)
            {
                if(touch_info.button2_long_key_one_time == 0)
                {
                    if(touch_info.button2_touch_on == 1)
                    {
                        touch_info.button2_touch_on = 0;

                        touch_info.change_occur = 1;
                        // long key have to execute one time, even the user press the button longer than TOUCH_LONG_KEY_TIME
                        touch_info.button2_long_key_one_time = 1;  // long key want to execute only one time at a(one) long press
                        touch_info.button2_long_key_on = 1;

                        touch_info.button2_long_key_detect_cnt_from_timer1 = 0;
                        // timer2 stop condition == other buttons are not in progress
                        if( (touch_info.botton1_long_key_in_progress == 0)
                          && (touch_info.botton3_long_key_in_progress == 0)
                          && (touch_info.botton4_long_key_in_progress == 0)
                          )
                        {
                            if(touch_info.long_key_timer_onoff == 1)
                                Timer1_Stop();
                        }
                    }
                    // need else? No -> long key is effective only at button1_touch_on == 1
                }
                // need else? (long_key_one_time == 1) && (long_key_detect_cnt == TOUCH_LONG_KEY_TIME) ????
            }
            // need else? --> protection code for (long_key_detect_cnt > TOUCH_LONG_KEY_TIME)
            else if(touch_info.button2_long_key_detect_cnt_from_timer1 > TOUCH_LONG_KEY_TIME)
            {
                touch_info.button2_long_key_detect_cnt_from_timer1 = 0;
            }
        }
        // need else? (long_key_on == 1) ??
        // ]== long key

    }
    else
    {
        if(touch_info.button2_touch_on == 1)        // normal case of touch
        {
            touch_info.button2_touch_on = 0;
            touch_info.change_occur = 1;            // update needed at main loop
            if(touch_info.button2_key_on == 1)      // (key_on == 1) : button touched and released
                touch_info.button2_key_on = 0;
            else
            {
                touch_info.button2_key_on = 1;
            }

            // long key process ==[
            // short key released then initial long key process too.
            touch_info.button2_long_key_detect_cnt_from_timer1 = 0;

            // timer2 stop condition == other buttons are not in progress
            if( (touch_info.botton1_long_key_in_progress == 0)
              && (touch_info.botton3_long_key_in_progress == 0)
              && (touch_info.botton4_long_key_in_progress == 0)
              )
            {
                Timer1_Stop();
            }
            // ]== long key
        }
        // need else?

        // long key process ==[
        touch_info.botton2_long_key_in_progress = 0;
        // ]== long key
    }

    ///////////////////////////////
    // check BTN00_E02 button
    ///////////////////////////////
    if (BTN00_E02.bTouch == 1)
    {
        if(touch_info.button3_touch_on == 0)    // set bit one time
        {
            // long key process ==[
            if(touch_info.botton3_long_key_in_progress == 0)
            {
            // ]== long key
                touch_info.button3_touch_on = 1;
            // long key process ==[
            }
            // ]== long key
        }
        // long key process ==[
        if(touch_info.button3_long_key_on == 0)
        {
            // start to count timer
            if(touch_info.botton3_long_key_in_progress == 0)
            {
                touch_info.botton3_long_key_in_progress = 1;
                touch_info.button3_long_key_detect_cnt_from_timer1 = 0;
                // check timer2_start is already in progress.
                if(touch_info.long_key_timer_onoff == 0)
                    Timer1_Start();
            }

            // condition of setting of long key flag
            if(touch_info.button3_long_key_detect_cnt_from_timer1 == TOUCH_LONG_KEY_TIME)
            {
                if(touch_info.button3_long_key_one_time == 0)
                {
                    if(touch_info.button3_touch_on == 1)
                    {
                        touch_info.button3_touch_on = 0;

                        touch_info.change_occur = 1;
                        // long key have to execute one time, even the user press the button longer than TOUCH_LONG_KEY_TIME
                        touch_info.button3_long_key_one_time = 1;  // long key want to execute only one time at a(one) long press
                        touch_info.button3_long_key_on = 1;

                        touch_info.button3_long_key_detect_cnt_from_timer1 = 0;
                        // timer2 stop condition == other buttons are not in progress
                        if( (touch_info.botton1_long_key_in_progress == 0)
                          && (touch_info.botton2_long_key_in_progress == 0)
                          && (touch_info.botton4_long_key_in_progress == 0)
                          )
                        {
                            if(touch_info.long_key_timer_onoff == 1)
                                Timer1_Stop();
                        }
                    }
                    // need else? No -> long key is effective only at button1_touch_on == 1
                }
                // need else? (long_key_one_time == 1) && (long_key_detect_cnt == TOUCH_LONG_KEY_TIME) ????
            }
            // need else? --> protection code for (long_key_detect_cnt > TOUCH_LONG_KEY_TIME)
            else if(touch_info.button3_long_key_detect_cnt_from_timer1 > TOUCH_LONG_KEY_TIME)
            {
                touch_info.button3_long_key_detect_cnt_from_timer1 = 0;
            }
        }
        // need else? (long_key_on == 1) ??
        // ]== long key
    }
    else
    {
        if(touch_info.button3_touch_on == 1)        // normal case of touch
        {
            touch_info.button3_touch_on = 0;
            touch_info.change_occur = 1;            // update needed at main loop
            if(touch_info.button3_key_on == 1)      // (key_on == 1) : button touched and released
                touch_info.button3_key_on = 0;
            else
            {
                touch_info.button3_key_on = 1;
            }
            // long key process ==[
            // short key released then initial long key process too.
            touch_info.button3_long_key_detect_cnt_from_timer1 = 0;

            // timer2 stop condition == other buttons are not in progress
            if( (touch_info.botton1_long_key_in_progress == 0)
              && (touch_info.botton2_long_key_in_progress == 0)
              && (touch_info.botton4_long_key_in_progress == 0)
              )
            {
                Timer1_Stop();
            }
            // ]== long key
        }
        // need else?

        // long key process ==[
        touch_info.botton3_long_key_in_progress = 0;
        // ]== long key
    }

    ///////////////////////////////
    // check BTN00_E03 button
    ///////////////////////////////
    if (BTN00_E03.bTouch == 1)
    {
        if(touch_info.button4_touch_on == 0)    // set bit one time
        {
            // long key process ==[
            if(touch_info.botton4_long_key_in_progress == 0)
            {
            // ]== long key
                touch_info.button4_touch_on = 1;
            // long key process ==[
            }
            // ]== long key
        }
        // long key process ==[
        if(touch_info.button4_long_key_on == 0)
        {
            // start to count timer
            if(touch_info.botton4_long_key_in_progress == 0)
            {
                touch_info.botton4_long_key_in_progress = 1;
                touch_info.button4_long_key_detect_cnt_from_timer1 = 0;
                // check timer2_start is already in progress.
                if(touch_info.long_key_timer_onoff == 0)
                    Timer1_Start();
            }

            // condition of setting of long key flag
            if(touch_info.button4_long_key_detect_cnt_from_timer1 == TOUCH_LONG_KEY_TIME)
            {
                if(touch_info.button4_long_key_one_time == 0)
                {
                    if(touch_info.button4_touch_on == 1)
                    {
                        touch_info.button4_touch_on = 0;

                        touch_info.change_occur = 1;
                        // long key have to execute one time, even the user press the button longer than TOUCH_LONG_KEY_TIME
                        touch_info.button4_long_key_one_time = 1;  // long key want to execute only one time at a(one) long press
                        touch_info.button4_long_key_on = 1;

                        touch_info.button4_long_key_detect_cnt_from_timer1 = 0;
                        // timer2 stop condition == other buttons are not in progress
                        if( (touch_info.botton1_long_key_in_progress == 0)
                          && (touch_info.botton2_long_key_in_progress == 0)
                          && (touch_info.botton3_long_key_in_progress == 0)
                          )
                        {
                            if(touch_info.long_key_timer_onoff == 1)
                                Timer1_Stop();
                        }
                    }
                    // need else? No -> long key is effective only at button1_touch_on == 1
                }
                // need else? (long_key_one_time == 1) && (long_key_detect_cnt == TOUCH_LONG_KEY_TIME) ????
            }
            // need else? --> protection code for (long_key_detect_cnt > TOUCH_LONG_KEY_TIME)
            else if(touch_info.button4_long_key_detect_cnt_from_timer1 > TOUCH_LONG_KEY_TIME)
            {
                touch_info.button4_long_key_detect_cnt_from_timer1 = 0;
            }
        }
        // need else? (long_key_on == 1) ??
        // ]== long key
    }
    else
    {
        if(touch_info.button4_touch_on == 1)        // normal case of touch
        {
            touch_info.button4_touch_on = 0;
            touch_info.change_occur = 1;            // update needed at main loop
            if(touch_info.button4_key_on == 1)      // (key_on == 1) : button touched and released
                touch_info.button4_key_on = 0;
            else
            {
                touch_info.button4_key_on = 1;
            }
            // long key process ==[
            // short key released then initial long key process too.
            touch_info.button4_long_key_detect_cnt_from_timer1 = 0;

            // timer2 stop condition == other buttons are not in progress
            if( (touch_info.botton1_long_key_in_progress == 0)
              && (touch_info.botton2_long_key_in_progress == 0)
              && (touch_info.botton3_long_key_in_progress == 0)
              )
            {
                Timer1_Stop();
            }
            // ]== long key
        }
        // need else?

        // long key process ==[
        touch_info.botton4_long_key_in_progress = 0;
        // ]== long key
    }
#else                       // single touch at a time

    __no_operation();                       // for break point
    //
    // If the sensor has a new touch, process it based on the key
    //
    if ( pSensor->bSensorTouch == true )
    {
        __no_operation();                       // for break point

        // case of (bSensorTouch == 1 && bSensorPrevTouch == 0)
        if( pSensor->bSensorPrevTouch == false )
        {
            //
            // Identify the dominant element in the sensor.
            // Of the four buttons, only one is allowed to be touched at a time,
            // and the button with the strongest response is selected.
            //
            switch (CAPT_getDominantButton(pSensor))
            {
                case 0:
                    touch_info.button1_touch_on = 1;
                    break;

                case 1:
                    touch_info.button2_touch_on = 1;
                    break;

                case 2:
                    touch_info.button3_touch_on = 1;
                    break;

                case 3:
                    touch_info.button4_touch_on = 1;
                    break;

                default:
                    break;
            }
        }

        // case of (bSensorTouch == 1 && bSensorPrevTouch == 1)
        else
        {
            // Long key case
            if( (pSensor->ui16Timeout >= TOUCH_LONG_KEY_TIME) && (touch_info.button_long_key_one_time == 0) )
            {
                touch_info.change_occur = 1;
                touch_info.button_long_key_one_time = 1;  // long key want to execute only one time at a(one) long press
                if(touch_info.button1_touch_on == 1)
                {
                    touch_info.button1_long_key_on = 1;
                    touch_info.button1_touch_on = 0;
                }
                if(touch_info.button2_touch_on == 1)
                {
                    touch_info.button2_long_key_on = 1;
                    touch_info.button2_touch_on = 0;
                }
                if(touch_info.button3_touch_on == 1)
                {
                    touch_info.button3_long_key_on = 1;
                    touch_info.button3_touch_on = 0;
                }
                if(touch_info.button4_touch_on == 1)
                {
                    touch_info.button4_long_key_on = 1;
                    touch_info.button4_touch_on = 0;
                }
            }
        }

    }
    else if ((pSensor->bSensorTouch == false) && (pSensor->bSensorPrevTouch == true))
    {
        __no_operation();                       // for break point

        // check update needed for main loop
        if( (touch_info.button1_long_key_on == 0)
            && (touch_info.button2_long_key_on == 0)
            && (touch_info.button3_long_key_on == 0)
            && (touch_info.button4_long_key_on == 0)
            )
        {
            touch_info.change_occur = 1;
        }

        if(touch_info.button1_touch_on == 1)
        {
            if(touch_info.button1_key_on == 1)
                touch_info.button1_key_on = 0;
            else
                touch_info.button1_key_on = 1;
        }
        if(touch_info.button2_touch_on == 1)
        {
            if(touch_info.button2_key_on == 1)
                touch_info.button2_key_on = 0;
            else
                touch_info.button2_key_on = 1;
        }
        if(touch_info.button3_touch_on == 1)
        {
            if(touch_info.button3_key_on == 1)
                touch_info.button3_key_on = 0;
            else
                touch_info.button3_key_on = 1;
        }
        if(touch_info.button4_touch_on == 1)
        {
            if(touch_info.button4_key_on == 1)
                touch_info.button4_key_on = 0;
            else
                touch_info.button4_key_on = 1;
        }

        touch_info.button1_touch_on = 0;
        touch_info.button2_touch_on = 0;
        touch_info.button3_touch_on = 0;
        touch_info.button4_touch_on = 0;
        touch_info.button_long_key_one_time = 0;
    }
#endif

}

// touch is sensing every [g_uiApp.ui16ActiveModeScanPeriod] msec.
void PushButtonEventHandler(tSensor *pSensor)
{
// single touch at a time

    __no_operation();                       // for break point
    //
    // If the sensor has a new touch, process it based on the key
    //
    if ( pSensor->bSensorTouch == true )
    {
        __no_operation();                       // for break point

        // case of (bSensorTouch == 1 && bSensorPrevTouch == 0)
        if( pSensor->bSensorPrevTouch == false )
        {
            //
            // Identify the dominant element in the sensor.
            // Of the four buttons, only one is allowed to be touched at a time,
            // and the button with the strongest response is selected.
            //
            switch (CAPT_getDominantButton(pSensor))
            {
                case 0:
                    touch_info.button1_touch_on = 1;
                    break;

                case 1:
                    touch_info.button2_touch_on = 1;
                    break;

                case 2:
                    touch_info.button3_touch_on = 1;
                    break;

                case 3:
                    touch_info.button4_touch_on = 1;
                    break;

                default:
                    break;
            }
        }

        // case of (bSensorTouch == 1 && bSensorPrevTouch == 1)
        else
        {
            // Long key case
            if( (pSensor->ui16Timeout >= TOUCH_LONG_KEY_TIME) && (touch_info.button_long_key_one_time == 0) )
            {
                touch_info.change_occur = 1;
                touch_info.button_long_key_one_time = 1;  // long key want to execute only one time at a(one) long press
                if(touch_info.button1_touch_on == 1)
                {
                    touch_info.button1_long_key_on = 1;
                    touch_info.button1_touch_on = 0;
                }
                if(touch_info.button2_touch_on == 1)
                {
                    touch_info.button2_long_key_on = 1;
                    touch_info.button2_touch_on = 0;
                }
                if(touch_info.button3_touch_on == 1)
                {
                    touch_info.button3_long_key_on = 1;
                    touch_info.button3_touch_on = 0;
                }
                if(touch_info.button4_touch_on == 1)
                {
                    touch_info.button4_long_key_on = 1;
                    touch_info.button4_touch_on = 0;
                }
            }
        }

    }
    else if ((pSensor->bSensorTouch == false) && (pSensor->bSensorPrevTouch == true))
    {
        __no_operation();                       // for break point

        // check update needed for main loop
        if( (touch_info.button1_long_key_on == 0)
            && (touch_info.button2_long_key_on == 0)
            && (touch_info.button3_long_key_on == 0)
            && (touch_info.button4_long_key_on == 0)
            )
        {
            touch_info.change_occur = 1;
        }

        if(touch_info.button1_touch_on == 1)
        {
            if(touch_info.button1_key_on == 1)
                touch_info.button1_key_on = 0;
            else
                touch_info.button1_key_on = 1;
        }
        if(touch_info.button2_touch_on == 1)
        {
            if(touch_info.button2_key_on == 1)
                touch_info.button2_key_on = 0;
            else
                touch_info.button2_key_on = 1;
        }
        if(touch_info.button3_touch_on == 1)
        {
            if(touch_info.button3_key_on == 1)
                touch_info.button3_key_on = 0;
            else
                touch_info.button3_key_on = 1;
        }
        if(touch_info.button4_touch_on == 1)
        {
            if(touch_info.button4_key_on == 1)
                touch_info.button4_key_on = 0;
            else
                touch_info.button4_key_on = 1;
        }

        touch_info.button1_touch_on = 0;
        touch_info.button2_touch_on = 0;
        touch_info.button3_touch_on = 0;
        touch_info.button4_touch_on = 0;
        touch_info.button_long_key_one_time = 0;
    }

}



