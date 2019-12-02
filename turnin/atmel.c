/*	Author: vle018
 *	Lab Section:
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#include <stdlib.h>
#include <math.h>
#include <avr/interrupt.h>

//#include <util/delay.h>




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
	ADMUX = (1<<REFS0);
	ADCSRA = (1<<ADEN)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);
}
uint16_t adc_read(int ch){
    ch &= 0b00000111;  
    ADMUX = (ADMUX & 0xF8)|ch; 
    ADCSRA |= (1<<ADSC);
    while(ADCSRA & (1<<ADSC));
    return (ADC);
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

//------------------Joystick Functions----------------**


typedef enum songstates{song_start, song_play, song_done } song_states;
int songUpdate(int state){
	double c = 277.18	;
	double d = 293.66	;
	double e = 329.63;
	double f = 369.99	;
	double g = 415.30;

	unsigned char a0 = (~PINA & 0x0F) & 0x01;
	unsigned char a1 = ((~PINA & 0x0F) & 0x02)>>1;
	//unsigned char a2 = ((~PINA & 0x0F) & 0x04)>>2;
	static unsigned char time = 0x00;
	double arr[27] = {e,d,c,d,e,   e,e,d,d,d,   e,g,g,0,e,   d,c,d,e,e,   e,e,d,d,e,   d,c}; //15
	static unsigned char index=0;
	static double freq=0;
	switch (state) { //transitions
		case song_start:
			//if((~PINA) & 0x01){
			state = song_play;
			time = 0x00;
			//}
			break;
		case song_play:
		if(time<=1){
			freq = arr[index] ;
			time++;
			break;
		}
		else {
			if(index < 27){
				index = index + 1;
				time = 0;
				break;
			}
			else{
				state = song_done;
			}
			break;
		}
		break;
		case song_done:
			freq = 0;
		default:
		break;
	}
	set_PWM(freq);
	return state;
	
}

typedef enum joystickStates{joystick_start, joystick_play ,joystick_wait} joystickStates;
int joystickUpdate(int state){
	unsigned int LRTemp = adc_read(0);
	static unsigned char bOutput;
	unsigned char change = 0;
	
	switch (state) {
		case joystick_start:
			bOutput = 0x04;
			break;
		case joystick_play:
			if(LRTemp > 1000 && bOutput < 8){ //right
				bOutput = bOutput << 1;
				change = 0x01;
			}
			else if(LRTemp < 300 && bOutput > 1){
				bOutput = bOutput >> 1;
				change = 0x01;
			}
			break;
		case joystick_wait:
			break;
		default:
			break;
	}
	
	switch (state) {
		case joystick_start:
			state = joystick_play;
			break;
		case joystick_play:
			if(change){
				state = joystick_wait;
			}
			else{
				state = joystick_play;
			}
			break;
		case joystick_wait:
			if(LRTemp > 550 && LRTemp < 580){
				state = joystick_play;
			}
			break;
		default:
			break;
	}
	
	PORTB = bOutput;
	return state;
}
int main(void) {
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;

	TimerSet(10);
	TimerOn();
	PWM_on();
	ADC_init();
	

	static joystickStates state = joystick_start;
	
	while (1) {
		state = joystickUpdate(state);

		while(!TimerFlag);
		TimerFlag = 0;
		
		
	}
    
    return 0;
}
