/*  Author: Christopher Ranc
 *  
 *  Main program to initialize timer, digital i/o pins, setup and turn on ISR 
 *  interrupts.  
 */

#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "analogIn.h"

/*  Define to convert digital voltage value to be within the amount of clock 
 *  ticks for the duty cycle time to be within 0 and 3 ms
 */
#define servoConvert(x) ((x / 1024.0) * 46)

uint8_t analogPin;
int servoT0, servoT1, servoT2;


ISR(TIMER0_OVF_vect){
    /*  Set duty cycle output for PD6 or pin 6 for Arduino Uno*/
    OCR0A = servoT0; 
    /*  Set duty cycle output for PD5 or pin 5 for Arduino Uno */
    OCR0B = servoT1;
}

ISR(TIMER2_OVF_vect){
    /*  Set duty cycle output for PB3 or pin 11 for Arduino Uno */
    OCR2A = servoT2;
}

ISR(ADC_vect){

    /*  Convert the current ADC value */
    switch(analogPin){
        case 0:
            servoT0 = servoConvert(ADC);
            analogPin++;
            break;
        case 1:
            servoT1 = servoConvert(ADC);
            analogPin++;
            break;
        case 2:
            servoT2 = servoConvert(ADC);
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
     *  for the Arduino Uno
     */
    DDRD |= _BV(DDD6) | _BV(DDD5);

    /*  Sets pin PB3 for output.  Also known as pin 11 for the Arduino Uno */
    DDRB |= _BV(DDB3);

    /*  Configure PWM outputs to clear on a compare match and set wave form to 
     *  operate as Fast PWM.  
     */
    TCCR0A |= _BV(COM0A1) | _BV(COM0B1) | _BV(WGM00) | _BV(WGM01);                                
    TCCR2A |= _BV(COM2A1) | _BV(WGM20) | _BV(WGM21);  

    /*  Enable timer interrupt for timer overflow occurences for the TCNT0 and 
     *  TCNT2 8-bit counters.  
     */    
    TIMSK0 |= _BV(TOIE0);                                                           
    TIMSK2 |= _BV(TOIE2);

    setupADC();

    sei();

    TCCR0B |= _BV(CS00) | _BV(CS02);
    TCCR2B |= _BV(CS20) | _BV(CS21) | _BV(CS22);

    while (1){
    }

    return 0;

}
