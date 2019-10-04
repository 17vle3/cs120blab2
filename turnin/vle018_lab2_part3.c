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
    DDRB = 0xFF; PORTB = 0x00;  //port b = outputs
    DDRC = 0xFF; PORTC = 0x00;

    unsigned char tmpB = 0x00;
    unsigned char tmpA = 0x00;
    unsigned char tmpC = 0x00;
    unsigned char tmpD = 0x00;
       
    while (1) {
	tmpA = PINA & 0x01;
	tmpB = PINA & 0x02;
	tmpC = PINA & 0x04;
	tmpD = PINA & 0x08;  
	unsigned char cntavail = 0x00; 
	PORTB = 0;
	PORTC = 0;
	    
	if(tmpA == 0x01 && tmpB == 0x00){
		PORTB = 1;	
	}
		
	if (tmpA == 0x01){
		cntavail = 1;
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
	if(PINA == 0x0F){
		cntavail =  cntavail | 0x80;	
	}
	PORTC = cntavail;
    }
    
    return 0;
}
