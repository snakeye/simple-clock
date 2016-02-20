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
#include "lib/max7219.h"

void set_brightness(uint8_t brightness)
{
	 //display_push_cmd(i, MAX7219_REG_BRIGHTNESS, brightness);
}

void init()
{
	//cli();
	//sei();
	
	// init display
	DDRB |= (1 << MAX7219_DIN) | (1 << MAX7219_CLK) | (1 << MAX7219_LOAD);

	// set initial values		
	max7219_write(MAX7219_REG_DECODE, 0);
	max7219_write(MAX7219_REG_SCAN, 7);
	max7219_write(MAX7219_REG_TEST, 0);
	max7219_write(MAX7219_REG_SHUTDOWN, 1);	
	
	max7219_write(MAX7219_REG_BRIGHTNESS, 0x08);

	// clear display
    for(uint8_t i = 1; i <= 8; i ++){
		max7219_write(i, 0);
	}
	
	//
	i2c_init();
}

int main(void)
{
	init();
		
    while(1)
    {
    }
}
