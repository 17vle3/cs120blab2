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
	unsigned char a =0x00;
	unsigned char a4 = 0x00;
	unsigned char a5 = 0x00;
	unsigned char a6 = 0x00;

       while (1) {
		a4 = (PINA & 0x10) >> 4;
		a5 = (PINA & 0x20) >>5;
		a6 = (PINA & 0x30) >>6;
		a = PINA ; //a = input a
		unsigned char fourLess = 0x00;
	       	unsigned char output = 0x00;
		if(a == 0x01 || a == 0x02) {
			output = 0x20;
			fourLess = 0x01;
		}
		else if(a == 0x03 || a == 0x04) {
			output = 0x30;
			fourLess = 0x01;
		}
		else if(a == 0x05 || a == 0x06) {
			output = 0x38;
		}
		else if(a >= 0x07 && a <= 0x09) {
			output = 0x3C;
		}
		else if(a >= 0x0A && a <= 0x0C) {
			output = 0x3E;
		}
		else if(a >= 0x0D && a <= 0x0F) {
			output = 0x3F;
		}
		
		if(fourLess){
			output = output | 0x40;
		}	

		if( a4 && a5 && !a6){
			output = output | 0x80;
		}

		PORTC = output;
    }
    
    return 0;
}

