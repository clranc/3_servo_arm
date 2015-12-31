/*  Author: Christopher Ranc
 *   
 *  Functions for initializing Analog to 
 *  Digital Conversion for analog input pins.
 *
 */
#include <stdio.h>
#include <stdint.h>
#include <avr/io.h>

/*  Enables ADC, enables ADC interrupts, and sets ADC prescaler to 128.
 *  This also presets the initial analog input to pin ADC0/PC0 or A0 on an 
 *  Arduino uno.  
 */
void setupADC();

/*  Sets ADSC pin in the ADCSRA register to begin analog to digital conversion.
 *  This is a necessary function due to the bit being cleared after a succesful
 *  conversion. 
 */
void startConv();

/*  Changes the analog pin being used by changing the ADMUX register.  Only 
 *  accepts integer values ranging from 0 - 5 as these are the amount of 
 *  analog inputs available for the atmega328p.  
 */
void changeInputPin(uint8_t pin); 
