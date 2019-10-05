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
	DDRB = 0x00; PORTB = 0xFF;  //port b = input
	DDRC = 0xFF; PORTC = 0x00; //port c = output 
	

       while (1) {
		unsigned char cntavail = 0x00; //count num of 1s in A and B (max 8)
		PORTB = 0;
	       
	       	//for every digit from the right, count the number of ones and add to cnt avail in a then b
	       	int i = 0;
	       	int count = 0;
		for(i = 0; i < 9 ; i = i * 2 ){
			unsigned char temp = (PINA & i);
			temp = temp >> count;
			if(temp) {
				cntavail = cntavail +1;	
			}
			++ count;
		}
	       	i = 0;
	        count = 0;
		for(i = 0; i < 9 ; i = i * 2 ){
			unsigned char temp = (PINB & i);
			temp = temp >> count;
			if(temp) {
				cntavail = cntavail +1;	
			}
			++ count;
		}
		PORTC = cntavail;
    }
    
    return 0;
}
