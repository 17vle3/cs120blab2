
#ifndef ADC_H_H_
#define ADC_H_H_

#define F_CPU 8000000UL					/* Define CPU Frequency e.g. here its 8 Mhz */
#include <avr/io.h>						/* Include AVR std. library file */
#include <util/delay.h>
#define ipin PINA
#define iport PORTA

/*void ADC_Init();
int ADC_Read(char);*/

void ADC_init();
void ADC_channel(unsigned char channel);
unsigned short ADC_read(unsigned char channel);

#endif /* ADC_H_H_ */
