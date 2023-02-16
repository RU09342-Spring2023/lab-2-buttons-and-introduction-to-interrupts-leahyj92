#define ARMED_STATE 0
#define WARNING_STATE 1
#define ALERT_STATE 2

#include <msp430.h>

unsigned int count = 0;
char state = ARMED_STATE;

// initialization
int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;               // Stop watchdog timer

    P6DIR |= BIT6;   // Initialize Green LED
    P6OUT &= ~BIT6;

    P1DIR |= BIT0;   // Initialize Red LED
    P1OUT &= ~BIT0;

    P2REN |= BIT3;   // Initialize Reset Button
    P2OUT |= BIT3;
    P2IES &= ~BIT3;
    P2IE |= BIT3;
    P2IFG &= ~BIT3;

    P4REN |= BIT1;   // Initialize Starting Button
    P4OUT |= BIT1;
    P4IES &= ~BIT1;
    P4IE |= BIT1;
    P4IFG &= ~BIT1;

    PM5CTL0 &= ~LOCKLPM5;

    __bis_SR_register(GIE);                 // Enter LPM3 w/interrupt


    while(1)
    {
        if(!(P2IN & BIT3)) //When button 2.3 is pressed
        {
            P1OUT &= ~BIT0; //Turns off Red LED
            count = 0; //Reset count variable
        }
      switch (state) {
        case ARMED_STATE:
        {
          P6OUT ^= BIT6; //Blinks Green LED
          P1OUT &= ~BIT0; //Keeps Red LED off
          __delay_cycles(1000000); //Blinks Green LED every second
          if(!(P4IN &= BIT1)) //When button 4.1 is pressed
          {
              state = WARNING_STATE;  //set state to warning state
              P6OUT &= ~BIT6; //Turns off Green LED
          }
          break;
        }
        case WARNING_STATE:
        {
          P1OUT ^= BIT0; //Blinks Red LED
          P6OUT &= ~BIT6; //Turns off Green LED
          __delay_cycles(500000); //Blinks every 0.5 seconds
          count++; //Adds to counter variable
          if(count >= 19) //When Red LED blinks 20 times
          {
              state = ALERT_STATE;  //set state to alert state
          } else if((P4IN &= BIT1)) //When Button 4.1 is let go
          {
              state = ARMED_STATE;  //set state to armed state
          }
          break;
        }
        case ALERT_STATE:
        {
            P1OUT |= BIT0; //Keeps Red LED on
            P6OUT &= ~BIT6; //Keeps Green LED off

            break;
        }
      }
    }
}

#pragma vector=PORT4_VECTOR
__interrupt void Port_4(void)
{
    P4IFG &= ~BIT1;
    state = ALERT_STATE; //Sets State to Alert State
}

#pragma vector=PORT2_VECTOR
__interrupt void Port_2(void)
{
    P2IFG &= ~BIT3;
    state = ARMED_STATE; //Set State to Armed State

}
