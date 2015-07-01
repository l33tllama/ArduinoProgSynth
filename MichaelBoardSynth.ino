/* Basic Programmable Synth
By Leo Febey
1 July 2015
Feel free to re-distribute

Intended for demo purposes at the Hobart Hackerspace Arduino workshop

Requires:
 - Adafruit MCP23017 port expander library - https://github.com/adafruit/Adafruit-MCP23017-Arduino-Library
 - Button by JChristensen - https://github.com/JChristensen/Button
 - Wire (built-in)
*/
#include <Wire.h>
#include <Button.h>
#include "Adafruit_MCP23017.h"
#define PIN_RGBLED_R 9
#define PIN_RGBLED_G 10
#define PIN_RGBLED_B 11
#define PIN_PIEZO_SPKR 8
#define PIN_BTN_L 2
#define PIN_BTN_R 3
#define PIN_POT_L 0
#define PIN_POT_R 1
#define MODE_EDIT 0x00
#define MODE_PLAY 0x01
#define INVERT true
#define PULLUP true
#define DEBOUNCE_MS 20
 
int durations[15];
char notes[15];
int currentMode = MODE_EDIT;
int currentNote = 0;

Button upBtn(PIN_BTN_L, PULLUP, INVERT, DEBOUNCE_MS);
Button downBtn(PIN_BTN_R, PULLUP, INVERT, DEBOUNCE_MS);

Adafruit_MCP23017 mcp;

void setup(){
  pinMode(PIN_RGBLED_R, OUTPUT);
  pinMode(PIN_RGBLED_G, OUTPUT);
  pinMode(PIN_RGBLED_B, OUTPUT);
  pinMode(PIN_PIEZO_SPKR, OUTPUT);
  pinMode(PIN_BTN_L, INPUT);
  pinMode(PIN_BTN_R, INPUT);
  
  
  mcp.begin();
  for(int i = 0; i < 16; i++){
    mcp.pinMode(i, OUTPUT);
  }
}

void loop(){
  downBtn.read();
  upBtn.read();
  if(upBtn.isPressed() && downBtn.isPressed()){
    if(currentMode == MODE_EDIT){
      playTone('c', 30);
      currentMode = MODE_PLAY;
      while(upBtn.isPressed() && downBtn.isPressed()){
        downBtn.read();
        upBtn.read();
      }
    } else {
      currentMode = MODE_EDIT;
      playTone('c', 30);
      delay(100);
      playTone('c', 30);
      while(upBtn.isPressed() && downBtn.isPressed()){
        downBtn.read();
        upBtn.read();
      }
    }
  } else {
    switch(currentMode){
      case MODE_EDIT:
        editMode();
        break;
      case MODE_PLAY:
        playMode();
        break;
    }
  }
}

void editMode(){
  
  //delay(100);
  upBtn.read();
  downBtn.read();
  if(upBtn.isPressed()){
    currentNote = (currentNote + 1) % 16;
     while(upBtn.isPressed()){
        upBtn.read();
        downBtn.read();
        if(upBtn.isPressed() && downBtn.isPressed()){
          break;
        } 
      }
  } else if (downBtn.isPressed()){
    currentNote = currentNote == 0? 15 : (currentNote - 1) % 16;
     while(downBtn.isPressed()){
       upBtn.read();
       downBtn.read();
       if(upBtn.isPressed() && downBtn.isPressed()){
         break;
       }
      }
  } else {   
  int volPotVal = analogRead(PIN_POT_L);
    int tonePotVal = analogRead(PIN_POT_R);
    char names[] = { 'c', 'd', 'e', 'f', 'g', 'a', 'b', 'C', 'x'};
    int noteNum = map(tonePotVal,0, 1023, 0, 8);
    int durVal = map(volPotVal, 0, 1023, 0, 500);
    playNote(names[noteNum], durVal);
    LEDNote(names[noteNum]);
    durations[currentNote] = durVal;
    notes[currentNote] = names[noteNum];
    for(int i = 0; i < 16; i++){
      if (i == currentNote){
        mcp.digitalWrite(i, HIGH);
      } else {
        mcp.digitalWrite(i, LOW);
      }
    }
  }
}

void playMode(){
  for(int i = 0; i < 16; i++){
    playNote(notes[i], durations[i]);
    mcp.digitalWrite(i, HIGH);
    for(int j = 0; j < 16; j++){
      if(j != i){
        mcp.digitalWrite(j, LOW);
      }
    }
  }
  
}
  
void LEDNote(char note){
   switch(note){
     case 'c':
       digitalWrite(PIN_RGBLED_R, HIGH);
       digitalWrite(PIN_RGBLED_G, LOW);
       digitalWrite(PIN_RGBLED_B, LOW);
       break;
     case 'd':
       digitalWrite(PIN_RGBLED_R, HIGH);
       analogWrite(PIN_RGBLED_G, 127);
       digitalWrite(PIN_RGBLED_B, LOW);
       break;
     case 'e':
       digitalWrite(PIN_RGBLED_R, HIGH);
       digitalWrite(PIN_RGBLED_G, LOW);
       analogWrite(PIN_RGBLED_B, 127);
       break;
     case 'f':
       digitalWrite(PIN_RGBLED_R, LOW);
       digitalWrite(PIN_RGBLED_G, HIGH);
       digitalWrite(PIN_RGBLED_B, LOW);
       break;
     case 'g':
       digitalWrite(PIN_RGBLED_R, LOW);
       digitalWrite(PIN_RGBLED_G, HIGH);
       digitalWrite(PIN_RGBLED_B, HIGH);
       break;
     case 'a':
       digitalWrite(PIN_RGBLED_R, LOW);
       digitalWrite(PIN_RGBLED_G, LOW);
       digitalWrite(PIN_RGBLED_B, HIGH);
       break;
     case 'b':
       analogWrite(PIN_RGBLED_R, 88);
       digitalWrite(PIN_RGBLED_G, HIGH);
       digitalWrite(PIN_RGBLED_B, HIGH);
       break;
     case 'C':
       analogWrite(PIN_RGBLED_R, HIGH);
       digitalWrite(PIN_RGBLED_G, HIGH);
       digitalWrite(PIN_RGBLED_B, HIGH);
       break;
      case 'x':
        digitalWrite(PIN_RGBLED_R, LOW);
       digitalWrite(PIN_RGBLED_G, LOW);
       digitalWrite(PIN_RGBLED_B, LOW);
       break;
   }
  
}



void playTone(int tone, int duration) {
  for (long i = 0; i < duration * 1000L; i += tone * 2) {
    digitalWrite(PIN_PIEZO_SPKR, HIGH);
    delayMicroseconds(tone);
    digitalWrite(PIN_PIEZO_SPKR, LOW);
    delayMicroseconds(tone);
  }
}

void playNote(char note, int duration) {
  char names[] = { 'c', 'd', 'e', 'f', 'g', 'a', 'b', 'C' };
  int tones[] = { 1915, 1700, 1519, 1432, 1275, 1136, 1014, 956 };

  // play the tone corresponding to the note name
  for (int i = 0; i < 8; i++) {
    if (names[i] == note) {
      playTone(tones[i], duration);
    }
  }
}   
