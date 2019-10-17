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
	DDRC = 0xFF; PORTC = 0x00; //port c = output 
	unsigned char a = 0x00;
	unsigned char output = 0x00;
       while (1) {
		
		a = (~PINA) & 0x0F; //a = input a
		
	       	
	    if(a == 0){
			output=0x40;
		}
		if(a == 0x01 || a == 0x02) {
			output = 0b01100000 ;
		}
		else if(a == 0x03 || a == 0x04) {
			output = 0b01110000;
		}
		else if(a == 0x05 || a == 0x06) {
			output = 0b00111000;
		}
		else if(a == 0x07 || a == 0x08 || a == 0x09) {
			output = 0b00111100;
		}
		else if(a == 0x0A || a == 0x0B || a == 0x0C) {
			output = 0b00111110;
		}
		else if(a == 0x0D || a == 0x0E || a == 0x0F) {
			output = 0b00111111;
		}
		
		PORTC = output;
    }
    
    return 0;
}

