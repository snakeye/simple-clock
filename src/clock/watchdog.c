/*
 * watchdog.c
 *
 * Created: 13.03.2016 15:11:59
 *  Author: snakeye
 */
 
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>

#include "watchdog.h"

void watchdog_init()
{
	//reset watchdog
	wdt_reset();
	
	//set up WDT interrupt
	WDTCSR = (1<<WDE)|(1<<WDP3)|(1<<WDP0);

	wdt_reset();
}
