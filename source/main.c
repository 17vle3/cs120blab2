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

typedef struct task {
   int state;                  // Task's current state
   unsigned long period;       // Task period
   unsigned long elapsedTime;  // Time elapsed since last task tick
   int (*TickFct)(int);        // Task tick function
} task;

task tasks[1];
const unsigned short tasksNum = 1;
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
unsigned char GetKeypadKey(){
	PORTC = 0xEF;
	asm("nop");
	if(GetBit(PINC,0) == 0) {return ('1');}
	if(GetBit(PINC,1) == 0) {return ('4');}
	if(GetBit(PINC,2) == 0) {return ('7');}
	if(GetBit(PINC,3) == 0) {return ('*');}
	PORTC = 0xDF;
	asm("nop");
	if(GetBit(PINC,0) == 0) {return ('2');}
	if(GetBit(PINC,1) == 0) {return ('5');}
	if(GetBit(PINC,2) == 0) {return ('8');}
	if(GetBit(PINC,3) == 0) {return ('0');}
	PORTC = 0xBF;
	asm("nop");
	if(GetBit(PINC,0) == 0) {return ('3');}
	if(GetBit(PINC,1) == 0) {return ('6');}
	if(GetBit(PINC,2) == 0) {return ('9');}
	if(GetBit(PINC,3) == 0) {return ('#');}
	PORTC = 0x7F;
	asm("nop");
	if(GetBit(PINC,0) == 0) {return ('A');}
	if(GetBit(PINC,1) == 0) {return ('B');}
	if(GetBit(PINC,2) == 0) {return ('C');}
	if(GetBit(PINC,3) == 0) {return ('D');}
}


int main(void) {
	DDRB = 0xFF; PORTB = 0x00;
	DDRC = 0xF0; PORTC = 0x0F;
	
	unsigned char i=0;
	tasks[i].state = one;
	tasks[i].period = 300;
	tasks[i].elapsedTime = tasks[i].period;
	tasks[i].TickFct = &threeLEDUpdate; 
	i=i+1;
	
	TimerSet(1);
	TimerOn();
	PWM_on();
	unsigned char x;
	
	//ADC_init();
	unsigned char x;
	
	while (1) {
		switch(x){
			case '\0': PORTB = 0x1F; break; 
			case '1': PORTB = 0x01; break;
			case '2': PORTB = 0x02; break;
			case '3': PORTB = 0x03; break;
			case '4': PORTB = 0x04; break;
			case '5': PORTB = 0x05; break;
			case '6': PORTB = 0x06; break;
			case '7': PORTB = 0x07; break;
			case '8': PORTB = 0x08; break;
			case '9': PORTB = 0x09; break;
			case 'A': PORTB = 0x0A; break;
			case 'B': PORTB = 0x0B; break;
			case 'C': PORTB = 0x0C; break;
			case 'D': PORTB = 0x0D; break;
			case '*': PORTB = 0x0E; break;
			case '0': PORTB = 0x00; break;
			case '#': PORTB = 0x0F; break;
			default: PORTB = 0x1B; break;
		}
		
		int i = 0x00;	
		for(i = 0; i < tasksNum; i++){
			
		}
		while(!TimerFlag);
		TimerFlag =0;
	}
    
    return 0;
}


