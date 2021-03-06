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

uint8_t encode_hour(uint8_t hour) {
	return dec2bcd(hour);
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

uint8_t ds3231_get_second() {
	uint8_t data;

	i2c_start(ADDR, TW_WRITE);
	i2c_write(0x00);
	i2c_stop();
	
	i2c_start(ADDR, TW_READ);
	i2c_read(&data, NACK);
	i2c_stop();
	
	// decode
	return bcd2dec(data);
}

uint8_t ds3231_get_minute() {
	uint8_t data;

	i2c_start(ADDR, TW_WRITE);
	i2c_write(0x01);
	i2c_stop();
	
	i2c_start(ADDR, TW_READ);
	i2c_read(&data, NACK);
	i2c_stop();
	
	// decode
	return bcd2dec(data);
}

uint8_t ds3231_get_hour() {
	uint8_t data;

	i2c_start(ADDR, TW_WRITE);
	i2c_write(0x02);
	i2c_stop();
	
	i2c_start(ADDR, TW_READ);
	i2c_read(&data, NACK);
	i2c_stop();
	
	// decode
	return decode_hour(data);
}

uint8_t ds3231_get_day() {
	uint8_t data;

	i2c_start(ADDR, TW_WRITE);
	i2c_write(0x04);
	i2c_stop();
	
	i2c_start(ADDR, TW_READ);
	i2c_read(&data, NACK);
	i2c_stop();
	
	// decode
	return bcd2dec(data);
}

uint8_t ds3231_get_month() {
	uint8_t data;

	i2c_start(ADDR, TW_WRITE);
	i2c_write(0x05);
	i2c_stop();
	
	i2c_start(ADDR, TW_READ);
	i2c_read(&data, NACK);
	i2c_stop();
	
	// decode
	return bcd2dec(data);
}

uint8_t ds3231_get_year() {
	uint8_t data;

	i2c_start(ADDR, TW_WRITE);
	i2c_write(0x06);
	i2c_stop();
	
	i2c_start(ADDR, TW_READ);
	i2c_read(&data, NACK);
	i2c_stop();
	
	// decode
	return bcd2dec(data);
}


time* ds3231_get_date_time()
{
	static uint8_t data[7] = {0};
	
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

uint8_t ds3231_get_status() {
	i2c_start(ADDR, TW_WRITE);
	i2c_write(0x0F);
	i2c_stop();
	
	uint8_t reg = 0;
	
	i2c_start(ADDR, TW_READ);
	i2c_read(&reg, ACK);
	i2c_stop();
	
	return reg;
}

void ds3231_set_second(uint8_t second) {
	i2c_start(ADDR, TW_WRITE);
	i2c_write(0x00);
	i2c_write(dec2bcd(second));
	i2c_stop();
}

void ds3231_set_minute(uint8_t minute) {
	i2c_start(ADDR, TW_WRITE);
	i2c_write(0x01);
	i2c_write(dec2bcd(minute));
	i2c_stop();
}

void ds3231_set_hour(uint8_t hour) {
	i2c_start(ADDR, TW_WRITE);
	i2c_write(0x02);
	i2c_write(dec2bcd(hour));
	i2c_stop();
}

void ds3231_set_day(uint8_t day) {
	i2c_start(ADDR, TW_WRITE);
	i2c_write(0x04);
	i2c_write(dec2bcd(day));
	i2c_stop();
}

void ds3231_set_month(uint8_t month) {
	i2c_start(ADDR, TW_WRITE);
	i2c_write(0x05);
	i2c_write(dec2bcd(month));
	i2c_stop();
}

void ds3231_set_year(uint8_t year) {
	i2c_start(ADDR, TW_WRITE);
	i2c_write(0x06);
	i2c_write(dec2bcd(year));
	i2c_stop();
}