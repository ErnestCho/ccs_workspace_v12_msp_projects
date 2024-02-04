/*
 * drv_clock.c
 *
 *  Created on: 2018. 7. 31.
 *      Author: yoch
 */
#include <msp430.h>
#include <drv_clock.h>

uint8_t CLOCK_init(uint8_t mclk_spd, uint8_t smclk_div, uint8_t aclk_src)
{
    uint8_t ret_val= mclk_spd<<4 + smclk_div;
    // check if TLV SegmentA is erased or not,
    // especially check clock related data...
    DCOCTL = 0;                               // Select lowest DCOx and MODx settings

    // check if calibration constant erased or not
    if (CALBC1_1MHZ==0xFF ||
        CALBC1_8MHZ==0xFF ||
        CALBC1_12MHZ==0xFF ||
        CALBC1_16MHZ==0xFF)
    {
        // default value of CALDCO_1MHZ and CALBC1_1MHZ
        // 1MHz:86, BA, 8MHz: 8D, 82, 12MHz: 8E, 92, 16MHz: 8F, 8E
        BCSCTL1 = 0x86;
        DCOCTL = 0xBA;
    }

    // setup for MCLK
    switch(mclk_spd)
    {
        case MCLK_16MHZ :
            BCSCTL1 = CALBC1_16MHZ;                   // Set range
            DCOCTL = CALDCO_16MHZ;                    // Set DCO step + modulation*/
        break;
        case MCLK_12MHZ :
            BCSCTL1 = CALBC1_12MHZ;                   // Set range
            DCOCTL = CALDCO_12MHZ;                    // Set DCO step + modulation*/
        break;
        case MCLK_8MHZ :
            BCSCTL1 = CALBC1_8MHZ;                    // Set range
            DCOCTL = CALDCO_8MHZ;                     // Set DCO step + modulation */
        break;
        case MCLK_1MHZ :
            BCSCTL1 = CALBC1_1MHZ;                    // Set range
            DCOCTL = CALDCO_1MHZ;                     // Set DCO step + modulation */
        break;
        case MCLK_MAX :
            BCSCTL1 = 0x0f;                    // Set range
            DCOCTL = 0xe0;                     // Set DCO step + modulation */
        break;
        default :
            BCSCTL1 = CALBC1_1MHZ;
            DCOCTL = CALDCO_1MHZ;
            break;
    }

    // setup SMCLK = set divisor for SMCLK (BCSCTL2:DIVSx[2:1])
    if(smclk_div < 4)
        BCSCTL2 |= (smclk_div<<1);

    // setup ACLK -> 1:LFXT1 clock, 2: VLO clock --> if no external 32kHz clock, use VLO clock
    BCSCTL1 &= ~(XTS | DIVA_0);             // low frequency mode, no divide ACLK
    if(aclk_src == ACLK_EXT_32K)
    {
        BCSCTL3 |= LFXT1S_0;                // 32768Hz LFXT1
    }
    else    // ACLK_INT_VLO
    {
        BCSCTL3 |= LFXT1S_2;                // VLOCLK
    }
    IFG1 &= ~OFIFG;                         // clear OSCFault flag

#if 0 // test
    // test purpose : remove comment if you want to check the clock output via related PORT.
    P1DIR |= 0x11;                            // P1.0,P1.1 and P1.4 outputs
    P1SEL |= 0x11;                            // P1.0,P1.4 ACLK, SMCLK output
#endif
    __delay_cycles(1000);

    return ret_val;
}


