#include <Wire.h> //<- muss für die Bib importiert werden
#include <TimerOne.h>
#include "NKQlockTwo.h"

#define BTN_MODE 6
#define BTN_OBEN 4
#define BTN_UNTEN 5
//750000
#define MODE_HHMM 1000000
#define MODE_SS 1000000

NKQlockTwo clock(0);

bool buttons[4] = {false, false, false, false};



void timing_loop(){
  sei();
  clock.readTime();
  //clock.displayTime();
}

void setup(){
  Wire.begin();
              
  pinMode(BTN_MODE, INPUT_PULLUP); // Button mode  
  pinMode(BTN_OBEN, INPUT_PULLUP); // Button mode  
  pinMode(BTN_UNTEN, INPUT_PULLUP); // Button mode  
              
  Timer1.initialize(MODE_HHMM);
  Timer1.attachInterrupt(timing_loop);
  
//  clock.setTime(1,2,3,4,5,6,7);
  clock.readTime();
}

void loop(){
  //Serial.println("Hallo Welt");
  if(!digitalRead(BTN_MODE)){
    if(!buttons[0]){
      buttons[0] = true;
      clock.displayHHMM ^= 1;
      if(clock.displayHHMM){
        Timer1.setPeriod(MODE_HHMM);
      }else{
        Timer1.setPeriod(MODE_SS);
      }
    }
  }else{
    buttons[0] = false;
  }
  
  if(!digitalRead(BTN_OBEN)){
    if(!buttons[1]){
      buttons[1] = true;
      clock.incHours();
    }
  }else{
    buttons[1] = false;
  }
  
  if(!digitalRead(BTN_UNTEN)){
    if(!buttons[2]){
      buttons[2] = true;
      clock.incMinutes();
    }
  }else{
    buttons[2] = false;
  }
  
  ulong data;
	for(int i = 0; i < 10; ++i){

		digitalWrite(LATCH_PIN, 0);
          data = clock.matrix[i];
          shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, (byte)(data >> 24));
          shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, (byte)(data >> 16));
          shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, (byte)(data >> 8));
          shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, (byte)(data));
          
		digitalWrite(LATCH_PIN, 1);
	}
//clock.displayMatrix();
//  delay(1000);

}
