/*
 * adc.c
 *
 *  Created on: 2022. 7. 13.
 *      Author: yoch
 */


#include <adc.h>

// ADC init of input ADC0(P1.1) for VBAT monitoring
void adc_init_interrupt(void)
{
    // INCH_0 ~ INCH7 : A0 ~ A7, INCH_10 : Internal Temp Sensor
    ADC10CTL1 = ADC_CH_VBAT_MONITOR + ADC10DIV_3;           // Temp Sensor ADC10CLK/4
#if 1
    ADC10CTL0 = SREF_1 + ADC10SHT_3 + REFON + REF2_5V + ADC10ON + ADC10IE;      // GND to 2.5V VREF
#else
    ADC10CTL0 = SREF_0 + ADC10SHT_3 + REFON + ADC10ON + ADC10IE;
#endif
}

uint16_t adc_vbat_acquire(void)
{
    uint16_t adc_val = 0;
    ADC10CTL0 |= ENC + ADC10SC;             // Sampling and conversion start
    __bis_SR_register(LPM_LEVEL + GIE);        // LPM0 with interrupts enabled
    adc_val = ADC10MEM;
    return adc_val;

}

uint16_t vbat_level = 0;

uint8_t vbat_checker(void)
{
    static uint8_t batt_status = VBAT_NO_INFO;
    //uint16_t vbat_level = 0;

    vbat_level = adc_vbat_acquire();

    if(batt_status == VBAT_NO_INFO)
    {
        if(vbat_level > ADC_CH_VBAT_LOW_BAT)
            batt_status = VBAT_NORMAL;
        else if( (vbat_level > ADC_CH_VBAT_CRITIC_BAT) && (vbat_level < ADC_CH_VBAT_LOW_BAT))
            batt_status = VBAT_LOW;
        else if(vbat_level < ADC_CH_VBAT_CRITIC_BAT)
            batt_status = VBAT_CRITICAL;
        else
            batt_status = VBAT_NO_INFO;
    }
    else if(batt_status == VBAT_NORMAL)
    {
        if(vbat_level < ADC_CH_VBAT_LOW_BAT)
            batt_status = VBAT_LOW;
    }
    else if(batt_status == VBAT_LOW)
    {
        if(vbat_level > ADC_CH_VBAT_LOW_BAT + ADC_CH_VBAT_THRESHOLD)
            batt_status = VBAT_NORMAL;
        else if(vbat_level < ADC_CH_VBAT_CRITIC_BAT)
            batt_status = VBAT_CRITICAL;
    }
    else if(batt_status == VBAT_CRITICAL)
    {
        if(vbat_level > ADC_CH_VBAT_CRITIC_BAT + ADC_CH_VBAT_THRESHOLD)
            batt_status = VBAT_LOW;
    }

    return batt_status;
}

uint8_t vbat_level_checker(void)
{
    uint8_t vbat_decimal_x10 = 0;

    vbat_level = adc_vbat_acquire();
    __no_operation();
#if 1   //2.5V ADC reference
    // detect voltage = 3.8V => 1024*(3.8/2)/2.5 = 778
    // detect voltage 3.7V => 758, 3.6V => 737, 3.5V => 737, 3.4V => 696, 3.3V => 676

    if(vbat_level >= 880)           vbat_decimal_x10 = 43;
    else if(vbat_level >= 860)      vbat_decimal_x10 = 42;
    else if(vbat_level >= 839)      vbat_decimal_x10 = 41;
    else if(vbat_level >= 819)      vbat_decimal_x10 = 40;
    else if(vbat_level >= 798)      vbat_decimal_x10 = 39;
    else if(vbat_level >= 778)      vbat_decimal_x10 = 38;
    else if(vbat_level >= 757)      vbat_decimal_x10 = 37;
    else if(vbat_level >= 737)      vbat_decimal_x10 = 36;
    else if(vbat_level >= 716)      vbat_decimal_x10 = 35;
    else if(vbat_level >= 696)      vbat_decimal_x10 = 34;
    else if(vbat_level >= 675)      vbat_decimal_x10 = 33;
    else if(vbat_level >= 655)      vbat_decimal_x10 = 32;
    else if(vbat_level >= 634)      vbat_decimal_x10 = 31;
    else if(vbat_level >= 614)      vbat_decimal_x10 = 30;
    else                            vbat_decimal_x10 = 29;
#else   //3.3V ADC reference
        // detect voltage = 3.8V => 1024*(3.8/2)/3.3 = 590
    // detect voltage 3.7V => 574, 3.6V => 559, 3.5V => 543, 3.4V => 528, 3.3V => 512
    if(vbat_level >= 667)           vbat_decimal_x10 = 43;
    else if(vbat_level >= 651)      vbat_decimal_x10 = 42;
    else if(vbat_level >= 636)      vbat_decimal_x10 = 41;
    else if(vbat_level >= 620)      vbat_decimal_x10 = 40;
    else if(vbat_level >= 605)      vbat_decimal_x10 = 39;
    else if(vbat_level >= 589)      vbat_decimal_x10 = 38;
    else if(vbat_level >= 574)      vbat_decimal_x10 = 37;
    else if(vbat_level >= 558)      vbat_decimal_x10 = 36;
    else if(vbat_level >= 543)      vbat_decimal_x10 = 35;
    else if(vbat_level >= 527)      vbat_decimal_x10 = 34;
    else if(vbat_level >= 512)      vbat_decimal_x10 = 33;
    else if(vbat_level >= 496)      vbat_decimal_x10 = 32;
    else if(vbat_level >= 481)      vbat_decimal_x10 = 31;
    else if(vbat_level >= 465)      vbat_decimal_x10 = 30;
    else                            vbat_decimal_x10 = 29;
#endif

    return vbat_decimal_x10;
}


// ADC10 interrupt service routine
#pragma vector=ADC10_VECTOR
__interrupt void ADC10_ISR (void)
{
    __bic_SR_register_on_exit(LPM_LEVEL + GIE);          // Clear CPUOFF bit from 0(SR)
}


