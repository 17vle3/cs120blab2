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
		PORTC = 0x08;
		// set SER = next bit of data to be sent.
		PORTC |= ((data >> i) & 0x01);
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
/**
int joyStickUp() {
	ADCValue = ADC;
	return (ADCValue > 700) ? 1 : 0;
}
int joyStickDown() {
	ADCValue = ADC;
	return (ADCValue < 300) ? 1 : 0;
}
int joyStickLeft() {
	ADCValue = ADC;
	return (ADCValue > 700) ? 1 : 0;
}
int joyStickRight() {
	ADCValue = ADC;
	return (ADCValue < 300) ? 1 : 0;
}**/


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

//------------------Shared Variables for MATRIX----------------
unsigned char col[8] = {0b01111111,0b10111111,0b11011111,0b11101111,0b11110111,0b11111011,0b11111101,0b11111110};

unsigned char row[4] = {0b01100010,0b01100100, 0b01101000,0b01110000, 0b01111110 }; //0 1 2 3 all
static unsigned char columnIndex =0;
unsigned char columnOutput = 0x00; //THE PORTC OUTPUT
static unsigned char rowOutput = 0x00; //THE PORTD OUTPUT
unsigned char colCheck[8] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};
unsigned const char song[] = { 0 ,1,2,3}; //possible 0 1 2 3 01 02 12 13 23 012 etc 
unsigned const char songSize = 5;
static unsigned char songIndex = 0;
static unsigned char existingColumns = 0; //which columns exist right now in binary 0x00 = none 0x01 = top column 0x02 = second column etc 
//------------------Shared Variables for POINTS----------------
unsigned static char points= 0;
unsigned static char start= 0;
unsigned char rightButtonPressed;
//------------------End Shared Variables----------------

typedef enum song_states{song_start, song_play, song_done, song_waitRelease, song_waitPress, song_waitRelease2 } song_states;
int song_Update(int state){
	double b = 	246.94	;
double c = 277.18	;
double d = 293.66	;
double e = 329.63;
double f = 369.99	;
double g = 415.30;
double a2= 440.00;
double b2 =493.88	;
double c2 =554.37	;
double d2 =587.33	;
double e2 =659.25	;
double f2 =	739.99;
double g2 =830.61	;

	unsigned char a0 = (~PINA & 0x0F) & 0x01;
	unsigned char a1 = ((~PINA & 0x0F) & 0x02)>>1;
	//unsigned char a2 = ((~PINA & 0x0F) & 0x04)>>2;
	static unsigned char time = 0x00;
	//unsigned char zero = ((~PINA & 0x0F) == 0x00);                         //f e c b a e c b a
	double arr[72] = {a2,f2,e2,c2,b2,    a2,e2,c2,b2,a2,    0,
		f2,e2,c2,b2,a2,    b2,a2,b2,a2,e2,    c,b2,a2,0,
		f2, e2, c2, b2,    a2, e2, c2, b2, a2,    0,
		f2,e2,c2,b2,a2,    b2,a2,b2,a2,e2,    c,b2,a2,0,
		e,f,c2,b2,f2,    e2,c2,b2,a2,0,   f2,e2,c2,b2,a2,    b2,a2,b2,a2,e2,    c2,b2,a2}; //15
	double arr1[72] = {2,1,2,2,2,   2,2,1,2,2,    2,
		1,1,1,1,1,   1,1,1,1,1,    1,1,2,2, //1 2 2 2 2    2 1 1 2 2 
		1,2,2,2,   2,2,1,2,2,    2,
		1,1,1,1,1,   1,1,1,1,1,    1,1,2,2,
		1,2,2,2,2,   2,1,2,2,2,    1,1,1,1,1,   1,1,1,1,1,  1,1,1 };
	static unsigned char index=0;
	static double freq=0;
	if(!start){
		return state;
	}
	
	
	switch (state) { //transitions
		case song_start:
			//if((~PINA) & 0x01){
				state = song_play;
				time = 0x00;
			//}
			break;
		case song_play:
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
					state = song_done;	
				}
				break;
			}
			break;
		case song_done:
			freq = 0;
			if(a0){
				state = song_waitRelease;
			}
			break;
		case song_waitRelease:
			if(!a0){
				state = song_waitPress;
			}
			break;
		case song_waitPress:
			if(a0){
				state = song_waitRelease2;
			}
			break;
		case song_waitRelease2:
			if(!a0){
				index = 0;
				state = song_play;
			}
			break;
		default:
			break;
	}
	set_PWM(freq);
	return state;
	
}
/**
 * This function moves the dots down by 1 row
 * This should be updated at the same speed as the display.
 **/
typedef enum updateColumnsStates{updateColumns_start, updateColumns_next} updateColumnsStates;
int updateColumns(int state){
	if(!start){
		return state;
	}
	static unsigned char time = 0;
	//function for adding new column here
	switch (state) { 
		case updateColumns_start:
			columnOutput = 0b11111110;
			state= updateColumns_next;
			break;
		case updateColumns_next:
			
			break;
		default:
			break;
	}
	switch (state) { 
		case updateColumns_start:
			state= updateColumns_next;
			break;
		case updateColumns_next:
			
			if(columnIndex>7){
				columnIndex=0;
			}
			columnOutput= col[columnIndex];
			columnIndex++;
			break;
		default:
			break;
	}
	
	if(time >= 8){
			if(songIndex>=songSize){
				songIndex=0;
			}
			rowOutput= row[song[songIndex]];
			songIndex++;
			time = 0;
	}
	time++;
	return state;
}
/**
 * This function adds a dot at the top 
 **/
typedef enum startButtonStates{updateStart_start, updateStart_next} startButtonStates;
int updateStart(int state){
	//function for adding new column here
	switch (state) { 
		case updateStart_start:
			if( startButton ){
				start = 0x01;
				PORTB = 0x01;
			}
			break;
		case updateStart_next:
			
			break;
		default:
			break;
	}
	return state;
}
//period 200
typedef enum scoreStates{score_start} scoreStates;
int scoreUpdate(int state){
	if(!start){
		return state;
	}
	static unsigned char time = 0;
	if(time >= 8){
		time = 0;
	}
	if (time == 6){
		PORTB = 0x01;
	}
	else{
		PORTB = 0x00;
	}
	
	time++;
	
	
	return state;
}

typedef enum displayStates{display_start} displayStates;
int displayUpdate(int state){
	
	switch (state) { 
		case display_start:
			transmit_data(columnOutput);
			PORTD= rowOutput;
			break;
		default:
			break;
	}
	return state;
}

int main(void) {
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;
	DDRC = 0xFF; PORTC = 0x00;
	DDRD = 0xFF; PORTD = 0x00;
	
	static task task1, task2, task4, task5, task3;
	task *tasks[] = {&task1, &task2, &task4, &task5, &task3};
	const unsigned short numTasks = sizeof(tasks)/sizeof(task*);
	
	//task 1
	task1.state = updateColumns_start;
	task1.period = 200;
	task1.elapsedTime = task1.period;
	task1.TickFct = &updateColumns; 
	
	
	//task 2
	task2.state = updateStart_start;
	task2.period = 20;
	task2.elapsedTime = task2.period;
	task2.TickFct = &updateStart; 
	
	//task4
	task4.state = score_start;
	task4.period = 200;
	task4.elapsedTime = task4.period;
	task4.TickFct = &scoreUpdate; 
	
	//task5
	task5.state = song_play;
	task5.period = 100;
	task5.elapsedTime = task5.period;
	task5.TickFct = &song_Update; 
	
	
	//task 3
	task3.state = display_start;
	task3.period = 20;
	task3.elapsedTime = task3.period;
	task3.TickFct = &displayUpdate; 
	
	
	TimerSet(10);
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

	
	while (1) {

		unsigned char LR, UD;
		
		unsigned short LRTemp, UDTemp;
		
		LRTemp = adc_read(0);
        LR = ADC_Scaler(1200, LRTemp, 3);
        UDTemp =  adc_read(1);
        UD = ADC_Scaler(1200, UDTemp, 3);

        unsigned char output = 0x00;
        if((UDTemp <= 200)){ //down 
			output |= 0x01;
		}
		if((UDTemp > 600)){
			output |= 0x02;
		}
		if(LR > 2){ //right
			output |= 0x04;
		}
		if(LR < 2){ //left 
			output |= 0x08;
		}
        PORTB = output;
        
		/**
		for(i = 0; i<numTasks; i++){
			if(tasks[i]->elapsedTime == tasks[i]->period){
				tasks[i]->elapsedTime = tasks[i]->TickFct(tasks[i]->state);
				tasks[i]->elapsedTime = 0;
			}
			tasks[i]->elapsedTime +=10;
		}
		while(!TimerFlag);
		TimerFlag = 0;**/
		
		
		
	}
    
    return 0;
}
