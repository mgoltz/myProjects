#include <hidef.h>      /* common defines and macros */
#include "derivative.h"      /* derivative-specific definitions */

#define LCD_DATA PORTK
#define LCD_CTRL PORTK
#define RS 0x01
#define EN 0x02

// GLOBAL Variables
volatile int NEXT_MOVE = 0;
volatile int ON = 0;

void COMWRT4(unsigned char);
void DATWRT4(unsigned char);
void MSDelay(unsigned int);
void main(void) 
{
  /* put your own code here */
  
  DDRB = 0xFF;      //PORTB0-PORTB3  as output 
  DDRP = 0xFF;      //PORTP0 and PORTP1 as output for 12EN=1 and 34EN=1                                  ````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````
  
  PORTB=0b00000000; // start with all off
  PTP=0b00001111;   //Turn on both 12EN and 34EN Enables for 754410 chip  
 
 
  // set up MDCU
  //MCCTL = 0xCF; // interrupt enable, load latest mod reg, mcen, pre=16
  //MCCNT = 15000;
  //MCCTL = 0xCF;
  
  // set up RTI
  CRGINT = 0b10000000;  // RTI enable
  RTICTL = 0b01000100;  // 5*2^13 = 40,960 => 40,960/8M = 0.00512sec/int ~ 195int/sec
  
    //set up main timer 
  TSCR1 = 0x80;
  
  DDRH = 0xFF;
  
  //set up LCD
  DDRK = 0xFF;
  COMWRT4(0x33); //reset sequence provided by data sheet
  COMWRT4(0x32); //reset sequence provided by data sheet
  COMWRT4(0x28);
  COMWRT4(0x06);
  COMWRT4(0x0E);
  COMWRT4(0x01);
  COMWRT4(0x80);
  
  EnableInterrupts;
 
for(;;) 
   {
  //PORTB=0b00000011;   
  //MSDelay(50);  
  //PORTB=0b00001001;    
  //MSDelay(50);  
  
  }
 
}

#pragma TRAP_PROC
#pragma CODE_SEG __SHORT_SEG NON_BANKED
interrupt VectorNumber_Vtimmdcu void mdcuInterrupt () {
  //static int mdcu_count = 0;
  //static int on = 0;
  static int numdigits = 0;
  static char digits[30];
  static int i;
  
  // ALWAYS CLEAR THE FLAG FIRST!
  MCFLG |= 0x80;
  
  //mdcu_count++;
  //if (mdcu_count >= 100) {
    //mdcu_count = 0;
    if (ON==1) {
      ON = 0;
      PORTB=0b00001100;
      PTH = 0x4F;
      //DATWRT4('0'); 
    } else {
      ON = 1;
      PORTB=0b00001001;
      PTH = 0x79;
      //DATWRT4('1'); 
    } 
    
    
      //send y to LCD 
 // numdigits = sprintf(digits, "%d", NEXT_MOVE);
  
  //for(i = 0; i < numdigits; i++)
 //   DATWRT4('u'); 
    
 //   NEXT_MOVE++;
  
}

 interrupt VectorNumber_Vrti void rtiInterrupt () {


  static int numdigits = 0;
  static char digits[30];
  static int i;
  static unsigned char RstepArray[] = {0x};

  // ALWAYS CLEAR THE FLAG FIRST!
  CRGFLG |= 0x80;

    if (ON==1) {
      ON = 0;
      PORTB=0b00000011;
      PTH = 0x4F;
      //DATWRT4('0'); 
    } else {
      ON = 1;
      PORTB=0b00000110;
      PTH = 0x79;
      //DATWRT4('1'); 
    }
  

}


//millisecond delay for XTAL=8MHz, PLL=48MHz
//The HCS12 Serial Monitor is used to download and  the program.
//Serial Monitor uses PLL=48MHz

 /*void MSDelay(unsigned int itime)
  {
    unsigned int i; unsigned int j;
    for(i=0;i<itime;i++)
      for(j=0;j<4000;j++);    //1 msec. tested using Scope
  }
   */
   
   void MSDelay(unsigned int itime)
{
  unsigned int i; unsigned int j;
  for(i=0;i<itime;i++)
  for(j=0;j<350;j++);
}
void COMWRT4(unsigned char command)
{
  unsigned char x;
        
  x = (command & 0xF0) >> 2;         //shift high nibble to center of byte for Pk5-Pk2
  LCD_DATA =LCD_DATA & ~0x3C;        //clear bits Pk5-Pk2
  LCD_DATA = LCD_DATA | x;           //sends high nibble to PORTK
  MSDelay(1);
  LCD_CTRL = LCD_CTRL & ~RS;         //set RS to command (RS=0)
  MSDelay(1);
  LCD_CTRL = LCD_CTRL | EN;          //rais enable
  MSDelay(5);
  LCD_CTRL = LCD_CTRL & ~EN;         //Drop enable to capture command
  MSDelay(15);                       //wait
  x = (command & 0x0F)<< 2;          // shift low nibble to center of byte for Pk5-Pk2
  LCD_DATA =LCD_DATA & ~0x3C;        //clear bits Pk5-Pk2
  LCD_DATA =LCD_DATA | x;            //send low nibble to PORTK
  LCD_CTRL = LCD_CTRL | EN;          //rais enable
  MSDelay(5);
  LCD_CTRL = LCD_CTRL & ~EN;         //drop enable to capture command
  MSDelay(15);
}

void DATWRT4(unsigned char data)
{
  unsigned char x;

  x = (data & 0xF0) >> 2;
  LCD_DATA =LCD_DATA & ~0x3C;                     
  LCD_DATA = LCD_DATA | x;
  MSDelay(1);
  LCD_CTRL = LCD_CTRL | RS;
  MSDelay(1);
  LCD_CTRL = LCD_CTRL | EN;
  MSDelay(1);
  LCD_CTRL = LCD_CTRL & ~EN;
  MSDelay(5);
       
  x = (data & 0x0F)<< 2;
  LCD_DATA =LCD_DATA & ~0x3C;                     
  LCD_DATA = LCD_DATA | x;
  LCD_CTRL = LCD_CTRL | EN;
  MSDelay(1);
  LCD_CTRL = LCD_CTRL & ~EN;
  MSDelay(15);
 }
 