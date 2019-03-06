#ifndef YM2413_H
#define	YM2413_H

#define _XTAL_FREQ 4000000
#define YD0 LATA0
#define YD1 LATA1
#define YD2 LATA2
#define YD3 LATA3
#define YD4 LATB4
#define YD5 LATE0
#define YD6 LATE1
#define YD7 LATB3

#define Ao LATB2
#define WE LATD5
#define CS LATD6
#define IC LATD7

#include "string.h"
int fnumbers[12] = {172,181,192,204,216,229,242,257,272,288,305,323};
char* notes[] = {"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};
void YAMAHA();
int get_bits(int n, int target);
int modifyBit(int n, int p, int b);
void YM2413Write(int address, int data);
void playNote(int channel, char* note, int oct, int instrument, int vol);
void stopNote(int channel,int sustain);
void delay_ms(int x);
void canon(int base_instrument, int main_instrument, float bpm);

#endif	/* YM2413_H */

