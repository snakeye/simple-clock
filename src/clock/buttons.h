/*
 * buttons.h
 *
 * Created: 13.03.2016 14:39:32
 *  Author: snakeye
 */ 

#ifndef BUTTONS_H_
#define BUTTONS_H_

#define BUTTON_CLICK_TIME 1
#define BUTTON_HOLD_TIME 100

void buttons_init();
void buttons_tick();

uint8_t button_hold_status(uint8_t button);

void on_button_click(uint8_t button);
void on_button_hold(uint8_t button);
void on_button_release(uint8_t button);

#endif /* BUTTONS_H_ */