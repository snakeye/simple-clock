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

uint8_t decode_hour(uint8_t hour)
{
	if((hour & 0b01000000)){
		// 12 hour mode
		uint8_t h = bcd2dec(hour & 0b00011111);
		if((hour & 0b00100000)) {
			//h += 12;
		}
		return h;
	}
	else {
		// 24 hour mode
		return bcd2dec(hour & 0b00111111);
	}
}

void ds3231_get_time(uint8_t* hour, uint8_t* minute)
{
	uint8_t data[2];
	
	i2c_start(ADDR, TW_WRITE);
	i2c_write(0x01);
	i2c_stop();
	
	i2c_start(ADDR, TW_READ);
	i2c_read(&data[0], NACK);
	i2c_stop();

	i2c_start(ADDR, TW_WRITE);
	i2c_write(0x02);
	i2c_stop();
	
	i2c_start(ADDR, TW_READ);
	i2c_read(&data[1], NACK);
	i2c_stop();
	
	// decode
	*minute = bcd2dec(data[0]);
	*hour = bcd2dec(data[1]);
}

time* ds3231_get_date_time()
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
	tm.sec = bcd2dec(data[0]);
	tm.min = bcd2dec(data[1]);
	tm.hour = decode_hour(data[2]);
	tm.day = bcd2dec(data[4]);
	tm.month = bcd2dec(data[5] & 0x1f);
	tm.year = bcd2dec(data[6]);
	
	return &tm;
}

int8_t ds3231_get_temperature()
{
	uint8_t data[2];
	
	i2c_start(ADDR, TW_WRITE);
	i2c_write(0x11);
	i2c_stop();
	
	i2c_start(ADDR, TW_READ);
	for(uint8_t i = 0; i < 2; i++){
		i2c_read(&data[i], NACK);
	}
	i2c_stop();
	
	return data[0];
}

void ds3231_set_control_register(uint8_t reg)
{
	i2c_start(ADDR, TW_WRITE);
	i2c_write(0x0E);
	i2c_write(reg);
	i2c_stop();
}