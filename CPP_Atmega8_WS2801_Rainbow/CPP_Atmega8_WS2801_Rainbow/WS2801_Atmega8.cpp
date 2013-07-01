/*
 * WS2801_Atmega8.cpp
 *
 * Created: 28/05/2013 5:13:35 PM
 *  Author: Finbot
 */ 

#include "WS2801_Atmega8.h"


	WS2801_ATMEGA8::WS2801_ATMEGA8(void)
	{
	}

	WS2801_ATMEGA8::WS2801_ATMEGA8(int n)
	{
		numPixels  = n;
		stripBuffer = stripBuffer[n];
	}
	
	void WS2801_ATMEGA8::Show(void)
	 {
		 _delay_ms(1); //wait low to ready frame on WS2801
		 for( int i = 0; i< numPixels; i++)//for each pixel in the buffer
		 {
				 for(uint32_t color_bit = 23 ; color_bit >= 0 && color_bit <= 23; color_bit--)
				 {
					 uint32_t mask = 1L << color_bit; //set mask
				 
					 PORTB = 0;//push to strip, full speed. 
					 if(stripBuffer[i] & mask) {PORTB= 0b00001000;}
					 PORTB = PORTB | 0b00010000;
					 PORTB = 0;
			 }
		 }
		 _delay_ms(10);
	 }
	
	void WS2801_ATMEGA8::SetPixel(int index, uint32_t pixel)
	{
		stripBuffer[index] = pixel;		
	}
	
	uint32_t WS2801_ATMEGA8::GetPixel(int index)
	{
		if(index >= NumberPixels()) return NULL;
		return stripBuffer[index];
	}
	
	uint32_t Color(uint8_t r, uint8_t g, uint8_t b)
	{
		uint32_t c;
		c = r;
		c <<= 8;
		c |= g;
		c <<= 8;
		c |= b;
		return c;
	}
	
	void WS2801_ATMEGA8::Clear(void)
	{
		PORTB = 0;
		for(int i = 0; i<numPixels*24; i++)
		{
			PORTB = 0b00010000;
			PORTB = 0;
		}		
		_delay_ms(10);
	}
	
	int WS2801_ATMEGA8::NumberPixels(void)
	{
		return numPixels;		
	}


