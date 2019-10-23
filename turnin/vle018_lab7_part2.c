/*	Author: vle018
 *  Partner(s) Name: Mari Hayashi
 *	Lab Section:
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#include "io.h"

typedef enum States{start,start2, zero, one, two, zeropress, onepress, twopress, waitPress} States;
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
	DDRC = 0xFF; PORTC = 0x00;
	DDRD = 0xFF; PORTD = 0x00;
	DDRA = 0x00; PORTA = 0xFF; //port b = output 
	TimerSet(25);
	TimerOn();
	
	LCD_init();
	//static unsigned int time = 0x00;
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
	static unsigned char b;
	static int time;
	static unsigned char value;
	static unsigned char add = 0x00;
	static unsigned char minus = 0x00;
	
	switch (state) { //transitions
		case start:
			value = 0x05;
			add = 0x00;
			minus = 0x00;
			time = 0;
			if(!button){
				state = zero;
			}
			break;
		case start2:
			add = 0x00;
			minus = 0x00;
			time = 0;
			if(!button){
				state = zero;
			}
			break;
		case zero:
			if(time <= 40){
				if(button){
					state = zeropress;
				}
				time = time + 1;
				break;
			}
			else{
				time = 0;
				state = one;
			}
			break;
		case one:
			if(time <= 40){
				if(button){
					state = onepress;
				}
				time = time + 1;
				break;
			}
			else{
				time = 0;
				state = two;
			}
			break;
		case two:
			if(time <= 40){
				if(button){
					state = twopress;
				}
				time = time + 1;
				break;
			}
			else{
				time = 0;
				state = zero;
			}
			break;
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
				state = start2;
				time = 0;
			}
			break;
		default:
			break;
	}
	
	
	
	
	switch (state) { //outputs
		case start:
			value = 0x05;
			b = 0x00;
			break;
		case zero:
			b = 0x01;
			break;
		case zeropress:
			if(!minus){
				if(value > 0x00){
					value = value - 1;
				}
				minus = 0x01;
			}
			b = 0x01;
			break;
		case one:
			b = 0x02;
			
			break;
		case onepress:
			if(!add){
				if(value < 0x09){
					value = value + 1;
				}
				add = 0x01;
			}
			b = 0x02;
			break;
		case two:
			b = 0x04;
			break;
		case twopress:
			if(!minus){
				if(value > 0x00){
					value = value - 1;
				}
				minus = 0x01;
			}
			b = 0x04;
			break;
		default:
			break;
	}
	
	
	LCD_WriteData(value + '0');
	LCD_Cursor(1);
	
	if(value == 0x09){
		LCD_DisplayString(1, "Victory");
	}
	
	PORTB = b;
	return state;
	
}

