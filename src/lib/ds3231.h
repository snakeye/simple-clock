/*
* ds3231.h
*
* Created: 21.02.2016 17:56:45
*  Author: snakeye
*/
#ifndef DS3231_H_
#define DS3231_H_

typedef struct {
	uint8_t sec;
	uint8_t min;
	uint8_t hour;
	uint8_t day;
	uint8_t month;
	uint8_t year;
} time;

time* ds3231_time();

#endif /* DS3231_H_ */