/*
* buttons.c
*
* Created: 13.03.2016 14:41:15
*  Author: snakeye
*/
#include <avr/io.h>

#include "buttons.h"

#define BUTTONS_NUMBER 2

uint16_t button_status[BUTTONS_NUMBER];
uint16_t button_press_time[BUTTONS_NUMBER];
uint8_t button_on_hold[BUTTONS_NUMBER];

void buttons_init()
{
	// port D inputs
	DDRC &= ~((1 << PORTC1) | (1 << PORTC2));
	// port D pull-up resistors
	PORTC |= (1 << PORTC1) | (1 << PORTC2);

	for(uint8_t i = 0; i < BUTTONS_NUMBER; i++) {
		button_status[i] = 0;
		button_press_time[i] = 0;
		button_on_hold[i] = 0;
	}
}

void buttons_tick(uint16_t tick)
{
	//uint8_t button[BUTTONS_NUMBER] = {
	//	(PINC & (1 << PORTC1)) == 0,
	//	(PINC & (1 << PORTC2)) == 0
	//};

	button_press_time[0] += (PINC & (1 << PORTC1)) == 0;
	return;
	
	uint8_t button[2];
	for(uint8_t i = 0; i < BUTTONS_NUMBER; i++) {
		button_status[i] = (button_status[i]<<1) | !button[i] | 0xe000;
		uint8_t pressed = (button_status[i] == 0xf000);

		if (pressed) {
			button_press_time[i] += 1;
			
			if(button_press_time[i] >= BUTTON_HOLD_TIME && !button_on_hold[i]){
				on_button_hold(i);
				button_on_hold[i] = 1;
			}
			else if(button_press_time[i] == 1) {
				on_button_press(i);
			}
		}
		else {
			if(button_press_time[i] > BUTTON_HOLD_TIME) {
				on_button_release(i);
				button_press_time[i] = 0;
				button_on_hold[i] = 0;
			}
			else if(button_press_time[i] >= BUTTON_CLICK_TIME) {
				on_button_click(i);
				button_press_time[i] = 0;
			}
		}
	}
}

uint16_t button_pressed(uint8_t button)
{
	return button_press_time[button];
}