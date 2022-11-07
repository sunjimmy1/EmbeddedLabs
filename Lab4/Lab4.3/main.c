#include <msp430.h>
void main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
    PM5CTL0 &= ~LOCKLPM5;
    P1DIR |= BIT0;               // P1.0  output
    //PWM Generator
    TB0CCR0 = 8191;
    TB0CCTL1 = OUTMOD_7;
    TB0CCR1 = 1638;
    TB0CTL = TBSSEL_1 + MC_1 + TBCLR;
    TB0CCTL0 |= CCIE;
    TB0CCTL1 |= CCIE;
    TB0CCTL0 &=~CCIFG;
    TB0CCTL1 &=~CCIFG;
    _enable_interrupts();
}
#pragma vector = TIMER0_B0_VECTOR       //define the interrupt service vector
__interrupt void ISR_TB0_CCR0 (void)    // interrupt service routine
    {
    P1OUT |=BIT0;
    TB0CCTL0 &=~CCIFG;
    }
#pragma vector = TIMER0_B1_VECTOR
__interrupt void ISR_TB0_CCR1  (void) {
    P1OUT &=~BIT0;
    TB0CCTL1 &=~CCIFG;
    }
