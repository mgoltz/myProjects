//Gregory Alway
//Michael Goltz
//Project 4 Part 3

#include <hidef.h>      /* common defines and macros */
#include "derivative.h"      /* derivative-specific definitions */
#include <stdio.h>

#define LCD_DATA PORTK
#define LCD_CTRL PORTK
#define RS 0x01
#define EN 0x02

// GLOBAL Variables
volatile int NEXT_MOVE = 0;
volatile int ON = 0; 

void main(void) {
  SYNR = 2;
  REFDV = 0;
  while(!(CRGFLG & 0x08)){};
  CLKSEL |= 0x80;

  
  //set up ATD
  ATD0CTL2 = 0x80; // power on ATD
  ATD0CTL3 = 0x10; // two conversions = 0x10 three conversions = 0x18
  ATD0CTL4 = 0x85; // was 0x84
  
  DDRB = 0xFF;      //PORTB0-PORTB3  as output 
  DDRP = 0x0F;      //PORTP0 and PORTP1 as output for 12EN=1 and 34EN=1
  
  PORTB=0b00000000; // start with all off
  PTP=0b00001111;   //Turn on both 12EN and 34EN Enables for 754410 chip  
 
 
  // set up MDCU for 0.1ms = 0.0001 seconds interrupt
  MCCTL = 0xCF; // interrupt enable, load latest mod reg, mcen, pre=16
  MCCNT = 150;  // (0.0001-sec)*(24000000-CLKperSEC)/(16-CLKperCOUNT) = COUNT
  MCCTL = 0xCF;
  

	EnableInterrupts;

   for(;;)
   {
    ATD0CTL5 = 0x34;
    while (!(ATD0STAT0 & 0x80));
     /* loop forever */
   }
  /* please make sure that you never leave main */
}



#pragma TRAP_PROC
#pragma CODE_SEG __SHORT_SEG NON_BANKED
interrupt VectorNumber_Vtimmdcu void mdcuInterrupt () {
  static int mdcu_count = 0;
  //bot 3
  static unsigned char steparray_left[] ={0xC0, 0x80, 0x90, 0x10, 0x30, 0x20, 0x60, 0x40}; //really right wheel
  static unsigned char steparray_right[] ={0x03, 0x01, 0x09, 0x08, 0x0C, 0x04, 0x06, 0x02};//really left wheel
  
  static int stepindex_left = 0;
  static int stepindex_right = 0;
  static int interruptcount_filter = 0;
  static int interruptcount_left = 0;
  static int interruptcount_right = 0;
  static int speed_left = 20;
  static int speed_right = 20;
  static int ramp_count = 0;
  static int stop = 0;
  
  static long x, rx;
  static int y, ry; 
  static long ylng, rylng;
  static int d[3] = {32};
  static int rd[3] ={32};
  static long dlng, rdlng;
  static long a[2] =  {-25575, 10508};
  static long b[3] =  {329, 658, 329};
  static int direction = 0;

  
  // ALWAYS CLEAR THE FLAG FIRST!
  MCFLG |= 0x80;
 
  
  interruptcount_filter++;
  interruptcount_left++;
  interruptcount_right++;
  
    // fixed point math butterworth filter
    // read sensors every millisecond = 1ms = 0.001sec
  if(interruptcount_filter == 10){
    
    interruptcount_filter = 0;
    
    //IIR
      // front sensor
    x = ATD0DR1H;
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
    
      // right sensor
    rx = ATD0DR0H;
    rx<<= 14;
      
    rdlng = rx - a[0] * rd[1] - a[1] * rd[2];
    rdlng <<= 2;
    rdlng >>= 16;
    rd[0] = rdlng;
    rylng = b[0] * rd[0] + b[1] * rd[1] + b[2] * rd[2];
    rylng <<=2;
    rylng >>= 16;
    ry = rylng;
    rd[2] = rd[1];
    rd[1] = rd[0];
  }
  
    // front sensor distance from wall to close slow down left wheel
    // straightens out robot when searching for wall
 if(y > 65)
    stop = 1;
  else if (y > 60) {
    speed_left = 32; // slowest speed
    stop = 0;
  }
  else if (y > 55) {
    speed_left = 29;
    stop = 0;
  }
  else if (y > 50) {
    speed_left = 26;
    stop = 0;
  }
  else if (y > 45) {
    speed_left = 23;
    stop = 0;
  }
  else {
    speed_left= 20; // normal speed
    stop = 0; 
  }
  
  
    // right sensor distance to close to wall speed up right wheel
    // keeps robot in straight line 
  if (ry > 46) 
    speed_right = speed_left - 5;
  else if(ry < 46) // right sensor distance to far from wall slow down right wheel
    speed_right = speed_left + 2;
  else
    speed_right = speed_left; 
  
    // not stopped 
  if(stop == 0){
      // 1.5 millisecond or greater when speed_left >= 15
      // 15 * 0.1second
      // half step every (speed_left * 0.1ms)
      // left speed time
      if (interruptcount_left >= speed_left){
          PORTB =  (PORTB & ~0xF0) |  (steparray_left[stepindex_left] & 0xF0);
          stepindex_left++;
          
          // reset step index
          if (stepindex_left > 8)
              stepindex_left = 0;
          
          // reset left wheel interrupt count
          interruptcount_left = 0;
      }
    
      // right speed time
      if (interruptcount_right >= speed_right){
          PORTB =  (PORTB & ~0x0F) |  (steparray_right[stepindex_right] & 0x0F);
          stepindex_right++;

          // reset step index
          if (stepindex_right > 8)
              stepindex_right = 0;
          
          // reset right wheel interrupt count
          interruptcount_right = 0;
      }
  }
}


interrupt VectorNumber_Vtimovf void ovfInterrupt () {
  static int ovf_count = 0;
  static int on = 0;
  
  // ALWAYS CLEAR THE FLAG FIRST!
  TFLG2 |= 0x80;
}

interrupt VectorNumber_Vrti void rtiInterrupt () {
  // ALWAYS CLEAR THE FLAG FIRST!
  CRGFLG |= 0x80;
}                