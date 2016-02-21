/*
* src.c
*
* Created: 20.02.2016 14:08:57
*  Author: snakeye
*/
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "lib/i2c.h"
#include "lib/display.h"

void init()
{
	//cli();
	//sei();

	//	
	display_init();
	
	//
	i2c_init();
}

void print(uint16_t val) 
{
	for(uint8_t i = 0; i < 4; i++) {
		uint8_t digit = val % 10;
		val = val / 10;
		
		display_set_char(3 - i, '0' + digit);
	}	
}

int main(void)
{
	init();
	
	uint16_t val = 0;
	
	while(1)
	{
		print(val);
		
		val += 1;
		if(val > 9999) {
			val = 0;
		}

		//_delay_ms(50);
	}
}
