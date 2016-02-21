/*
* ds3231.c
*
* Created: 21.02.2016 17:57:00
*  Author: snakeye
*/
#include <avr/io.h>

#include "i2c.h"
#include "ds3231.h"

#define ADDR 0xD0

time tm;

// Convert Decimal to Binary Coded Decimal (BCD)
uint8_t dec2bcd (uint8_t num)
{
	return ((num / 10 * 16) + (num % 10));
}

// Convert Binary Coded Decimal (BCD) to Decimal
uint8_t bcd2dec (uint8_t num)
{
	return ((num / 16 * 10) + (num % 16));
}

time* ds3231_time()
{
	uint8_t data[7];
	
	i2c_start(ADDR, TW_WRITE);
	i2c_write(0x00);
	i2c_stop();
	
	i2c_start(ADDR, TW_READ);
	for(uint8_t i = 0; i < 7; i++){
		i2c_read(&data[i], NACK);
	}
	i2c_stop();
	
	// decode
	tm.sec = bcd2dec(data[0] & 0x7F);
	
	return &tm;
}
