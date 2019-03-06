#include "setting_hardaware/setting.h"
#include "setting_hardaware/ym2413.h"
#include <stdlib.h>
#include "stdio.h"
#include "string.h"
#define _XTAL_FREQ 4000000      /* define macros to use internal __delay_us function */
#define Trigger_Pulse LATC3
#define capacity 500
float Distance = 0;
float Distance2 = 0;
int Time;
int Time2;
int turn = 1;
int turnCount = 0;
volatile char dummy;
volatile int record = 0;
char rm[500];
int rm_header = 0;
char mflag = 0;
// using namespace std;
#define base 3125
#define XMAS_NOTES 49
#define BEE_NOTES 50
#define yield_rate 10


#define A RD1
#define B RD2
#define C RD3
#define D RA1
#define E RA2
#define F RC4
#define G RC0
#define d1 RC3
#define d2 RC2
#define d3 RC7
#define d4 RC0



/*
#define G4 0x4FC
#define A4 0x470
#define B4 0x3F4
#define C5 0x3BC
#define D5 0x353
 */
#define C4 238
#define D4 212
#define E4 188
#define F4 178
#define G4 158
#define A4 141
#define B4 126
#define C5 119
#define D5 105
#define E5 94
#define F5 89
#define G5 79
#define A5 70
#define B5 62
#define C6 59
int half_cycle;
int index;
int tmr_cnt = 0;
int Xmas[XMAS_NOTES] = {
    B4,B4,B4,B4,B4,B4,B4,D5,G4,A4,B4,
    C5,C5,C5,C5,C5,B4,B4,B4,B4,A4,A4,B4,A4,D5,
    B4,B4,B4,B4,B4,B4,B4,D5,G4,A4,B4,
    C5,C5,C5,C5,C5,B4,B4,B4,D5,D5,C5,A4,G4
};
int waitXmas[XMAS_NOTES] = {
    4,4,8,4,4,8,4,4,4,4,16,
    4,4,4,4,4,4,4,4,4,4,4,4,8,8,
    4,4,8,4,4,8,4,4,4,4,16,
    4,4,4,4,4,4,4,4,4,4,4,4,8
};
int bee[BEE_NOTES] = {
    A4,
    D5,B4,B4,C5,A4,A4,G4,A4,B4,C5,D5,D5,D5,
    D5,B4,B4,C5,A4,A4,G4,B4,D5,D5,B4,
    A4,A4,A4,A4,A4,B4,C5,
    B4,B4,B4,B4,B4,C5,D5,
    D5,B4,B4,C5,A4,A4,G4,B4,D5,D5,G4
};
int waitBee[BEE_NOTES] = {
    4,4,8,4,4,8,4,4,4,4,4,4,8,
    4,4,8,4,4,8,4,4,4,4,16,
    4,4,4,4,4,4,8,
    4,4,4,4,4,4,8,
    4,4,8,4,4,8,4,4,4,4,8
};

void displayWord(char w){
   
LATDbits.LATD1 = w >> 6;
LATDbits.LATD2 = w << 1 >> 1 >> 5;
LATDbits.LATD3 = w << 2 >> 2 >> 4;
LATAbits.LATA1 =  w << 3 >> 3 >> 3;
LATAbits.LATA2 = w << 4 >> 4 >> 2;
LATCbits.LATC4 = w << 5 >> 5 >> 1;
LATCbits.LATC0 = w << 6 >> 6;

}

void showPace(int i) {
    int a,b,c,d,e,f,g,h; //just variables
    char seg[]= 
    {
        0b11000000, //Hex value to display the number 0
        0b11111001, //Hex value to display the number 1
        0b10100100, //Hex value to display the number 2
        0b10110000, //Hex value to display the number 3
        0b10011001, //Hex value to display the number 4
        0b10010010, //Hex value to display the number 5
        0b10000010, //Hex value to display the number 6
        0b11011000, //Hex value to display the number 7
        0b10000000, //Hex value to display the number 8
        0b10010000  //Hex value to display the number 9
    }; //End of Array for displaying numbers from 0 to 
     //***Splitting "i" into four digits***//  
        a=i%10;//4th digit is saved here
        b=i/10;
        c=b%10;//3rd digit is saved here
        d=b/10;
        e=d%10; //2nd digit is saved here
        f=d/10;
        g=f%10; //1st digit is saved here
        h=f/10;
        //***End of splitting***//
        //LATD = seg[0];//Turn ON display 1 and print 4th digit
       displayWord(seg[g]);
        d1=1; //Turn ON display 1 and print 4th digit
        __delay_ms(0.2);
        d1=0;     //Turn OFF display 1 after 5ms delay
         displayWord(seg[e]);
        d2=1; //Turn ON display 2 and print 3rd digit
        __delay_ms(0.2);
        d2=0;     //Turn OFF display 2 after 5ms delay
        displayWord(seg[c]);
        d3=1; //Turn ON display 3 and print 2nd digit
        __delay_ms(0.2);
        d3=0;     //Turn OFF display 3 after 5ms delay
         displayWord(seg[a]);
        d4=1; //Turn ON display 4 and print 1st digit
        __delay_ms(0.2);
        d4=0;     //Turn OFF display 4 after 5ms delay
//        if(flag>=1){
//            i++;flag=0; //only if flag is hundred "i" will be incremented 
//        }
//        flag++; //increment flag for each flash
}

void Trigger_Pulse_10us()
{
    Trigger_Pulse = 1;
    __delay_us(10);
    //    for (int i = 0; i < 1000; ++i);
    Trigger_Pulse = 0;
}



void delay(int xc) {
    /*
     * 16: full note
     * 8: half note
     * 4: quarter note
     */
    T0CON = 0x84;
    TMR0 = 65535 - xc*base;
    
    INTCONbits.TMR0IF = 0;
    while (!INTCONbits.TMR0IF);
    
    //    return;
}
char str[10];
int i=0, x=1;
int cnt=0;
int cnt2=0, max_cnt2=20;
int clk_cnt = 0;
int auto_flag=0;
int value;
int past[4];
int past_cnt=0, max_past_cnt=4;
int blink_mode = 0;
int blink_cnt = 0;
int max_cnt = 0;
int numUnit = 0;
int numLed = 0;
int lightUp = 0;
int backHome = 0;
int breath_flag = 0;


void sevenSetting(){
    TRISAbits.RA0 = 0;
    TRISCbits.RC0 = 0;
    TRISCbits.RC2 = 0;
    TRISCbits.RC3 = 0;
    TRISCbits.RC4 = 0;
    TRISCbits.RC5 = 0;
    TRISCbits.RC6 = 0;
    TRISCbits.RC7 = 0;
    TRISAbits.RA1 = 0;
    TRISDbits.RD1 = 0;
    TRISDbits.RD2 = 0;
    TRISDbits.RD3 = 0;
   
}


void detect1(){
//    TRISB = 0xff;
    TRISBbits.RB0 = 1;
    
    //TRISCbits.RC3 = 0;
    INTCON2bits.RBPU=0;
    Trigger_Pulse = 0;
    T1CON = 0xA0;
    TMR1IF = 0;             /* make Timer1 Overflow Flag to '0' */
    TMR1=0;
//    while(1) {
//        playNote(1, 0, 4, 5, 0);
//        __delay_ms(500);
//    }
    int ring = 0;
    while(1) { 
//        d1 = 1;
//        A = 1;
//        B = 0;
//        C = 0;
//        D = 0;
//        E = 0;
//        F = 0;
//        G = 0;
        Trigger_Pulse_10us();               /* transmit at least 10 us pulse to HC-SR04 */
        while(PORTBbits.RB0==0);            /* wait for rising edge at Echo pin of HC-SR04 */
        TMR1=0;                             /* Load Timer1 register with 0 */
        TMR1ON=1;                           /* turn ON Timer1*/
        while(PORTBbits.RB0==1 && !TMR1IF); /* wait for falling edge at Echo pin of HC-SR04*/
        Time = TMR1;                        /* copy Time when echo is received from an object */
        TMR1ON=0;                           /* turn OFF Timer1 */
        int Distance2 = Distance;
        Distance = ((float)Time/117.00 * 2);           /* distance = (velocity x Time)/2 */
       
        if (Distance > 1 && Distance < 3){
            if (ring == 0) {
                ring = 1;
//                playNote(0, "C", 4, 3, 0);
//                playNote(1, "E", 4, 3, 0);
//                playNote(2, "G", 4, 3, 0);
//                playNote(3, "C", 5, 3, 0);
////                playNote(0, 0, 4, 3, 0);
////                playNote(1, 4, 4, 3, 0);
////                playNote(2, 7, 4, 3, 0);
////                playNote(3, 0, 5, 3, 0);
//                __delay_ms(750);
//                stopNote(0, 1);
//                stopNote(1, 1);
//                stopNote(2, 1);
//                stopNote(3, 1);
                canon(3, 3, 1);

            }

        }
        else {
           
           ring = 0;
        }
        __delay_ms(50);
        
    }
}


void main(void)
{ 
    TRISD = 0;
    LATD = 255;
    Nop();
    SYSTEM_Initialize() ;
    TRISBbits.RB1 = 1;
    TRISDbits.RD1 = 0;
    INTCON2bits.INTEDG1 = 0;
    INTCON3bits.INT1IE = 1;
    INTCON3bits.INT1IF = 0;
    TMR0 = 65535 - 1*base;
    T0CON = 0x84;
    TMR0 = 65535 - 4*base;
    INTCONbits.TMR0IF = 0;
    LATD = 1;
    sevenSetting(); // DO NOT TOUCH THIS, otherwise the sensor doesn't work
    while(1) {
        YAMAHA();
        
        //showPace(1234);
        detect1();  
              
    }

    return;
    
}





// old version:
// void interrupt high_priority Hi_ISR(void)
int music_cnt = 0;
int music_index = 0;
int music_duty = 0;
void __interrupt(high_priority) Hi_ISR(void)
{
    if(INTCON3bits.INT1IF == 1){
        record = record + 1;
        if(record > 2)
            record = 0;
        if(record == 0){
           rm_header = 0;
            T0CONbits.TMR0ON = 0;
           INTCONbits.T0IE = 0;
           TRISCbits.RC1 = 1;
          
       }
       else if(record == 1){
           rm_header = 0;
             for(i = 0;i < capacity;i++){
               rm[i] = 0;
            }
             T0CONbits.TMR0ON = 1;
          INTCONbits.T0IE = 1;
          TRISCbits.RC1 = 1;
      
            
        }
        else{
           rm_header = 0;
          int i;
        
         T0CONbits.TMR0ON = 1;
          INTCONbits.T0IE = 1;
      }
       
       INTCON3bits.INT1F = 0;
       
   }
    
     if(INTCONbits.TMR0IF == 1){
         
         
        TMR0 = 65535 - 1*base;
       INTCONbits.TMR0IF = 0;
        
        if(record == 1){
           rm_header = rm_header + 1;
            if(rm_header == capacity - 1)
           rm_header = capacity - 1;
        
        }
        
        
       if(record == 2){
           
       rm_header = rm_header + 1;
       if(rm_header == capacity - 1)
           rm_header = 0;
        
        if(rm[rm_header] == 0){
           
           TRISCbits.RC1 = 1;
        }
        else if(rm[rm_header] == 1){
          ;
//           playMe();
          
       }
       else{
            ;
//            playSo();
           
       }
     
       }
        
       
   }
    
   
    
    return ;
}