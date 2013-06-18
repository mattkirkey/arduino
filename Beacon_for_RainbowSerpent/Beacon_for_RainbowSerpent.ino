#include <TimerOne.h>

#include "SPI.h"
#include "Adafruit_WS2801.h"
#include <BeaconEntity.h>

#define p(x) Serial.print(x)
#define pl(x) Serial.println(x)

const int dataPin  = 11;    // Yellow wire on Adafruit Pixels
const int clockPin = 12;    // Green wire on Adafruit Pixels
int brightness = 128; //0-255


const int countEntities = 3;//dont change this
const int StripLength = 100;
const int entitySize = 5;
const int entitiesDelay = 50; //time in MS for a half step
int lastReversal = 2;
bool randomOn = false;

Entity ENTITIES[countEntities];

// Set the first variable to the NUMBER of pixels. 25 = 25 pixels in a row
Adafruit_WS2801 strip = Adafruit_WS2801(2, dataPin, clockPin);
uint32_t pixels[StripLength];


void setup() {
  
  Serial.begin(57600);
  pl("*** ARDUINO RESTART ***");
  pinMode(2,INPUT_PULLUP);
  
  InitializeEntities();
  InitializeStrip();
  

}

void loop() {
  // Some example procedures showing how to display to the pixels
  pl();
  pl("LoopStart");
  threeEntities();
}

void InitializeStrip()
{
  strip.begin();
  strip.show();
  colorWipe(Color(brightness, 0, 0), 20);
  delay(1000);
  colorWipe(Color(0, brightness, 0), 20);
  delay(1000);
  colorWipe(Color(0, 0, brightness), 20);
  delay(1000);
  colorWipe(Color(0, 0, 0), 20);
  delay(1000);
}



void changeBrightness()
{
  pl("Interrupt Triggered");
  //toggle the brightnes
  if(brightness > 3)
    {brightness /= 2;}
  else 
    {brightness = 254;}
  p("new brightness = "); pl(brightness);
  
  //update the brightness
  setBrightness();
  //delay(1000);
}

void setBrightness()
{
  ENTITIES[0].color = Color(brightness,0,0); //red
  ENTITIES[0].colorHalf = Color(brightness/2,0,0); //red
  ENTITIES[2].color = Color(0,0,brightness); //blue
  ENTITIES[2].colorHalf = Color(0,0,brightness/2); //blue
  ENTITIES[1].color = Color(0,brightness,0); //green
  ENTITIES[1].colorHalf = Color(0,brightness/2,0); //green
}

void  checkForButtonPress()
{
  if(digitalRead(2)==LOW)
    {
      delay(20);
      if(digitalRead(2)==LOW)
      {
        changeBrightness();
      }
    }
}

void InitializeEntities()
{  
  pl("Initialize Entities");
  setBrightness();
  ENTITIES[0].direction = up;  
  ENTITIES[0].position = 0;
  
  ENTITIES[1].direction = up;  
  ENTITIES[1].position = StripLength*2/3*2*-1;
  
  ENTITIES[2].direction = up;  
  ENTITIES[2].position = StripLength*2/3*2*2*-1;
  
}


void threeEntities()
{
  pl();
  pl("ThreeEntities");
  
  displayEntities();
  moveEntities();
  delay(entitiesDelay);
  checkForButtonPress();
}



void displayEntities()
{
  clearAllLEDs();
  processEntitiesToLEDBuffer();
  pushPixelsToStrip();
}

void clearAllLEDs()
{
  pl("  clearAllLEDs");
  for(int i = 0;i<StripLength;i++)
  {
    pixels[i] = 0;
  }
}

void processEntitiesToLEDBuffer()
{
  pl("  processEntitiesToLEDBuffer");
  for(int i = 0; i < countEntities; i++)//for each entity
  {
    
    if(ENTITIES[i].position>=0)//check if position is in frame yet
    {
      
      if(ENTITIES[i].halfStep)
      {

        if(ENTITIES[i].direction == up)
        {
          pixels[ENTITIES[i].position-1] |= ENTITIES[i].colorHalf; //half fade the first pixel
          pl(ENTITIES[i].color);
          pl(pixels[ENTITIES[i].position-1]);
          for(int j = 0; j < entitySize-1; j++)//full brightness for the middle pixels
          {
            pixels[ENTITIES[i].position+j] |= ENTITIES[i].color;
          }
          pixels[ENTITIES[i].position+entitySize-1] |= ENTITIES[i].colorHalf; //half fade the last pixel
          ENTITIES[i].halfStep = false;//next step will NOT a half step
        }
        else //if(ENTITIES[i].direction == down)
        {
          pixels[ENTITIES[i].position] |= ENTITIES[i].colorHalf; //half fade the first pixel
          for(int j = 1; j < entitySize; j++)//full brightness for the middle pixels
          {
            pixels[ENTITIES[i].position+j] |= ENTITIES[i].color;
          }
          pixels[ENTITIES[i].position+entitySize] |= ENTITIES[i].colorHalf; //half fade the last pixel
          ENTITIES[i].halfStep = false;//next step will NOT be a half step
        }
      }
      else //not half step, display full unit
      {
        for(int j = 0; j < entitySize; j++)//for entity size (each pixel)
        {
          pixels[ENTITIES[i].position+j] |= ENTITIES[i].color;
        }
        ENTITIES[i].halfStep = true;//next step WILL be a half step        
      }
    }
  }
  showPixels();
}


void pushPixelsToStrip()
{
  pl("  pushPixelsToStrip");
  for(int i = 0; i<StripLength; i++)
  {
    strip.setPixelColor(i,pixels[i]);
  }
    strip.show();
}

void showPixels()
{
  pl("ShowPixels");
  for(int i = 0; i<StripLength; i++)
  {
    pl(pixels[i]);
  }
}


void moveEntities()
{
  pl("  moveEntities");
  for(int i = 0;i<countEntities;i++)
  {
    
   if(ENTITIES[i].position <0){ENTITIES[i].position++;}//startup, enter from off screen

   else if(ENTITIES[i].halfStep == true) //only move entities that have completed a half step(false = complete)
    {
     if( ENTITIES[i].direction == up)
      {
         if(ENTITIES[i].position == StripLength - entitySize)
         {
           ENTITIES[i].direction = down;
           ENTITIES[i].position--;
         }
         else
         {
           ENTITIES[i].position++;
         }
      }
      else //diretion is down
      {
         if(ENTITIES[i].position == 0)
         {
           ENTITIES[i].direction = up;
           ENTITIES[i].position++;
         }
         else
         {
           ENTITIES[i].position--;
         }
       }
    }//end if(ENTITIES[i].halfStep == true)
   }//end for each entity
   //showEntityPositions();
}

void showEntityPositions()
{
  p("  ~Entity 0 Position: ");
  pl(ENTITIES[0].position);
  
  p("  ~Entity 1 Position: ");
  pl(ENTITIES[1].position);
  
  p("  ~Entity 2 Position: ");
  pl(ENTITIES[2].position);
}




// fill the dots one after the other with said color
// good for testing purposes
void colorWipe(uint32_t c, uint8_t wait) {
  int i;
  
  for (i=0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, c);
      strip.show();
      delay(wait);
  }
}

/* Helper functions */

// Create a 24 bit color value from R,G,B
uint32_t Color(byte r, byte g, byte b)
{
  uint32_t c;
  c = r;
  c <<= 8;
  c |= g;
  c <<= 8;
  c |= b;
  return c;
}


