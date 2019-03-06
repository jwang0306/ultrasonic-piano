// CONFIG
#pragma config OSC  = INTIO67,WDT=OFF,LVP=OFF
//#pragma config OSC = HS        // Oscillator Selection bits (HS oscillator)
#pragma config WDT = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRT = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config BOREN = ON       // Brown-out Reset Enable bit (BOR enabled)
#pragma config LVP = OFF         // Low-Voltage (Single-Supply) In-Circuit Serial Programming Enable bit (RB3/PGM pin has PGM function; low-voltage programming enabled)
#pragma config CPD = OFF        // Data EEPROM Memory Code Protection bit (Data EEPROM code protection off)
// CONFIG5L
#pragma config CP0 = OFF        // Code Protection bit (Block 0 (000800-001FFFh) not code-protected)
#pragma config CP1 = OFF        // Code Protection bit (Block 1 (002000-003FFFh) not code-protected)
#pragma config CP2 = OFF        // Code Protection bit (Block 2 (004000-005FFFh) not code-protected)
#pragma config CP3 = OFF        // Code Protection bit (Block 3 (006000-007FFFh) not code-protected)

// CONFIG5H
#pragma config CPB = OFF        // Boot Block Code Protection bit (Boot block (000000-0007FFh) not code-protected)
#pragma config CPD = OFF        // Data EEPROM Code Protection bit (Data EEPROM not code-protected)

// CONFIG6L
#pragma config WRT0 = OFF       // Write Protection bit (Block 0 (000800-001FFFh) not write-protected)
#pragma config WRT1 = OFF       // Write Protection bit (Block 1 (002000-003FFFh) not write-protected)
#pragma config WRT2 = OFF       // Write Protection bit (Block 2 (004000-005FFFh) not write-protected)
#pragma config WRT3 = OFF       // Write Protection bit (Block 3 (006000-007FFFh) not write-protected)

// CONFIG6H
#pragma config WRTC = OFF       // Configuration Register Write Protection bit (Configuration registers (300000-3000FFh) not write-protected)
#pragma config WRTB = OFF       // Boot Block Write Protection bit (Boot block (000000-0007FFh) not write-protected)
#pragma config WRTD = OFF       // Data EEPROM Write Protection bit (Data EEPROM not write-protected)

#pragma config PBADEN = OFF

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#include <xc.h>
#define _XTAL_FREQ 20000000
//***Define the signal pins of all four displays***//
#define s1 RA7
#define s2 RA6
#define s3 RC0
#define s4 RC1

//***End of definition**////

void showPace(int i) {
    int a, b, c, d, e, f, g, h; //just variables
    int seg[] ={
        0b11000000, //Hex value to display the number 0
        0b11111001, //Hex value to display the number 1
        0b10100100, //Hex value to display the number 2
        0b10110000, //Hex value to display the number 3
        0b10011001, //Hex value to display the number 4
        0b10010010, //Hex value to display the number 5
        0b10000010, //Hex value to display the number 6
        0b11011000, //Hex value to display the number 7
        0b10000000, //Hex value to display the number 8
        0b10010000 //Hex value to display the number 9
    }; //End of Array for displaying numbers from 0 to 
    //***Splitting "i" into four digits***//  
    a = i % 10; //4th digit is saved here
    b = i / 10;
    c = b % 10; //3rd digit is saved here
    d = b / 10;
    e = d % 10; //2nd digit is saved here
    f = d / 10;
    g = f % 10; //1st digit is saved here
    h = f / 10;
    //***End of splitting***//
    //LATD = seg[0];//Turn ON display 1 and print 4th digit
    LATD = seg[g];
    s1 = 1; //Turn ON display 1 and print 4th digit
    __delay_ms(0.2);
    s1 = 0; //Turn OFF display 1 after 5ms delay
    LATD = seg[e];
    s2 = 1; //Turn ON display 2 and print 3rd digit
    __delay_ms(0.2);
    s2 = 0; //Turn OFF display 2 after 5ms delay
    LATD = seg[c];
    s3 = 1; //Turn ON display 3 and print 2nd digit
    __delay_ms(0.2);
    s3 = 0; //Turn OFF display 3 after 5ms delay
    LATD = seg[a];
    s4 = 1; //Turn ON display 4 and print 1st digit
    __delay_ms(0.2);
    s4 = 0; //Turn OFF display 4 after 5ms delay
    //        if(flag>=1){
    //            i++;flag=0; //only if flag is hundred "i" will be incremented 
    //        }
    //        flag++; //increment flag for each flash
}

void initialize_7() {
    //*****I/O Configuration****//
    TRISAbits.RA7 = 0;
    TRISAbits.RA6 = 0;

    TRISCbits.RC0 = 0;
    TRISCbits.RC1 = 0;
    PORTCbits.RC0 = 0;
    PORTCbits.RC1 = 0;
    TRISD = 0x00;
    LATD = 0x00;
}


int displayNum = 0;
void main() {

    OSCCONbits.IRCF2 = 1;
    OSCCONbits.IRCF1 = 1;
    OSCCONbits.IRCF0 = 0;

    initialize_7();
    PIE2bits.TMR3IE = 1;
    IPR2bits.TMR3IP = 1;
    PIR2bits.TMR3IF = 0;
    
    INTCONbits.GIE = 1;
    RCONbits.IPEN = 1;
    
    INTCON2bits.INTEDG1 = 0;
    INTCON3bits.INT1IE = 1;
    INTCON3bits.INT1IF = 0;
    INTCON3bits.INT1IP = 1;

    T3CONbits.RD16 = 1;
    T3CONbits.T3CKPS = 3;//prescaler 1: 8
    T3CONbits.TMR3ON = 1;
    TMR3 = 0;
    
    RCONbits.IPEN = 1;
    INTCONbits.GIE = 1;
    
    while (1) {
        showPace(displayNum);
    }

}

int count = 0;
int startFlag = 0;

void __interrupt(high_priority) Hi_ISR(void) {
    if (INT1IE && INT1IF) {
        if (startFlag == 0) {
            count = 0;
            displayNum = 0;
            startFlag = 1;
        }
        else if (startFlag == 1) {
            count = 0;
            displayNum = 0;
            startFlag = 0;
        }
        INT1IF = 0;
    }
    
    if (TMR3IE && TMR3IF) {
        count++;
        PIR2bits.TMR3IF = 0;
    }
    
    if (startFlag == 1 && count % 2 == 0) {
        displayNum++;
    }

}