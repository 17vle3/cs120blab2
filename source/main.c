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

typedef enum States{start, sequence, sequencewaitzero, sequencewaitinput, unlock, waitzero, waita7 } States;

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
	static unsigned char count = 0x00;
	unsigned char arr[6] = {0x01,0x02,0x01};
	static unsigned int index = 0x00;
	switch (state) { //transitions
		case start:
			state = num ? sequencewaitzero : start;
			break;
			
		case sequencewaitinput:
			if(zero){
				state = sequencewaitinput; break;
			}
		case sequence:
			if(index < 6){
				if(PINA == arr[index]){
					count = count + 1;
				}
				if(index == 5 && count == 6){
					state = unlock; 
				}
				else if(index == 5){
					index = 0;
					state = start;
				}
				else{
					index = index +1;
					state = sequencewaitzero; 
				}
			}
			break;
		case sequencewaitzero:
			if(PINA == 0x00){
				state = sequencewaitinput;
			}
			else{
				state = sequencewaitzero;
			}
			break;
		case unlock:
			if(b==0x01){
				state = waita7;
			}
			else{
				state = waitzero;
			}
			break;
		case waita7:
			if(a7){
				state = waitzero;
			}
			else if(num){
				state = sequence;
			}
			else{
				state = waita7;
			}
			break;
		case waitzero:
			if(zero){
				state = start;
			}
			else{
				state = waitzero;
			}
		default:
			state = state;
			break;
	}
	
	switch (state) { //c output
		case start:
			//b=0;
			break;
		case sequence:
			break;
		case unlock:
			if(b == 0x00){
				b=0x01;
			}
			else{
				b=0x00;
			}
			break;
		case waitzero:
			break;
		case waita7:
			break;
		default:
			break;
	}
			
	
			
	
	PORTB = b;
	return state;
	
}

