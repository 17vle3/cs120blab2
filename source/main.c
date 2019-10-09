/*	Author: vle018
 *  Partner(s) Name: Mari Hayashi
 *	Lab Section:
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

typedef enum States{start, one , two , unlock, waitzero, waitzero2, lock } States;

int main(void) {
	DDRB = 0xFF; PORTB = 0x00; //port b00 = inputs 
	DDRA = 0x00; PORTA = 0xFF; //port b = output 

	States state = start;
	while (1) {
			
			state = stateUpdate(state);
			
	}
    
    return 0;
}
int stateUpdate(int state){
	static unsigned char b =0x00;
	unsigned char x = PINA & 0x01;
	unsigned char y = (PINA & 0x02) >> 1;
	unsigned char num = (PINA & 0x04) >> 2;
	unsigned char a7 = (PINA & 0x80) >> 7;
	unsigned char zero = (PINA == 0x00);
	unsigned char locked = 0x01;
	
	switch (state) { //transitions
		case start:
			state = num ? one : start;
			break;
		case one:
			if(num){
				state = one;
			}
			else if (zero){
				state = two;
			}
			else{
				state = start;
			}
			break;
		case two:
			if(zero){
				state = two;
			}
			else if (y && locked){
				state = unlock;
			}
			else if (y && !locked){
				state = waitzero2;
			}
			else{
				state = start;
			}
			break;
		case unlock:
			if(zero){
				state = waitzero;
			}
			else{
				state = unlock;
			}
			break;
		case waitzero:
			if(a7){
				state = start;
			}
			else if(num){
				state = one;
			}
			else{
				state = waitzero;
			}
			break;
		case waitzero2:
			if(zero){
				state = start;
			}
			else{
				state = waitzero2;
			}
		default:
			state = state;
			break;
	}
	
	switch (state) { //c output
		case start:
			locked = 0x01;
			b=0;
			break;
		case one:
			break;
		case two:
			break;
		case unlock:
			locked = 0x00;
			b=1;
			break;
		case waitzero:
			locked = 0x00;
			b=1;
			break;
		case waitzero:
			locked = 0x01;
			b=0;
			break;
		default:
			break;
	}
			
	
			
	
	PORTB = b;
	return state;
	
}

