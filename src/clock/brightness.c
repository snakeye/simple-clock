/*
 * brightness.c
 *
 * Created: 13.03.2016 14:29:55
 *  Author: snakeye
 */ 
#include <avr/io.h>
#include <avr/interrupt.h>

#include "brightness.h"

uint16_t adc_value = 0x08;

/**
* ADC Interrupt handler
*/
ISR(ADC_vect)
{
	uint16_t adc = (ADCL | (ADCH << 8));
	
	// average
	adc_value = adc;
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

uint8_t get_brightness()
{
	uint8_t brightness = (adc_value * MAX_BRIGHTNESS) / 1024;

	return brightness;
}