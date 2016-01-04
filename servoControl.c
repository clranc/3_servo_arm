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

/*  Define to convert digital voltage value to be within the amount of clock 
 *  ticks for the duty cycle time to be within 0 and 3 ms
 */
#define servoConvert(x) ((x / 1024.0) * 39)


#define SERVOMAX 39
#define SERVOMIN 0

/*  Close */
#define CLAW_SERVOMAX 32
/*  Open */
#define CLAW_SERVOMIN 21

uint8_t analogPin;
int servoT0, servoT1, servoT2;

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
        case 0:
            servoT0 = servoConvert(ADC);
            analogPin++;
            break;
        case 1:
            servoT1 = servoConvert(ADC);
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

    analogPin = 0;
    
    /*  Sets pin PD6 and PD5 for output. Respectively known as pins 6 and 5 
     *  for the Arduino Uno.
     */
    DDRD |= _BV(DDD6) | _BV(DDD5);

    /*  Sets pin PB3 for output.  Also known as pin 11 for the Arduino Uno */
    DDRB |= _BV(DDB1);

    /*  Configure PWM outputs to clear on a compare match and set wave form to 
     *  operate as Fast PWM for TCNT0 and TCNT1 counters.
     */
    TCCR0A |= _BV(COM0A1) | _BV(COM0B1) | _BV(WGM00) | _BV(WGM01);

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

    /*  Set prescalers to increment the counters every 1024 cycles. */
    TCCR0B |= _BV(CS00) | _BV(CS02);
    TCCR1B |= _BV(CS10) | _BV(CS12);

    /*  Set servos to starting position */
    servoT0 = SERVOMAX / 2;
    servoT1 = servoT0;
    servoT2 = CLAW_SERVOMAX;

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
