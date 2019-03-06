#include <xc.h>

void TIMER1_Initialize() {
    RCONbits.IPEN = 0x01;
    T1CON = 0b10110101;
//    PIE1bits.TMR1IE = 1; // enable timer1
    IPR1bits.TMR1IP = 1;
    PIR1bits.TMR1IF = 0;
    INTCONbits.GIE = 1;
    TMR1 = 65535 - 6250;
}

