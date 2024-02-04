/*
 * drv_clock.h
 *
 *  Created on: 2018. 7. 31.
 *      Author: yoch
 */

#ifndef DRV_CLOCK_H_
#define DRV_CLOCK_H_

#include <type_define.h>
// main clock(MCLK) setting
#define MCLK_1MHZ       1
#define MCLK_8MHZ       2
#define MCLK_12MHZ      3
#define MCLK_16MHZ      4
#define MCLK_MAX        5       // only for test purpose..

// sub main clock(SMCLK) divide rate
#define SMCLK_DIV_1     0
#define SMCLK_DIV_2     1
#define SMCLK_DIV_4     2
#define SMCLK_DIV_8     3

// aux clock(ACLK) clock source
#define ACLK_EXT_32K  1      // should have external 32.768kHz clock
#define ACLK_INT_VLO  2      // typical 12kHz, VLO spec 4~20kHz


// function definition
uint8_t CLOCK_init(uint8_t mclk_spd, uint8_t smclk_div, uint8_t aclk_src);




#endif /* DRV_CLOCK_H_ */
