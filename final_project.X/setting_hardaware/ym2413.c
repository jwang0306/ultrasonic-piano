#include "ym2413.h"
#include <xc.h>

//char* canonNote[] = {
//    
//}



void YAMAHA(){
//    ADCON1 = 15;
//    
//    TRISCbits.RC3 = 0;
//    TRISCbits.RC4 = 1;
//    TRISCbits.RC5 = 0;
    TRISBbits.RB2 = 0;
    TRISDbits.RD5 = 0;
    TRISDbits.RD6 = 0;
    TRISDbits.RD7 = 0;
            
    TRISAbits.RA0 = 0;
    TRISAbits.RA1 = 0;
    TRISAbits.RA2 = 0;
    TRISAbits.RA3 = 0;
    TRISBbits.RB4 = 0;
    TRISEbits.RE0 = 0;
    TRISEbits.RE1 = 0;
    TRISBbits.RB3 = 0;
    
    TRISDbits.RD4 = 0;
    TRISDbits.RD1 = 0;
    TRISDbits.RD2 = 0;
    TRISDbits.RD3 = 0;
   
    CS = 1;
    WE = 0;
    IC = 0;
    __delay_ms(100);
    IC = 1;
    
//    while(1) {
////    playNote(0, 0,  4, 5, 0);
// 
//   // stopNote(r,0);
//   
//        playNote(1, 0, 4, 5, 0);
////        playNote(2, 7, 4, 5, 0);
////        playNote(3, 1, 2, 3, 15);
//        
//         __delay_ms(500);
////        for (int i = 0; i < 2000; ++i);
//        //  stopNote(0, 1);
////        stopNote(1, 1);
////        stopNote(2, 1);
//    }
}

int get_bits(int n, int target){
    int result = (target & (1 << n)) >> n;
    return result;
}

int modifyBit(int n, int p, int b) { 
    int mask = 1 << p; 
    return (n & ~mask) | ((b << p) & mask); 
}

void YM2413Write(int address, int data){
    
    //  CS WE A0
    //  1  x  x  = Bus Inactive
    //  0  0  0  = Write register address
    //  0  0  1  = Write register content
    
    CS = 1;
    Ao = 0;
    YD0 = get_bits(0, address);
    YD1 = get_bits(1, address);
    YD2 = get_bits(2, address);
    YD3 = get_bits(3, address);
    YD4 = get_bits(4, address);
    YD5 = get_bits(5, address);
    YD6 = get_bits(6, address);
    YD7 = get_bits(7, address);
    CS = 0;
    
    __delay_us(3.5);
    CS = 1;
    
    CS = 1;
    Ao = 1;
    YD0 = get_bits(0, data);
    YD1 = get_bits(1, data);
    YD2 = get_bits(2, data);
    YD3 = get_bits(3, data);
    YD4 = get_bits(4, data);
    YD5 = get_bits(5, data);
    YD6 = get_bits(6, data);
    YD7 = get_bits(7, data);
    CS = 0;
    
    __delay_us(22);
    
    CS = 1;
}

void playNote(int channel, char* note, int oct, int instrument, int vol) {

    YM2413Write(0x0E, 0);
    
    int noteNumber;
    // find the notenumber in the notes array
    for (int i = 0; i < 12; i++) {
//        if (strcmp(notes[i], note) == 0) {5
        if (notes[i] == note) {
            // select the right f number from the array 
            if (i > 11) i=i-12;
            noteNumber = i;
            break;
        }
    }
    YM2413Write(0x10 + channel, fnumbers[noteNumber]);
    
    int yB20 = 0;
    yB20 = modifyBit(yB20, 0, get_bits(8, fnumbers[noteNumber]));
    yB20 = modifyBit(yB20, 1, get_bits(0, oct));
    yB20 = modifyBit(yB20, 2, get_bits(1, oct));
    yB20 = modifyBit(yB20, 3, get_bits(2, oct));
    yB20 = modifyBit(yB20, 4, 1);
    YM2413Write(0x20 + channel, yB20);
    
    int yB30 = instrument << 4 | vol;
    YM2413Write(0x30 + channel, yB30);
}


void stopNote(int channel,int sustain){
  int yB20 = 0;
  yB20 = modifyBit(yB20, 5, sustain);
  YM2413Write((0x20 + channel), yB20);
}

void delay_ms(int x) {
//    for (int i = 0; i < x; i++) {
//        __delay_ms(1);
//    }
    T0CON = 0x84;
    TMR0 = 65535 - x*2400;
    INTCONbits.TMR0IF = 0;
    while (!INTCONbits.TMR0IF);

    return;
}

void canon(int base_instrument, int main_instrument, float bpm) {
    
    float time = bpm*16;
    
    // =================================FIRST PART====================================
    playNote(0, "D", 3, base_instrument, 0);
    delay_ms(time/4);
    playNote(1, "A", 4, main_instrument, 2);
    delay_ms(time/4);
    stopNote(1, 1);
    playNote(1, "D", 5, main_instrument, 2);
    delay_ms(time/4);
    stopNote(1, 1);
    playNote(1, "F#", 5, main_instrument, 2);
    delay_ms(time/4);
    stopNote(1, 1);
    stopNote(0, 1);

    playNote(0, "A", 2, base_instrument, 0);
    delay_ms(time/4);
    playNote(1, "E", 4, main_instrument, 2);
    delay_ms(time/4);
    stopNote(1, 1);
    playNote(1, "A", 4, main_instrument, 2);
    delay_ms(time/4);
    stopNote(1, 1);
    playNote(1, "C#", 5, main_instrument, 2);
    delay_ms(time/4);
    stopNote(1, 1);
    stopNote(0, 1);
    
    playNote(0, "B", 2, base_instrument, 0);
    delay_ms(time/4);
    playNote(1, "F#", 4, main_instrument, 2);
    delay_ms(time/4);
    stopNote(1, 1);
    playNote(1, "B", 4, main_instrument, 2);
    delay_ms(time/4);
    stopNote(1, 1);
    playNote(1, "D", 5, main_instrument, 2);
    delay_ms(time/4);
    stopNote(1, 1);
    stopNote(0, 1);
    
    playNote(0, "F#", 2, base_instrument, 0);
    delay_ms(time/4);
    playNote(1, "F#", 4, main_instrument, 2);
    delay_ms(time/4);
    stopNote(1, 1);
    playNote(1, "A", 4, main_instrument, 2);
    delay_ms(time/4);
    stopNote(1, 1);
    playNote(1, "C#", 5, main_instrument, 2);
    delay_ms(time/4);
    stopNote(1, 1);
    stopNote(0, 1);
    
    playNote(0, "G", 2, base_instrument, 0);
    delay_ms(time/4);
    playNote(1, "D", 4, main_instrument, 2);
    delay_ms(time/4);
    stopNote(1, 1);
    playNote(1, "G", 4, main_instrument, 2);
    delay_ms(time/4);
    stopNote(1, 1);
    playNote(1, "B", 4, main_instrument, 2);
    delay_ms(time/4);
    stopNote(1, 1);
    stopNote(0, 1);
    
    playNote(0, "D", 2, base_instrument, 0);
    delay_ms(time/4);
    playNote(1, "D", 4, main_instrument, 2);
    delay_ms(time/4);
    stopNote(1, 1);
    playNote(1, "F#", 4, main_instrument, 2);
    delay_ms(time/4);
    stopNote(1, 1);
    playNote(1, "A", 4, main_instrument, 2);
    delay_ms(time/4);
    stopNote(1, 1);
    stopNote(0, 1);
    
    playNote(0, "G", 2, base_instrument, 0);
    delay_ms(time/4);
    playNote(1, "D", 4, main_instrument, 2);
    delay_ms(time/4);
    stopNote(1, 1);
    playNote(1, "G", 4, main_instrument, 2);
    delay_ms(time/4);
    stopNote(1, 1);
    playNote(1, "B", 4, main_instrument, 2);
    delay_ms(time/4);
    stopNote(1, 1);
    stopNote(0, 1);
    
    playNote(0, "A", 2, base_instrument, 0);
    delay_ms(time/4);
    playNote(1, "E", 4, main_instrument, 2);
    delay_ms(time/4);
    stopNote(1, 1);
    playNote(1, "A", 4, main_instrument, 2);
    delay_ms(time/4);
    stopNote(1, 1);
    playNote(1, "C#", 5, main_instrument, 2);
    delay_ms(time/4);
    stopNote(1, 1);
    stopNote(0, 1);
    
    // ----------------------------------------------------------------------
    
    playNote(0, "D", 3, base_instrument, 0);
    playNote(2, "D", 4, base_instrument, 0);
    delay_ms(time/4);
    playNote(1, "F#", 4, main_instrument, 2);
    delay_ms(time/4);
    stopNote(1, 1);
    playNote(1, "A", 4, main_instrument, 2);
    delay_ms(time/4);
    stopNote(1, 1);
    playNote(1, "D", 5, main_instrument, 2);
    delay_ms(time/4);
    stopNote(0, 1);
    stopNote(1, 1);
    stopNote(2, 1);
    
    playNote(0, "A", 2, base_instrument, 0);
    playNote(2, "A", 3, base_instrument, 0);
    delay_ms(time/4);
    playNote(1, "E", 4, main_instrument, 2);
    delay_ms(time/4);
    stopNote(1, 1);
    playNote(1, "A", 4, main_instrument, 2);
    delay_ms(time/4);
    stopNote(1, 1);
    playNote(1, "C#", 5, main_instrument, 2);
    delay_ms(time/4);
    stopNote(0, 1);
    stopNote(1, 1);
    stopNote(2, 1);
    
    playNote(0, "B", 2, base_instrument, 0);
    playNote(2, "B", 3, base_instrument, 0);
    delay_ms(time/4);
    playNote(1, "D", 4, main_instrument, 2);
    delay_ms(time/4);
    stopNote(1, 1);
    playNote(1, "F#", 4, main_instrument, 2);
    delay_ms(time/4);
    stopNote(1, 1);
    playNote(1, "B", 4, main_instrument, 2);
    delay_ms(time/4);
    stopNote(0, 1);
    stopNote(1, 1);
    stopNote(2, 1);
    
    playNote(0, "F#", 2, base_instrument, 0);
    playNote(2, "F#", 3, base_instrument, 0);
    delay_ms(time/4);
    playNote(1, "C#", 4, main_instrument, 2);
    delay_ms(time/4);
    stopNote(1, 1);
    playNote(1, "F#", 4, main_instrument, 2);
    delay_ms(time/4);
    stopNote(1, 1);
    playNote(1, "A", 4, main_instrument, 2);
    delay_ms(time/4);
    stopNote(0, 1);
    stopNote(1, 1);
    stopNote(2, 1);
    
    playNote(0, "G", 2, base_instrument, 0);
    playNote(2, "G", 3, base_instrument, 0);
    delay_ms(time/4);
    playNote(1, "B", 3, main_instrument, 2);
    delay_ms(time/4);
    stopNote(1, 1);
    playNote(1, "D", 4, main_instrument, 2);
    delay_ms(time/4);
    stopNote(1, 1);
    playNote(1, "G", 4, main_instrument, 2);
    delay_ms(time/4);
    stopNote(0, 1);
    stopNote(1, 1);
    stopNote(2, 1);
    
    playNote(0, "D", 2, base_instrument, 0);
    playNote(2, "D", 3, base_instrument, 0);
    delay_ms(time/4);
    playNote(1, "A", 3, main_instrument, 2);
    delay_ms(time/4);
    stopNote(1, 1);
    playNote(1, "D", 4, main_instrument, 2);
    delay_ms(time/4);
    stopNote(1, 1);
    playNote(1, "F#", 4, main_instrument, 2);
    delay_ms(time/4);
    stopNote(0, 1);
    stopNote(1, 1);
    stopNote(2, 1);
    
    playNote(0, "G", 2, base_instrument, 0);
    playNote(2, "G", 3, base_instrument, 0);
    delay_ms(time/4);
    playNote(1, "B", 3, main_instrument, 2);
    delay_ms(time/4);
    stopNote(1, 1);
    playNote(1, "D", 4, main_instrument, 2);
    delay_ms(time/4);
    stopNote(1, 1);
    playNote(1, "G", 4, main_instrument, 2);
    delay_ms(time/4);
    stopNote(0, 1);
    stopNote(1, 1);
    stopNote(2, 1);
    
    playNote(0, "A", 2, base_instrument, 0);
    playNote(2, "A", 3, base_instrument, 0);
    delay_ms(time/4);
    playNote(1, "C#", 4, base_instrument, 2);
    delay_ms(time/4);
    stopNote(1, 1);
    playNote(1, "E", 4, base_instrument, 2);
    delay_ms(time/4);
    stopNote(1, 1);
    playNote(1, "A", 4, base_instrument, 2);
    delay_ms(time/4);
    stopNote(0, 1);
    stopNote(1, 1);
    stopNote(2, 1);

    // =================================SECOND PART====================================
    playNote(1, "F#", 5, main_instrument, 0);
    playNote(2, "D", 3, base_instrument, 0);
    delay_ms(time/4);
    stopNote(0, 1);
    playNote(0, "A", 3, base_instrument, 2);
    delay_ms(time/4);
    stopNote(0, 1);
    playNote(0, "D", 4, base_instrument, 2);
    delay_ms(time/4);
    stopNote(0, 1);
    playNote(0, "F#", 4, base_instrument, 2);
    delay_ms(time/4);
    stopNote(0, 1);
    stopNote(1, 1);
    stopNote(2, 1);
    
    playNote(1, "E", 5, main_instrument, 0);
    playNote(2, "A", 2, base_instrument, 0);
    delay_ms(time/4);
    stopNote(0, 1);
    playNote(0, "E", 3, base_instrument, 2);
    delay_ms(time/4);
    stopNote(0, 1);
    playNote(0, "A", 3, base_instrument, 2);
    delay_ms(time/4);
    stopNote(0, 1);
    playNote(0, "C#", 4, base_instrument, 2);
    delay_ms(time/4);
    stopNote(0, 1);
    stopNote(1, 1);
    stopNote(2, 1);
    
    playNote(1, "D", 5, main_instrument, 0);
    playNote(2, "B", 2, base_instrument, 0);
    delay_ms(time/4);
    stopNote(0, 1);
    playNote(0, "F#", 3, base_instrument, 2);
    delay_ms(time/4);
    stopNote(0, 1);
    playNote(0, "B", 3, base_instrument, 2);
    delay_ms(time/4);
    stopNote(0, 1);
    playNote(0, "D", 4, base_instrument, 2);
    delay_ms(time/4);
    stopNote(0, 1);
    stopNote(1, 1);
    stopNote(2, 1);
    
    playNote(1, "C#", 5, main_instrument, 0);
    playNote(2, "F#", 2, base_instrument, 0);
    delay_ms(time/4);
    stopNote(0, 1);
    playNote(0, "C#", 3, base_instrument, 2);
    delay_ms(time/4);
    stopNote(0, 1);
    playNote(0, "F#", 3, base_instrument, 2);
    delay_ms(time/4);
    stopNote(0, 1);
    playNote(0, "A", 3, base_instrument, 2);
    delay_ms(time/4);
    stopNote(0, 1);
    stopNote(1, 1);
    stopNote(2, 1);
    
    playNote(1, "B", 4, main_instrument, 0);
    playNote(2, "G", 2, base_instrument, 0);
    delay_ms(time/4);
    stopNote(0, 1);
    playNote(0, "D", 3, base_instrument, 2);
    delay_ms(time/4);
    stopNote(0, 1);
    playNote(0, "G", 3, base_instrument, 2);
    delay_ms(time/4);
    stopNote(0, 1);
    playNote(0, "B", 3, base_instrument, 2);
    delay_ms(time/4);
    stopNote(0, 1);
    stopNote(1, 1);
    stopNote(2, 1);
    
    playNote(1, "A", 4, main_instrument, 0);
    playNote(2, "D", 2, base_instrument, 0);
    delay_ms(time/4);
    stopNote(0, 1);
    playNote(0, "A", 2, base_instrument, 2);
    delay_ms(time/4);
    stopNote(0, 1);
    playNote(0, "D", 3, base_instrument, 2);
    delay_ms(time/4);
    stopNote(0, 1);
    playNote(0, "F#", 3, base_instrument, 2);
    delay_ms(time/4);
    stopNote(0, 1);
    stopNote(1, 1);
    stopNote(2, 1);
    
    playNote(1, "B", 4, main_instrument, 0);
    playNote(2, "G", 2, base_instrument, 0);
    delay_ms(time/4);
    stopNote(0, 1);
    playNote(0, "D", 3, base_instrument, 2);
    delay_ms(time/4);
    stopNote(0, 1);
    playNote(0, "G", 3, base_instrument, 2);
    delay_ms(time/4);
    stopNote(0, 1);
    playNote(0, "B", 3, base_instrument, 2);
    delay_ms(time/4);
    stopNote(0, 1);
    stopNote(1, 1);
    stopNote(2, 1);
    
    playNote(1, "C#", 5, main_instrument, 0);
    playNote(2, "A", 2, base_instrument, 0);
    delay_ms(time/4);
    stopNote(0, 1);
    playNote(0, "E", 3, base_instrument, 2);
    delay_ms(time/4);
    stopNote(0, 1);
    playNote(0, "A", 3, base_instrument, 2);
    delay_ms(time/4);
    stopNote(0, 1);
    playNote(0, "C#", 4, base_instrument, 2);
    delay_ms(time/4);
    stopNote(0, 1);
    stopNote(1, 1);
    stopNote(2, 1);
    
    //--------------------------------------------------------------------------
    playNote(1, "F#", 5, main_instrument, 0);
    playNote(3, "D", 3, base_instrument, 0);
    playNote(2, "D", 5, main_instrument, 2);
    delay_ms(time/4);
    stopNote(0, 1);
    stopNote(2, 1);
    playNote(0, "A", 3, base_instrument, 2);
    playNote(2, "F#", 4, main_instrument, 2);
    delay_ms(time/4);
    stopNote(0, 1);
    stopNote(2, 1);
    playNote(0, "D", 4, base_instrument, 2);
    playNote(2, "A", 4, main_instrument, 2);
    delay_ms(time/4);
    stopNote(0, 1);
    stopNote(2, 1);
    playNote(0, "F#", 4, base_instrument, 2);
    playNote(2, "D", 5, main_instrument, 2);
    delay_ms(time/4);
    stopNote(0, 1);
    stopNote(1, 1);
    stopNote(2, 1);
    stopNote(3, 1);
    
    playNote(1, "E", 5, main_instrument, 0);
    playNote(3, "A", 2, base_instrument, 0);
    playNote(2, "C#", 5, main_instrument, 2);
    delay_ms(time/4);
    stopNote(0, 1);
    stopNote(2, 1);
    playNote(0, "A", 3, base_instrument, 2);
    playNote(2, "E", 4, main_instrument, 2);
    delay_ms(time/4);
    stopNote(0, 1);
    stopNote(2, 1);
    playNote(0, "C#", 4, base_instrument, 2);
    playNote(2, "A", 4, main_instrument, 2);
    delay_ms(time/4);
    stopNote(0, 1);
    stopNote(2, 1);
    playNote(0, "E", 4, base_instrument, 2);
    playNote(2, "C#", 5, main_instrument, 2);
    delay_ms(time/4);
    stopNote(0, 1);
    stopNote(1, 1);
    stopNote(2, 1);
    stopNote(3, 1);
    
    playNote(1, "D", 5, main_instrument, 0);
    playNote(3, "B", 2, base_instrument, 0);
    playNote(2, "B", 4, main_instrument, 2);
    delay_ms(time/4);
    stopNote(0, 1);
    stopNote(2, 1);
    playNote(0, "F#", 3, base_instrument, 2);
    playNote(2, "D", 4, main_instrument, 2);
    delay_ms(time/4);
    stopNote(0, 1);
    stopNote(2, 1);
    playNote(0, "B", 3, base_instrument, 2);
    playNote(2, "F#", 4, main_instrument, 2);
    delay_ms(time/4);
    stopNote(0, 1);
    stopNote(2, 1);
    playNote(0, "D", 4, base_instrument, 2);
    playNote(2, "B", 4, main_instrument, 2);
    delay_ms(time/4);
    stopNote(0, 1);
    stopNote(1, 1);
    stopNote(2, 1);
    stopNote(3, 1);
    
    playNote(1, "C#", 5, main_instrument, 0);
    playNote(3, "F#", 2, base_instrument, 0);
    playNote(2, "A", 4, main_instrument, 2);
    delay_ms(time/4);
    stopNote(0, 1);
    stopNote(2, 1);
    playNote(0, "F#", 3, base_instrument, 2);
    playNote(2, "C#", 4, main_instrument, 2);
    delay_ms(time/4);
    stopNote(0, 1);
    stopNote(2, 1);
    playNote(0, "A", 3, base_instrument, 2);
    playNote(2, "F#", 4, main_instrument, 2);
    delay_ms(time/4);
    stopNote(0, 1);
    stopNote(2, 1);
    playNote(0, "C#", 4, base_instrument, 2);
    playNote(2, "A", 4, main_instrument, 2);
    delay_ms(time/4);
    stopNote(0, 1);
    stopNote(1, 1);
    stopNote(2, 1);
    stopNote(3, 1);

    playNote(1, "B", 4, main_instrument, 0);
    playNote(3, "G", 2, base_instrument, 0);
    playNote(2, "G", 4, main_instrument, 2);
    delay_ms(time/4);
    stopNote(0, 1);
    stopNote(2, 1);
    playNote(0, "D", 3, base_instrument, 2);
    playNote(2, "B", 3, main_instrument, 2);
    delay_ms(time/4);
    stopNote(0, 1);
    stopNote(2, 1);
    playNote(0, "G", 3, base_instrument, 2);
    playNote(2, "D", 4, main_instrument, 2);
    delay_ms(time/4);
    stopNote(0, 1);
    stopNote(2, 1);
    playNote(0, "B", 3, base_instrument, 2);
    playNote(2, "G", 4, main_instrument, 2);
    delay_ms(time/4);
    stopNote(0, 1);
    stopNote(1, 1);
    stopNote(2, 1);
    stopNote(3, 1);
    
    playNote(1, "A", 4, main_instrument, 0);
    playNote(3, "D", 2, base_instrument, 0);
    playNote(2, "F#", 4, main_instrument, 2);
    delay_ms(time/4);
    stopNote(0, 1);
    stopNote(2, 1);
    playNote(0, "D", 3, base_instrument, 2);
    playNote(2, "A", 3, main_instrument, 2);
    delay_ms(time/4);
    stopNote(0, 1);
    stopNote(2, 1);
    playNote(0, "F#", 3, base_instrument, 2);
    playNote(2, "D", 4, main_instrument, 2);
    delay_ms(time/4);
    stopNote(0, 1);
    stopNote(2, 1);
    playNote(0, "A", 3, base_instrument, 2);
    playNote(2, "F#", 4, main_instrument, 2);
    delay_ms(time/4);
    stopNote(0, 1);
    stopNote(1, 1);
    stopNote(2, 1);
    stopNote(3, 1);
    
    playNote(1, "B", 4, main_instrument, 0);
    playNote(3, "G", 2, base_instrument, 0);
    playNote(2, "G", 4, main_instrument, 2);
    delay_ms(time/4);
    stopNote(0, 1);
    stopNote(2, 1);
    playNote(0, "D", 3, base_instrument, 2);
    playNote(2, "B", 3, main_instrument, 2);
    delay_ms(time/4);
    stopNote(0, 1);
    stopNote(2, 1);
    playNote(0, "G", 3, base_instrument, 2);
    playNote(2, "D", 4, main_instrument, 2);
    delay_ms(time/4);
    stopNote(0, 1);
    stopNote(2, 1);
    playNote(0, "B", 3, base_instrument, 2);
    playNote(2, "G", 4, main_instrument, 2);
    delay_ms(time/4);
    stopNote(0, 1);
    stopNote(1, 1);
    stopNote(2, 1);
    stopNote(3, 1);
    
    playNote(1, "C#", 5, main_instrument, 0);
    playNote(3, "A", 2, base_instrument, 0);
    playNote(2, "A", 4, main_instrument, 2);
    delay_ms(time/4);
    stopNote(0, 1);
    stopNote(2, 1);
    playNote(0, "E", 3, base_instrument, 2);
    playNote(2, "C#", 4, main_instrument, 2);
    delay_ms(time/4);
    stopNote(0, 1);
    stopNote(2, 1);
    playNote(0, "A", 3, base_instrument, 2);
    playNote(2, "E", 4, main_instrument, 2);
    delay_ms(time/4);
    stopNote(0, 1);
    stopNote(2, 1);
    playNote(0, "C#", 4, base_instrument, 2);
    playNote(2, "A", 4, main_instrument, 2);
    delay_ms(time/4);
    stopNote(0, 1);
    stopNote(1, 1);
    stopNote(2, 1);
    stopNote(3, 1);
    
    // ==========================THIRD PART ===================================
    playNote(1, "D", 5, main_instrument, 1);
    playNote(2, "D", 3, base_instrument, 0);
    delay_ms(time/4);
    stopNote(0, 1);
    stopNote(1, 1);
    playNote(1, "C#", 5, main_instrument, 1);
    playNote(0, "A", 3, base_instrument, 2);
    delay_ms(time/4);
    stopNote(0, 1);
    stopNote(1, 1);
    playNote(1, "D", 5, main_instrument, 1);
    playNote(0, "F#", 4, base_instrument, 2);
    delay_ms(time/4);
    stopNote(0, 1);
    stopNote(1, 1);
    playNote(1, "D", 4, main_instrument, 1);
    playNote(0, "D", 4, base_instrument, 2);
    delay_ms(time/4);
    stopNote(0, 1);
    stopNote(1, 1);
    stopNote(2, 1);
    
    playNote(1, "C#", 4, main_instrument, 1);
    playNote(2, "A", 2, base_instrument, 0);
    delay_ms(time/4);
    stopNote(0, 1);
    stopNote(1, 1);
    playNote(1, "A", 4, main_instrument, 1);
    playNote(0, "E", 3, base_instrument, 2);
    delay_ms(time/4);
    stopNote(0, 1);
    stopNote(1, 1);
    playNote(1, "E", 4, main_instrument, 1);
    playNote(0, "C#", 3, base_instrument, 2);
    delay_ms(time/4);
    stopNote(0, 1);
    stopNote(1, 1);
    playNote(1, "F#", 4, main_instrument, 1);
    playNote(0, "E", 4, base_instrument, 2);
    delay_ms(time/4);
    stopNote(0, 1);
    stopNote(1, 1);
    stopNote(2, 1);
    
    playNote(1, "D", 4, main_instrument, 1);
    playNote(2, "B", 2, base_instrument, 0);
    delay_ms(time/4);
    stopNote(0, 1);
    stopNote(1, 1);
    playNote(1, "D", 5, main_instrument, 1);
    playNote(0, "F#", 3, base_instrument, 2);
    delay_ms(time/4);
    stopNote(0, 1);
    stopNote(1, 1);
    playNote(1, "C#", 5, main_instrument, 1);
    playNote(0, "D", 4, base_instrument, 2);
    delay_ms(time/4);
    stopNote(0, 1);
    stopNote(1, 1);
    playNote(1, "B", 4, main_instrument, 1);
    playNote(0, "F#", 3, base_instrument, 2);
    delay_ms(time/4);
    stopNote(0, 1);
    stopNote(1, 1);
    stopNote(2, 1);
    
    playNote(1, "C#", 5, main_instrument, 1);
    playNote(2, "A", 2, base_instrument, 0);
    delay_ms(time/4);
    stopNote(0, 1);
    stopNote(1, 1);
    playNote(1, "F#", 5, main_instrument, 1);
    playNote(0, "F#", 3, base_instrument, 2);
    delay_ms(time/4);
    stopNote(0, 1);
    stopNote(1, 1);
    playNote(1, "A", 5, main_instrument, 1);
    playNote(0, "C#", 4, base_instrument, 2);
    delay_ms(time/4);
    stopNote(0, 1);
    stopNote(1, 1);
    playNote(1, "B", 5, main_instrument, 1);
    playNote(0, "F#", 3, base_instrument, 2);
    delay_ms(time/4);
    stopNote(0, 1);
    stopNote(1, 1);
    stopNote(2, 1);
    
    playNote(1, "G", 5, main_instrument, 1);
    playNote(3, "B", 4, main_instrument, 1);
    playNote(2, "G", 2, base_instrument, 0);
    delay_ms(time/4);
    stopNote(0, 1);
    stopNote(1, 1);
    playNote(1, "F#", 5, main_instrument, 1);
    playNote(0, "D", 3, base_instrument, 2);
    delay_ms(time/4);
    stopNote(0, 1);
    stopNote(1, 1);
    playNote(1, "E", 5, main_instrument, 1);
    playNote(0, "B", 3, base_instrument, 2);
    delay_ms(time/4);
    stopNote(0, 1);
    stopNote(1, 1);
    playNote(1, "G", 5, main_instrument, 1);
    playNote(0, "D", 3, base_instrument, 2);
    delay_ms(time/4);
    stopNote(0, 1);
    stopNote(1, 1);
    stopNote(2, 1);
    stopNote(3, 1);
    
    playNote(1, "F#", 5, main_instrument, 1);
    playNote(3, "A", 4, main_instrument, 1);
    playNote(2, "D", 2, base_instrument, 0);
    delay_ms(time/4);
    stopNote(0, 1);
    stopNote(1, 1);
    playNote(1, "E", 5, main_instrument, 1);
    playNote(0, "A", 2, base_instrument, 2);
    delay_ms(time/4);
    stopNote(0, 1);
    stopNote(1, 1);
    playNote(1, "D", 5, main_instrument, 1);
    playNote(0, "F#", 3, base_instrument, 2);
    delay_ms(time/4);
    stopNote(0, 1);
    stopNote(1, 1);
    playNote(1, "C#", 5, main_instrument, 1);
    playNote(0, "A", 2, base_instrument, 2);
    delay_ms(time/4);
    stopNote(0, 1);
    stopNote(1, 1);
    stopNote(2, 1);
    stopNote(3, 1);
    
    playNote(1, "B", 4, main_instrument, 1);
    playNote(3, "D", 4, main_instrument, 1);
    playNote(2, "G", 2, base_instrument, 0);
    delay_ms(time/4);
    stopNote(0, 1);
    stopNote(1, 1);
    playNote(1, "A", 4, main_instrument, 1);
    playNote(0, "D", 3, base_instrument, 2);
    delay_ms(time/4);
    stopNote(0, 1);
    stopNote(1, 1);
    playNote(1, "G", 4, main_instrument, 1);
    playNote(0, "B", 3, base_instrument, 2);
    delay_ms(time/4);
    stopNote(0, 1);
    stopNote(1, 1);
    playNote(1, "F#", 4, main_instrument, 1);
    playNote(0, "D", 3, base_instrument, 2);
    delay_ms(time/4);
    stopNote(0, 1);
    stopNote(1, 1);
    stopNote(2, 1);
    stopNote(3, 1);
    
//    playNote(1, "D", 4, main_instrument, 1);
//    playNote(3, "E", 4, main_instrument, 1);
//    playNote(2, "A", 3, base_instrument, 0);
//    delay_ms(time/8);
//    stopNote(1, 1);
//    playNote(1, "C#", 4, main_instrument, 1);
//    delay_ms(time/8);
//    stopNote(1, 1);
//    playNote(1, "B", 3, main_instrument, 1);
//    delay_ms(time/4);
//    stopNote(1, 1);
//    stopNote(2, 1);
//    playNote(1, "C#", 4, main_instrument, 1);
//    playNote(2, "A", 2, base_instrument, 2);
//    delay_ms(time/4);
//    stopNote(1, 1);
//    playNote(1, "A", 3, main_instrument, 1);
//    delay_ms(time/4);
//    stopNote(1, 1);
//    stopNote(2, 1);
//    stopNote(3, 1);
    
}