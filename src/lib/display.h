/*
 * display.h
 *
 * Created: 21.02.2016 17:08:18
 *  Author: snakeye
 */ 

#ifndef DISPLAY_H_
#define DISPLAY_H_

#define DISPLAY_DIGITS 4

void display_init();

void display_clear();
void display_set_char(uint8_t position, char ch);
void display_set_dot(uint8_t position, uint8_t enabled);
void display_set_dots(uint8_t enabled);

#endif /* DISPLAY_H_ */