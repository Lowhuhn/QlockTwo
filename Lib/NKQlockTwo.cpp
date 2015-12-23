/*
  NKQlockTwo.cpp - Library for QlockTwo.
  Created by Nils Kaminski 2015
  Released into the public domain.
*/
//#include "/Applications/Arduino.app/Contents/Resources/Java/hardware/arduino/cores/arduino/Arduino.h"
#include "Arduino.h"
#include "NKQlockTwo.h"
#include <Wire.h>

#define DS3231_I2C_ADDRESS 0x68

ulong seconds_numbers[10][7] = {
	{0b01110,0b10001,0b10011,0b10101,0b11001,0b10001,0b01110}, //0
	{0b00100, 0b01100,0b00100,0b00100,0b00100,0b00100,0b01110}, //1
	{0b01110, 0b10001,0b10001,0b00010,0b00100,0b01000,0b11111}, //2
	{0b11111,0b00010,0b00100,0b00010,0b00001,0b10001,0b01110}, //3
	{0b00010,0b00110,0b01010,0b10010,0b11111,0b00010,0b00010}, //4
	{0b11111,0b10000,0b11110,0b00001,0b00001,0b10001,0b01110}, //5
	{0b00110,0b01000,0b10000,0b11110,0b10001,0b10001,0b01110}, //6
	{0b11111,0b00001,0b00010,0b00100,0b01000,0b01000,0b01000}, //7
	{0b01110,0b10001,0b10001,0b01110,0b10001,0b10001,0b01110}, //8 
	{0b01110,0b10001,0b10001,0b01111,0b00001,0b00010,0b01100} //9
};


byte decToBcd(byte val)
{
  return( (val/10*16) + (val%10) );
}
// Convert binary coded decimal to normal decimal numbers
byte bcdToDec(byte val)
{
  return( (val/16*10) + (val%16) );
}


NKQlockTwo::NKQlockTwo(ulong x){

	for(int i = 0;i < 10;++i){
		matrix[i] = x;
	}

	pinMode(LATCH_PIN, OUTPUT);
	pinMode(CLOCK_PIN, OUTPUT);
	pinMode(DATA_PIN, OUTPUT);

	seconds = 0;
	minutes =0;
	hours = 0;
	dayOfWeek = 1;
	day = 1;
	month = 1;
	year = 1;

	timeHash = 0;

	displayHHMM = true;
}

void NKQlockTwo::readTime(){
	Wire.beginTransmission(DS3231_I2C_ADDRESS);
	Wire.write(0); // set DS3231 register pointer to 00h
	Wire.endTransmission();
	Wire.requestFrom(DS3231_I2C_ADDRESS, 3);
	// request seven bytes of data from DS3231 starting from register 00h
	seconds = bcdToDec(Wire.read() & 0x7f);
	minutes = bcdToDec(Wire.read());
	hours = bcdToDec(Wire.read() & 0x3f);
	/*dayOfWeek = bcdToDec(Wire.read());
	day = bcdToDec(Wire.read());
	month = bcdToDec(Wire.read());
	year = bcdToDec(Wire.read());*/
	timeToMatrix();
}

void NKQlockTwo::setTime(){

	// sets time and date data to DS3231
  Wire.beginTransmission(DS3231_I2C_ADDRESS);
  Wire.write(0); // set next input to start at the seconds register
  Wire.write(decToBcd(seconds)); // set seconds
  Wire.write(decToBcd(minutes)); // set minutes
  Wire.write(decToBcd(hours)); // set hours
  Wire.write(decToBcd(dayOfWeek)); // set day of week (1=Sunday, 7=Saturday)
  Wire.write(decToBcd(day)); // set date (1 to 31)
  Wire.write(decToBcd(month)); // set month
  Wire.write(decToBcd(year)); // set year (0 to 99)
  Wire.endTransmission();
}

void NKQlockTwo::incHours(){
	if((++hours) > 12){
		hours = 1;
	}
	setTime();
}

void NKQlockTwo::incMinutes(){
	if((++minutes) > 59){
		minutes = 0;
	}
	setTime();
}

void NKQlockTwo::displayTime(){
	Serial.print(hours, DEC);
	Serial.print(":");
	Serial.print(minutes, DEC);
	Serial.print(":");
	Serial.println(seconds, DEC);
}

void NKQlockTwo::resetMatrix(){
	for(int i = 0; i < 10; ++i){
		matrix[i] = ROWOFF;
	}
}

void NKQlockTwo::timeToMatrix(){
	if(displayHHMM){
		ulong hash = ((ulong)(hours+1))*((ulong)(minutes+1));
		if(timeHash == hash){
			return;
		}
		displayTime();
		timeHash = hash;
		setTime_HHMM();
	}else{
		ulong hash2 = (ulong)(seconds+1);
		if(timeHash == hash2){
			return;
		}
		displayTime();
		timeHash = hash2;
		setTime_SS();
	}
}

void NKQlockTwo::setHour(int h){
	switch(h % 12){
		case 0:
			matrix[8] = SETZEILE(8, (ZWOLF) );
			break;
		case 1:
			matrix[5] = SETZEILE(5, EINS);
			break;
		case 2:
			matrix[5] = SETZEILE(5, ZWEI);
			break;
		case 3:
			matrix[6] = SETZEILE(6, DREI);
			break;
		case 4:
			matrix[6] = SETZEILE(6, VIER);
			break;
		case 5:
			matrix[4] = SETZEILE(4, FUNF);
			break;
		case 6:
			matrix[7] = SETZEILE(7, SECHS);
			break;
		case 7:
			matrix[8] = SETZEILE(8, SIEBEN);
			break;
		case 8:
			matrix[7] = SETZEILE(7, ACHT);
			break;
		case 9:
			matrix[9] = SETZEILE(9, NEUN);
			break;
		case 10:
			matrix[9] = SETZEILE(9, ZEHN);
			break;
		case 11:
			matrix[4] = SETZEILE(4, ELF);
			break;	
	}
}

void NKQlockTwo::setTime_HHMM(){
//First reset the matrix
	resetMatrix();

	matrix[0] = SETZEILE(0, ES | IST);

	int m = minutes / 5;
	if(m < 5){
		setHour(hours);
	}else{
		setHour(hours+1);
	}

	switch(m){
		case 0: //00 - 04 ES IST X UHR
            if(hours%12 == 1){
              matrix[5] = SETZEILE(5, EIN);
            }
            if(hours == 9){
              matrix[9] = SETZEILE(9, UHR | NEUN);
            }else if(hours == 10){
              matrix[9] = SETZEILE(9, UHR | ZEHN);
            }else{
              matrix[9] = SETZEILE(9, UHR);
            }
			break;
		case 1: //05 - 09 ES IST FÜNF NACH X
			matrix[0] = SETZEILE(0, ES | IST | FUNFVORNACH);
			SETNACH;
			break;
		case 2: //10 - 14 ES IST ZEHN NACH X
			matrix[1] = SETZEILE(1, ZEHNVORNACH);
			SETNACH;
			break;
		case 3: //15 - 19 ES IST VIERTEL NACH X
			matrix[2] = SETZEILE(2, VIERTEL);
			SETNACH;
			break;
		case 4: //20 - 24 ES IST ZWANZIG NACH X
			matrix[1] = SETZEILE(1, ZWANZIGVORNACH);
			SETNACH;
			break;
		case 5: //25 - 29 ES IST FÜNF VOR HALB X+1
			//matrix[0] |= SETZEILE(0, FUNFVORNACH);
            matrix[0] = SETZEILE(0, ES | IST | FUNFVORNACH);
			if(hours+1 == 5){
				matrix[4] = SETZEILE(4, FUNF | HALB);        
			}else if(hours +1 == 11){
				matrix[4] = SETZEILE(4, ELF | HALB);
			}else{
			 	matrix[4] = SETZEILE(4, HALB);
			}
			SETVOR;
			//matrix[3] = SETZEILE(3, 
			break;
		case 6: //30-34 ES IST HALB X+1
			if(hours+1 == 5){
				matrix[4] = SETZEILE(4, FUNF | HALB);        
			}else if(hours +1 == 11){
				matrix[4] = SETZEILE(4, ELF | HALB);
			}else{
				matrix[4] = SETZEILE(4, HALB);
			}
			break;
		case 7: //35 - 39 ES IST FÜNF NACH HALB X+1
			//matrix[0] |= SETZEILE(0, FUNFVORNACH);
            matrix[0] = SETZEILE(0, ES | IST | FUNFVORNACH);
			if(hours+1 == 5){
				matrix[4] = SETZEILE(4, FUNF | HALB);        
			}else if(hours +1 == 11){
				matrix[4] = SETZEILE(4, ELF | HALB);
			}else{
				matrix[4] = SETZEILE(4, HALB);
			}
			SETNACH;
			break;
		case 8: //40 - 44 ES IST ZWANZIG VOR VOR X+1
			matrix[1] = SETZEILE(1, ZWANZIGVORNACH);
			SETVOR;
			break;
		case 9: //45 - 49 ES IST VIRTEL VOR X+1 
			matrix[2] = SETZEILE(2, VIERTEL);
			SETVOR;
			break;
		case 10: //50 - 54 ES IST ZEHN VOR X+1
			matrix[1] = SETZEILE(1, ZEHNVORNACH);
			SETVOR;
			break;
		case 11: //55-59 ES IST FÜNF VOR X+1
			matrix[0] = SETZEILE(0, ES | IST | FUNFVORNACH);
			SETVOR;
                        break;
	}

	m = minutes%5;
	ulong d = 0;
	switch(m){
		case 1:
			d = PLUS1;
			break;
		case 2:
			d = PLUS2;
			break;
		case 3:
			d = PLUS3;
			break;
		case 4:
			d = PLUS4;
			break;
		case 0:
			return;
	}
	int i = 1;
	for(i = 1; i < 10; ++i){
		if(matrix[i] == ROWOFF){
			matrix[i] = d;
			break;
		}
	}

  return;
}

void NKQlockTwo::setTime_SS(){
	resetMatrix();

	ulong* a = seconds_numbers[seconds/10];
	ulong* b = seconds_numbers[seconds%10];

	for(int i = 0; i < 7; ++i){
		matrix[2+i] = SETZEILE(2+i, 0xFFFF0000 | (a[i] << 5+1+5) | (b[i] << 5));
	}

}