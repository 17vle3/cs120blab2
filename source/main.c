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
#include "io.h"
#include <util/delay.h>

#define zero (~PINA & 0x01)
#define one (~PINA & 0x02)>>1
#define two (~PINA & 0x04)>>2
#define three (~PINA & 0x08)>>3
#define startButton (~PINA & 0x10)>>4


void transmit_data(unsigned char data) {
	int i;
	for (i = 0; i < 8 ; ++i) {
		// Sets SRCLR to 1 allowing data to be set
		// Also clears SRCLK in preparation of sending data
		PORTC = 0x01;
		// set SER = next bit of data to be sent.
		PORTC |= ((data >> i) & 0x08);
		// set SRCLK = 1. Rising edge shifts next bit of data into the shift register
		PORTC |= 0x02;
	}
	// set RCLK = 1. Rising edge copies data from “Shift” register to “Storage” register
	PORTC |= 0x04;
	// clears all lines in preparation of a new transmission
	PORTC = 0x00;
}	

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
unsigned short ADC_Scaler(unsigned short max, unsigned short value, unsigned char d)
{
	unsigned char position;
    unsigned short div = max / d;

	int i; 
    for(i = 1; i <= d; i++)
    {
        //Sets boundaries
        if((value <= i*div) && (value >= (i-1)*div) )
        {
            position = i;
        }
    }
    return position;
}
	
typedef enum joystickStates{joystick_start,joystick_wait} joystickStates;
int joystickUpdate(int state){
	static unsigned char bOutput = 0x04;

	unsigned int LRTemp = adc_read(0);

	switch (state) { 
		case joystick_start:
			if(LRTemp > 800 && bOutput < 8){ //right
				bOutput = bOutput << 1;
			}
			if(LRTemp < 400 && bOutput > 1){
				bOutput = bOutput >> 1;
			}
			state = joystick_wait;
			break;
		case joystick_wait:{
			if(LRTemp > 550 && LRTemp < 580){
				state = joystick_start;
			}
		}
		default:
			break;
	}
	PORTB = bOutput;

	return state;
}

int main(void) {
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;

	TimerSet(100);
	TimerOn();
	PWM_on();
	ADC_init();
	
	LCD_init();
	int i;
/**
 * 012
 * 345
 * 123
**/
	static unsigned char state = joystick_start;
	
	while (1) {
		unsigned int LRTemp = adc_read(0);
		//bigger than 550 less than 580
		joystickUpdate(state);
		while(!TimerFlag);
		TimerFlag = 0;
		
		
		
	}
    
    return 0;
}
