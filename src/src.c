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

// Registers:
// 0 - Seg A
// 1 - Seg B
// 2 - Seg C
// 3 - Seg D
// 4 - Seg F
// 5 - Seg G
// 6 - Point
// 7 - Seg E
uint8_t charset[] = {
	//
	0b00000000,
	// !
	0b00000000,
	// "
	0b00000000,
	// #
	0b00000000,
	// $
	0b00000000,
	// %
	0b00000000,
	// &
	0b00000000,
	// '
	0b00000000,
	// (
	0b00000000,
	// )
	0b00000000,
	// *
	0b00000000,
	// +
	0b00000000,
	// ,
	0b00000000,
	// -
	0b00000000,
	// .
	0b00000000,
	// /
	0b00000000,
	// 0
	0b10011111,
	// 1
	0b00000110,
	// 2
	0b10101011,
	// 3
	0b00101111,
	// 4
	0b00110110,
	// 5
	0b00111101,
	// 6
	0b10111101,
	// 7
	0b00000111,
	// 8
	0b10111111,
	// 9
	0b00111111,
	// :
	0b00000000,
	// ;
	0b00000000,
	// <
	0b00000000,
	// =
	0b00000000,
	// >
	0b00000000,
	// ?
	0b00000000,
	// @
	0b00000000,
	// A
	0b00000000,
	// B
	0b00000000,
	// C
	0b00000000,
	// D
	0b00000000,
	// E
	0b00000000,
	// F
	0b00000000,
	// G
	0b00000000,
	// H
	0b00000000,
	// I
	0b00000000,
	// J
	0b00000000,
	// K
	0b00000000,
	// L
	0b00000000,
	// M
	0b00000000,
	// N
	0b00000000,
	// O
	0b00000000,
	// P
	0b00000000,
	// Q
	0b00000000,
	// R
	0b00000000,
	// S
	0b00000000,
	// T
	0b00000000,
	// U
	0b00000000,
	// V
	0b00000000,
	// W
	0b00000000,
	// X
	0b00000000,
	// Y
	0b00000000,
	// Z
	0b00000000,
	// [
	0b00000000,
	// backslash
	0b00000000,
	// ]
	0b00000000,
	// ^
	0b00000000,
	// _
	0b00000000,
	// `
	0b00000000,
	// a
	0b00000000,
	// b
	0b00000000,
	// c
	0b00000000,
	// d
	0b00000000,
	// e
	0b00000000,
	// f
	0b00000000,
	// g
	0b00000000,
	// h
	0b00000000,
	// i
	0b00000000,
	// j
	0b00000000,
	// k
	0b00000000,
	// l
	0b00000000,
	// m
	0b00000000,
	// n
	0b00000000,
	// o
	0b00000000,
	// p
	0b00000000,
	// q
	0b00000000,
	// r
	0b00000000,
	// s
	0b00000000,
	// t
	0b00000000,
	// u
	0b00000000,
	// v
	0b00000000,
	// w
	0b00000000,
	// x
	0b00000000,
	// y
	0b00000000,
	// z
	0b00000000,
	// {
	0b00000000,
	// |
	0b00000000,
	// }
	0b00000000,
	// ~
	0b00000000,
	//
	0b00000000,
};

uint8_t display_digits[8] = {0};

#define SET_BIT(val, bit) (val |= (1 << (bit)))
#define CLR_BIT(val, bit) (val &= ~(1 << (bit)))

void display_set_char(uint8_t position, char ch)
{
	uint8_t shift = 6 - position;
	uint8_t bitmask = charset[ch - 32];
		
	for(uint8_t pos = 0; pos < 8; pos++) {
		
		if((bitmask & (1 << pos))){
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
		display_digits[6] |= (1 << shift);
	}
	else {
		display_digits[6] &= ~(1 << shift);
	}
	
	max7219_write(7, display_digits[6]);
}

void display_set_dots(uint8_t enabled)
{
	if(enabled) {
		display_digits[7] |= (1 << 2);
	}
	else {
		display_digits[7] &= ~(1 << 2);
	}
	
	max7219_write(8, display_digits[7]);
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
	
	max7219_write(MAX7219_REG_BRIGHTNESS, 0x04);

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
		
	uint16_t val = 0;
	
	while(1)
	{
		uint16_t tmp = val;
		for(uint8_t i = 0; i < 4; i++) {
			uint8_t digit = tmp % 10;
			tmp = tmp / 10;
			
			display_set_char(3 - i, '0' + digit);	
		}
	
		val += 1;
		if(val > 9999) {
			val = 0;
		}

		_delay_ms(50);
	}
}
