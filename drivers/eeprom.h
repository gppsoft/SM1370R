/*
 * EEPROM.h
 *
 * Created: 02.11.2015 22:00:09
 *  Author: Pavel
 */ 


#ifndef EEPROM_H_
#define EEPROM_H_

#include <avr/io.h>

class EEPROM{
	public:
		void write(unsigned int uiAddress,uint8_t ucData);
		uint8_t readByte(unsigned int uiAddress);
		float readFloat(unsigned int uiAddress);
		uint16_t readInt16(unsigned int uiAddress);
		void save(uint8_t* ucData, unsigned int uiSize, unsigned int uiAddress);
};

extern EEPROM eeprom;

#endif /* EEPROM_H_ */