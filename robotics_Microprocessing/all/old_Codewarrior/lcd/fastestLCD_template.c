#include <stdio.h>
#include <hidef.h>      /* common defines and macros */
#include "derivative.h"      /* derivative-specific definitions */


#define LCD_DATA PORTK
#define LCD_CTRL PORTK
#define RS 0x01
#define EN 0x02

void COMWRT1(unsigned char);
void COMWRT2(unsigned char);
void DATWRT1(unsigned char);
void DATWRT2(unsigned char);

void MSDelay(unsigned int);

volatile unsigned int ovfCount = 0;

void main(void) 
{
    unsigned int start, finish;
    long unsigned int totalCounts;
    float microseconds;
    char strMicroseconds[10];
    char charMicroseconds[4] = " us";
    unsigned int i;
    
	CLKSEL = 0;
    SYNR = 2;
    REFDV = 0;
    while (!(CRGFLG & 0x08));
    CLKSEL = 0x80;

    TSCR1 = 
    TSCR2 = 
    MCCTL = 

    EnableInterrupts;
    
    DDRK = 0xFF;   
    COMWRT(0x33);   //reset sequence provided by data sheet
    COMWRT(0x32);   //reset sequence provided by data sheet
    COMWRT(0x28);   //Function set to four bit data length
                                         //2 line, 5 x 7 dot format
    COMWRT(0x06);  //entry mode set, increment, no shift
    COMWRT(0x0E);  //Display set, disp on, cursor on, blink off
    COMWRT(0x01);  //Clear display
    COMWRT(0x80);  //set start posistion, home position

    ovfCount = 0;
    start = TCNT;
    
    // write phrase here
	
    finish = TCNT;
    totalCounts = (finish - start);
    if (ovfCount) totalCounts += (ovfCount-1)*65536;
    
    microseconds = totalCounts*128/24;
    sprintf(strMicroseconds,"%6.1f",microseconds);
    i=0;
    while (charMicroseconds[i] != '\0') {
      DATWRT(charMicroseconds[i]);
      i++;
    }
    
   
    while(1) {
    }
	
}

void MSDelay(unsigned int num) 
{
  unsigned int i = 0;
  MCCNT = 
  MCCTL = 0x4C;
  for (i=0; i<num; i++) {
    while (!(MCFLG & 0x80));
    MCFLG = 0x80;  
  }
}

void COMWRT(unsigned char data)
{
        unsigned char x;
       
        x = (data & 0xF0) >> 2;
        LCD_DATA =LCD_DATA & ~0x3C;                     
        LCD_DATA = LCD_DATA | x;
        MSDelay(   );
        LCD_CTRL = LCD_CTRL | EN;
        MSDelay(   );
        LCD_CTRL = LCD_CTRL & ~EN;
        MSDelay(   );
       
        x = (data & 0x0F)<< 2;
        LCD_DATA =LCD_DATA & ~0x3C;                     
        LCD_DATA = LCD_DATA | x;
        LCD_CTRL = LCD_CTRL | EN;
        MSDelay(   );
        LCD_CTRL = LCD_CTRL & ~EN;
        MSDelay(   );
}

void DATWRT(unsigned char data)
{
        unsigned char x;
        
        x = (data & 0xF0) >> 2;
        LCD_DATA =LCD_DATA & ~0x3C;                     
        LCD_DATA = LCD_DATA | x;
        MSDelay(   );
        LCD_CTRL = LCD_CTRL | EN;
        MSDelay(   );
        LCD_CTRL = LCD_CTRL & ~EN;
        MSDelay(   );
       
        x = (data & 0x0F)<< 2;
        LCD_DATA =LCD_DATA & ~0x3C;                     
        LCD_DATA = LCD_DATA | x;
        LCD_CTRL = LCD_CTRL | EN;
        MSDelay(   );
        LCD_CTRL = LCD_CTRL & ~EN;
        MSDelay(   );
}

#pragma TRAP_PROC
#pragma CODE_SEG __SHORT_SEG NON_BANKED
interrupt VectorNumber_Vtimovf void mainISR (void){ 
  ovfCount++; 
  TFLG2 = 0x80;
}

