#include <hidef.h>      /* common defines and macros */
#include "derivative.h"      /* derivative-specific definitions */
#include <stdio.h>

#define LCD_DATA PORTK
#define LCD_CTRL PORTK
#define RS 0x01
#define EN 0x02

void COMWRT4(unsigned char);
void DATWRT4(unsigned char);
void MSDelay(unsigned int);

// GLOBAL Variables
volatile int NEXT_MOVE = 0;

void main(void) {

  //set up LCD
  DDRK = 0xFF;
  COMWRT4(0x33); //reset sequence provided by data sheet
  COMWRT4(0x32); //reset sequence provided by data sheet
  COMWRT4(0x28);
  COMWRT4(0x06);
  COMWRT4(0x0E);
  COMWRT4(0x01);
  COMWRT4(0x80);

  // set up RTI
  CRGINT = 0b10000000;
  RTICTL = 0b01111011;
  
  //set up main timer 
  TSCR1 = 0x80;
  
  //set up ATD
  ATD0CTL2 = 0x80;
  ATD0CTL3 = 0x08;
  ATD0CTL4 = 0x84;

  //
  DDRP = 0xFF;
  DDRB = 0xFF;
  
	EnableInterrupts;

   for(;;)
   {
    ATD0CTL5 = 0x02;
    while (!(ATD0STAT0 & 0x80));
     /* loop forever */
   }
  /* please make sure that you never leave main */
}

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
 
  void move_forward(){
  // pass in proximity value
  // create while loop that checks for proximity
  // move forward until closest proximity reached
  unsigned char output[4] = {0xCC,0x99,0x33,0x66};
  PTP = 0x0F; // enable all INPUT lines
  PORTB = output[NEXT_MOVE];
    
  NEXT_MOVE++;
    
 }


#pragma TRAP_PROC
#pragma CODE_SEG __SHORT_SEG NON_BANKED
interrupt VectorNumber_Vtimmdcu void mdcuInterrupt () {
  static int mdcu_count = 0;
  static int on = 0;
  
  // ALWAYS CLEAR THE FLAG FIRST!
  MCFLG |= 0x80;
}

interrupt VectorNumber_Vtimovf void ovfInterrupt () {
  static int ovf_count = 0;
  static int on = 0;
  
  // ALWAYS CLEAR THE FLAG FIRST!
  TFLG2 |= 0x80;
}

interrupt VectorNumber_Vrti void rtiInterrupt () {

  static int i;
  static char digits[30];
  static int numdigits = 0;
  static int starttime, endtime;
  static int counts = 0;
  static int time = 0;
  static long x;
  static int y; 
  static long ylng;
  static int d[3] = {32};
  static long dlng;
  static long a[2] =  {-25575, 10508};
  static long b[3] =  {329, 658, 329}; 

  
  // ALWAYS CLEAR THE FLAG FIRST!
  CRGFLG |= 0x80;
 
  
  //Clear display
  COMWRT4(0x01);
  MSDelay(1);
  
  //IIR
  starttime = TCNT;
  x = ATD0DR0H;
  x<<= 14;
    
  dlng = x - a[0] * d[1] - a[1] * d[2];
  dlng <<= 2;
  dlng >>= 16;
  d[0] = dlng;
  ylng = b[0] * d[0] + b[1] * d[1] + b[2] * d[2];
  ylng <<=2;
  ylng >>= 16;
  y = ylng;
  d[2] = d[1];
  d[1] = d[0];
   
  //calculate computation time
  endtime =  TCNT;
  
  if(TFLG2 == 0x80) {
    TFLG2 = 0x80;
    counts = 65536 + endtime - starttime;
  } else{
    counts = endtime - starttime;
  }
  time = counts /24;
  
  //send y to LCD 
  numdigits = sprintf(digits, "%d", y);
  
  for(i = 0; i < numdigits; i++)
    DATWRT4(digits[i]); 
 
  //send time to LCD
  numdigits = sprintf(digits, "%d", time);
  DATWRT4(' ');
  for(i = 0; i < numdigits; i++)
    DATWRT4(digits[i]); 
    
  //write units to LCD
  DATWRT4(0b11100100);
  DATWRT4('s');
 
  TFLG2 = 0x80;
  
  
  if (NEXT_MOVE > 3) {
    NEXT_MOVE = NEXT_MOVE%4;  
  }
  
  //Move cursor to second line
  COMWRT4(0xC0);
  numdigits = sprintf(digits, "%d", NEXT_MOVE);
  DATWRT4(' ');
  for(i = 0; i < numdigits; i++)
    DATWRT4(digits[i]); 
 
  move_forward();
  

}