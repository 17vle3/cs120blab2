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
unsigned char GetKeypadKey(){
	PORTA = 0xEF;
	asm("nop");
	if(GetBit(PINA,0) == 0) {return ('1');}
	if(GetBit(PINA,1) == 0) {return ('4');}
	if(GetBit(PINA,2) == 0) {return ('7');}
	if(GetBit(PINA,3) == 0) {return ('*');}
	PORTA = 0xDF;
	asm("nop");
	if(GetBit(PINA,0) == 0) {return ('2');}
	if(GetBit(PINA,1) == 0) {return ('5');}
	if(GetBit(PINA,2) == 0) {return ('8');}
	if(GetBit(PINA,3) == 0) {return ('0');}
	PORTA = 0xBF;
	asm("nop");
	if(GetBit(PINA,0) == 0) {return ('3');}
	if(GetBit(PINA,1) == 0) {return ('6');}
	if(GetBit(PINA,2) == 0) {return ('9');}
	if(GetBit(PINA,3) == 0) {return ('#');}
	PORTA = 0x7F;
	asm("nop");
	if(GetBit(PINA,0) == 0) {return ('A');}
	if(GetBit(PINA,1) == 0) {return ('B');}
	if(GetBit(PINA,2) == 0) {return ('C');}
	if(GetBit(PINA,3) == 0) {return ('D');}
	return ('\0');
}
//------------------Shared Variables----------------
unsigned char col[8] = {0b11111110,0b11111101,0b11111011,0b11110111,0b11101111,0b11011111,0b10111111,0b01111111};
unsigned char row[4] = {0b01100010,0b01100100, 0b01101000,0b01110000};
unsigned char columnOutput = 0x00;
static unsigned char rowOutput = 0x00;
unsigned char colCheck[8] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};
unsigned const char song[] = {0}; //possible 0 1 2 3 01 02 12 13 23 012 etc 
unsigned const char songSize = 1;
static unsigned char songIndex = 0;
static unsigned char existingColumns = 0; //which columns exist right now in binary 0x00 = none 0x01 = top column 0x02 = second column etc 

//------------------End Shared Variables----------------
/**
 * This function moves the dots down by 1 row
 * This should be updated at the same speed as the display.
 **/
typedef enum updateColumnsStates{updateColumns_start} updateColumnsStates;
int updateColumns(){
	//function for adding new column here
	
	switch (state) { 
		case updateColumns_start:
			int i;
			columnOutput = columnOutput<< 1;
			for(i = 0; i < 8 ; i++){
				if(existingColumns & colCheck[i]){
					columnOutput = columnOutput&col[i];
				}
			} 
			break;
		default:
			break;
	}
	return updateColumns_start;
}
/**
 * This function adds a dot at the top 
 **/
typedef enum newDotStates{updateNewDot_start} newDotStates;
int updateNewDot(){
	//function for adding new column here
	
	switch (state) { 
		case updateNewDot_start:
			if(songIndex >= songSize){
				break;
			}
			if()
			break;
		default:
			break;
	}
	return updateColumns_start;
}


int main(void) {
	DDRB = 0x00; PORTB = 0xFF;
	DDRC = 0xFF; PORTC = 0x00;
	DDRD = 0xFF; PORTD = 0x00;
	
	int i;
	
	static task task1,task2;
	task *tasks[] = {&task1, &task2};
	const unsigned short numTasks = sizeof(tasks)/sizeof(task*);
	
	//task 1
	task1.state = rollDown_start;
	task1.period = 30;
	task1.elapsedTime = task1.period;
	task1.TickFct = &rollDownUpdate; 
	
	
	
	
	TimerSet(10);
	TimerOn();
	//PWM_on();
	
	
	LCD_init();
	
	while (1) {
		/**
		for(i = 0; i<numTasks; i++){
			if(tasks[i]->elapsedTime == tasks[i]->period){
				tasks[i]->elapsedTime = tasks[i]->TickFct(tasks[i]->state);
				tasks[i]->elapsedTime = 0;
			}
			tasks[i]->elapsedTime +=10;
		}**/
		
		PORTC = 0b11111110;
		PORTD = 0b01111110;
		
		while(!TimerFlag);
		TimerFlag = 0;
	}
    
    return 0;
}
/**
for D: (ROWS)
* (or them together)
* PORTD = 0b01111110; (every column lit)
* first column: 0b01100010
* second: 0b01100100
* third: 0b01101000
* fourth: 0b01110000

PORTC (COLUMNS) 
(and them together) 
* 1 0b11111110
* 2 0b11111101
* 3 0b11111011
* 4 0b11110111
* 5 0b11101111
* 6 0b11011111
* 7 0b10111111
* 8 0b01111111

//last row 0b01111111

**/
