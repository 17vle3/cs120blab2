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
unsigned char GetBit(unsigned char C, unsigned char index){
	unsigned char maskValue = 0x01 << index;
	return ((C & maskValue) == 0x00) ? 0x00 : 0x01;
}
volatile unsigned char TimerFlag = 0;

unsigned long _avr_timer_M = 1;
unsigned long _avr_timer_cntcurr = 0;

typedef struct _task {
   unsigned char state;                  // Task's current state
   unsigned long int period;       // Task period
   unsigned long int elapsedTime;  // Time elapsed since last task tick
   int (*TickFct)(int);        // Task tick function
} task;


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
/**
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
}**/
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
//------------------Shared Variables----------------
unsigned char led0_output = 0x00;
unsigned char led1_output = 0x00;
unsigned char pause = 0;
//------------------End Shared Variables----------------

enum display_States {display_display};
int displaySMTick(int state){
	static unsigned char currNum;
	unsigned char output = GetKeypadKey();
	switch(state){
		case display_display: state = display_display; break;
		default: state = display_display; break;
	}
	switch(state){
		case display_display: 
			output = led0_output | led1_output << 1;
		default:
			break;
	}
	PORTB = output;
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
	return ('\0');
}


int main(void) {
	DDRB = 0xFF; PORTB = 0x00;
	DDRC = 0xF0; PORTC = 0x0F;
	
	static task task1,task2,task3,task4;
	task *tasks[] = {&task1, &task2, &task3, &task4};
	const unsigned short numTasks = sizeof(tasks)/sizeof(task*);
	
	//task 1
	task1.state = start;
	task1.period = 50;
	task1.elapsedTime = task1.period;
	task1.TickFct = &pauseButtonSMTick; 
	//task2
	task2.state = start;
	task2.period = 500;
	task2.elapsedTime = task2.period;
	task2.TickFct = &toggleLED0SMTick; 
	//tasks3
	task3.state = start;
	task3.period = 1000;
	task3.elapsedTime = tass3.period;
	task3.TickFct = &toggleLED1SMTick; 
	//tasks4
	task4.state = start;
	task4.period = 10;
	task4.elapsedTime = task4.period;
	task4.TickFct = &displaySMTick; 
	
	TimerSet(10);
	TimerOn();
	//PWM_on();
	unsigned char x;
	
	//ADC_init();
	
	while (1) {
		x = GetKeypadKey();
		
		/**
		while(!TimerFlag);
		TimerFlag =0;**/
	}
    
    return 0;
}
