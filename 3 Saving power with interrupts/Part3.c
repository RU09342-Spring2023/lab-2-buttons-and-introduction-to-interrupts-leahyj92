#define ARMED_STATE 0
#define WARNING_STATE 1
#define ALERT_STATE 2

#include <msp430.h>

unsigned int detected = 0;
unsigned int count = 0;
char state = ARMED_STATE;

// initialization
int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;               // Stop watchdog timer

    P6DIR |= BIT6;   //
    P6OUT &= ~BIT6;  //

    P1DIR |= BIT0;   //
    P1OUT &= ~BIT0;  //

    P2REN |= BIT3;   //
    P2OUT |= BIT3;   //
    P2IES &= ~BIT3;  //
    P2IE |= BIT3;    //
    P2IFG &= ~BIT3;  //

    P4REN |= BIT1;   //
    P4OUT |= BIT1;   //
    P4IES &= ~BIT1;  //
    P4IE |= BIT1;    //
    P4IFG &= ~BIT1;  //

    PM5CTL0 &= ~LOCKLPM5;

    __bis_SR_register(GIE);                 // Enter LPM3 w/interrupt

// Do something in the ARMED state
// If something happens, you can move into the WARNING_STATE
    while(1)
    {
        if(!(P2IN & BIT3))
        {
            P1OUT &= ~BIT0;
            count = 0;
        }
      switch (state) {
        case ARMED_STATE:
        {
          P6OUT ^= BIT6;
          P1OUT &= ~BIT0;
          __delay_cycles(1000000);
          if(!(P4IN &= BIT1))
          {
              state = WARNING_STATE;  //set state to warning state
              P6OUT &= ~BIT6;
          }
          break;
        }
        case WARNING_STATE:
        {
          P1OUT ^= BIT0;
          P6OUT &= ~BIT6;
          __delay_cycles(500000);
          count++;
          if(count >= 19)
          {
              state = ALERT_STATE;  //set state to alert state
          } else if((P4IN &= BIT1))
          {
              state = ARMED_STATE;  //set state to armed state
          }
          break;
        }
        case ALERT_STATE:
        {
            P1OUT |= BIT0;
            P6OUT &= ~BIT6;

            break;
        }
      }
    }
}

#pragma vector=PORT4_VECTOR
__interrupt void Port_4(void)
{
    P4IFG &= ~BIT1;
    state = ALERT_STATE;
}

#pragma vector=PORT2_VECTOR
__interrupt void Port_2(void)
{
    P2IFG &= ~BIT3;
    state = ARMED_STATE;

}
