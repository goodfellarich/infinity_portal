#include <stdio.h>
#include "Adafruit_NeoPixel.h"

// Auduino, Prime number generator and display
// 6-digit 7-segment display
//
//
// Analog 0: 
// Analog 1: 
// Analog 2: 
// Analog 3: 
// Analog 4: 
//
// Digital 0:     
// Digital 1:     
// Digital 2:     
// Digital 3:     
// Digital 4:     
// Digital 5:     
// Digital 6:     
// Digital 7:     
// Digital 8:     
// Digital 9:     
// Digital 10:    
// Digital 11:    
// Digital 12:    
// Digital 13:    
//
// Changelog:

//Global Declarations:

#define D_OUT 6
const uint8_t SEGMENT[7] = {7,8,9,10,11,12,13};
uint8_t       digit_value;
int           d[6] = {0,0,0,0,0,0};
int           index_count;
Adafruit_NeoPixel strip = Adafruit_NeoPixel(36, D_OUT, NEO_GRB + NEO_KHZ800);


//**************************************//
void setup() {
  pinMode(D_OUT, OUTPUT); 
  digitalWrite(D_OUT, LOW);

  strip.begin();
  strip.show(); // Initialize all pixels to 'off'

}
//**************************************//


//**************************************//
// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, c);
      strip.show();
      delay(wait);
  }
}
//**************************************//

//**************************************//
void rainbow(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i+j) & 255));
    }
    strip.show();
    delay(wait);
  }
}
//**************************************//

//**************************************//
// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}
//**************************************//

//**************************************//
//Theatre-style crawling lights.
void theaterChase(uint32_t c, uint16_t wait) {
  for (int j=0; j<10; j++) {  //do 10 cycles of chasing
    for (int q=0; q < 3; q++) {
      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, c);    //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}
//**************************************//

//**************************************//
// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  if(WheelPos < 85) {
   return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if(WheelPos < 170) {
   WheelPos -= 85;
   return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
   WheelPos -= 170;
   return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}
//**************************************//

//**************************************//
//Theatre-style crawling lights with rainbow effect
void theaterChaseRainbow(uint8_t wait) {
  for (int j=0; j < 256; j++) {     // cycle all 256 colors in the wheel
    for (int q=0; q < 3; q++) {
        for (int i=0; i < strip.numPixels(); i=i+3) {
          strip.setPixelColor(i+q, Wheel( (i+j) % 255));    //turn every third pixel on
        }
        strip.show();

        delay(wait);

        for (int i=0; i < strip.numPixels(); i=i+3) {
          strip.setPixelColor(i+q, 0);        //turn every third pixel off
        }
    }
  }
}

//**************************************//

void loop() {  
  
  //colorWipe(strip.Color(255, 0, 0), 10); // Red
  //colorWipe(strip.Color(0, 255, 0), 10); // Green
  //colorWipe(strip.Color(0, 0, 255), 10); // Blue

  //rainbow(20);
  //rainbowCycle(20);
  //theaterChaseRainbow(10);
  theaterChase(strip.Color(0,255,0),1000); //blue

}

