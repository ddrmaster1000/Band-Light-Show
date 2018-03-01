//Authors
//2016 v1: Braden Rosengren and David Ticknor and Steven Warren
//2017 v2: David Ticknor and Steven Warren
//For the Isu Marching Band. Funded by Critical Tinkers.

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(8, 9); //ardunio radio pins

//*********** If you want to send pattern 0 (LEDs off), reset the transmitter. This is useful to reset LED
//*********** patterns at the end of the show, so the drumline can finish their sets after the lights come up with the LEDs on. 

const byte rxAddr[6] = "00001"; //array size being sent
char text[6]; //'text' array is name of array being sent. Must match rxAddr
String stringOne = ""; //define a blank string for later use
byte index = 0; //variable of size byte that will be used 
volatile int counter = 129; //multiple of numpattern - 1 to initilize to blank
const int numPattern = 10; //total number of patterns you have to be sent. This needs to match how many patterns you have coded in the reciever code.
long debouncing_time = 2000; //Debouncing Time in Milliseconds. Basically the time before you are able to change patterns. 
volatile unsigned long last_micros;

void setup()
{
  Serial.begin(9600); 
  attachInterrupt(1, DebounceInterrupt, RISING); //interrupt.(interuppt pin, function being called, interuppt on rising edge)
  radio.begin(); //start NRF radio
  radio.openWritingPipe(rxAddr); //start writing to byte rxAddr
  
  radio.stopListening(); //stop NRF from listening for anything
}

void loop()
{
  stringOne = counter; //set string to counter
  stringOne.toCharArray(text,6); //set string one into 'text' array of size 6
  Serial.println(text); //print out on serial what is being sent through NRF
  radio.write(&text, sizeof(text)); //write to be sent. text array pointer, send actual text array 
  delay(125); //need some delay for NRF to actually transmit
}

void DebounceInterrupt() { //debouncing function. Mainly a glorified wait that checks between last time a switch has been flipped.
  if((long)(micros() - last_micros) >= debouncing_time * 1000) {
    changePattern();
    last_micros = micros();
  }
}

void changePattern(){ //change pattern function. This value is being sent to the recievers, which sets the specifc pattern
  counter++; //increase pattern by 1
  counter %= numPattern; 
}

