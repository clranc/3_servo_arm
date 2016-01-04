/*  Author: Christopher Ranc
 * 
 *  C module for analogIn prototype
 */
#include <stdint.h>
#include <avr/io.h>
#include "analogIn.h"

 
void startConv(){

        /* Sets ADSC bit to begin conversion */
        ADCSRA |= _BV(ADSC);   
}

void setupADC(){
    
    /*  Enable ADC, enable ADC interrupts, and sets ADC prescaler to 128 */
    ADCSRA |= _BV(ADEN) | _BV(ADIE) | _BV(ADPS0) | _BV(ADPS1) | _BV(ADPS2) ; 
    
    /*  Sets reference voltage to Vref, In the case of an Arduino uno it's 5V.
     *  The default ADC input pin is set as 0.   
     */
    ADMUX |= _BV(REFS0); 
    
    /* Runs the startConv function to begin initial analog conversion*/
    startConv();
}

void changeInputPin(uint8_t pin){
    
    /* Clears mux selection bits and selects new pin*/  
    ADMUX = (ADMUX & ~((1 << 0) | (1 << 1) | (1 << 2) | (1 << 3))) | pin;
}
