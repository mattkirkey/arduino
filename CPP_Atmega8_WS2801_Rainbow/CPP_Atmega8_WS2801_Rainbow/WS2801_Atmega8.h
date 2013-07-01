/*
 * WS2801_Atmega8.h
 *
 * Created: 28/05/2013 5:11:53 PM
 *  Author: Finbot
 */ 


#ifndef WS2801_ATMEGA8_H_
#define WS2801_ATMEGA8_H_

#include <avr/io.h>
#include <avr/delay.h>
#include <stdlib.h>

class WS2801_ATMEGA8
{
	public:
		WS2801_ATMEGA8(void);
		WS2801_ATMEGA8(int countPixels);
		void Show(void);
		void SetPixel(int index, uint32_t pixel);
		uint32_t GetPixel(int index);
		void Clear(void);
		uint32_t Color(uint8_t r, uint8_t g, uint8_t b);
		int NumberPixels(void);
		
	
		int numPixels;
		uint32_t stripBuffer[];
};
#endif /* WS2801_ATMEGA8_H_ */