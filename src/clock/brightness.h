/*
 * brightness.h
 *
 * Created: 13.03.2016 14:30:12
 *  Author: snakeye
 */ 

#ifndef BRIGHTNESS_H_
#define BRIGHTNESS_H_

#define MAX_BRIGHTNESS 0x0f

// initialize ADC
void adc_init(uint8_t reg);

// start brightness measurement
void measure_brightness_async();

// get current brightness value
uint8_t get_brightness();

#endif /* BRIGHTNESS_H_ */