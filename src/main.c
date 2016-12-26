/*
* src.c
*
* Created: 20.02.2016 14:08:57
*  Author: snakeye
*/
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <util/delay.h>

#include "lib/i2c.h"
#include "lib/display.h"
#include "lib/ds3231.h"

#include "clock/brightness.h"
#include "clock/buttons.h"
#include "clock/watchdog.h"

typedef enum {
	MODE_DISPLAY = 0,
	MODE_SETTINGS = 1
} DISPLAY_MODE;

typedef enum {
	//
	SUBMODE_TIME = 0,
	SUBMODE_DATE = 1,
	SUBMODE_YEAR = 2,
	SUBMODE_SECOND = 3,
	SUBMODE_TEMP = 4,
	//
	SUBMODE_SET_HOUR = 5,
	SUBMODE_SET_MINUTE = 6,
	SUBMODE_SET_SECOND = 7,
	SUBMODE_SET_DAY = 8,
	SUBMODE_SET_MONTH = 9,
	SUBMODE_SET_YEAR = 10,
	SUBMODE_SET_AUTO_BRIGHTNESS = 11,
	SUBMODE_SET_MAX_BRIGHTNESS = 12
} DISPLAY_SUBMODE;

DISPLAY_MODE display_mode = MODE_DISPLAY;
DISPLAY_SUBMODE display_submode = SUBMODE_TIME;

uint8_t brightness = 0;
uint8_t target_brightness = 0;

uint16_t sqw = 0;

ISR( INT0_vect) {
	
}

ISR( INT1_vect ) {
	sqw = (sqw + 1) % 1024;
}

void initInterrupts(void) {	
	EIMSK |= (1 << INT0) | ( 1 << INT1 );        // Enable both interrupts
	EICRA |= (1 << ISC01) | ( 1 << ISC11 );      // Falling edge on both
	
	PORTD |= ( 1 << PORTD2 ) | ( 1 << PORTD3 ); // Pull up resistors
	
	sei();
}

void init()
{
	//cli();
	sei();

	//
	display_init();

	//
	i2c_init();
	
	// initialize ADC
	adc_init(ADC0D);
	
	// Start button processing
	buttons_init();
	
	initInterrupts();
	
	// Start watchdog timer
	//watchdog_init();
	
	target_brightness = get_brightness();
	brightness = target_brightness;
}

void print(uint16_t val)
{
	for(uint8_t i = 0; i < DISPLAY_DIGITS; i++) {
		uint8_t digit = val % 10;
		val = val / 10;
		
		display_set_char(3 - i, '0' + digit);
	}
}

void print_hex(uint16_t val) {
	static char chars[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
	for(uint8_t i = 0; i < DISPLAY_DIGITS; i++) {
		uint8_t digit = val % 16;
		val = val / 16;
		
		display_set_char(3 - i, chars[digit]);
	}
}

void print_time(uint8_t hour, uint8_t minute)
{
	if (hour == 255) {
		display_set_char(0, ' ');
		display_set_char(1, ' ');
	}
	else {
		uint8_t hl = hour % 10;
		uint8_t hh = (hour / 10) % 10;
		
		display_set_char(0, hh > 0 ? '0' + hh : ' ');
		display_set_char(1, '0' + hl);
	}
	
	if (minute == 255) {
		display_set_char(2, ' ');
		display_set_char(3, ' ');
	}
	else {
		uint8_t ml = minute % 10;
		uint8_t mh = (minute / 10) % 10;

		display_set_char(2, '0' + mh);
		display_set_char(3, '0' + ml);
	}
}

void print_seconds(uint8_t seconds) {
	display_set_char(0, ' ');
	display_set_char(1, ' ');

	if (seconds == 255) {
		display_set_char(2, ' ');
		display_set_char(3, ' ');
	}
	else {
		uint8_t sl = seconds % 10;
		uint8_t sh = (seconds / 10) % 10;

		display_set_char(2, '0' + sh);
		display_set_char(3, '0' + sl);
	}
}

void print_temp(int8_t temp) {
	uint8_t tl = temp % 10;
	uint8_t th = (temp / 10) % 10;

	display_set_char(0, temp < 0 ? '-' : ' ');

	display_set_char(1, '0' + th);
	display_set_char(2, '0' + tl);
	
	display_set_char(3, '*');
}

// button 1 cycles display modes
void on_button_0(){
	if(display_mode == MODE_DISPLAY){
		switch(display_submode) {
			case SUBMODE_TIME:
			display_submode = SUBMODE_SECOND;
			break;
			case SUBMODE_SECOND:
			display_submode = SUBMODE_DATE;
			break;
			case SUBMODE_DATE:
			display_submode = SUBMODE_YEAR;
			break;
			case SUBMODE_YEAR:
			display_submode = SUBMODE_TEMP;
			break;
			case SUBMODE_TEMP:
			display_submode = SUBMODE_TIME;
			break;
			default: 
			break;
		}
	}
	else if(display_mode == MODE_SETTINGS) {
		switch(display_submode){
			case SUBMODE_SET_HOUR:
			display_submode = SUBMODE_SET_MINUTE;
			break;
			case SUBMODE_SET_MINUTE:
			display_submode = SUBMODE_SET_SECOND;
			break;
			case SUBMODE_SET_SECOND:
			display_submode = SUBMODE_SET_DAY;
			break;
			case SUBMODE_SET_DAY:
			display_submode = SUBMODE_SET_MONTH;
			break;
			case SUBMODE_SET_MONTH:
			display_submode = SUBMODE_SET_YEAR;
			break;
			case SUBMODE_SET_YEAR:
			display_submode = SUBMODE_SET_HOUR;
			break;
			default: 
			break;
		}
	}
}

// button 0 changes values
void on_button_1(){
	if(display_mode == MODE_SETTINGS) {
		switch(display_submode){
			case SUBMODE_SET_HOUR:
			ds3231_set_hour((ds3231_get_hour() + 1) % 24);
			break;
			case SUBMODE_SET_MINUTE:
			ds3231_set_minute((ds3231_get_minute() + 1) % 60);
			break;
			case SUBMODE_SET_SECOND:
			ds3231_set_second(0);
			break;
			case SUBMODE_SET_DAY:
			ds3231_set_day(1 + ((ds3231_get_day() + 1) % 31));
			break;
			case SUBMODE_SET_MONTH:
			ds3231_set_month(1 + ((ds3231_get_month() + 1) % 12));
			break;
			case SUBMODE_SET_YEAR:
			ds3231_set_year((ds3231_get_year() + 1) % 25);
			break;
			default:
			break;
		}
	}
}

void on_button_press(uint8_t button) {
}

void on_button_release(uint8_t button) {
}

void on_button_hold(uint8_t button)
{
	if(button == 0) {
		if(display_mode == MODE_DISPLAY) {
			display_mode = MODE_SETTINGS;
			display_submode = SUBMODE_SET_HOUR;
		}
		else {
			display_mode = MODE_DISPLAY;
			display_submode = SUBMODE_TIME;
		}
	}
}

void on_button_click(uint8_t button) {
	switch(button) {
		case 0:
		on_button_0();
		break;
		case 1:
		on_button_1();
		break;
	}
}

void update_brightness(uint16_t tick)
{
	if ((tick % 20) == 0){
		// start measuring brightness
		measure_brightness_async();
	}
	
	if((tick % 20) == 10){
		// read measurement
		target_brightness = get_brightness();
	}
	
	if((tick % 5) == 0) {
		// update display brightness
		if(brightness > target_brightness) {
			brightness -= 1;
			display_set_brightness(brightness);
		}
		else if (brightness < target_brightness) {
			brightness += 1;
			display_set_brightness(brightness);
		}
	}
}

void update_display(uint16_t tick)
{
	uint8_t y = sqw / 512;
    uint8_t y2 = (sqw / 256) % 2;

	if(display_mode == MODE_DISPLAY){
		switch(display_submode) {
			case SUBMODE_TIME:
			print_time(ds3231_get_hour(), ds3231_get_minute());
			display_set_dots(y);
			break;
			case SUBMODE_SECOND:
			print_seconds(ds3231_get_second());
			display_set_dots(1);
			break;
			case SUBMODE_DATE:
			print_time(ds3231_get_day(), ds3231_get_month());
			display_set_dot(1, 1);
			display_set_dots(0);
			break;
			case SUBMODE_YEAR:
			print(2000 + ds3231_get_year());
			display_set_dots(0);
			break;
			case SUBMODE_TEMP:
			print_temp(ds3231_get_temperature());
			display_set_dots(0);
			break;
			default: 
			break;
		}
	}
	else if(display_mode == MODE_SETTINGS) {
		switch(display_submode){
			case SUBMODE_SET_HOUR:
			print_time(y2 ? ds3231_get_hour() : -1, ds3231_get_minute());
			display_set_dots(y);
			break;
			case SUBMODE_SET_MINUTE:
			print_time(ds3231_get_hour(), y2 ? ds3231_get_minute() : -1);
			display_set_dots(y);
			break;
			case SUBMODE_SET_SECOND:
			print_seconds(y2 ? ds3231_get_second() : -1);
			display_set_dots(1);
			break;
			case SUBMODE_SET_DAY:
			print_time(y2 ? ds3231_get_day() : -1, ds3231_get_month());
			display_set_dot(1, 1);
			display_set_dots(0);
			break;
			case SUBMODE_SET_MONTH:
			print_time(ds3231_get_day(), y2 ? ds3231_get_month() : -1);
			display_set_dot(1, 1);
			display_set_dots(0);
			break;
			case SUBMODE_SET_YEAR:
			print_time(20, y2 ? ds3231_get_year() : -1);
			display_set_dots(0);
			break;
			default:
			break;
		}
	}
}

int main(void)
{
	init();
	
	//display_set_dots(0);
	
	//DDRD &= ~((1 << PORTD3));

	// enable 1024Hz square wave
	ds3231_set_control_register(1 << DS3231_RS1);
	
	uint16_t tick = 0;
	
	while(1)
	{
		//
		buttons_tick(tick);
		
		//
		update_display(tick);

		// brightness
		update_brightness(tick);
		
		// rest a bit
		_delay_us(1);
		//_delay_ms(1);
		tick = (tick + 1) % 100;

		// reset watchdog timer
		wdt_reset();
	}
	
	return 0;
}
