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

typedef enum States{releaseoff, presson, releaseon, pressoff } States;

int main(void) {
	DDRB = 0xFF; PORTB = 0x00; //port b00 = inputs 
	DDRA = 0x00; PORTA = 0xFF; //port b = output 

	States state = releaseoff;
	while (1) {
			state = stateUpdate(state);
			
	}
    
    return 0;
}
int stateUpdate(int state){
	static unsigned char b;
	unsigned char button = PINA & 0x01;
	switch (state) { //transitions
		case releaseoff:
			if(button) ? state = presson; : state = releaseoff;
			break;
		case presson:
			if(!button) ? state = releaseon; : state = presson;
			break;
		case releaseon:
			if(button) ? state = pressoff; : state = releaseon;
			break;
		case pressoff:
			if(!button) ? state = releaseoff; : state = pressoff;
			break;
		default:
			state = releaseoff;
			break;
			
		
	}
	switch (state) { //actions
		case releaseoff:
			b=0x01;
			break;
		case presson:
			b=0x02;
			break;
		case releaseon:
			b=0x02
			break;
		case pressoff:
			b=0x01
			break;
	}
	PORTB = b;
	return state;
	
}

