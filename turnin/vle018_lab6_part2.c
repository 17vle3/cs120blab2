/*	Author: vle018
 *  Partner(s) Name: Mari Hayashi
 *	Lab Section:
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#include <avr/interrupt.h>

typedef enum States{start, zero, one, two, zeropress, onepress, twopress, waitPress} States;
volatile unsigned char TimerFlag = 0;

unsigned long _avr_timer_M = 1;
unsigned long _avr_timer_cntcurr = 0;

void TimerOn(){
	TCCR1B = 0x0B;
	OCR1A = 125;
	TIMSK1 = 0x02;
	TCNT1 = 0;
	_avr_timer_cntcurr = _avr_timer_M;
	SREG |= 0x80;
}
void TimerOff(){
	TCCR1B = 0x00;
}
void TimerISR(){
	TimerFlag = 1;
}
ISR(TIMER1_COMPA_vect){
	_avr_timer_cntcurr--;
	if(_avr_timer_cntcurr == 0){
		TimerISR();
		_avr_timer_cntcurr = _avr_timer_M;
	}
}
void TimerSet (unsigned long M){
	_avr_timer_M = M;
	_avr_timer_cntcurr = _avr_timer_M;
}
int main(void) {
	DDRB = 0xFF; PORTB = 0x00; //port b00 = inputs 
	DDRA = 0x00; PORTA = 0xFF; //port b = output 
	TimerSet(1000);
	TimerOn();
	
	States state = start;
	while (1) {
			state = stateUpdate(state);
			while(!TimerFlag);
			TimerFlag =0;
	}
    
    return 0;
}
int stateUpdate(int state){
	unsigned char button = (~PINA & 0x0F) & 0x01;
	static unsigned char b= 0x00;
	switch (state) { //transitions
		case start:
			state = zero;
			break;
		case zero:
			if(!button){
				state = one;
			}
			else{
				state = zeropress;
			}
			break;
		case one:
			if(!button){
				state = two;
			}
			else{
				state = onepress;
			}
			break;
		case two:
			if(!button){
				state = zero;
			}
			else{
				state = twopress;
			}
		case zeropress:
			if(!button){
				state = waitPress;
			}
			break;
		case onepress:
			if(!button){
				state = waitPress;
			}
			break;
		case twopress:
			if(!button){
				state = waitPress;
			}
			break;
		case waitPress:
			if(button){
				state = start;
			}
			break;
		default:
			break;
	}
	switch (state) { //outputs
		case start:
			b = 0x00;
			break;
		case zero:
			b = 0x01;
			break;
		case zeropress:
			b = 0x01;
			break;
		case one:
			b = 0x02;
			break;
		case onepress:
			b = 0x02;
			break;
		case two:
			b = 0x04;
			break;
		case twopress:
			b = 0x04;
			break;
		default:
			break;
	}
	PORTB = b;
	return state;
	
}

