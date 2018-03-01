//Authors
//2016 v1: Braden Rosengren and David Ticknor and Steven Warren
//2017 v2: David Ticknor and Steven Warren
//For the Isu Marching Band. Funded by Critical Tinkers.

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include "LPD8806.h"
#include "SPI.h" // Comment out this line if using Trinket or Gemma
#ifdef __AVR_ATtiny85__
#include <avr/power.h>
#endif


int drumcovernumber = 8; //Enter drums number as displayed on lid. This is the number of drum in the order starting at 1

int drum = (drumcovernumber)-1; //this math is used in LED code to make the seperate LED strips act like one long strip
int offset = 20 * drum;
int nLEDs = 180;


RF24 radio(8,9); //radio pins 

const byte rxAddr[6] = "00001"; //define size of array being recieved

int dataPin  = 7; //arduino data pin. Make sure NRF shield is wired correctly
int clockPin = 6; //arduino clock pin. Make sure NRF shield is wired correctly
LPD8806 strip = LPD8806(nLEDs, dataPin, clockPin); //sets up LED functions for use
volatile int pattern = 0; //this value can change anytime from ISR sent from transmitter
int num = 0; 
volatile int quit = 0;
int flag = 0;
int parity = 0;


void setup(){
  
  attachInterrupt(0, interrupt, FALLING); //If flag is detected on falling edge of data interrupt pin 0 of arduino,go to interrupt function
  strip.begin(); //int led strip functions
  strip.show(); 
  Serial.begin(9600); 
  radio.begin(); //nrf int
  radio.openReadingPipe(0, rxAddr); //these two line put the nrf chips into a reading mode
  radio.startListening(); 
  randomSeed(analogRead(0)); //this is a random num gen used for an LED function
}

void interrupt(){ //interrupt function, basically if the info recieved does not match the current pattern, interrupt
  char text[6] = {0};
  radio.read(&text, sizeof(text)); 
  String str(text);
  num = str.toInt();
  Serial.println(num); //this prints on serial the current pattern recieved. Useful for debugging

  if(num != pattern){
    pattern = num;
    quit = 1;
  } 
}
//The following are cases for each pattern number recieved from transmitter. The function calls are using the skeleton functions from the LPD8806 libraries.
//If you would like to change pattern or add more, just keep the general outline of case statements, and add LED function call. If you do add a pattern, make sure to change the transmitter code accordingly so numPatterns matches.
void loop(){
  switch(pattern){
    case 0:
      colorWipe(strip.Color(127, 0, 0), 25);//Red scroll from 1 to 9
      break;
    case 1:
      colorChaseThreeOneDrum(strip.Color(127,   0,   127), 30); //3 LED scroll, purple 1 to 9
      break;
    case 2:
      colorChase(strip.Color(127, 127, 0), 20);
      break;
    case 3:
      cycloneScroll();//steve's red3 gold3 red3 scroll
      break;
    case 4:
      flashRandom();//random flash
      break;
    case 5:
      colorWipeRandom(30);//Steve's random color flashing 
      break;
    case 6:
      theaterChase(strip.Color(127,   50,   50), 45);//single dot(?) scroll
      break;
    case 7:
      colorChaseThree(strip.Color(50,   70,   80), 40);
      break;
    case 8:
      colorWipe(strip.Color(100, 50, 0), 30);
      break;
    case 9:
      theaterChaseRainbow(50);
      break;
    case 10:
      colorWipe(strip.Color(random(128), random(128), random(128)), 30);
      break;
    default:
      stripClear;
      break;
  }
}

//Begin LED strip patterns. These are uncommented mainly because they use the examples from the libraries, with tweaks to colors, and speed.

void cycloneScroll(){
  quit = 0;
  parity++; 
  parity %= 2;

  if(drum < 3){
    do{stripClear();}while(0);
    for (int i = 0; i < strip.numPixels(); i++) {
      if(quit == 0){
        if(parity == 0){strip.setPixelColor(i - offset, 127, 0, 0);}
        else      {strip.setPixelColor(i - offset, 100, 50, 0);}
        strip.show();
        delay(10);
      }
      else{
        stripClear();
        return;
      }
    }
  }
  else if(drum > 5){
     do{stripClear();}while(0);
     for (int i=0; i < strip.numPixels(); i++) {
      if(quit == 0){
        if(parity == 0){strip.setPixelColor(i - offset + 120, 127, 0, 0);}
        else      {strip.setPixelColor(i - offset + 120, 100, 50, 0);}
        strip.show();
        delay(10);
      }
      else{
        stripClear();
        return;
      }
    }
  }
  else if (drum > 2 && drum < 6){
    do{stripClear();}while(0);
    for (int i=0; i < strip.numPixels(); i++) {
      if(quit == 0){
        if(parity == 0){strip.setPixelColor(i - offset + 60, 100, 50, 0);}
        else      {strip.setPixelColor(i - offset + 60, 127, 0, 0);}
        strip.show();
        delay(10);
      }
      else{
        stripClear();
        return;
      }
    }
  } 
}



//// Fill the dots progressively along the strip.
void colorWipe(uint32_t c, uint8_t wait) {
  int i;
  quit = 0;

  do{stripClear();} while(0);
  for (i=0; i < strip.numPixels() + 20; i++) {
    if(quit == 0){
    strip.setPixelColor(i-offset, c);
    strip.show();
    delay(wait);
    }
    else if(quit == 1){
      stripClear();
      return;
    }
  }
}



void colorWipeRandom(uint8_t wait) {
  int i;
  quit = 0;

  do{stripClear();} while(0);
  for (i=0; i < strip.numPixels() + 20; i++) {
    if(quit == 0){
    strip.setPixelColor(i-offset, strip.Color(random(128), random(128), random(128)));
    strip.show();
    delay(wait);
    }
    else if(quit == 1){
      stripClear();
      return;
    }
  }
}


void colorChase(uint32_t c, uint8_t wait) {
  int i;
  quit = 0;
  
  for(i=0; i<strip.numPixels(); i++) {
    if(quit == 0){
      strip.setPixelColor(i-offset, c); // Set new pixel 'on'
      strip.show();              // Refresh LED states
      strip.setPixelColor(i-offset, 0); // Erase pixel, but don't refresh!
      delay(wait);
    }
    else{
      stripClear();
    return;
    } 
  }

  strip.show(); // Refresh to turn off last pixel
}


void colorChaseThree(uint32_t c, uint8_t wait) {
  int i;
  quit = 0;
  
  for(i=0; i < strip.numPixels(); i++) {
    if(quit == 0){
      strip.setPixelColor(i - offset, c); // Set new pixel 'on'
      strip.setPixelColor(i - offset - 1, c);
      strip.setPixelColor(i - offset - 2, c);
      strip.setPixelColor(i - offset - 3, 0); // Erase pixel, but don't refresh!
      strip.show();              // Refresh LED states
      delay(wait);
    }
    else{
      stripClear();
    return;
    } 
  }

  strip.show(); // Refresh to turn off last pixel
}


void colorChaseThreeOneDrum(uint32_t c, uint8_t wait) {
  int i;
  quit = 0;
  
  for(i=0; i < 23; i++) {
    if(quit == 0){
      strip.setPixelColor(i, c); // Set new pixel 'on'
      strip.setPixelColor(i - 1, c);
      strip.setPixelColor(i - 2, c);
      strip.setPixelColor(i - 3, 0); // Erase pixel, but don't refresh!
      strip.show();              // Refresh LED states
      delay(wait);
    }
    else{
      stripClear();
    return;
    } 
  }

  strip.show(); // Refresh to turn off last pixel
}


//Flash random colors on the strip
void flashRandom(){
  int i;
  int r = 0;
  int g = 0;
  int b = 0;
  quit = 0;
  
  stripClear();

//  delay(random(80,120));
    
  switch(random(0,4)){
    case 0: // Red
      r = 127;
      g = 0;
      b = 0;
      break;
    case 1: // Green
      r = 0;
      g = 127;
      b = 0;
      break;
    case 2: // Blue
      r = 0;
      g = 0;
      b = 127;
      break;
    case 3: //White
      r = 127;
      g = 127;
      b = 127;
      break;
  }
  
  // set pixel colors
  for (i=0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, r, g, b); 
  }
  
  strip.show(); 
  delay(random(100, 170));

  if(quit){
    stripClear();
    return;
  }
}


void theaterChase(uint32_t c, uint8_t wait) {
  quit = 0;
  for (int j=0; j<10; j++) {  //do 10 cycles of chasing
    for (int q=0; q < 3; q++) {
      if(quit == 0){
        for (int i=0; i < strip.numPixels(); i=i+3) {
          strip.setPixelColor(i+q, c);    //turn every third pixel on
        }
        strip.show();
      
        delay(wait);
      
        for (int i=0; i < strip.numPixels(); i += 3) {
          strip.setPixelColor(i+q, 0);        //turn every third pixel off
        }
      }
      else{
        stripClear();
        return;
      }
    }
  }
}


void theaterChaseRainbow(uint8_t wait) {
  quit = 0;
  for (int j=0; j < 384; j++) {     // cycle all 384 colors in the wheel
    for (int q=0; q < 3; q++) {
      if(quit == 0){
        for (int i=0; i < strip.numPixels(); i=i+3) {
          strip.setPixelColor(i+q, Wheel( (i+j) % 384));    //turn every third pixel on
        }
        strip.show();
       
        delay(wait);
       
        for (int i=0; i < strip.numPixels(); i=i+3) {
          strip.setPixelColor(i+q, 0);        //turn every third pixel off
        }
      }
      else{
        stripClear();
        return;
      }
    }
  }
}


uint32_t Wheel(uint16_t WheelPos)
{
  byte r, g, b;
  switch(WheelPos / 128)
  {
    case 0:
      r = 127 - WheelPos % 128;   //Red down
      g = WheelPos % 128;      // Green up
      b = 0;                  //blue off
      break; 
    case 1:
      g = 127 - WheelPos % 128;  //green down
      b = WheelPos % 128;      //blue up
      r = 0;                  //red off
      break; 
    case 2:
      b = 127 - WheelPos % 128;  //blue down 
      r = WheelPos % 128;      //red up
      g = 0;                  //green off
      break; 
  }
  return(strip.Color(r,g,b));
}


void stripClear(){
  for(int i = 0; i < strip.numPixels(); i++){
    strip.setPixelColor(i, strip.Color(0,0,0));
  }
  strip.show();
}

