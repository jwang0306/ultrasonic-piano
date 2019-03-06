#include <xc.h>

/* Choose a CCPXCON to make PWM */
void PWM_Initialize() {
    
    // enable timer2, prescaler = 16
    T2CON = 0b00000111;
    
    // set PWM mode (11XX)
//    CCP2CONbits.CCP2M3 = 1;
//    CCP2CONbits.CCP2M2 = 1;
    CCP1CON = 0b00111100;//set PWM mode
    
    // enable RCX to PWM output, which is the pin you should plug in the wire
    TRISCbits.RC2 = 0;
//    PR2 = 0b11111111;
    PR2 = 49;
}

void PWM_Duty(unsigned int duty) {
    CCP1CONbits.DC1B = duty & 0b11;
    CCPR1L = duty>>2;
}