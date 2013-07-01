/*
 * TestUpload.cpp
 *
 * Created: 23/05/2013 10:29:32 PM
 *  Author: Finbot
 */ 

#define F_CPU 12000000UL


#include <avr/io.h>
#include <avr/delay.h>
#include <stdlib.h>


#define BIT(x)            (1 << (x))    //Set a particular bit mask
#define CHECKBIT(x,b)     (x&b)        //Checks bit status
#define SETBIT(x,b)     x|=b;        //Sets the particular bit
#define CLEARBIT(x,b)     x&=~b;        //Sets the particular bit
#define TOGGLEBIT(x,b)     x^=b;        //Toggles the particular bit

//declarations
void delay_ms(int wait);
void PushBufferToStrip();
void ForceGreen();
uint32_t Wheel(int WheelPos);
void RainbowCycle(int delay);
void Pulsator(int wait);
void Police();
void Police2(int wait);
void Popper(int wait);
void ClearBuffer();
void CheckButtonPress();
void ToggleProgram();
void FlipREDAspLed();
void FlipYellowAspLed();
void Police2HelperRed();
void Police2HelperBlue();
void WarmUp(int wait);
uint32_t Color(uint8_t r, uint8_t g, uint8_t b);
void Chaser(int wait);

//Variables
#define numPixels 50
uint32_t stripBuffer [numPixels];

enum pattern { rainbow, popper, pulsator, police, police2, chaser};
pattern CurrentPattern = chaser;

int main(void)
{
	DDRB |= 0b00011000; //DDRB is also used for USB comms. PORTB3:4:5 are connected to the 10 Pin Header. PORRTB:1 is INPUT from USB. Must set only the DDRB flags required for the header. 
	DDRB &= 0b11011111; //input for 5
	PORTB |= 0b00100000;//pullup resistor for button
	DDRC = 255;
	PORTC = 255;
	
	CurrentPattern = chaser;
	
	WarmUp(5);
	Chaser(20);
		 
    while(1)
    {
		CheckButtonPress();
		switch(CurrentPattern)
		{
			case rainbow:	
				RainbowCycle(1);
				break;
			case popper:
				Popper(10);
				break;
			case pulsator:
				Pulsator(50);
				break;
			case police:
				Police();
				break;
			case police2:
				Police2(70);
				break;
			case chaser:
				Chaser(25);
				break;
		}		
	}
}

//HELPERS
void CheckButtonPress()
{
	FlipYellowAspLed();
	if(! (PINB & (1<<PD5)))
	{
		FlipREDAspLed();
		delay_ms(5); //debounce
		if( !(PINB & (1<<PD5)))	
		{
			ToggleProgram();
			delay_ms(100); //longpress
			PORTB |= 0b00100000;
			if( !(PINB & (1<<PD5)))	delay_ms(100); //longpress
			PORTB |= 0b00100000;
			if( !(PINB & (1<<PD5)))	delay_ms(100); //longpress
			PORTB |= 0b00100000;
			if( !(PINB & (1<<PD5)))	delay_ms(100); //longpress
			PORTB |= 0b00100000;
			if( !(PINB & (1<<PD5)))	delay_ms(100); //longpress
			PORTB |= 0b00100000;
			if( !(PINB & (1<<PD5)))	delay_ms(100); //longpress
			if( !(PINB & (1<<PD5)))	CurrentPattern = pulsator;
		}	
	}
}

void ToggleProgram()
{
	switch(CurrentPattern)
	{
		case rainbow:
		CurrentPattern = popper;
		break;
		case popper:
		CurrentPattern = police;
		break;
		case police:
		CurrentPattern = police2;
		break;
		case police2:
		CurrentPattern = chaser;
		break;
		case chaser:
		CurrentPattern = rainbow;
		break;
		case pulsator:
		CurrentPattern = rainbow;
		break;
	}
}

void PushBufferToStrip()
 {
	 _delay_ms(1); //wait low to ready frame on WS2801
	 for( int i = 0; i<numPixels; i++)//for each pixel in the buffer
	 {
			 for(long color_bit = 23 ; color_bit >= 0 ; color_bit--)
			 {
				 long mask = 1L << color_bit;
				 
				 PORTB = 0b00100000;;
				 if(stripBuffer[i] & mask) {PORTB = 0b00101000;}
				 PORTB = PORTB | 0b00110000;
				 PORTB = 0b00100000;
		 }
	 }
	 delay_ms(10);
 }

void FlipREDAspLed()
 {
	//flip main lights to indicate a latch wait time was executed. 
	PORTC = 0xfd;
	delay_ms(5);
	PORTC = 0xff;	 
 }
 
void FlipYellowAspLed()
  {
	  //flip main lights to indicate a latch wait time was executed.
	  PORTC = 0xfe;
	  delay_ms(5);
	  PORTC = 0xff;
  } 

void ClearStrip()
{
	for(int i = 0; i<numPixels*24; i++)
	{
		  PORTB = 0b00010000;
		  PORTB = 0;
	}	
	delay_ms(1);
}

void ClearBuffer()
{
	for(int i = 0; i<numPixels; i++)
	{
		stripBuffer[i] = 0;
	}
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

uint32_t Wheel(int WheelPos)
	{
		if (WheelPos < 85) {
			return Color(WheelPos * 3, 255 - WheelPos * 3, 0);
		} else if (WheelPos < 170) {
			WheelPos -= 85;
			return Color(255 - WheelPos * 3, 0, WheelPos * 3);
		} else {
			WheelPos -= 170;
			return Color(0, WheelPos * 3, 255 - WheelPos * 3);
		}
	}

void delay_ms(int wait)
{
	int w = wait;
	while(w--)
	{
		_delay_ms(1);
	}
}
//END HELPERS




//PROGRAMS
void Chaser(int wait)
{
	int purpleLocation, greenLocation;
	purpleLocation = 0;
	greenLocation = numPixels/2;

	for(int i = 0; i<numPixels && CurrentPattern == chaser; i++)
	{
		ClearBuffer();
			
		//purpleEntity
		stripBuffer[purpleLocation] = 0x0f000f;
		if(purpleLocation == 0)
		{
			stripBuffer[purpleLocation+1] = 0x0f000f;
			stripBuffer[numPixels] = 0x0f000f;
			
		}
		else if (purpleLocation == numPixels-1)
		{
			stripBuffer[0] = 0x0f000f;
			stripBuffer[purpleLocation-1] = 0x0f000f;
		}
		else
		{
			stripBuffer[purpleLocation-1] = 0x0f000f;
			stripBuffer[purpleLocation+1] = 0x0f000f;		
		}
		
		//green entity
		stripBuffer[greenLocation] |= 0x000f00;
		if(greenLocation == 0)
		{
			stripBuffer[greenLocation+1] |= 0x000f00;
			stripBuffer[numPixels] |= 0x000f00;
			
		}
		else if (greenLocation == numPixels-1)
		{
			stripBuffer[0] = 0x0f000f;
			stripBuffer[greenLocation-1] |= 0x000f00;
		}
		else
		{
			stripBuffer[greenLocation-1] |= 0x000f00;
			stripBuffer[greenLocation+1] |= 0x000f00;
		}
	
		PushBufferToStrip();
		delay_ms(wait);
		CheckButtonPress();
		
		purpleLocation++;
		greenLocation--;
		if(purpleLocation ==  numPixels) purpleLocation=0;
		if(greenLocation == -1) greenLocation=numPixels-1;
	}	
	
}

void WarmUp(int wait)
{
	for(uint16_t i = 1; i<255; i++)
	{
		for (int j=0; j < numPixels; j++) stripBuffer[j] = Color(i,0,i);
		FlipREDAspLed();
		PushBufferToStrip();
		delay_ms(wait);
		i*=1.07;
	}		
	
	for(uint16_t i = 255; i>128; i--)
	{
		for (int j=0; j < numPixels; j++) stripBuffer[j] = Color(i,0,i);
		FlipREDAspLed();
		PushBufferToStrip();
		delay_ms(wait);
		i*=0.96;
	}
}

void Pulsator(int wait)
{
	while ( !(PINB & (1<<PD5)))
	{
		
		for (int i=0; i < numPixels; i++) 
		{
			stripBuffer[i] = 0xffffff;
		}
		PushBufferToStrip();
		delay_ms(wait);
		
		//CheckButtonPress();
		
		ClearStrip();
		delay_ms(wait);
	}
	CurrentPattern = rainbow;
}

void Popper(int wait)
{
	ClearBuffer();
	ClearStrip();
	for (int i=0; i < numPixels && CurrentPattern == popper; i++) 
	{
		stripBuffer[i] = 0xff0000;
		stripBuffer[(i+(numPixels/3)) % numPixels] = 0xff0000; //1/3 way round
		stripBuffer[(i+((numPixels/3)*2)) % numPixels] = 0xff0000; //1/3 way round
		PushBufferToStrip();
		delay_ms(wait);
		
		ClearBuffer();
		PushBufferToStrip();
		delay_ms(wait);
		CheckButtonPress();
	}
}

void Police()
{
	for (int i=0; i < numPixels && CurrentPattern == police; i++) 
	{
		if(i % 2 == 0)
		{
			stripBuffer[i] = 0x0000FF;			
		}
		else
		{
			stripBuffer[i] = 0xff0000;
		}					
	}
	PushBufferToStrip();
	
	for(int i = 0; i<10; i++)//wait and check for button press
	{
		delay_ms(50);
		CheckButtonPress();
	}

	
	for (int i=0; i < numPixels&& CurrentPattern == police; i++)
	{
		if(i % 2 == 0)
		{
			stripBuffer[i] = 0xFF0000;
		}
		else
		{
			stripBuffer[i] = 0x0000ff;
		}
	}
	
	PushBufferToStrip();
	
	for(int i = 0; i<10; i++)//wait and check for button press
	{
		delay_ms(50);
		CheckButtonPress();
	}
}

void Police2(int wait)
{
	for(int i = 0; i<2 && CurrentPattern == police2; i++)
	{
		Police2HelperRed();
		delay_ms(wait);	
			CheckButtonPress();
		ClearStrip();
		delay_ms(wait);
			CheckButtonPress();
	}
	
	delay_ms(wait*2); //pause
	CheckButtonPress();
	
	for(int i = 0; i<2 && CurrentPattern == police2; i++)
	{
		Police2HelperBlue();
		delay_ms(wait);
			CheckButtonPress();
		ClearStrip();
		delay_ms(wait);
			CheckButtonPress();
	}
	
	delay_ms(wait*2); //pause
}

void Police2HelperRed()
{
	ClearBuffer();
	for (int i=0; i < numPixels && CurrentPattern == police2; i++) {if(i%4<2) {stripBuffer[i] = 0xff0000;}}
	PushBufferToStrip();
}

void Police2HelperBlue()
{
	ClearBuffer();
	for (int i=0; i < numPixels && CurrentPattern == police2; i++) {if(i%4>1) {stripBuffer[i] = 0x0000ff;}}
	PushBufferToStrip();
}

void RainbowCycle(int wait) 
{
	int i, j;
	
	for (j=0; j < 256 && CurrentPattern == rainbow; j++) 
	{     // 5 cycles of all 25 colors in the wheel
		for (i=0; i < numPixels; i++) 
		{
			stripBuffer[i] = Wheel( ( ( i * 256 / numPixels) + j) % 256) ;
		}
		  // write all the pixels out
		PushBufferToStrip();
		delay_ms(wait);
		CheckButtonPress();
	}}
	
	
	
	
	
	
	
	
