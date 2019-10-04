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
    DDRB = 0x00; PORTB = 0xFF; //port b = outputs
    DDRC = 0x00; PORTC = 0xFF;
    DDRD = 0xFF; PORTD = 0x00;

    unsigned char add = 0x00;
    unsigned char aC = 0x00;
    unsigned char cA = 0x00;
    unsigned char aTemp = 0x00;
    unsigned char bTemp = 0x00;
    unsigned char cTemp = 0x00;
    while (1) {
	aTemp = PINA;
	bTemp = PINB;
	cTemp = PINC;
	add = aTemp + bTemp + cTemp;
	aC = PINA - PINC;
	cA = PINC - PINA;

	unsigned char output = 0x00;
	    
	if(add > 0x8C ){
		output = 0x01;	
	}
		
	if ( (aC> 0b1010000) || (cA > 0b1010000 ) ){
		output = output| 0x02;
	}	
	PORTD = output;
    }
    
    return 0;
}


