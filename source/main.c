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
       
    while (1) {
	add = PINA + PINB +PINC;
	aC = PINA - PINC;
	cA = PINC - PINA;

	unsigned char output = 0x00;
	    
	if(add > 0b10001100){
		output = 1;	
	}
		
	if ( (aC> 0b1010000) || (cA > 0b1010000 ) ){
		output = output1;
	}	
	if(tmpB==0x02){
		cntavail = cntavail + 1;
	}
	if(tmpC == 0x04){
		cntavail = cntavail + 1;
	}
	if(tmpD == 0x08){
		cntavail = cntavail + 1;
	}
	if(cntavail == 0x04){
		cntavail =  0b10000100;	
	}
	PORTC = cntavail;
    }
    
    return 0;
}


