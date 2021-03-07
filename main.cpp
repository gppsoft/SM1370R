/*
* SM1370R.cpp
*
* Created: 26.08.2020 21:35:58
* Author : Pavel
*/

#include "settings.h"

#include <avr/io.h>
#include <stdlib.h>
#include <stdio.h>
#include <util/delay.h>

#include "drivers/u8glib/u8g.h"
#include "drivers/eeprom.h"
#include "atmegaPins.h"
#include "utils.h"
#include "time.h"
#include "bitmaps.h"

//=============DRIVERS==========================
u8g_t display;
EEPROM eeprom;
Time time;
//=============DRIVERS==========================

char output_buffer[30];
volatile unsigned long control_button_pressed_time =0;
volatile unsigned long control_button_up_time = 0;

uint8_t currentChannel = 0;
uint16_t currentChannelRssi = 0;

uint8_t channels_pin_sequence[] = {
	0b0001,//A1
	0b0011,//A2
	0b0101,//A3
	0b0111,//A4
	0b1001,//A5
	0b1011,//A6
	0b1101,//A7
	0b1111,//A8
	//0b0000,//B1
	//0b0010,//B2
	//0b0100,//B3
	//0b0110,//B4
	0b1000,//B5
	//0b1010,//B6
	//0b1100,//B7
	//0b1110,//B8
};


const char channel_1[] PROGMEM= "1080";
const char channel_2[] PROGMEM= "1120";
const char channel_3[] PROGMEM= "1160";
const char channel_4[] PROGMEM= "1200";
const char channel_5[] PROGMEM= "1240";
const char channel_6[] PROGMEM= "1280";
const char channel_7[] PROGMEM= "1320";
const char channel_8[] PROGMEM= "1360";
const char channel_9[] PROGMEM= "1258";

const char *const channels_names[] PROGMEM  = {
	channel_1,
	channel_2,
	channel_3,
	channel_4,
	channel_5,
	channel_6,
	channel_7,
	channel_8,
	channel_9,
};

void init_ADC()
{
	ADMUX = (1<<REFS0);
	ADCSRA = (1<<ADEN)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);
}

void delay(int ms){
	while (0 < ms){
		_delay_ms(1);
		--ms;
	}
}

void init_hardware(){

	init_ADC();
	
	//Pins for SM1370
	setPinMode(DDRD,S1,INPUT);
	setPinMode(DDRD,CS1,INPUT);
	setPinMode(DDRD,CS2,INPUT);
	setPinMode(DDRD,CS3,INPUT);
	
	//Control button pin
	setPinMode(DDRD,CONTROL_BUTTON,INPUT);
	
	time.init();

	u8g_InitI2C(&display, &u8g_dev_ssd1306_128x64_i2c, U8G_I2C_OPT_FAST);
	
	sei();//Разрешаем прерывания
}



uint16_t analogRead(int pin){
	ADMUX = pin|(ADMUX & 0xF8);
	ADCSRA |= 1<<ADSC;
	while(ADCSRA & (1<<ADSC));
	return ADC;
}


void switchPin(uint8_t pin,bool flag){
	if(flag){
		setPinMode(DDRD,pin,OUTPUT);
		}else{
		setPinMode(DDRD,pin,INPUT);
	}
}

uint8_t findBestChannel() {
	
	uint8_t bestChannel = 0;
	uint16_t bestRssi = 0;
	uint16_t rssi[9];
	
	u8g_SetFont(&display, u8g_font_10x20);

	for (uint8_t i = 0; i<sizeof(channels_pin_sequence); i++) {

		switchPin(S1,(channels_pin_sequence[i] >> 0)  & 0x01);
		switchPin(CS1,(channels_pin_sequence[i] >> 1)  & 0x01);
		switchPin(CS2,(channels_pin_sequence[i] >> 2)  & 0x01);
		switchPin(CS3,(channels_pin_sequence[i] >> 3)  & 0x01);
		
		delay(50);
		
		rssi[i]  = analogRead(RSSI_PIN);
		
		if(rssi[i]>=bestRssi){
			bestRssi = rssi[i];
			bestChannel = i;
		}
		
		u8g_FirstPage(&display);
		do
		{
			u8g_DrawStrP(&display,8,16,U8G_PSTR("Searching..."));
			for(uint8_t chIndex = 0; chIndex<i;chIndex++){
				uint8_t height = map(rssi[chIndex],MIN_RSSI_ANALOG_VALUE,MAX_RSSI_ANALOG_VALUE,0,40);
				u8g_DrawBox(&display,chIndex*16,64-height,10,height);
			}
			
		} while (u8g_NextPage(&display));
		
	}
	
	delay(100);
	
	return bestChannel;
}

void selectChannel(uint8_t channel, bool save = true){
	switchPin(S1,(channels_pin_sequence[channel] >> 0)  & 0x01);
	switchPin(CS1,(channels_pin_sequence[channel] >> 1)  & 0x01);
	switchPin(CS2,(channels_pin_sequence[channel] >> 2)  & 0x01);
	switchPin(CS3,(channels_pin_sequence[channel] >> 3)  & 0x01);
	
	currentChannel = channel;
	
	if(save) eeprom.write(SAVE_SELECTED_CHANNEL_ADDRESS,currentChannel);
}


void displaySelectedChannel(uint8_t channel){
	u8g_FirstPage(&display);
	
	do
	{
	    u8g_SetFont(&display, u8g_font_fur35n);
		strcpy_P(output_buffer, (char *)pgm_read_word(&(channels_names[channel])));
		u8g_DrawStr(&display,6,40,output_buffer);
		u8g_DrawFrame(&display,0,0,127,45);
		
		u8g_SetFont(&display, u8g_font_6x10);
		sprintf_P(output_buffer, PSTR("RSSI:%u%%"), map(currentChannelRssi,MIN_RSSI_ANALOG_VALUE,MAX_RSSI_ANALOG_VALUE,0,100));
		u8g_DrawStr(&display,3,57,output_buffer);
		u8g_DrawBox(&display,3,59,map(currentChannelRssi,MIN_RSSI_ANALOG_VALUE,MAX_RSSI_ANALOG_VALUE,0,122),3);
		u8g_DrawFrame(&display,0,48,127,16);
		
	} while (u8g_NextPage(&display));
}


void checkButtonPressed(){
	if(digitalRead(PIND,CONTROL_BUTTON)){
		
		control_button_pressed_time = time.millis();
		while(digitalRead(PIND,CONTROL_BUTTON) && time.millis()-control_button_pressed_time < 1000);
		
		control_button_up_time = time.millis()-control_button_pressed_time;
		
		if(control_button_up_time < 200){
			currentChannel++;
			if(currentChannel>sizeof(channels_pin_sequence)-1){
				currentChannel=0;
			}
			selectChannel(currentChannel);
			}else if(control_button_up_time >=1000){
			selectChannel(findBestChannel());
		}
		}else{
		control_button_pressed_time = 0;
	}
}

void readCurrentRSSI(){
	uint16_t rssi = 0;
	
	for(uint8_t i = 0; i<GET_RSSI_COUNT ;i++){
		rssi+=analogRead(RSSI_PIN);
	}
	
	currentChannelRssi =(uint16_t) (rssi/GET_RSSI_COUNT);
	
	if(currentChannelRssi<MIN_RSSI_ANALOG_VALUE) currentChannelRssi = MIN_RSSI_ANALOG_VALUE;
	if(currentChannelRssi>MAX_RSSI_ANALOG_VALUE) currentChannelRssi = MAX_RSSI_ANALOG_VALUE;
}

void displaySplashScreen(){
	u8g_FirstPage(&display);
	
	do
	{
		u8g_SetFont(&display, u8g_font_10x20);
		u8g_DrawStrP(&display,54,24,U8G_PSTR("SM1370R"));
		u8g_SetFont(&display, u8g_font_6x12);
		u8g_DrawStrP(&display,54,36,U8G_PSTR("1.3Ghz video"));
		u8g_SetFont(&display, u8g_font_04b_24);
		u8g_DrawStrP(&display,70,64,U8G_PSTR("by sakhpilots.ru"));
		u8g_DrawXBMP(&display,0,8,48,48,with_plane);
		
	} while (u8g_NextPage(&display));
	
	delay(2000);
}

int main(void)
{
	init_hardware();
	displaySplashScreen();
	selectChannel(eeprom.readByte(SAVE_SELECTED_CHANNEL_ADDRESS),false);
	
	while (true)
	{
		displaySelectedChannel(currentChannel);
		checkButtonPressed();
		readCurrentRSSI();
	}

}

