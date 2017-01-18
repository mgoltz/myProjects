//Gregory Alway
//Michael Goltz

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

  // set up RTI
  CRGINT = 0b10000000;
  RTICTL = 0b01111011;

  
  //set up ATD
  ATD0CTL2 = 0x80;
  ATD0CTL3 = 0x10;
  ATD0CTL4 = 0x84;
  
  DDRB = 0xFF;      //PORTB0-PORTB3  as output 
  DDRP = 0x0F;      //PORTP0 and PORTP1 as output for 12EN=1 and 34EN=1                                  ````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````
  
  PORTB=0b00000000; // start with all off
  PTP=0b00001111;   //Turn on both 12EN and 34EN Enables for 754410 chip  
 
 
  // set up MDCU
  MCCTL = 0xCF; // interrupt enable, load latest mod reg, mcen, pre=16
  MCCNT = 150;
  MCCTL = 0xCF;
  

	EnableInterrupts;

   for(;;)
   {
    ATD0CTL5 = 0x14;
    while (!(ATD0STAT0 & 0x80));
     /* loop forever */
   }
  /* please make sure that you never leave main */
}



#pragma TRAP_PROC
#pragma CODE_SEG __SHORT_SEG NON_BANKED
interrupt VectorNumber_Vtimmdcu void mdcuInterrupt () {
  static int mdcu_count = 0;
  static unsigned char steparray_left[] = {0x04,0x06, 0x02, 0x03,0x01, 0x09, 0x08, 0x0C};
  static unsigned char steparray_right[] ={0xC0, 0x80, 0x90, 0x10, 0x30, 0x20, 0x60, 0x40};
  static int stepindex_left = 0;
  static int stepindex_right = 0;
  static int interruptcount_filter = 0;
  static int interruptcount_left = 0;
  static int interruptcount_right = 0;
  static int speed_left = 20;
  static int speed_right = 20;
  static int ramp_count = 0;
  static int stop = 0;
  
  static long x, lx;
  static int y, ly; 
  static long ylng, lylng;
  static int d[3] = {32};
  static int ld[3] ={32};
  static long dlng, ldlng;
  static long a[2] =  {-25575, 10508};
  static long b[3] =  {329, 658, 329};
  static int direction = 0;

  
  // ALWAYS CLEAR THE FLAG FIRST!
  MCFLG |= 0x80;
 
  
  interruptcount_filter++;
  interruptcount_left++;
  interruptcount_right++;
  
  if(interruptcount_filter == 500){
    
    interruptcount_filter = 0;
    
    //IIR
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
    
    lx = ATD0DR0H;
    lx<<= 14;
      
    ldlng = lx - a[0] * ld[1] - a[1] * ld[2];
    ldlng <<= 2;
    ldlng >>= 16;
    ld[0] = ldlng;
    lylng = b[0] * ld[0] + b[1] * ld[1] + b[2] * ld[2];
    lylng <<=2;
    lylng >>= 16;
    ly = lylng;
    ld[2] = ld[1];
    ld[1] = ld[0];
  }
  
  if(y >80)
    stop = 1;
  else
    stop = 0;
  
  //ramp up motor. 10000 interrupts = 1 sec
  /*if (ramp_count < 10000){
    ramp_count++;
    speed_left = 1500 / (ramp_count / 100);
    speed_right = 1500 / (ramp_count / 100);
  }*/
  
  
  if( 30 <ly && ly < 45 ) {
    if(direction ==0)
      speed_left = 17;
    else if (direction == 1)
      speed_left = 19;
  }
  //if wall too close speed up
  else if (ly > 35){
    speed_left = 15;
    direction = 0;
  }
  //if wall too far slow down
  else if (ly < 30){
    
    speed_left = 21; 
    direction = 1;
  }
   
  if(stop == 0){
       if (interruptcount_left >= speed_left){
      PORTB =  (PORTB & ~0x0F) |  (steparray_left[stepindex_left] & 0x0F);
      stepindex_left++;
      if (stepindex_left> 8)
        stepindex_left = 0;
      interruptcount_left = 0;
    }
    
    if (interruptcount_right >= speed_right){
      PORTB =  (PORTB & ~0xF0) |  (steparray_right[stepindex_right] & 0xF0);
      stepindex_right++;
      if (stepindex_right> 8)
        stepindex_right = 0;
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