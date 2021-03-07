/*
 * time.cpp
 *
 * Created: 01.02.2020 23:56:06
 *  Author: Pavel
 */ 

#include "time.h"

void Time::init(){
	TIMSK0 |= (1 << TOIE0);
	TCCR0B |= (1 << CS01) | (1 << CS00);
}

Time::Time(){	
	timer0_overflow_count=0;
	timer0_millis =0;
	timer0_fract=0;
}

unsigned long Time::millis()
{
	unsigned long m;
	uint8_t oldSREG = SREG;
	
	cli();
	m = timer0_millis;
	
	SREG = oldSREG;
	
	return m;
}

unsigned long Time::micros() {
	unsigned long m;
	uint8_t oldSREG = SREG, t;
	
	cli();
	m = timer0_overflow_count;

	t = TCNT0;

	if ((TIFR0 & _BV(TOV0)) && (t < 255))
	m++;
	
	SREG = oldSREG;
	
	return ((m << 8) + t) * (64 / clockCyclesPerMicrosecond());
}

//Прерывание для таймера. Для подсчета millis и micros
ISR (TIMER0_OVF_vect)  {
	
	unsigned long m = time.timer0_millis;
	unsigned char f = time.timer0_fract;

	m += MILLIS_INC;
	f += FRACT_INC;
	if (f >= FRACT_MAX) {
		f -= FRACT_MAX;
		m += 1;
	}

	time.timer0_fract = f;
	time.timer0_millis = m;
	time.timer0_overflow_count++;
}