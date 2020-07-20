#include <stdio.h>
#include "Adafruit_NeoPixel.h"

// Changelog:

//Global Declarations:
#define MODE  3  //mode button
#define POWER 4  //inhibit/enable light strip
#define D_OUT 6  //data out, for light strip
#define LS_IN 8  //light sensor input discrete
typedef enum color_type_t {red,green,blue,white,aqua,purple,dynamic} color_type_t;
typedef enum seq_type_t {seq0,seq1} seq_type_t;
int           prox           = 100;
int           seq_delay      = 100;
int           off_timer      = 0;
uint16_t      counter        = 0;
bool          portal_off     = false;
uint8_t       color_red      = 0;
uint8_t       color_grn      = 0;
uint8_t       color_blu      = 255;
byte          color_dynamic  = 0;
bool          color_chg_req  = false;
bool          mode_debounced = true;
color_type_t  color_state    = blue;
seq_type_t    seq_pattern    = seq0;
Adafruit_NeoPixel strip      = Adafruit_NeoPixel(36, D_OUT, NEO_GRB + NEO_KHZ800);

//**************************************//
void setup() {
  pinMode(D_OUT, OUTPUT);
  pinMode(A0, INPUT);
  pinMode(LS_IN, INPUT);
  pinMode(MODE, INPUT_PULLUP);
  pinMode(POWER, INPUT_PULLUP);
  digitalWrite(D_OUT, LOW);
  attachInterrupt(digitalPinToInterrupt(MODE),mode_isr,LOW);

  strip.begin();
  Serial.begin(9600);
  while(!Serial);
  strip.show(); // Initialize all pixels to 'off'
}
//**************************************//


//**************************************//
// Fill the dots one after the other with a color
//**************************************//
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, c);
      strip.show();
      delay(wait);
  }
}
//**************************************//

//**************************************//
//3-Dot chasing pattern
//Warning: this function is in work (not complete)
//***************************************//
void chase_3(uint32_t c, uint8_t wait){
  for (int i=0; i < strip.numPixels(); i=i+3) {
    strip.setPixelColor(i, c);    //turn every third pixel on
  }
  strip.show();
  delay(wait);
}

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
  int wait_div = 10;
  int wait_short = wait/wait_div;
  
  for (int j=0; j<1; j++) {  //do 1 cycle of chasing
    for (int q=0; q < 3; q++) {
      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, c);    //turn every third pixel on
      }
      strip.show();
      for(int x=0; x<wait_div; x++){
        delay(wait_short);
      }
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
  for (int j=0; j < 256; j+=32) {     // cycle all 256 colors in the wheel
    if((analogRead(A0)<100) && (prox!=1000)){
        break;
      }
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

//**************************************//
//ISR for mode button                   //
//**************************************//
void mode_isr(){
  if(mode_debounced){
    color_chg_req  = true;
    mode_debounced = false;
  }  
}

//**************************************//
//color controller                      //
//**************************************//
void color_control(){
  switch(color_state){
      case blue :
        color_blu   = 0;
        color_red   = 255;
        color_grn   = 0;
        color_state = red;
        break;
      case red  :
        color_blu   = 0;
        color_red   = 0;
        color_grn   = 255;
        color_state = green;
        break;
      case green:
        color_blu   = 255;
        color_red   = 255;
        color_grn   = 255;
        color_state  = white;
        break;
      case white:
        color_blu   = 128;
        color_red   = 255;
        color_grn   = 0;
        color_state = purple;
        break;
      case purple:
        color_blu   = 255;
        color_red   = 0;
        color_grn   = 128;
        color_state = aqua;
        break;
      case aqua:
        color_blu   = 255;
        color_red   = 0;
        color_grn   = 0;
        color_state = dynamic;
        break;
      case dynamic:
        color_blu   = 255;
        color_red   = 0;
        color_grn   = 0;
        color_state = blue;
      default:
        color_blu   = 255;
        color_red   = 0;
        color_grn   = 0;
        color_state = blue;
    }
}
//**********************************//

//**************************************//
//sequence pattern randomizer           //
//**************************************//
void seq_randomize(){
  seq_pattern = random(0,2);
  switch(seq_pattern){
      case seq0 :
        for(int i=0; i<8; i++){
          colorWipe(strip.Color(color_red,color_blu,color_grn),seq_delay/30); //turns string on sequentially
          colorWipe(strip.Color(0,0,0),seq_delay/30); //turns string off sequentially
        }
        break;
      case seq1 :
        rainbowCycle(seq_delay/30);
        break;
      default:
        rainbowCycle(seq_delay/30);
        break;
  }
}
//**************************************//
   
void loop() {  

  //mode button color control///////////
  if(color_chg_req){
    color_control();
    color_chg_req = false;
    delay(1000);
    mode_debounced = true;
  }

  //proximity sensor//////////////////
  prox = analogRead(A0);
  if(prox > 100){
    seq_delay = 10;
    theaterChaseRainbow(seq_delay);
  }
  else{
    seq_delay = 100;
  }
    
  //light sensor timer/////////////////
  if(digitalRead(LS_IN)){ //light sensor off (reads high when off)
    off_timer++;
    if(off_timer>=1000){
      off_timer  = 1000; //hold at max value
      portal_off = true;
    }
  }
  else{
    portal_off = false;
    off_timer  = 0; //reset timer counter
  }

  //light strip control////////////////
  if(!digitalRead(POWER)){ //power button -> light sensor override
    if(color_state==dynamic){
      color_dynamic++;
      theaterChase(Wheel(color_dynamic),seq_delay);
    }
    else theaterChase(strip.Color(color_red,color_blu,color_grn),seq_delay);
    counter++;
    if(counter==1000){
      seq_randomize();
      counter = 0;
    }
  }
  else if(portal_off){ //controlled by light sensor
    strip.show(); //all pixels off
  }
  else{
    if(color_state==dynamic){
      color_dynamic++;
      theaterChase(Wheel(color_dynamic),seq_delay);
    }
    else theaterChase(strip.Color(color_red,color_blu,color_grn),seq_delay);
    counter++;
    if(counter==1000){
      seq_randomize();
      counter = 0;
    }
  }

}

