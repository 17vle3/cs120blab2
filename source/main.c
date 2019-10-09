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

typedef enum States{start, next, pone , sone, reset } States;

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
	static unsigned char c;
	unsigned char add = PINA & 0x01;
	unsigned char minus = (PINA & 0x02) >> 1;
	unsigned char both = (PINA == 0x03);
	switch (state) { //transitions
		case start:
			state =  next;
			break;
		case next:
			if(both){
				state = reset;
			}
			else if (add){
				state = pone;
			}
			else if (minus){
				state = sone;
			}
			else{
				state = next;
			}
			break;
		case pone:
			if(both){
				state = reset;
			}
			else if(!add && !minus){
				state = next;
			}
			else{
				
			}
			break;
		case sone:
			if(both){
				state = reset;
			}
			else if(!add && !minus){
				state = next;
			}
			break;
		case reset:
			if(!add && !minus){
				state = next;
			}
			break;
		
		default:
			state = next;
			break;
			
		
	}
	switch (state) { //actions
		case start:
			c=0x07;
			break;
		case pone:
			if(c<9){
				c=c+1;
			}
			break;
		case sone:
			if(c>0){
				c=c-1;
			}
			break;
		case reset:
			c=0x00;
			break;
		case next:
			break;
	}
	PORTC = c;
	return state;
	
}

