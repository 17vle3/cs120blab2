/*	Author: vle018
 *	Lab Section:
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#include "io.h"

#define zero ~PINA & 0x01
#define one (~PINA & 0x02)>>1
#define two (~PINA & 0x04)>>2
#define three (~PINA & 0x08)>>3
#define startButton (~PINA & 0x10)>>4

/**-------------------------------for shift register--------------------------------------------**/
#define HC595_PORT   PORTD
#define HC595_DDR    DDRD

#define HC595_DS_POS ~PIND & 0x01      //Data pin (DS) pin location

#define HC595_SH_CP_POS PB1 (~PIND & 0x02)>>1     //Shift Clock (SH_CP) pin location 
#define HC595_ST_CP_POS PB2  (~PIND & 0x04)>>2
 
void HC595Init()
{
   //Make the Data(DS), Shift clock (SH_CP), Store Clock (ST_CP) lines output
   HC595_DDR|=((1<<HC595_SH_CP_POS)|(1<<HC595_ST_CP_POS)|(1<<HC595_DS_POS));
}
#define HC595DataHigh() (HC595_PORT|=(1<<HC595_DS_POS))

#define HC595DataLow() (HC595_PORT&=(~(1<<HC595_DS_POS)))

//Sends a clock pulse on SH_CP line
void HC595Pulse()
{
   //Pulse the Shift Clock

   HC595_PORT|=(1<<HC595_SH_CP_POS);//HIGH

   HC595_PORT&=(~(1<<HC595_SH_CP_POS));//LOW

}

//Sends a clock pulse on ST_CP line
void HC595Latch()
{
   //Pulse the Store Clock

   HC595_PORT|=(1<<HC595_ST_CP_POS);//HIGH
   _delay_loop_1(1);

   HC595_PORT&=(~(1<<HC595_ST_CP_POS));//LOW
   _delay_loop_1(1);
}
void HC595Write(uint8_t data)
{
   //Send each 8 bits serially

   //Order is MSB first
   for(uint8_t i=0;i<8;i++)
   {
      //Output the data on DS line according to the
      //Value of MSB
      if(data & 0b10000000)
      {
         //MSB is 1 so output high

         HC595DataHigh();
      }
      else
      {
         //MSB is 0 so output high
         HC595DataLow();
      }

      HC595Pulse();  //Pulse the Clock line
      data=data<<1;  //Now bring next bit at MSB position

   }

   //Now all 8 bits have been transferred to shift register
   //Move them to output latch at one
   HC595Latch();
}

/**-------------------------------for shift register--------------------------------------------**/


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
//------------------Shared Variables for MATRIX----------------
unsigned char col[8] = {0b11111110,0b11111101,0b11111011,0b11110111,0b11101111,0b11011111,0b10111111,0b01111111};
unsigned char row[4] = {0b01100010,0b01100100, 0b01101000,0b01110000, 0b01111110 }; //0 1 2 3 all
static unsigned char columnIndex =0;
unsigned char columnOutput = 0x00; //THE PORTC OUTPUT
static unsigned char rowOutput = 0x00; //THE PORTD OUTPUT
unsigned char colCheck[8] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};
unsigned const char song[] = {4, 0 ,1,2,3}; //possible 0 1 2 3 01 02 12 13 23 012 etc 
unsigned const char songSize = 5;
static unsigned char songIndex = 0;
static unsigned char existingColumns = 0; //which columns exist right now in binary 0x00 = none 0x01 = top column 0x02 = second column etc 
//------------------Shared Variables for POINTS----------------
unsigned static char points= 0;
unsigned static char start= 0;
//------------------End Shared Variables----------------
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
			HC595Write(columnOutput);
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
	
	static task task1, task2, task4, task3;
	task *tasks[] = {&task1, &task2, &task4, &task3};
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
	
	//task 3
	task3.state = display_start;
	task3.period = 20;
	task3.elapsedTime = task3.period;
	task3.TickFct = &displayUpdate; 
	
	
	TimerSet(10);
	TimerOn();
	//PWM_on();
	
	
	LCD_init();
	int i;
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
