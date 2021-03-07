/*
 * EEPROM.cpp
 *
 * Created: 02.11.2015 22:04:13
 *  Author: Pavel
 */ 

#include "EEPROM.h"

void EEPROM::write(unsigned int uiAddress,uint8_t ucData){
	while(EECR & (1<<EEPE));//Ожидание завершения предыдущей записи
	EEAR=uiAddress;//Адрес в памяти EEPROM
	EEDR=ucData;//Даныне для записи
	EECR|=(1<<EEMPE);//Разрешаем запись
	EECR|=(1<<EEPE);//Производим запись
}

uint8_t EEPROM::readByte(unsigned int uiAddress){
	while(EECR & (1<<EEPE));//Ожидание завершения предыдущей записи
	EEAR=uiAddress;//Адрес чтения
	EECR|=(1<<EERE);//Разрешаем чтение
	return EEDR;//Читаем
}

float EEPROM::readFloat(unsigned int uiAddress){
	float value = 0.0;
	uint8_t* p = (uint8_t*)(void*)&value;
	for (uint8_t i = 0; i < sizeof(value); i++)
	*p++ = eeprom.readByte(uiAddress++);
	return value;
}

uint16_t EEPROM::readInt16(unsigned int uiAddress){
	uint16_t value = 0;
	uint8_t* p = (uint8_t*)(void*)&value;
	for (uint8_t i = 0; i < sizeof(value); i++)
	*p++ = eeprom.readByte(uiAddress++);
	return value;
}

void EEPROM::save(uint8_t* ucData, unsigned int uiSize, unsigned int uiAddress){
	for(unsigned int i=0;i<uiSize;i++){
		write(uiAddress,ucData[i]);
		uiAddress++;
	}
}