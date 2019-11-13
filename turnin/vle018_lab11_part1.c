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

typedef enum States{start} States;

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

void TimerISR(){
	TimerFlag = 1;
	/**
	unsigned char i;
	for (i = 0; i < tasksNum; ++i) { // Heart of the scheduler code
		if ( tasks[i].elapsedTime >= tasks[i].period ) { // Ready
			tasks[i].state = tasks[i].TickFct(tasks[i].state);
			tasks[i].elapsedTime = 0;
		}
		tasks[i].elapsedTime += 1;
  }**/
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
//------------------Shared Variables----------------
unsigned char led0_output = 0x00;
unsigned char led1_output = 0x00;
unsigned char playNext = 0;
//------------------End Shared Variables----------------
typedef enum playNext_States{playNext_one, playNext_zero} playNext_States; 
//every .5 seconds, play next letter.
//20 ms playNext = 1, 480 ms playNext = 0; 
int pauseButtonSMTick(int state){
	static unsigned int time = 0x00;
	switch (state) { 
		case playNext_one:
			playNext = 1;
			state = playNext_wait;
			break;
		case playNext_zero:
			playNext = 0;
			if( time >= 24){
				state = playNext_one;
			}
			else{
				state = playNext_zero;
			}
			break;
		default:
			break;
	}
	return state;
	
}

//every 10 ms check playNext 
//if playNext = 1, update, wait for zero
//LCD_DisplayString(unsigned char column, const unsigned char* string):
enum display_States {display_displayNext, display_wait };
int displaySMTick(int state){
	unsigned char output;
	//"CS120B is Legend... wait for it DARY!"
	const unsigned char display[27] = "Legend... wait for it DARY!"; //12
	static unsigned char index = 0x00;
	int i;
	switch(state){
		case display_displayNext: 
			LCD_Cursor(0);
			for(i = 0; i < 16 ; i++){
				LCD_WriteData(display[index+i]);	
			}
			if(index < 27-16){
				index++;
			}
		default:
			break;
	}
	
	switch(state){
		case display_displayNext:
			state = display_wait; 
			break;
		case display_wait:
			state = display_displayNext; 
			break;
		default: 
			break;
	}
	

	return state;
}
/**
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
**/

int main(void) {
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;
	DDRC = 0xFF; PORTC = 0x00;
	DDRD = 0xFF; PORTD = 0x00;
	
	static task task2;
	task *tasks[] = {&task2};
	const unsigned short numTasks = sizeof(tasks)/sizeof(task*);
	
	
	task2.state = display_displayNext;
	task2.period = 500;
	task2.elapsedTime = task2.period;
	task2.TickFct = &displaySMTick; 
	
	
	TimerSet(10);
	TimerOn();
	//PWM_on();
	unsigned short i;
	
	//ADC_init();
	
	while (1) {
		for(i = 0; i<numTasks; i++){
			if(tasks[i]->elapsedTime == tasks[i]->period){
				tasks[i]->elapsedTime = tasks[i]->TickFct(tasks[i]->state);
				tasks[i]->elapsedTime = 0;
			}
			tasks[i]->elapsedTime +=10;
		}
		while(!TimerFlag);
		TimerFlag = 0;
	}
    
    return 0;
}
