/*
* ds3231.h
*
* Created: 21.02.2016 17:56:45
*  Author: snakeye
*/
#ifndef DS3231_H_
#define DS3231_H_    

#define DS3231_A1IE 0
#define DS3231_A2IE 1
#define DS3231_INTCN 2
#define DS3231_RS1 3
#define DS3231_RS2 4
#define DS3231_CONV 5
#define DS3231_BBSQW 6
#define DS3231_EOSC 7

typedef struct {
	uint8_t sec;
	uint8_t min;
	uint8_t hour;
	uint8_t day;
	uint8_t month;
	uint8_t year;
} time;

void ds3231_get_time(uint8_t* hour, uint8_t* minute);

uint8_t ds3231_get_second();
uint8_t ds3231_get_minute();
uint8_t ds3231_get_hour();
uint8_t ds3231_get_day();
uint8_t ds3231_get_month();
uint8_t ds3231_get_year();

void ds3231_set_second(uint8_t second);
void ds3231_set_minute(uint8_t minute);
void ds3231_set_hour(uint8_t hour);
void ds3231_set_day(uint8_t day);
void ds3231_set_month(uint8_t month);
void ds3231_set_year(uint8_t year);

time* ds3231_get_date_time();

int8_t ds3231_get_temperature();

void ds3231_set_control_register(uint8_t reg);	
uint8_t ds3231_get_status();

#endif /* DS3231_H_ */