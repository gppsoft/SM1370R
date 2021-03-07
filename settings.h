/*
 * settings.h
 *
 * Created: 10.02.2016 13:34:53
 *  Author: Pavel
 */ 


#ifndef SETTINGS_H_
#define SETTINGS_H_

#define F_CPU 16000000UL

//=============================SETTINGS===========================================
#define UART_BAUND_RATE 57600L 

#define S1 PIND2
#define CS1 PIND3
#define CS2 PIND4
#define CS3 PIND5
#define CONTROL_BUTTON PIND6

#define RSSI_PIN 0
#define GET_RSSI_COUNT 100
#define MIN_RSSI_ANALOG_VALUE 120
#define MAX_RSSI_ANALOG_VALUE 270
#define SAVE_SELECTED_CHANNEL_ADDRESS 1023


#endif /* SETTINGS_H_ */