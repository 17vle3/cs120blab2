/*	Author: vle018
 *  Partner(s) Name: Mari Hayashi
 *	Lab Section:

 *	Exercise Description: A car's passenger-seat weight sensor outputs a 9-bit value (ranging from 0 to 511) 
 * and connects to input PD7..PD0PB0 on the microcontroller. 
 * If the weight is equal to or above 70 pounds,
 *  the airbag should be enabled by setting PB1 to 1. 
 * If the weight is above 5 but below 70, the airbag 
 * should be disabled and an "Airbag disabled" icon should light by 
 * setting PB2 to 1. (Neither B1 nor B2 should be set if the weight 
 * is 5 or less, as there is no passenger).
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

int main(void) {
	DDRD = 0x00; PORTD = 0xFF; //port a = inputs 
	DDRB = 0xFE; PORTB = 0x01; //port b = output 

	unsigned char aIntoB =0x00;
	unsigned char aIntoC =0x00;

	while (1) {
			aIntoC = (PINA & 0b00001111) << 4;
			aIntoB = (PINA & 0b11110000) >> 4;
			
			PORTC = aIntoC;
			PORTB = aIntoB;
	}
    
    return 0;
}

