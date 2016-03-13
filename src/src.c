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
	MODE_TIME,
	MODE_DATE,
	MODE_YEAR,
	MODE_WEEKDAY,
	MODE_TEMP,
	MODE_SET_HOUR,
	MODE_SET_MINUTE,
	MODE_SET_SECOND,
	MODE_SET_DAY,
	MODE_SET_MONTH,
	MODE_SET_YEAR
} DISPLAY_MODE;

DISPLAY_MODE display_mode = MODE_TIME;

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
}

void print(uint16_t val)
{
	for(uint8_t i = 0; i < DISPLAY_DIGITS; i++) {
		uint8_t digit = val % 10;
		val = val / 10;
		
		display_set_char(3 - i, '0' + digit);
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

void on_button_click(uint8_t button)
{
}

void on_button_hold(uint8_t button)
{
}

void on_button_release(uint8_t button)
{
	
}

int main(void)
{
	init();
	
	//display_set_dots(0);
	// enable 1024Hz square wave
	//ds3231_set_control_register((1 << DS3231_RS1));
		
	while(1)
	{
		//
		uint8_t hour, minute;
		ds3231_get_time(&hour, &minute);
		print_time(hour, minute);
		//print(bStatus);

		/////////////////////////////////////////////////////////////////////

		//
		buttons_tick();
		
		// brightness
		measure_brightness_async();
		display_set_brightness(get_brightness());
						
		// rest a bit
		_delay_ms(10);

		// reset watchdog timer
		wdt_reset();
	}
	
	return 0;
}
