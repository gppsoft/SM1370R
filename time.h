/*
 * time.h
 *
 * Created: 01.02.2020 23:52:56
 *  Author: Pavel
 */ 


#ifndef TIME_H_
#define TIME_H_

#include "settings.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>

#define clockCyclesPerMicrosecond() ( F_CPU / 1000000L )
#define clockCyclesToMicroseconds(a) ( (a) / clockCyclesPerMicrosecond() )
#define microsecondsToClockCycles(a) ( (a) * clockCyclesPerMicrosecond() )

#define MICROSECONDS_PER_TIMER0_OVERFLOW (clockCyclesToMicroseconds(64 * 256))
#define MILLIS_INC (MICROSECONDS_PER_TIMER0_OVERFLOW / 1000)
#define FRACT_INC ((MICROSECONDS_PER_TIMER0_OVERFLOW % 1000) >> 3)
#define FRACT_MAX (1000 >> 3)




class Time{
	public:
		volatile unsigned long timer0_overflow_count;
	 	volatile unsigned long timer0_millis;
	    volatile unsigned char timer0_fract;
	
	public:
	void init();
		Time();
		unsigned long millis();
		unsigned long micros();
		

		
};


#endif /* TIME_H_ */

extern Time time;