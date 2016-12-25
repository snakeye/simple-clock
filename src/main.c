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

uint8_t brightness;
uint8_t target_brightness;

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
	uint8_t hl = hour % 10;
	uint8_t hh = (hour / 10) % 10;
	
	display_set_char(0, hh > 0 ? '0' + hh : ' ');
	display_set_char(1, '0' + hl);
	
	uint8_t ml = minute % 10;
	uint8_t mh = (minute / 10) % 10;

	display_set_char(2, '0' + mh);
	display_set_char(3, '0' + ml);
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
			display_submode = SUBMODE_SET_AUTO_BRIGHTNESS;
			break;
			case SUBMODE_SET_AUTO_BRIGHTNESS:
			display_submode = SUBMODE_SET_MAX_BRIGHTNESS;
			break;
			case SUBMODE_SET_MAX_BRIGHTNESS:
			display_submode = SUBMODE_SET_HOUR;
			break;
		}
	}
}

// button 0 changes values
void on_button_1(){
	if(display_mode == MODE_SETTINGS) {
		switch(display_submode){
			case SUBMODE_SET_HOUR:
			break;
			case SUBMODE_SET_MINUTE:
			break;
			case SUBMODE_SET_SECOND:
			break;
			case SUBMODE_SET_DAY:
			break;
			case SUBMODE_SET_MONTH:
			break;
			case SUBMODE_SET_YEAR:
			break;
			case SUBMODE_SET_AUTO_BRIGHTNESS:
			break;
			case SUBMODE_SET_MAX_BRIGHTNESS:
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
	//print_time(display_mode, display_submode);
	//print_hex(button_status[0]);
	//print_time((PINC & (1 << PORTC1)) == 0, (PINC & (1 << PORTC2)) == 0);
    //print(tick);

   	//static uint8_t hour, minute;
	//ds3231_get_time(&hour, &minute);
	//print_time(display_mode, display_submode);

    time* tm = ds3231_get_date_time();
	print_time(tm->hour, tm->sec);

	display_set_dots((tick / 25) % 2);
}

int main(void)
{
	init();
	
	//display_set_dots(0);
	// enable 1024Hz square wave
	ds3231_set_control_register((1 << DS3231_RS1));
	
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
		//_delay_us(1);
		_delay_ms(1);
		tick = (tick + 1) % 100;

		// reset watchdog timer
		wdt_reset();
	}
	
	return 0;
}
