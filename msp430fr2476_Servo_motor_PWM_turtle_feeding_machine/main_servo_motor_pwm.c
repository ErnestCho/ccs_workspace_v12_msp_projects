#include <msp430.h> 
#include <stdint.h>


/* --COPYRIGHT--,BSD_EX
 * Copyright (c) 2018, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *******************************************************************************
 *
 *                       MSP430 CODE EXAMPLE DISCLAIMER
 *
 * MSP430 code examples are self-contained low-level programs that typically
 * demonstrate a single peripheral function or device feature in a highly
 * concise manner. For this the code may rely on the device's power-on default
 * register values and settings such as the clock configuration and care must
 * be taken when combining code from several examples to avoid potential side
 * effects. Also see www.ti.com/grace for a GUI- and www.ti.com/msp430ware
 * for an API functional library-approach to peripheral configuration.
 *
 * --/COPYRIGHT--*/
//******************************************************************************
//  MSP430FR267x Demo - Configure MCLK for 8MHz sourced from DCO.
//
//  Description: Default DCODIV is MCLK and SMCLK source.
//  By default, FR267x select XT1 as FLL reference.
//  If XT1 is present, the XIN and XOUT pin needs to configure.
//  If XT1 is absent, switch to select REFO as FLL reference automatically.
//  XT1 is considered to be absent in this example.
//  f(DCOCLK) = 2^FLLD * (FLLN+1) * (fFLLREFCLK / n).
//  FLLD = 0, FLLN =243, n=1, DIVM =1, f(DCOCLK) = 2^0 * (243+1)*32768Hz = 8MHz,
//  f(DCODIV) = (243+1)*32768Hz = 8MHz,
//  ACLK = default REFO ~32768Hz, SMCLK = MCLK = f(DCODIV) = 8MHz.
//  Toggle LED to indicate that the program is running.
//
//                MSP430FR2676
//         ---------------
//     /|\|               |
//      | |               |
//      --|RST            |
//        |          P1.0 |---> LED
//        |               |
//        |               |
//        |          P2.2 |---> ACLK  = 32768Hz
//
//   Longyu Fang
//   Texas Instruments Inc.
//   August 2018
//   Built with IAR Embedded Workbench v7.12.1 & Code Composer Studio v8.1.0
//******************************************************************************
#include <msp430.h>

void Software_Trim();                       // Software Trim to get the best DCOFTRIM value
#define MCLK_FREQ_MHZ 8                     // MCLK = 8MHz

void Software_Trim()
{
    unsigned int oldDcoTap = 0xffff;
    unsigned int newDcoTap = 0xffff;
    unsigned int newDcoDelta = 0xffff;
    unsigned int bestDcoDelta = 0xffff;
    unsigned int csCtl0Copy = 0;
    unsigned int csCtl1Copy = 0;
    unsigned int csCtl0Read = 0;
    unsigned int csCtl1Read = 0;
    unsigned int dcoFreqTrim = 3;
    unsigned char endLoop = 0;

    do
    {
        CSCTL0 = 0x100;                         // DCO Tap = 256
        do
        {
            CSCTL7 &= ~DCOFFG;                  // Clear DCO fault flag
        }while (CSCTL7 & DCOFFG);               // Test DCO fault flag

        __delay_cycles((unsigned int)3000 * MCLK_FREQ_MHZ);// Wait FLL lock status (FLLUNLOCK) to be stable
                                                           // Suggest to wait 24 cycles of divided FLL reference clock
        while((CSCTL7 & (FLLUNLOCK0 | FLLUNLOCK1)) && ((CSCTL7 & DCOFFG) == 0));

        csCtl0Read = CSCTL0;                   // Read CSCTL0
        csCtl1Read = CSCTL1;                   // Read CSCTL1

        oldDcoTap = newDcoTap;                 // Record DCOTAP value of last time
        newDcoTap = csCtl0Read & 0x01ff;       // Get DCOTAP value of this time
        dcoFreqTrim = (csCtl1Read & 0x0070)>>4;// Get DCOFTRIM value

        if(newDcoTap < 256)                    // DCOTAP < 256
        {
            newDcoDelta = 256 - newDcoTap;     // Delta value between DCPTAP and 256
            if((oldDcoTap != 0xffff) && (oldDcoTap >= 256)) // DCOTAP cross 256
                endLoop = 1;                   // Stop while loop
            else
            {
                dcoFreqTrim--;
                CSCTL1 = (csCtl1Read & (~DCOFTRIM)) | (dcoFreqTrim<<4);
            }
        }
        else                                   // DCOTAP >= 256
        {
            newDcoDelta = newDcoTap - 256;     // Delta value between DCPTAP and 256
            if(oldDcoTap < 256)                // DCOTAP cross 256
                endLoop = 1;                   // Stop while loop
            else
            {
                dcoFreqTrim++;
                CSCTL1 = (csCtl1Read & (~DCOFTRIM)) | (dcoFreqTrim<<4);
            }
        }

        if(newDcoDelta < bestDcoDelta)         // Record DCOTAP closest to 256
        {
            csCtl0Copy = csCtl0Read;
            csCtl1Copy = csCtl1Read;
            bestDcoDelta = newDcoDelta;
        }

    }while(endLoop == 0);                      // Poll until endLoop == 1

    CSCTL0 = csCtl0Copy;                       // Reload locked DCOTAP
    CSCTL1 = csCtl1Copy;                       // Reload locked DCOFTRIM
    while(CSCTL7 & (FLLUNLOCK0 | FLLUNLOCK1)); // Poll until FLL is locked
}

#define MAIN_CLOCK_1MHZ     1
#define MAIN_CLOCK_8MHZ     2
#define MAIN_CLOCK_16MHZ    3

void set_main_clock(uint8_t speed)
{
    if(speed == MAIN_CLOCK_1MHZ)
    {
        // select REFO instead of XT1CLK
        //do
        //{
        //    CSCTL7 &= ~(XT1OFFG | DCOFFG);      // Clear XT1 and DCO fault flag
        //    SFRIFG1 &= ~OFIFG;
        //}while (SFRIFG1 & OFIFG);               // Test oscillator fault flag

        __bis_SR_register(SCG0);                // Disable FLL
        CSCTL3 |= SELREF__REFOCLK;              // Set REFO as FLL reference source
        //CSCTL3 = SELREF__XT1CLK;                // Set XT1 as FLL reference source
        CSCTL1 = DCOFTRIMEN_1 | DCOFTRIM0 | DCOFTRIM1 | DCORSEL_0;// DCOFTRIM=3, DCO Range = 1MHz
        CSCTL2 = FLLD_0 + 30;                   // DCODIV = 1MHz
        __delay_cycles(3);
        __bic_SR_register(SCG0);                // Enable FLL
        Software_Trim();                        // Software Trim to get the best DCOFTRIM value

        CSCTL4 = SELMS__DCOCLKDIV | SELA__XT1CLK;  // set ACLK = XT1CLK = 32768Hz
                                                   // DCOCLK = MCLK and SMCLK source
        CSCTL5 |= DIVS_3;                    // CLOCK divider : SMCLK = 1/8 DCOCLK
    }
    else if(speed == MAIN_CLOCK_8MHZ)
    {
        __bis_SR_register(SCG0);                // disable FLL
        CSCTL3 |= SELREF__REFOCLK;              // Set REFO as FLL reference source
        CSCTL1 = DCOFTRIMEN_1 | DCOFTRIM0 | DCOFTRIM1 | DCORSEL_3;// DCOFTRIM=3, DCO Range = 8MHz
        CSCTL2 = FLLD_0 + 243;                  // DCODIV = 8MHz
        __delay_cycles(3);
        __bic_SR_register(SCG0);                // enable FLL
        Software_Trim();                        // Software Trim to get the best DCOFTRIM value

        CSCTL4 = SELMS__DCOCLKDIV | SELA__REFOCLK; // set default REFO(~32768Hz) as ACLK source, ACLK = 32768Hz
                                                // default DCODIV as MCLK and SMCLK source
        CSCTL5 |= DIVS_3;                    // CLOCK divider : SMCLK = 1/8 DCOCLK
    }
    else if(speed == MAIN_CLOCK_16MHZ)
    {
        // Configure one FRAM waitstate as required by the device datasheet for MCLK
        // operation beyond 8MHz _before_ configuring the clock system.
        FRCTL0 = FRCTLPW | NWAITS_1;

        __bis_SR_register(SCG0);                           // disable FLL
        CSCTL3 |= SELREF__REFOCLK;                         // Set REFO as FLL reference source
        CSCTL1 = DCOFTRIMEN_1 | DCOFTRIM0 | DCOFTRIM1 | DCORSEL_5;// DCOFTRIM=5, DCO Range = 16MHz
        CSCTL2 = FLLD_0 + 487;                             // DCOCLKDIV = 16MHz
        __delay_cycles(3);
        __bic_SR_register(SCG0);                           // enable FLL
        Software_Trim();                                   // Software Trim to get the best DCOFTRIM value

         CSCTL4 = SELMS__DCOCLKDIV | SELA__REFOCLK;        // set default REFO(~32768Hz) as ACLK source, ACLK = 32768Hz
                                                           // default DCOCLKDIV as MCLK and SMCLK source
         CSCTL5 |= DIVS_3;                    // CLOCK divider : SMCLK = 1/8 DCOCLK
    }
    else        // 8MHz
    {
        __bis_SR_register(SCG0);                // disable FLL
        CSCTL3 |= SELREF__REFOCLK;              // Set REFO as FLL reference source
        CSCTL1 = DCOFTRIMEN_1 | DCOFTRIM0 | DCOFTRIM1 | DCORSEL_3;// DCOFTRIM=3, DCO Range = 8MHz
        CSCTL2 = FLLD_0 + 243;                  // DCODIV = 8MHz
        __delay_cycles(3);
        __bic_SR_register(SCG0);                // enable FLL
        Software_Trim();                        // Software Trim to get the best DCOFTRIM value

        CSCTL4 = SELMS__DCOCLKDIV | SELA__REFOCLK; // set default REFO(~32768Hz) as ACLK source, ACLK = 32768Hz
                                                // default DCODIV as MCLK and SMCLK source
        CSCTL5 |= DIVS_3;                    // CLOCK divider : SMCLK = 1/8 DCOCLK
    }

}



// SW1 = P4.0
#define SW1_PORT_DIR            P4DIR
#define SW1_PORT_OUT            P4OUT
#define SW1_PORT_REN            P4REN
#define SW1_PORT_IES            P4IES
#define SW1_PORT_IE             P4IE
#define SW1_PORT_IFG            P4IFG
#define SW1_BIT                 BIT0

// SW2 = P2.3
#define SW2_PORT_DIR            P2DIR
#define SW2_PORT_OUT            P2OUT
#define SW2_PORT_REN            P2REN
#define SW2_PORT_IES            P2IES
#define SW2_PORT_IE             P2IE
#define SW2_PORT_IFG            P2IFG
#define SW2_BIT                 BIT3


#define DISABLE     0
#define ENABLE      1

uint8_t switch_status = 0;
///////////////////////////////////////////////////////////////////////
// GPIO Switch interrupt part
///////////////////////////////////////////////////////////////////////

// GPIO_P1InterruptSet() : external interrupt setup for P1
// param : bits - multi-bits setup
void GPIO_SW1_SW2_Interrupt_Set(void)
{
#if 0
    SW1_PORT_DIR &= ~SW1_BIT;                         // port as Input
    SW1_PORT_OUT |= SW1_BIT;                          // Configure P1 as pulled-up
    SW1_PORT_REN |= SW1_BIT;                          // P1 pull-up register enable
    SW1_PORT_IES &= ~SW1_BIT;                         // P1 edge, 0: low to high edge
    SW1_PORT_IE  |= SW1_BIT;                          // P1 interrupt enabled
    SW1_PORT_IFG &= ~SW1_BIT;                         // P1 IFG cleared
#endif
    SW2_PORT_DIR &= ~SW2_BIT;
    SW2_PORT_OUT |= SW2_BIT;                          // Configure P2.x as pulled-up
    SW2_PORT_REN |= SW2_BIT;                          // P2.x pull-up register enable
    SW2_PORT_IES &= ~SW2_BIT;                         // P2.x edge, 0: low to high edge
    SW2_PORT_IE  |= SW2_BIT;                          // P2.x interrupt enabled
    SW2_PORT_IFG &= ~SW2_BIT;                         // P2.x IFG cleared
}

// 16MHz
//
//
#define PWM_PERIOD_20ms_SMCLK_1MHz          999//2500
#define PWM_PERIOD_20ms_SMCLK_8MHz          999//20000
#define PWM_PERIOD_20ms_SMCLK_16MHz         40000 // 16MHz/8(SMCLK DIVIDOR) = 2MHz, 20ms = 20000*2

#define PWM_PERIOD_2ms_SMCLK_1MHz           999//250
#define PWM_PERIOD_2ms_SMCLK_8MHz           999//2000
#define PWM_PERIOD_2ms_SMCLK_16MHz          4000

// this value is tuned for SMCLK 16MHz(=16MHz/8)
#define TIMER0_SMCLK_50usec                 100
#define TIMER0_SMCLK_25usec                 50
#define TIMER0_SMCLK_5usec                  10
#define TIMER0_SMCLK_2p5usec                5


#define PWM_DUTY_1ms_SMCLK_16MHz            2000
#define PWM_DUTY_1p25ms_SMCLK_16MHz         2500
#define PWM_DUTY_1p5ms_SMCLK_16MHz          3000
#define PWM_DUTY_1p75ms_SMCLK_16MHz         3500
#define PWM_DUTY_2ms_SMCLK_16MHz            4000
#define PWM_DUTY_2p5ms_SMCLK_16MHz          5000
#define PWM_DUTY_3ms_SMCLK_16MHz            6000


// reference --> https://gcc.gnu.org/onlinedocs/cpp/Argument-Prescan.html#Argument-Prescan

//#define AFTERX(x) X_ ## x
//#define XAFTERX(x) AFTERX(x)
//#define TABLESIZE 1024
//#define BUFSIZE TABLESIZE
//-->then AFTERX(BUFSIZE) expands to X_BUFSIZE, and XAFTERX(BUFSIZE) expands to X_1024. (Not to X_TABLESIZE. Prescan always does a complete expansion.)


// PWM(x) PORT setting
#define _SERVO_PORT_INIT(port, reg)       P##port##reg
#define SERVO_PORT_INIT(port, reg)        _SERVO_PORT_INIT(port, reg)

// PWM1(TA0.1) : P1.1
#define SERVO_PWM1_PORT         1
#define SERVO_PWM1_BIT          BIT1

// PWM2(TA0.2) : P1.2
#define SERVO_PWM2_PORT         1
#define SERVO_PWM2_BIT          BIT2

// PWM3(TA1.1) : P1.5
#define SERVO_PWM3_PORT         1
#define SERVO_PWM3_BIT          BIT5

// PWM4(TA1.2) : P1.4
#define SERVO_PWM4_PORT         1
#define SERVO_PWM4_BIT          BIT4

#if 0   // TIMER2_PWM_PORT_AS_DEFAULT
// PWM5(TA2.1) : P3.3
#define SERVO_PWM5_PORT         3
#define SERVO_PWM5_BIT          BIT3

// PWM6(TA2.2) : P3.0
#define SERVO_PWM6_PORT         3
#define SERVO_PWM6_BIT          BIT0
#else
// PWM5(TA2.1) : P5.7
#define SERVO_PWM5_PORT         5       //3
#define SERVO_PWM5_BIT          BIT7    //BIT3

// PWM6(TA2.2) : P6.0
#define SERVO_PWM6_PORT         6       //3
#define SERVO_PWM6_BIT          BIT0    //BIT0
#endif

#if 0   // TIMER3_PWM_PORT_AS_DEFALUT
// PWM7(TA3.1) : P4.0
#define SERVO_PWM7_PORT         4
#define SERVO_PWM7_BIT          BIT0

// PWM8(TA3.2) : P3.7
#define SERVO_PWM8_PORT         3
#define SERVO_PWM8_BIT          BIT7
#else
// PWM7(TA3.1) : P4.6 // <-- P4.0
#define SERVO_PWM7_PORT         4       //4
#define SERVO_PWM7_BIT          BIT6    //BIT0

// PWM8(TA3.2) : P4.5 // <-- P3.7
#define SERVO_PWM8_PORT         4       //3
#define SERVO_PWM8_BIT          BIT5    //BIT7
#endif  // TIMER3_PWM_PORT_AS_DEFALUT

// PWM9(TB0.1) : P4.7
#define SERVO_PWM9_PORT         4
#define SERVO_PWM9_BIT          BIT7

// PWM10(TB0.2) : P5.0
#define SERVO_PWM10_PORT        5
#define SERVO_PWM10_BIT         BIT0

// PWM11(TB0.3) : P5.1
#define SERVO_PWM11_PORT        5
#define SERVO_PWM11_BIT         BIT1

// PWM12(TB0.4) : P5.2
#define SERVO_PWM12_PORT        5
#define SERVO_PWM12_BIT         BIT2

// PWM13(TB0.5) : P4.3
#define SERVO_PWM13_PORT        4
#define SERVO_PWM13_BIT         BIT3

// PWM14(TB0.6) : P4.4
#define SERVO_PWM14_PORT        4
#define SERVO_PWM14_BIT         BIT4

// setup all TAx.1,2 TB1.1~7 related port as timer PWM output pin
void GPIO_init_for_All_timer_PWM_out(void)
{
#if 0 // use only 5 ea
    SERVO_PORT_INIT(SERVO_PWM1_PORT, DIR)  |=   SERVO_PWM1_BIT;             // PxDIR  |=  BITx
    SERVO_PORT_INIT(SERVO_PWM1_PORT, SEL1) |=   SERVO_PWM1_BIT;             // PxSEL1 |=  BITx
    SERVO_PORT_INIT(SERVO_PWM1_PORT, SEL0) &= ~(SERVO_PWM1_BIT);            // PxSEL0 &= ~BITx

    SERVO_PORT_INIT(SERVO_PWM2_PORT, DIR)  |=   SERVO_PWM2_BIT;             // PxDIR  |=  BITx
    SERVO_PORT_INIT(SERVO_PWM2_PORT, SEL1) |=   SERVO_PWM2_BIT;             // PxSEL1 |=  BITx
    SERVO_PORT_INIT(SERVO_PWM2_PORT, SEL0) &= ~(SERVO_PWM2_BIT);            // PxSEL0 &= ~BITx

    SERVO_PORT_INIT(SERVO_PWM3_PORT, DIR)  |=   SERVO_PWM3_BIT;             // PxDIR  |=  BITx
    SERVO_PORT_INIT(SERVO_PWM3_PORT, SEL1) |=   SERVO_PWM3_BIT;             // PxSEL1 |=  BITx
    SERVO_PORT_INIT(SERVO_PWM3_PORT, SEL0) &= ~(SERVO_PWM3_BIT);            // PxSEL0 &= ~BITx

    SERVO_PORT_INIT(SERVO_PWM4_PORT, DIR)  |=   SERVO_PWM4_BIT;             // PxDIR  |=  BITx
    SERVO_PORT_INIT(SERVO_PWM4_PORT, SEL1) |=   SERVO_PWM4_BIT;             // PxSEL1 |=  BITx
    SERVO_PORT_INIT(SERVO_PWM4_PORT, SEL0) &= ~(SERVO_PWM4_BIT);            // PxSEL0 &= ~BITx

#if 1       // TIMER2 as P5.7, P6.0
    SYSCFG3 |= TA2RMP;      // Timer2(PWM5, PWM6) remap : {TA2.1, TA2.2} = {P5.7, P6.0} <--{P3.3, P3.0}
#endif
    SERVO_PORT_INIT(SERVO_PWM5_PORT, DIR)  |=   SERVO_PWM5_BIT;         // PxDIR  |=  BITx
    SERVO_PORT_INIT(SERVO_PWM5_PORT, SEL1) &= ~(SERVO_PWM5_BIT);    // PxSEL1 &= ~BITx
    SERVO_PORT_INIT(SERVO_PWM5_PORT, SEL0) |=   SERVO_PWM5_BIT;     // PxSEL0 |=  BITx

    SERVO_PORT_INIT(SERVO_PWM6_PORT, DIR)  |=   SERVO_PWM6_BIT;         // PxDIR  |=  BITx
    SERVO_PORT_INIT(SERVO_PWM6_PORT, SEL1) &= ~(SERVO_PWM6_BIT);    // PxSEL1 &= ~BITx
    SERVO_PORT_INIT(SERVO_PWM6_PORT, SEL0) |=   SERVO_PWM6_BIT;     // PxSEL0 |=  BITx
#if 1       // TIMER3 as P4.6, P4.5
    SYSCFG3 |= TA3RMP;      // Timer3(PWM7, PWM8) remap : {TA3.1, TA3.2} = {P4.6, P4.5} <-- {P4.0, P3.7}
    SERVO_PORT_INIT(SERVO_PWM7_PORT, DIR)  |=   SERVO_PWM7_BIT;             // PxDIR  |=  BITx
    SERVO_PORT_INIT(SERVO_PWM7_PORT, SEL1) |=   SERVO_PWM7_BIT;             // PxSEL1 |=  BITx
    SERVO_PORT_INIT(SERVO_PWM7_PORT, SEL0) &= ~(SERVO_PWM7_BIT);            // PxSEL0 &= ~BITx

    SERVO_PORT_INIT(SERVO_PWM8_PORT, DIR)  |=   SERVO_PWM8_BIT;             // PxDIR  |=  BITx
    SERVO_PORT_INIT(SERVO_PWM8_PORT, SEL1) |=   SERVO_PWM8_BIT;             // PxSEL1 |=  BITx
    SERVO_PORT_INIT(SERVO_PWM8_PORT, SEL0) &= ~(SERVO_PWM8_BIT);            // PxSEL0 &= ~BITx
#else       // TIMER3 as P4.0, P3.7
    SERVO_PORT_INIT(SERVO_PWM7_PORT, DIR)  |=   SERVO_PWM7_BIT;             // PxDIR  |=  BITx
    SERVO_PORT_INIT(SERVO_PWM7_PORT, SEL1) &= ~(SERVO_PWM7_BIT);    // PxSEL1 &= ~BITx
    SERVO_PORT_INIT(SERVO_PWM7_PORT, SEL0) |=   SERVO_PWM7_BIT;     // PxSEL0 |=  BITx

    SERVO_PORT_INIT(SERVO_PWM8_PORT, DIR)  |=   SERVO_PWM8_BIT;             // PxDIR  |=  BITx
    SERVO_PORT_INIT(SERVO_PWM8_PORT, SEL1) &= ~(SERVO_PWM8_BIT);    // PxSEL1 &= ~BITx
    SERVO_PORT_INIT(SERVO_PWM8_PORT, SEL0) |=   SERVO_PWM8_BIT;     // PxSEL0 |=  BITx
#endif
#endif
    SERVO_PORT_INIT(SERVO_PWM9_PORT, DIR)  |=   SERVO_PWM9_BIT;             // PxDIR  |=  BITx
    SERVO_PORT_INIT(SERVO_PWM9_PORT, SEL1) |=   SERVO_PWM9_BIT;             // PxSEL1 |=  BITx
    SERVO_PORT_INIT(SERVO_PWM9_PORT, SEL0) &= ~(SERVO_PWM9_BIT);            // PxSEL0 &= ~BITx

    SERVO_PORT_INIT(SERVO_PWM10_PORT, DIR)  |=   SERVO_PWM10_BIT;           // PxDIR  |=  BITx
    SERVO_PORT_INIT(SERVO_PWM10_PORT, SEL1) |=   SERVO_PWM10_BIT;           // PxSEL1 |=  BITx
    SERVO_PORT_INIT(SERVO_PWM10_PORT, SEL0) &= ~(SERVO_PWM10_BIT);          // PxSEL0 &= ~BITx

    SERVO_PORT_INIT(SERVO_PWM11_PORT, DIR)  |=   SERVO_PWM11_BIT;           // PxDIR  |=  BITx
    SERVO_PORT_INIT(SERVO_PWM11_PORT, SEL1) |=   SERVO_PWM11_BIT;           // PxSEL1 |=  BITx
    SERVO_PORT_INIT(SERVO_PWM11_PORT, SEL0) &= ~(SERVO_PWM11_BIT);          // PxSEL0 &= ~BITx

    SERVO_PORT_INIT(SERVO_PWM12_PORT, DIR)  |=   SERVO_PWM12_BIT;           // PxDIR  |=  BITx
    SERVO_PORT_INIT(SERVO_PWM12_PORT, SEL1) |=   SERVO_PWM12_BIT;           // PxSEL1 |=  BITx
    SERVO_PORT_INIT(SERVO_PWM12_PORT, SEL0) &= ~(SERVO_PWM12_BIT);          // PxSEL0 &= ~BITx

    SERVO_PORT_INIT(SERVO_PWM13_PORT, DIR)  |=   SERVO_PWM13_BIT;           // PxDIR  |=  BITx
    SERVO_PORT_INIT(SERVO_PWM13_PORT, SEL1) |=   SERVO_PWM13_BIT;           // PxSEL1 |=  BITx
    SERVO_PORT_INIT(SERVO_PWM13_PORT, SEL0) &= ~(SERVO_PWM13_BIT);          // PxSEL0 &= ~BITx

    SERVO_PORT_INIT(SERVO_PWM14_PORT, DIR)  |=   SERVO_PWM14_BIT;           // PxDIR  |=  BITx
    SERVO_PORT_INIT(SERVO_PWM14_PORT, SEL1) |=   SERVO_PWM14_BIT;           // PxSEL1 |=  BITx
    SERVO_PORT_INIT(SERVO_PWM14_PORT, SEL0) &= ~(SERVO_PWM14_BIT);          // PxSEL0 &= ~BITx

}


uint16_t pwm_period = 0;

void timer_init_for_All_PWM_out(uint8_t speed)
{
    GPIO_init_for_All_timer_PWM_out();


    if(speed == MAIN_CLOCK_16MHZ)
        pwm_period = PWM_PERIOD_20ms_SMCLK_16MHz;
    else if(speed == MAIN_CLOCK_8MHZ)
        pwm_period = PWM_PERIOD_20ms_SMCLK_8MHz;
    else if(speed == MAIN_CLOCK_1MHZ)
        pwm_period = PWM_PERIOD_20ms_SMCLK_1MHz;
#if 0
    TA0CCR0 = pwm_period;                   // PWM Period 20msec, 2x CCRx
    TA1CCR0 = pwm_period;                   // PWM Period 20msec, 2x CCRx
    TA2CCR0 = pwm_period;                   // PWM Period 20msec, 2x CCRx
    TA3CCR0 = pwm_period;                   // PWM Period 20msec, 2x CCRx
#endif
    TB0CCR0 = pwm_period;                   // PWM Period 20msec, 6x CCRx
#if 0
    TA0CCTL1 = OUTMOD_7;                         // CCR reset/set like ----___
    TA0CCTL2 = OUTMOD_7;                         // CCR reset/set like ----___

    TA1CCTL1 = OUTMOD_7;                         // CCR reset/set like ----___
    TA1CCTL2 = OUTMOD_7;                         // CCR reset/set like ----___

    TA2CCTL1 = OUTMOD_7;                         // CCR reset/set like ----___
    TA2CCTL2 = OUTMOD_7;                         // CCR reset/set like ----___

    TA3CCTL1 = OUTMOD_7;                         // CCR reset/set like ----___
    TA3CCTL2 = OUTMOD_7;                         // CCR reset/set like ----___
#endif
    TB0CCTL1 = OUTMOD_7;                         // CCR reset/set like ----___
    TB0CCTL2 = OUTMOD_7;                         // CCR reset/set like ----___
    TB0CCTL3 = OUTMOD_7;                         // CCR reset/set like ----___
    TB0CCTL4 = OUTMOD_7;                         // CCR reset/set like ----___
    TB0CCTL5 = OUTMOD_7;                         // CCR reset/set like ----___
    TB0CCTL6 = OUTMOD_7;                         // CCR reset/set like ----___


    TA0CTL = TASSEL_2 + MC_1;                   // SMCLK, up mode
    TA1CTL = TASSEL_2 + MC_1;                   // SMCLK, up mode
    TA2CTL = TASSEL_2 + MC_1;                   // SMCLK, up mode
    TA3CTL = TASSEL_2 + MC_1;                   // SMCLK, up mode
    TB0CTL = TASSEL_2 + MC_1;                   // SMCLK, up mode

#if 0
    TA0CCR1 = PWM_DUTY_2ms_SMCLK_16MHz;
    TA0CCR2 = PWM_DUTY_2ms_SMCLK_16MHz;
    TA1CCR1 = PWM_DUTY_2ms_SMCLK_16MHz;
    TA1CCR2 = PWM_DUTY_2ms_SMCLK_16MHz;
    TA2CCR1 = PWM_DUTY_2ms_SMCLK_16MHz;
    TA2CCR2 = PWM_DUTY_2ms_SMCLK_16MHz;
    TA3CCR1 = PWM_DUTY_2ms_SMCLK_16MHz;
    TA3CCR2 = PWM_DUTY_2ms_SMCLK_16MHz;
#endif
    TB0CCR1 = PWM_DUTY_3ms_SMCLK_16MHz;
    __delay_cycles(8000000);   // 0.5sec
    TB0CCR2 = PWM_DUTY_3ms_SMCLK_16MHz;
    __delay_cycles(8000000);   // 0.5sec
    TB0CCR3 = PWM_DUTY_3ms_SMCLK_16MHz;
    __delay_cycles(8000000);   // 0.5sec
    TB0CCR4 = PWM_DUTY_3ms_SMCLK_16MHz;
    __delay_cycles(8000000);   // 0.5sec
    TB0CCR5 = PWM_DUTY_3ms_SMCLK_16MHz;
    __delay_cycles(8000000);   // 0.5sec
    TB0CCR6 = PWM_DUTY_3ms_SMCLK_16MHz;
    __delay_cycles(8000000);   // 0.5sec

}

// servo_num -
//    0 : Global,
//    1~14 : each motor,
// angle : -180~180 (1ms ~ 2ms)
void servo_angle(uint8_t servo_num, int16_t angle)
{
    int16_t angle_cal = 0;
    uint16_t pwm_duty = 0;
    uint8_t local_servo_num = servo_num+8;

    angle_cal = angle + 180;    // -180~180 --> 0~360
    pwm_duty = PWM_DUTY_1ms_SMCLK_16MHz + (PWM_DUTY_2ms_SMCLK_16MHz-PWM_DUTY_1ms_SMCLK_16MHz)*angle_cal/360;

    switch(local_servo_num)
    {
#if 0
        case 0 :
        {
            ;
        }break;
        case 1 :            // TA0CCR1
        {
            TA0CCR1 = pwm_duty;
        }break;
        case 2 :            // TA0CCR2
        {
            TA0CCR2 = pwm_duty;
        }break;
        case 3 :            // TA1CCR1
        {
            TA1CCR1 = pwm_duty;
        }break;
        case 4 :            // TA1CCR2
        {
            TA1CCR2 = pwm_duty;
        }break;
        case 5 :            // TA2CCR1
        {
            TA2CCR1 = pwm_duty;
        }break;
        case 6 :            // TA2CCR2
        {
            TA2CCR2 = pwm_duty;
        }break;
        case 7 :            // TA3CCR1
        {
            TA3CCR1 = pwm_duty;
        }break;
        case 8 :            // TA3CCR2
        {
            TA3CCR2 = pwm_duty;
        }break;
#endif
        case 9 :            // TB0CCR1
        {
            TB0CCR1 = pwm_duty;
        }break;
        case 10 :            // TB0CCR2
        {
            TB0CCR2 = pwm_duty;
        }break;
        case 11 :            // TB0CCR3
        {
            TB0CCR3 = pwm_duty;
        }break;
        case 12 :            // TB0CCR4
        {
            TB0CCR4 = pwm_duty;
        }break;
        case 13 :            // TB0CCR5
        {
            TB0CCR5 = pwm_duty;
        }break;
        case 14 :            // TB0CCR6
        {
            TB0CCR6 = pwm_duty;
        }break;
        default :
        {
            ;
        }break;
    }

}


#define TIMER0_SMCLK_20ms             PWM_PERIOD_20ms_SMCLK_16MHz

#define CONST_TIMER0_CCR0               TIMER0_SMCLK_20ms
#define CONST_TIMER0_CCR1               TIMER0_SMCLK_20ms
#define CONST_TIMER0_CCR2               TIMER0_SMCLK_20ms

void timerA0_init(void)
{
#if 0
    TA0CTL = TASSEL_2 + MC_2;                   // SMCLK, Cont. mode
    // CCR0 interrupt enabled : 10msec
    TA0CCTL0 = CCIE;
    TA0CCR0 = CONST_TIMER0_CCR0;
    // CCR1 interrupt enabled : 100msec
    TA0CCTL1 = CCIE;
    TA0CCR1 = CONST_TIMER0_CCR1;
    // CCR2 interrupt enabled : 200msec
    TA0CCTL2 = CCIE;
    TA0CCR2 = CONST_TIMER0_CCR2;
#endif
    TA0CCTL0 |= CCIE;                             // TACCR0 interrupt enabled
    TA0CCR0 = CONST_TIMER0_CCR0;
    TA0CTL |= TASSEL__SMCLK | MC__CONTINUOUS;     // ACLK, continuous mode

}


uint8_t button_left = 0;
uint8_t button_right = 0;
uint8_t button_pressed = 0;

int16_t servo_angle_ccr = 0;    //
//int16_t servo_angle_deg[5] = {0,0,0,0,0};    // -180~180

uint8_t time_tick = 0;
uint8_t servo_status = 0;

void main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer

    // Clear port lock
    PM5CTL0 &= ~LOCKLPM5;

    set_main_clock(MAIN_CLOCK_16MHZ);

    timer_init_for_All_PWM_out(MAIN_CLOCK_16MHZ);

    P1DIR |= BIT0;      // LED port
    P1OUT |= BIT0;
    timerA0_init();

    //GPIO_SW1_SW2_Interrupt_Set();

    SW1_PORT_IFG &= ~SW1_BIT;                         // Clear PORT IFG for SW1
    SW2_PORT_IFG &= ~SW2_BIT;                         // Clear PORT IFG for SW1
    __bis_SR_register(LPM0_bits + GIE);

    while(1)
    {
        ///////////////////////////////////////////////////
        // check switch buttons are pressed : SW1
        ///////////////////////////////////////////////////
        if(switch_status & BIT0) // switch1 function : action change
        {
            switch_status &= ~BIT0;

            button_pressed = 1;
            if(button_right++ > 5)
            {
                button_right = 0;
            }
        }

        /////////////////////////////////////////////////
        // check switch buttons are pressed : SW2
        /////////////////////////////////////////////////
        if(switch_status & BIT1)    // switch2 function : brightness or delay time change
        {
            switch_status &= ~BIT1;

            button_pressed = 1;
            if(button_left++ > 9)
            {
                button_left = 0;
            }
        }

#if 1
        if(time_tick == 1)
        {
            time_tick = 0;
            servo_status++;

            if(servo_status == 1)
            {
                servo_angle_ccr = PWM_DUTY_1ms_SMCLK_16MHz;
                TB0CCR1 = servo_angle_ccr;
            }
            else if(servo_status == 2)
            {
                servo_angle_ccr = PWM_DUTY_1ms_SMCLK_16MHz;
                TB0CCR2 = servo_angle_ccr;
            }
            else if(servo_status == 3)
            {
                servo_angle_ccr = PWM_DUTY_1ms_SMCLK_16MHz;
                TB0CCR3 = servo_angle_ccr;
            }
            else if(servo_status == 4)
            {
                servo_angle_ccr = PWM_DUTY_1ms_SMCLK_16MHz;
                TB0CCR4 = servo_angle_ccr;
            }
            else if(servo_status == 5)
            {
                servo_angle_ccr = PWM_DUTY_1ms_SMCLK_16MHz;
                TB0CCR5 = servo_angle_ccr;

                // end up the status
                TA0CCTL0 &= (~CCIE);                             // TACCR0 interrupt disabled
                P1OUT ^= BIT0;
                __delay_cycles(8000000);
                P1OUT ^= BIT0;
                __delay_cycles(8000000);
                P1OUT ^= BIT0;
                __delay_cycles(8000000);
                P1OUT ^= BIT0;
                __delay_cycles(8000000);
                P1OUT ^= BIT0;
                __delay_cycles(8000000);
                P1OUT ^= BIT0;
                __delay_cycles(8000000);
                P1OUT ^= BIT0;
                __delay_cycles(8000000);
                P1OUT ^= BIT0;
                __delay_cycles(8000000);
                P1OUT &= ~BIT0;


                TA0CTL = TASSEL_2 + MC_0;                   // SMCLK, stop mode
                TA1CTL = TASSEL_2 + MC_0;                   // SMCLK, stop mode
                TA2CTL = TASSEL_2 + MC_0;                   // SMCLK, stop mode
                TA3CTL = TASSEL_2 + MC_0;                   // SMCLK, stop mode
                TB0CTL = TASSEL_2 + MC_0;                   // SMCLK, stop mode
                __delay_cycles(16000000);

            }

        }
#else
        __delay_cycles(160000000);

        servo_angle_ccr = PWM_DUTY_1ms_SMCLK_16MHz;
        TB0CCR1 = servo_angle_ccr;
        __delay_cycles(160000000);

        servo_angle_ccr = PWM_DUTY_1ms_SMCLK_16MHz;
        TB0CCR2 = servo_angle_ccr;
        __delay_cycles(160000000);

        servo_angle_ccr = PWM_DUTY_1ms_SMCLK_16MHz;
        TB0CCR3 = servo_angle_ccr;
        __delay_cycles(160000000);

        servo_angle_ccr = PWM_DUTY_1ms_SMCLK_16MHz;
        TB0CCR4 = servo_angle_ccr;
        __delay_cycles(160000000);

        servo_angle_ccr = PWM_DUTY_1ms_SMCLK_16MHz;
        TB0CCR5 = servo_angle_ccr;
        __delay_cycles(160000000);
#endif

    }
}


#define ONE_SEC         50
#define TEN_SEC         (50*10)
#define ONE_MIN         (50*60)
#define ONE_HOUR        180000//(50*60*60)
#define EIGHT_HOUR      1440000//(50*60*60*8)
#define TWENTY_TWO_H    (50L*60*60*22)
#define ONE_DAY         (50L*60*60*24)//4320000//(50*60*60*24)
#define TWO_DAY         (50L*60*60*20*2)
uint8_t s_timer = 0;
uint32_t l_timer = 0;
// 20ms continuous mode timer
#pragma vector=TIMER0_A0_VECTOR
__interrupt void ISR_Timer0_A0(void)
{
    TA0CCR0 += CONST_TIMER0_CCR0;               // Add Offset to CCR1

    s_timer++;
    l_timer++;
    if(s_timer == 50)       // 1sec
    {
        s_timer = 0;
        P1OUT ^= BIT0;
    }

    //if(l_timer == TEN_SEC)      // 10 sec
    //if(l_timer == ONE_MIN)      // 1 min
    //if(l_timer == ONE_DAY)    // 1day
    //if(l_timer == TWENTY_TWO_H)    // 1day
    if(l_timer == TWO_DAY)    // 2 days
    {
        l_timer = 0;
        time_tick = 1;
    }
    __bic_SR_register_on_exit(LPM0_bits);
}


// Port 4 interrupt service routine
#pragma vector=PORT4_VECTOR
__interrupt void Port_4(void)
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

}

// Port 2 interrupt service routine
#pragma vector=PORT2_VECTOR
__interrupt void Port_2(void)
{
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



