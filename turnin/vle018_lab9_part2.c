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
typedef enum States{start, next, add, sub, play, release} States;
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
void ADC_init(){
	ADCSRA |= (1<<ADEN) | (1<< ADSC) | (1<<ADATE);
}
void set_PWM(double frequency){
	static double current_frequency;
	if(frequency != current_frequency){
		if(!frequency){ TCCR3B &= 0x08; }
		else {TCCR3B |= 0x03;}
		
		if(frequency < 0.954) { OCR3A = 0xFFFF;}
		else if (frequency > 31250){ OCR3A = 0x0000;}
		else {OCR3A = (short) (8000000/(128 * frequency)) - 1;}
		
		TCNT3 = 0;
		current_frequency = frequency;
	}
}
void PWM_on(){
	TCCR3A = (1 << COM3A0);
	TCCR3B = (1 << WGM32) | (1 << CS31) | (1  << CS30);
	set_PWM(0);
}
void PWM_off(){
	TCCR3A = 0x00;
	TCCR3B = 0x00;
}
int main(void) {
	DDRB = 0xFF; PORTB = 0x00;
	DDRA = 0x00; PORTA = 0xFF; //port b = output 
	
	//LCD_init();
	TimerSet(25);
	TimerOn();
	PWM_on();
	
	States state = start;
	//ADC_init();
	while (1) {
		
			
		state = stateUpdate(state);
			//while(!TimerFlag);
			//TimerFlag =0;
	}
    
    return 0;
}

int stateUpdate(int state){
	unsigned char a0 = (~PINA & 0x0F) & 0x01;
	unsigned char a1 = ((~PINA & 0x0F) & 0x02)>>1;
	unsigned char a2 = ((~PINA & 0x0F) & 0x04)>>2;	
	unsigned char zero = ((~PINA & 0x0F) == 0x00);
	double arr[8] = {261.63, 293.66, 329.63, 349.23, 392.00, 440.00,493.88, 523.25};
	static unsigned char index=0;
	static double freq=261.63;
	switch (state) { //transitions
		case start:
			state = next;
			break;
		case next:
			if(a1){
				state = add;
			}
			else if(a2){
				state = sub;
			}
			else if(a0){
				state = play;
			}
			break;
		case add:
			if(a1){
				state = release;
				break;
			}
			if(a0){
				state = play;
			}
			else if(!a1){
				state = next;
				break;
			}
		case sub:
			if(a2){
				state = release;
				break;
			}
			else if(a0){
				state = play;
			}
			else if(!a2){
				state = next;
				break;
			}
		case play:
			if(a0){
				state = play;
			}
			if(!a0){
				state = release;
			}
			break;
		case release:
			if(zero){
				state = next;
			}
		default:
			break;
	}
	switch (state) { //outputs
		case start:
			break;
		case next:
			break;
		case add:
			if(index < 0x07){
				index = index + 0x01;
				freq = arr[index];
			}
			break;
		case sub:
			if(index > 0x00){
				index = index - 0x01;
				freq = arr[index];
			}
			break;
		case play:
			set_PWM(freq);
			break;
		case release:
			set_PWM(0);
			break;
		default:
			break;
	}
	return state;
	
}
