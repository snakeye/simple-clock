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

void ds3231_get_time(uint8_t* hour, uint8_t* minute);
time* ds3231_get_date_time();
int8_t ds3231_get_temperature();

void ds3231_set_sqw_rate(uint8_t rate);	

#endif /* DS3231_H_ */