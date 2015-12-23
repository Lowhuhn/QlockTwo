#ifndef NKQlockTwo_h
#define NKQlockTwo_h 1

//#include "/Applications/Arduino.app/Contents/Resources/Java/hardware/arduino/cores/arduino/Arduino.h"
#include "Arduino.h"

#define uint unsigned int
#define ulong unsigned long

#define LATCH_PIN 7
#define CLOCK_PIN 12
#define DATA_PIN 11



#define ROWOFF 0x0000FFFF
#define ROWON 0xFFFF0000

//Zeile 1
//0b1100 0000 0000 0000
#define ES 0xFFFFC000
//0b0001 1100 0000 0000
#define IST 0xFFFF1C00
//0b0000 0001 1110 0000
#define FUNFVORNACH 0xFFFF01E0

//Zeile 2
#define ZEHNVORNACH 0xFFFFF000
#define ZWANZIGVORNACH 0xFFFF0FE0

//Zeile 3
#define VIERTEL 0xFFFF0FE0
#define DREIVIERTEL 0xFFFFFFE0

//Zeile 4
#define VOR 0xFFFFE000
#define NACH 0xFFFF01E0

//Zeile 5
#define HALB 0xFFFFF000
#define HALB_2 0x0000F000
#define ELF 0xFFFF0700
#define FUNF 0xFFFF01E0

//Zeile 6
#define EIN 0xFFFFE000
#define EINS 0xFFFFF000
#define ZWEI 0xFFFF01E0

//Zeile 7
#define DREI 0xFFFFF000
#define VIER 0xFFFF01E0

//Zeile 8 
#define SECHS 0xFFFFF800
#define ACHT 0xFFFF01E0

//Zeile 9
#define SIEBEN 0xFFFFFC00
#define ZWOLF 0xFFFF03E0

//Zeile 10
#define ZEHN 0xFFFFF000
#define NEUN 0xFFFF1E00
#define UHR 0xFFFF00E0

#define ZEILE_ON(z) (0x80000000 >> (z))
#define SETZEILE(zeile, val) ZEILE_ON(zeile)|(~(val))
#define SETVOR matrix[3] = SETZEILE(3, VOR);
#define SETNACH matrix[3] = SETZEILE(3, NACH);

//00000000 000 1 234 0 0000 0000 0000 0000
#define PLUS1 0x0010FFFF
#define PLUS2 0x0018FFFF
#define PLUS3 0x001CFFFF
#define PLUS4 0x001EFFFF

class NKQlockTwo
{
public:
	NKQlockTwo(ulong x);
	void readTime();
	void displayTime();
	void resetMatrix();
	void setTime();
	void incHours();
	void incMinutes();

	bool displayHHMM;
	ulong matrix[10];// = {0,0,0,0,0,0,0,0,0,0};

private:
	byte seconds;
	byte minutes;
	byte hours;
	byte dayOfWeek;
	byte day;
	byte month;
	byte year;

	ulong timeHash;

	void setHour(int h);
	void setTime_HHMM();
	void setTime_SS();
	void timeToMatrix();
};

#endif