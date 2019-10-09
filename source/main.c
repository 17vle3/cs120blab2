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

typedef enum States{start, next, add, sub, reset, release } States;

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
	unsigned char a0 = PINA & 0x01;
	unsigned char a1 = (PINA & 0x02) >> 1;
	unsigned char both = (a0 & a1);
	switch (state) { //transitions
		case start:
			state =  next;
			break;
		case next:
			if(both){
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
			if(both){
				state = reset;
			}
			else {
				state = release;
			}
			break;
		case sub:
			if(both){
				state = reset;
			}
			else {
				state = release;
			}
			break;
		case reset:
			state = release;
			break;
		case release:
			if(!a0 && !a1){
				state = next;
			}
			else{
				state = release;
			}
		default:
			state = next;
			break;
			
		
	}
	switch (state) { //actions
		case start:
			c=0x07;
			break;
		case add:
			if(c< 0x09){
				c=c+ 0x01;
			}
			break;
		case sub:
			if(c>0x00){
				c= c - 0x01;
			}
			break;
		case reset:
			c=0x00;
			break;
		case next:
			c=c;
			break;
		case release:
			break;
		default:
			break;
	}
	PORTC = c;
	return state;
	
}

