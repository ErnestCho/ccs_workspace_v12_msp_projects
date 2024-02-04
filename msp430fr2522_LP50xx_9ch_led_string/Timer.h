#ifndef TIMER_H_
#define TIMER_H_

/****************************** Define Statements **************************/
#define CONST_TIMER1_CCR0 410  //100ms             //1024    // 250ms

/****************************** Local Variables **************************/

//
#define     ENABLE      1
#define     DISABLE     0

extern unsigned short touch_long_key_detect_cnt_from_timer1;

/****************************** Local Functions **************************/
void Timer1_Init();
void Timer1_A0_Init();
void Timer1_A0_Int(unsigned char mode);
void Timer1_Stop();      // 100ms timer
void Timer1_Start();      // 100ms timer


/****************************** Public Variables  **********************/


#endif /* TIMER_H_ */
