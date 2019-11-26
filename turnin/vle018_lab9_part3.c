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

//A, B, C♯, D, E, F♯, and G♯




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
	
	if (frequency != current_frequency){
	if (!frequency) { TCCR3B &= 0x08; }
	else { TCCR3B |= 0x03; }

	if (frequency < 0.954) { OCR3A = 0xFFFF; }
	else if (frequency > 31250) { OCR3A = 0x0000; }

	else { OCR3A = (short) (8000000 / (128 * frequency)) - 1; }

	TCNT3 = 0;
	current_frequency = frequency;
	}
}

void PWM_on(){
	TCCR3A = (1 << COM3A0);
	TCCR3B = (1 << WGM32) | (1 << CS31) | (1 << CS30);
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
	TimerSet(3);
	TimerOn();
	PWM_on();
	
	States state = play;
	//ADC_init();
	while (1) {	
		state = stateUpdate(state);
		while(!TimerFlag);
		TimerFlag =0;
	}
    
    return 0;
}

int stateUpdate(int state){
	double b = 987.77	;
double c = 1108.73;
double d = 1174.66;
double e = 1318.51;
double f = 1479.98;
double g = 1661.22;
double a2= 1760.00;
double b2 =1975.53;
double c2 = 2217.46;
double d2 =2349.32;
double e2 =2637.02;
double f2 =2959.96	;
double g2 =3322.44;

	unsigned char a0 = (~PINA & 0x0F) & 0x01;
	unsigned char a1 = ((~PINA & 0x0F) & 0x02)>>1;
	//unsigned char a2 = ((~PINA & 0x0F) & 0x04)>>2;
	static unsigned char time = 0x00;
	unsigned char zero = ((~PINA & 0x0F) == 0x00);                         //f e c b a e c b a
	double arr[72] = {a2,f2,e2,c2,b2,    a2,e2,c2,b2,a2,    0,
		f2,e2,c2,b2,a2,    b2,a2,b2,a2,e2,    c,b2,a2,0,
		f2, e2, c2, b2,    a2, e2, c2, b2, a2,    0,
		f2,e2,c2,b2,a2,    b2,a2,b2,a2,e2,    c,b2,a2,0,
		e,f,c2,b2,f2,    e2,c2,b2,a2,0,   f2,e2,c2,b2,a2,    b2,a2,b2,a2,e2,    c2,b2,a2}; //15
	double arr1[72] = {20,10,20,20,20,   20,20,10,10,20,    20,
		10,10,10,10,10,   10,10,10,10,10,    10,10,20,20, //1 2 2 2 2    2 1 1 2 2 
		10,20,20,20,   20,20,10,10,20,    20,
		10,10,10,10,10,   10,10,10,10,10,    10,10,20,20,
		10,20,20,20,20,   20,10,20,20,20,    10,10,10,10,10,   10,10,10,10,10,  10,10,10 };
	static unsigned char index=0;
	static double freq=0;
	switch (state) { //transitions
		case start:
			//if((~PINA) & 0x01){
				state = play;
				time = 0x00;
			//}
			break;
		case play:
			if(time<=arr1[index]){
				freq = arr[index] ;
				time++; 
				break;
			}
			else {
				if(index < 72){
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
