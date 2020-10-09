#define F_CPU 16000000UL
#include <avr/io.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <inttypes.h>
#include "ili9341.h"
# include <avr/eeprom.h>
#include "ili9341gfx.h"
#include "ADC.h"
#include "I2C.h"
#include <avr/pgmspace.h>


#define LED_ROT_ON		PORTD |= (1<<PD0)
#define LED_ROT_OFF		PORTD &= ~(1<<PD0)
#define TST_DOWN		!(PINC&(1<<PC6))
int main(void)
{

	DDRD |= (1<<PD0);
	PORTD &= ~(1<<PD0);													// LED Rot
											
	DDRC &= ~(1<<PC6);
	PORTC |= (1<<PC6);
	
	DDRC &= ~(1<<PC7);
	PORTC |= (1<<PC7);

	
	
	while(1)
	{
		if(TST_DOWN)
		{
			LED_ROT_ON;
		}
	}
	
}//end of main



