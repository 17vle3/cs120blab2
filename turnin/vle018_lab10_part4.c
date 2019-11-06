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

volatile unsigned char TimerFlag = 0;

unsigned long _avr_timer_M = 1;
unsigned long _avr_timer_cntcurr = 0;

typedef enum ThreeStates{one, two, three} ThreeStates;
typedef enum BlinkStates{on, off} BlinkStates;
typedef enum ButtonStates{start, soundOn, soundOff} ButtonStates;
typedef enum FreqStates {begin, high, low, waitZero} FreqStates;
typedef struct task {
   int state;                  // Task's current state
   unsigned long period;       // Task period
   unsigned long elapsedTime;  // Time elapsed since last task tick
   int (*TickFct)(int);        // Task tick function
} task;

task tasks[5];
const unsigned short tasksNum = 5;
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
	unsigned char i;
	for (i = 0; i < tasksNum; ++i) { // Heart of the scheduler code
		if ( tasks[i].elapsedTime >= tasks[i].period ) { // Ready
			tasks[i].state = tasks[i].TickFct(tasks[i].state);
			tasks[i].elapsedTime = 0;
		}
		tasks[i].elapsedTime += 1;
  }
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

unsigned char threeLEDb = 0x00;
unsigned char blinkLEDb = 0x00;
unsigned char buttonLEDb = 0x00;
static unsigned char buttonPeriod = 0x05;
int blinkLEDUpdate(int state){
	switch (state) { //transitions
		case on:
			blinkLEDb= 0x08;
			state = off;
			break;
		case off:
			blinkLEDb= 0x00;
			state = on;
			break;
		default:
			break;
	}
	return state;
	
}
int threeLEDUpdate(int state){
	switch (state) { //transitions
		case one:
			threeLEDb = 0x01;
			state = two;
			break;
		case two:
			threeLEDb = 0x02;
			state = three;
			break;
		case three:
			threeLEDb = 0x04;
			state = one;
		default:
			break;
	}
	return state;	
}
int buttonUpdate(int state){
	unsigned char a0 = (~PINA) & 0x01;
	switch (state) { //transitions
		case start:
			buttonLEDb =0x00;
			if(a0){
				state = soundOn;
			}
			break;
		case soundOn:
			buttonLEDb =0x10;
			if(!a0){
				state = start;
			}
			else{
				state = soundOff;
			}
			break;
		case soundOff:
			buttonLEDb =0x00;
			if(!a0){
				state = start;
			}
			else{
				state = soundOn;
			}
			break;	
		
		default:
			break;
	}
	return state;
	
}
//begin, high, low, waitZero
int freqUpdate(int state){
	unsigned char a1 = ((~PINA) & 0x02)>>1;
	unsigned char a2 = ((~PINA) & 0x04)>>2;
	
	switch (state) { //transitions
		case begin:
			if(a1){
				state = high;
			}
			else if(a2){
				state = low;
			}
			break;
		case high:
			if(buttonPeriod > 1){
				buttonPeriod = buttonPeriod - 1;
				tasks[3].period = buttonPeriod;
			}
			
			state = waitZero;
			break;
		case low:
			if(buttonPeriod < 10){
				buttonPeriod = buttonPeriod + 1;
				tasks[3].period = buttonPeriod;
			}
			state = waitZero;
			break;	
		case waitZero:
			if ( (~PINA) & 0x00){
				state = begin;
			}
		default:
			break;
	}
	return state;
	
}
void CombineLEDsSM(int b){
	PORTB = blinkLEDb | threeLEDb | buttonLEDb;
}
int main(void) {
	DDRB = 0xFF; PORTB = 0x00;
	DDRA = 0x00; PORTA = 0xFF;
	
	unsigned char i=0;
	tasks[i].state = one;
	tasks[i].period = 300;
	tasks[i].elapsedTime = tasks[i].period;
	tasks[i].TickFct = &threeLEDUpdate; 
	i=i+1;
	tasks[i].state = on;
	tasks[i].period = 1000;
	tasks[i].elapsedTime = tasks[i].period;
	tasks[i].TickFct = &blinkLEDUpdate; 
	i = i+1;
	tasks[i].state = 0;
	tasks[i].period = 1;
	tasks[i].elapsedTime = tasks[i].period;
	tasks[i].TickFct = &freqUpdate; 
	i = i+1;
	tasks[i].state = start;
	tasks[i].period = buttonPeriod;
	tasks[i].elapsedTime = tasks[i].period;
	tasks[i].TickFct = &buttonUpdate; 
	i = i+1;
	tasks[i].state = 0;
	tasks[i].period = 2;
	tasks[i].elapsedTime = tasks[i].period;
	tasks[i].TickFct = &CombineLEDsSM; 
	
	
	
	//LCD_init();
	TimerSet(1);
	TimerOn();
	PWM_on();
	
	
	//ADC_init();
	while (1) {
		int i = 0x00;	
		for(i = 0; i < tasksNum; i++){
			
		}
		while(!TimerFlag);
		TimerFlag =0;
	}
    
    return 0;
}


