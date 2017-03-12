/*  Author: Christopher Ranc
 *  Name  : servoControl.c
 *
 *  Main program to initialize timers, digital i/o pins, setup and turn on ISR 
 *  interrupts.  
 */

#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "analogIn.h"

/*  Tick values for max and min servo positions found using a prescale value of
 *  1024.  
 */
/*  Max position at a duty cycle of 2.5 ms */
#define SERVOMAX 39
/*  Min position at a duty cycle of about .5 ms */
#define SERVOMIN 7

/*  Claw servo tick values were found using a prescale of 256*/
/*  Close at a duty cycle of about 2 ms*/
#define CLAW_SERVOMAX 128
/*  Open  at a duty cycle of about 1.3 ms*/
#define CLAW_SERVOMIN 84

/* Values used to determine duty cycle factor*/ 
#define POSITION_VAL_MAX 5000.0
#define RATE_MAX 2.0

/*  Determines new duty cycle to be between .5 and 2.5ms */
#define servoConvert(x) (((x / POSITION_VAL_MAX) * (SERVOMAX - SERVOMIN)) + SERVOMIN)
/*  Determines rate to increase or decrease duty cycle factor */
#define servoRatePositiveConvert() (RATE_MAX * (ADC - 520.0) / 504.0)
#define servoRateNegativeConvert() (RATE_MAX * (ADC - 510.0) / 510.0)

uint8_t analogPin;
int servoT0, servoT1, servoT2;
float currentPositionT0, currentPositionT1;

/*  Interrupt for ending first timer that controls the input for the first
 *  2 servos
 */
ISR(TIMER0_OVF_vect){
    /*  Set duty cycle output for PD6 or pin 6 for Arduino Uno*/
    OCR0A = servoT0; 
    /*  Set duty cycle output for PD5 or pin 5 for Arduino Uno */
    OCR0B = servoT1;
}

/*  Interrupts for second timer that controls the input for the 3rd servo */
ISR(TIMER1_OVF_vect){
    /*  Set duty cycle output for PB1 or pin 9 for Arduino Uno */
    OCR1A = servoT2;
}

ISR(ADC_vect){
    switch(analogPin){
        
        /*  Set postion of servo attached to PD6 */
        case 0:

            /*  If ADC is above 520 then currentPositionT0 will be increased */
            if (ADC > 520){
                currentPositionT0 += servoRatePositiveConvert();
                if (currentPositionT0 > POSITION_VAL_MAX)
                    currentPositionT0 = POSITION_VAL_MAX;
            }
            /*  If ADC is below 510 then currentPositionT0 will be decremented */
            else if (ADC < 510){
                currentPositionT0 += servoRateNegativeConvert();
                if (currentPositionT0 < 0)
                    currentPositionT0 = 0;
            }
            
            servoT0 = servoConvert(currentPositionT0); 
            analogPin++;
            break;

        /*  Set postion of servo attached to PD5 */
        case 1:
            /*  If ADC is above 520 then currentPositionT1 will be increased */
            if (ADC > 520){
                currentPositionT1 += servoRatePositiveConvert();
                if (currentPositionT1 > POSITION_VAL_MAX)
                    currentPositionT1 = POSITION_VAL_MAX;
            }
            /*  If ADC is below 51 then currentPositionT1 will be decremented */
            else if (ADC < 510){
                currentPositionT1 += servoRateNegativeConvert();
                if (currentPositionT1 < 0)
                    currentPositionT1 = 0;
            }
 
            servoT1 = servoConvert(currentPositionT1);
            analogPin = 0;
            break;
        default :
            analogPin  = 0;
            break;
    }

    /*  Change to next analog pin */
    changeInputPin(analogPin);

    /*  Start Next Conversion */
    startConv();
}

int main (){

    /*  Initial Values */
    analogPin = 0;
    currentPositionT0 = POSITION_VAL_MAX / 2;
    currentPositionT1 = currentPositionT0;
    servoT2 = CLAW_SERVOMAX;
    
    /*  Sets pin PD6 and PD5 for output. Respectively known as pins 6 and 5 
     *  for the Arduino Uno.
     */
    DDRD |= _BV(DDD6) | _BV(DDD5);

    /*  Sets pin PB1 for output.  Also known as pin 9 for the Arduino Uno */
    DDRB |= _BV(DDB1);

    /*  Configure PWM outputs to clear on a compare match and set wave form to 
     *  operate as Fast PWM for TCNT0 and TCNT1 counters.
     */
    TCCR0A |= _BV(COM0A1) | _BV(COM0B1) | _BV(WGM00) | _BV(WGM01);
*
    TCCR1A |= _BV(COM1A1) | _BV(WGM10);
    /*  For the TCNT1 counter the second configuration register has to have the
     *  the WGM12 bit enabled to operate at Fast PWM mode wanted.  */
    TCCR1B |= _BV(WGM12);

    /*  Enable timer interrupt for timer overflow occurences for the TCNT0 and 
     *  TCNT1 counters.  
     */    
    TIMSK0 |= _BV(TOIE0);                                                           
    TIMSK1 |= _BV(TOIE1);

    /*  Start ADC */
    setupADC();
    /*  Enable interrupts*/
    sei();

    /*  Set prescaler for TCNT0 to increment it every 1024 cycles. */
    TCCR0B |= _BV(CS00) | _BV(CS02);
    /*  Set prescaler for TCNT1 to increment it every 256 cycles.  This is to 
     *  prevent the ISR's of both counters from interfering.  This would make
     *  the servos jitter sporadically at certain positions otherwise.
     */
    TCCR1B |= _BV(CS12);

    while (1){

        /*  If statement to control claw servo via button*/
        if ((PIND & _BV(PIND2)) == _BV(PIND2)){
            if (servoT2 == CLAW_SERVOMAX)
                servoT2 = CLAW_SERVOMIN;
            else
               servoT2 = CLAW_SERVOMAX;
            
            /*  While loop to to prevent change by holding down button */
            while((PIND & _BV(PIND2)) == _BV(PIND2)){
            }
            /*  Delay to prevent button input glitch when released */
            _delay_ms(150);
        }
    }

    return 0;

}
