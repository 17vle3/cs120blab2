/*	Author: vle018
 *  Partner(s) Name: Mari Hayashi
 *	Lab Section:

 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

int main(void) {
	DDRA = 0x00; PORTA = 0xFF; //port a = inputs
	DDRB = 0xFF; PORTB = 0x00; //port b = output 
	DDRC = 0xFF; PORTC = 0x00; //port c = output 
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

