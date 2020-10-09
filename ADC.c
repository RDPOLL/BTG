#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>
uint8_t read_adc(uint8_t refs)
{
	uint16_t result = 0;

	ADMUX = (1 << ADLAR) | (1 << MUX1) | (1 << refs);

	ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADSC);

	while (ADCSRA & (1 << ADSC)) continue;
			 
	result = ADCH;
				
	//~ result = result * 1294;												//Anzeige für mV
	//~ result = result / 100;
	//~ sprintf(string,"%04d mV", result);
	//~ glcd_draw_string_xy(10, 10, string);
	return result;
}
