/*
 * buttons.c
 *
 * Created: 13.03.2016 14:41:15
 *  Author: snakeye
 */ 
#include <avr/io.h>

#include "buttons.h"

#define BUTTONS_NUMBER 2

uint8_t buttons_status[BUTTONS_NUMBER];
uint16_t buttons_hold_time[BUTTONS_NUMBER];
uint8_t buttons_on_hold[BUTTONS_NUMBER];

void buttons_init()
{
	// port D inputs
	DDRC &= ~((1 << PORTC1) | (1 << PORTC2));
	// port D pull-up resistors
	PORTC |= (1 << PORTC1) | (1 << PORTC2);

	for(uint8_t i = 0; i < BUTTONS_NUMBER; i++) {
		buttons_hold_time[i] = 0;
		buttons_on_hold[i] = 0;
		buttons_hold_time[i] = 0;
	}
}

void buttons_tick()
{
	buttons_status[0] = (PINC & (1 << PORTC1)) == 0;
	buttons_status[1] = (PINC & (1 << PORTC2)) == 0;

	for(uint8_t i = 0; i < BUTTONS_NUMBER; i++) {
		if(buttons_status[i]) {
			buttons_hold_time[i]++;
			
			if(buttons_hold_time[i] >= BUTTON_HOLD_TIME && !buttons_on_hold[i]){
				on_button_hold(i);
				buttons_on_hold[i] = 1;				
			}
		}
		else {
			if(buttons_hold_time[i] >= BUTTON_HOLD_TIME) {
				on_button_release(i);
				buttons_hold_time[i] = 0;
				buttons_on_hold[i] = 0;
			}
			else if(buttons_hold_time[i] >= BUTTON_CLICK_TIME) {
				on_button_click(i);
				buttons_hold_time[i] = 0;
			}
		}
	}
}

uint8_t button_hold_status(uint8_t button)
{
	return buttons_on_hold[button];
}