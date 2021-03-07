/*
 * AtmegaPins.h
 *
 * Created: 3/5/2013 3:39:07 PM
 *  Author: mfolz
 */ 


#ifndef ATMEGAPINS_H_INCLUDED_
#define ATMEGAPINS_H_INCLUDED_

#define SETBIT(ADDRESS,BIT) (ADDRESS |= (1<<BIT))
#define CLEARBIT(ADDRESS,BIT) (ADDRESS &= ~(1<<BIT))
#define FLIPBIT(ADDRESS,BIT) (ADDRESS ^= (1<<BIT))
#define CHECKBIT(ADDRESS,BIT) (ADDRESS & (1<<BIT))

#define LOW 0
#define HIGH 1
#define OUTPUT 1
#define INPUT 0

#define digitalWrite(PORT,PIN,STATE) if(STATE == HIGH){ PORT |= (1 << PIN); } else{ PORT &= ~(1 << PIN); }
#define setPinMode(PORT,PIN,MODE)  if(MODE == OUTPUT){ PORT |= (1 << PIN); } else{ PORT &= ~(1 << PIN); }
#define digitalRead(PORT,PIN)((PORT & (1 << PIN)) != 0)

#endif /* ATMEGAPINS_H_ */