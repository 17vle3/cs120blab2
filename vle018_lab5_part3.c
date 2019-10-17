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

/**volatile unsigned char TimerFlag = 0;
void TimerISR(){
	TimerFlag = 1;
}**/
typedef enum States{start, playnext, waitinput, waitzero } States;

int main(void) {
	DDRB = 0xFF; PORTB = 0x00; //port b00 = inputs 
	DDRA = 0x00; PORTA = 0xFF; //port b = output 
	//TimerSet(100);
	//TimerOn();
	int index = 0x00;
	States state = start;
	while (1) {
			if((state == start || state == waitzero) && ((~PINA & 0x0F) & 0x01)) index ++;
			state = stateUpdate(state,(index-1)%6);
			//while(!TimerFlag){}
			//TimerFlag =0;
	}
    
    return 0;
}
int stateUpdate(int state, int index){
	static unsigned char b= 0x00;
	unsigned char a0 = (~PINA & 0x0F) & 0x01;
	unsigned char arr[6] = {0x01,0x04,0x10,0x04,0x01,0x3F};
	switch (state) { //transitions
		case start:
			if(a0){
				state = playnext;
			}
			break;
		case playnext:
			if(a0){
				break;
			}
			else{
				state = waitzero;
			}
			break;
		case waitinput:
			if(a0){
				state = playnext;
			}
			break;
		case waitzero:
			if(a0){
				state = playnext;
			}
		default:
			break;
	}
	switch (state) { //outputs
		case start:
			//b = 0x01;
			break;
		case playnext:
			b = arr[index];
			if(index = 2){
				index = 0;
			}
			else{
				index++;
			}
			break;
		case waitinput:
			break;
		default:
			break;
	}
	PORTB = b;
	return state;
	
}

