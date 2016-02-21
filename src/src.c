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
#include "lib/ds3231.h"

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
	for(uint8_t i = 0; i < DISPLAY_DIGITS; i++) {
		uint8_t digit = val % 10;
		val = val / 10;
		
		display_set_char(3 - i, '0' + digit);
	}
}

int main(void)
{
	init();
	
	display_set_dots(1);
	
	while(1)
	{
		time* tm = ds3231_time();
		
		print(tm->sec);
		
		_delay_ms(20);
	}
	
	return 0;
}
