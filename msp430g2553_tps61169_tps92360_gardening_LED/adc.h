/*
 * adc.h
 *
 *  Created on: 2022. 7. 13.
 *      Author: yoch
 */

#ifndef ADC_H_
#define ADC_H_

#include <msp430.h>
#include <stdint.h>

#define ADC_CH_VBAT_MONITOR             INCH_0  //INCH_6  // input channel #
#if 1   //2.5V ADC reference
// detect voltage = 3.8V => 1024*(3.8/2)/2.5 = 778
// detect voltage 3.7V => 758, 3.6V => 737, 3.5V => 737, 3.4V => 696, 3.3V => 676
#define ADC_CH_VBAT_LOW_BAT             758     // 3.7V
#define ADC_CH_VBAT_CRITIC_BAT          737     // 3.5V
#else   //3.3V ADC reference
// detect voltage = 3.8V => 1024*(3.8/2)/3.3 = 590
// detect voltage 3.7V => 574, 3.6V => 559, 3.5V => 543, 3.4V => 528, 3.3V => 512
#define ADC_CH_VBAT_LOW_BAT             574     // 3.7V
#define ADC_CH_VBAT_CRITIC_BAT          543     // 3.5V
#endif
#define ADC_CH_VBAT_THRESHOLD           5

#define VBAT_NO_INFO    0
#define VBAT_NORMAL     1
#define VBAT_LOW        2
#define VBAT_CRITICAL   3

#define LPM_LEVEL       LPM1_bits


// ADC init of input ADC0(P1.1) for VBAT monitoring
void adc_init_interrupt(void);
uint16_t adc_vbat_acquire(void);
uint8_t vbat_checker(void);
uint8_t vbat_level_checker(void);


#endif /* ADC_H_ */
