/*
 * i2c.c
 */

#include "i2c.h"

#include <msp430.h>
#include <stdint.h>

#if defined (__MSP430FR2676__)
#define SDA         BIT2                                                        // i2c sda pin
#define SCL         BIT3                                                        // i2c scl pin
#elif defined (__MSP430FR2522__)
#define SDA         BIT2                                                        // i2c sda pin
#define SCL         BIT3                                                        // i2c scl pin
#elif defined (__MSP430G2553__)
#define SDA         BIT7                                                        // i2c sda pin
#define SCL         BIT6                                                        // i2c scl pin
#else
 @@ error
#endif

unsigned char *PTxData;                                                       // Pointer to TX data
unsigned char TxByteCtr;                                                      // number of bytes to TX

void i2c_init(void)
{
#if defined (__MSP430FR2676__)
    //P1SEL0   |= SCL + SDA;                                              // Assign I2C pins to USCI_B0

    UCB0CTLW0 |= UCSWRST;                                               // Enable SW reset
    UCB0CTLW0 |= UCMST + UCMODE_3 + UCSYNC + UCSSEL_2;                  // I2C Master, synchronous mode
                                                                        // Use SMCLK(=2MHz), keep SW reset
    UCB0BRW   = 10;                                                     // fSCL = SMCLK/10 = ~200kHz with SMCLK = 2MHz
    UCB0CTLW0 &= ~UCSWRST;                                               // Clear SW reset, resume operation
    //UCB0IE    |= UCTXIE0;                                               // Enable TX interrupt
    UCB0IE |= UCTXIE0 | UCNACKIE;                     // transmit and NACK interrupt enable

    __delay_cycles(1000);                             // Delay between transmissions

#elif defined (__MSP430FR2522__)
    //P1SEL0   |= SCL + SDA;                                              // Assign I2C pins to USCI_B0

    UCB0CTLW0 |= UCSWRST;                                               // Enable SW reset
    UCB0CTLW0 |= UCMST + UCMODE_3 + UCSYNC + UCSSEL_2;                  // I2C Master, synchronous mode
                                                                        // Use SMCLK(=2MHz), keep SW reset
    UCB0BRW   = 10;                                                     // fSCL = SMCLK/10 = ~200kHz with SMCLK = 2MHz
    UCB0CTLW0 &= ~UCSWRST;                                               // Clear SW reset, resume operation
    //UCB0IE    |= UCTXIE0;                                               // Enable TX interrupt
    UCB0IE |= UCTXIE0 | UCNACKIE;                     // transmit and NACK interrupt enable

    __delay_cycles(1000);                             // Delay between transmissions

#elif defined (__MSP430G2553__)

    P1SEL    |= SCL + SDA;                                              // Assign I2C pins to USCI_B0
    P1SEL2   |= SCL + SDA;                                              // Assign I2C pins to USCI_B0
    UCB0CTL1 |= UCSWRST;                                                // Enable SW reset
    UCB0CTL0  = UCMST + UCMODE_3 + UCSYNC;                              // I2C Master, synchronous mode
    UCB0CTL1  = UCSSEL_2 + UCSWRST;                                     // Use SMCLK, keep SW reset
    UCB0BR0   = 40;                                                     // fSCL = SMCLK/10 = ~100kHz with SMCLK = 1MHz
    UCB0BR1   = 0;
    UCB0CTL1 &= ~UCSWRST;                                               // Clear SW reset, resume operation
    IE2      |= UCB0TXIE;                                               // Enable TX interrupt
#else
 @@ error
#endif
} // end i2c_init

void i2c_write(unsigned char slave_address, unsigned char *DataBuffer, unsigned char ByteCtr)
{
    UCB0I2CSA = slave_address;

    PTxData = DataBuffer;
    TxByteCtr = ByteCtr;

#if defined (__MSP430FR2676__)
    while (UCB0CTLW0 & UCTXSTP);                                         // Ensure stop condition got sent
    UCB0CTLW0 |= UCTR | UCTXSTT;                                         // I2C TX, start condition
    while (UCB0CTLW0 & UCTXSTP);                                         // Ensure stop condition got sent
    __bis_SR_register(LPM0_bits + GIE);                                    // Enter LPM0 w/ interrupts
                                                                        // Remain in LPM0 until all data is TX'd
#elif defined (__MSP430FR2522__)
    while (UCB0CTLW0 & UCTXSTP);                                         // Ensure stop condition got sent
    UCB0CTLW0 |= UCTR | UCTXSTT;                                         // I2C TX, start condition
    while (UCB0CTLW0 & UCTXSTP);                                         // Ensure stop condition got sent
    __bis_SR_register(LPM0_bits + GIE);                                    // Enter LPM0 w/ interrupts
                                                                        // Remain in LPM0 until all data is TX'd
#elif defined (__MSP430G2553__)
    while (UCB0CTL1 & UCTXSTP);                                         // Ensure stop condition got sent
    UCB0CTL1 |= UCTR + UCTXSTT;                                         // I2C TX, start condition
    while (UCB0CTL1 & UCTXSTP);                                         // Ensure stop condition got sent
    __bis_SR_register(CPUOFF + GIE);                                    // Enter LPM0 w/ interrupts
                                                                        // Remain in LPM0 until all data is TX'd
#else
 @@ error
#endif

}

/* =============================================================================
 * The USCIAB0TX_ISR is structured such that it can be used to transmit any
 * number of bytes by pre-loading TXByteCtr with the byte count. Also, TXData
 * points to the next byte to transmit.
 * ============================================================================= */
#if defined (__MSP430FR2522__) || defined (__MSP430FR2676__)
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = USCI_B0_VECTOR
__interrupt void USCIB0_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(USCI_B0_VECTOR))) USCIB0_ISR (void)
#else
#error Compiler not supported!
#endif
{
    switch(__even_in_range(UCB0IV,USCI_I2C_UCBIT9IFG))
    {
        case USCI_NONE: break;                        // Vector 0: No interrupts break;
        case USCI_I2C_UCALIFG: break;
        case USCI_I2C_UCNACKIFG:
            UCB0CTL1 |= UCTXSTT;                      //resend start if NACK
        break;                                      // Vector 4: NACKIFG break;
        case USCI_I2C_UCSTTIFG: break;                // Vector 6: STTIFG break;
        case USCI_I2C_UCSTPIFG: break;                // Vector 8: STPIFG break;
        case USCI_I2C_UCRXIFG3: break;                // Vector 10: RXIFG3 break;
        case USCI_I2C_UCTXIFG3: break;                // Vector 14: TXIFG3 break;
        case USCI_I2C_UCRXIFG2: break;                // Vector 16: RXIFG2 break;
        case USCI_I2C_UCTXIFG2: break;                // Vector 18: TXIFG2 break;
        case USCI_I2C_UCRXIFG1: break;                // Vector 20: RXIFG1 break;
        case USCI_I2C_UCTXIFG1: break;                // Vector 22: TXIFG1 break;
        case USCI_I2C_UCRXIFG0: break;                // Vector 24: RXIFG0 break;
        case USCI_I2C_UCTXIFG0:
            if (TxByteCtr)                                // Check TX byte counter
            {
                UCB0TXBUF = *PTxData++;            // Load TX buffer
                TxByteCtr--;                              // Decrement TX byte counter
            }
            else
            {
                UCB0CTLW0 |= UCTXSTP;                     // I2C stop condition
                UCB0IFG &= ~UCTXIFG;                      // Clear USCI_B0 TX int flag
                __bic_SR_register_on_exit(LPM0_bits);     // Exit LPM0
            }
        break;                                      // Vector 26: TXIFG0 break;
        case USCI_I2C_UCBCNTIFG: break;               // Vector 28: BCNTIFG
        case USCI_I2C_UCCLTOIFG: break;               // Vector 30: clock low timeout
        case USCI_I2C_UCBIT9IFG: break;               // Vector 32: 9th bit
        default: break;
    }

}
#elif defined (__MSP430G2553__)
#pragma vector = USCIAB0TX_VECTOR
__interrupt void USCIAB0TX_ISR(void) {
    if (TxByteCtr) {                                                    // Check TX byte counter
        UCB0TXBUF = *PTxData++;                                         // Load TX buffer
        TxByteCtr--;                                                    // Decrement TX byte counter
    } else {
        UCB0CTL1 |= UCTXSTP;                                            // I2C stop condition
        IFG2 &= ~UCB0TXIFG;                                             // Clear USCI_B0 TX int flag
        __bic_SR_register_on_exit(CPUOFF);                              // Exit LPM0
  }
}

#else
 @@ error
#endif








