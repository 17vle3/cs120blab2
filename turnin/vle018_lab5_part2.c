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

typedef enum States{start, next, add, sub, reset, waitzero } States;

int main(void) {
	DDRC = 0xFF; PORTC = 0x00; //port b00 = inputs 
	DDRA = 0x00; PORTA = 0xFF; //port b = output 

	States state = start;
	while (1) {
			
			state = stateUpdate(state);
			
	}
    
    return 0;
}
int stateUpdate(int state){
	static unsigned char c = 0x00;
	unsigned char a0 = (~PINA & 0x0F) & 0x01;
	unsigned char a1 = ((~PINA & 0x0F) & 0x02)>>1;
	unsigned char both = (a0 && a1);
	unsigned char zero = !a0 && !a1;
	
	switch (state) { //transitions
		case start:
			if(a0 || a1){
				state = next;
			}
			break;
		case waitzero:
			if(a0 == 0x00 && a1 == 0x00){
				state = next;
			}
			else if(a0 && a1){
				state = reset;
			}
			break;
		case next: //goes to states if there is input. if zero, stay here
			if(a1 && a0){
				state = reset;
			}
			else if (a0){
				state = add;
			}
			else if (a1){
				state = sub;
			}
			else{
				state = next;
			}
			break;
		case add:
			if(c < 0x09){
				c = c + 0x01;
			}
			state = waitzero;
			break;
		case sub:
			if(c > 0x00){
				c= c - 0x01;
			}
			state = waitzero;
			break;
		case reset:
			c=0x00;
			state = waitzero;
			break;
		
		default:
			break;
	}
	PORTC = c;
	return state;
	
}

