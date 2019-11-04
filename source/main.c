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
typedef enum States{start, play, done, waitRelease, waitPress, waitRelease2 } States;
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
		while(!TimerFlag);
		TimerFlag =0;
	}
    
    return 0;
}

int stateUpdate(int state){
	unsigned char a0 = (~PINA & 0x0F) & 0x01;
	unsigned char a1 = ((~PINA & 0x0F) & 0x02)>>1;
	unsigned char a2 = ((~PINA & 0x0F) & 0x04)>>2;
	static unsigned char time = 0x00;
	unsigned char zero = ((~PINA & 0x0F) == 0x00);                         //f c a# g d# f f d# f f g f d#
	double arr[31] = {466.16,  466.16, 392.00, 523.25 , 392.00, 311.13, 349.23,//a# a# g c g d# f 6 
		0, 349.23 , 311.13, 349.23, 311.13, 349.23, 0, 349.23, 0, //pause f 10
		349.23,523.25,466.16, 392.00,311.13,   349.23, 0 , 349.23,311.13, 349.23, 0 , 349.23,392.00, 349.23, 311.13}; //15
	double arr1[31] = {10,10,10,10,10,10,20,
		1,10,10,20,10,20,1,20,1,
		10,10,10,10,10,  20,1,10,10,10,1,10,10,10};
	static unsigned char index=0;
	static double freq=0;
	switch (state) { //transitions
		case start:
			if(a0){
				state = play;
				time = 0x00;
			}
			break;
		case play:
			if(time<=arr1[index]){
				freq = arr[index];
				time++; 
				break;
			}
			else {
				if(index < 31){
					index = index + 1;
					time = 0;
					break;
				}
				else{
					state = done;	
				}
				break;
			}
			break;
		case done:
			freq = 0;
			if(a0){
				state = waitRelease;
			}
			break;
		case waitRelease:
			if(!a0){
				state = waitPress;
			}
			break;
		case waitPress:
			if(a0){
				state = waitRelease2;
			}
			break;
		case waitRelease2:
			if(!a0){
				index = 0;
				state = play;
			}
			break;
		default:
			break;
	}
	set_PWM(freq);
	return state;
	
}
