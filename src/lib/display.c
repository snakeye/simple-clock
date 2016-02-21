/*
* display.c
*
* Created: 21.02.2016 17:08:31
*  Author: snakeye
*/
#include <avr/io.h>

#include "max7219.h"
#include "charset.h"

#include "display.h"

uint8_t display_digits[8] = {0};

#define SET_BIT(val, bit) (val |= (1 << (bit)))
#define CLR_BIT(val, bit) (val &= ~(1 << (bit)))

void display_init()
{
	// enable port
	DDRB |= (1 << MAX7219_DIN) | (1 << MAX7219_CLK) | (1 << MAX7219_LOAD);

	// set initial values
	max7219_write(MAX7219_REG_DECODE, 0);
	max7219_write(MAX7219_REG_SCAN, 7);
	max7219_write(MAX7219_REG_TEST, 0);
	max7219_write(MAX7219_REG_SHUTDOWN, 1);
	
	// set brightness
	max7219_write(MAX7219_REG_BRIGHTNESS, 0x04);

	display_clear();
}

void display_clear()
{
	for(uint8_t i = 0; i < 8; i ++){
		display_digits[i] = 0;
		max7219_write(i + 1, 0);
	}
}

void display_set_char(uint8_t position, char ch)
{
	uint8_t shift = 6 - position;
	uint8_t bitmask = charset[ch - 32];
	
	for(uint8_t pos = 0; pos < 8; pos++) {
		
		if((bitmask & (1 << pos))) {
			SET_BIT(display_digits[pos], shift);
		}
		else {
			CLR_BIT(display_digits[pos], shift);
		}
		
		max7219_write(pos + 1, display_digits[pos]);
	}
}

void display_set_dot(uint8_t position, uint8_t enabled)
{
	uint8_t shift = 6 - position;
	
	if(enabled) {
		SET_BIT(display_digits[6], shift);
	}
	else {
		CLR_BIT(display_digits[6], shift);
	}
	
	max7219_write(7, display_digits[6]);
}

void display_set_dots(uint8_t enabled)
{
	if(enabled) {
		SET_BIT(display_digits[7], 2);
	}
	else {
		CLR_BIT(display_digits[7], 2);
	}
	
	max7219_write(8, display_digits[7]);
}
