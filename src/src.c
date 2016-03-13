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
#include "lib/display.h"
#include "lib/ds3231.h"

#define MAX_BRIGHTNESS 0x0f

uint16_t brightness = 0x08;

/**
* ADC Interrupt handler
*/
ISR(ADC_vect)
{
	uint16_t br = (ADCL | (ADCH << 8));
	
	// average
	brightness = br;
}

void adc_init(uint8_t reg)
{
	// init ADC
	// VCC with capacitor on AREF
	// ADC0 as source
	ADMUX = (1 << REFS0) | (reg & 0b111);
	// ADC enable
	// prescale = 128
	// interrupt enable
	ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0)  | (1 << ADIE);
}

/**
* Start ADC conversion
*/
void measure_brightness_async()
{
	ADCSRA |= (1 << ADSC);
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
	adc_init(0);
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
	
	display_set_char(0, '0' + hh);
	display_set_char(1, '0' + hl);
	
	uint8_t ml = minute % 10;
	uint8_t mh = (minute / 10) % 10;	

	display_set_char(2, '0' + mh);
	display_set_char(3, '0' + ml);
}

int main(void)
{
	init();
	
	display_set_dots(0);
	
	uint8_t hour, minute;
	
	while(1)
	{
		ds3231_get_time(&hour, &minute);
		
		//
		//print_time(hour, minute);
		print(brightness);

		
		uint8_t b = (brightness * MAX_BRIGHTNESS) / 1024	;
		display_set_brightness(b);
		
		//
		measure_brightness_async();
		
		
		_delay_ms(100);
	}
	
	return 0;
}
