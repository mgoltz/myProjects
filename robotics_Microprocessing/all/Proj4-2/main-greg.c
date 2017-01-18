//Gregory Alway
//Michael Goltz

#include <hidef.h>      /* common defines and macros */
#include "derivative.h"      /* derivative-specific definitions */
#include <stdio.h>

// GLOBAL Variables
volatile int NEXT_MOVE = 0;
volatile int ON = 0; 

void main(void) {
  // set up RTI
  CRGINT = 0b10000000;
  RTICTL = 0b01111011;

  
  //set up ATD
  ATD0CTL2 = 0x80;
  ATD0CTL3 = 0x08;
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
    ATD0CTL5 = 0x05;
    while (!(ATD0STAT0 & 0x80));
     /* loop forever */
   }
  /* please make sure that you never leave main */
}



#pragma TRAP_PROC
#pragma CODE_SEG __SHORT_SEG NON_BANKED
interrupt VectorNumber_Vtimmdcu void mdcuInterrupt () {
  static int mdcu_count = 0;
  static int i;
  static unsigned char steparray[] = {0xC4,0x86, 0x92, 0x13,0x31, 0x29, 0x68, 0x4C}; //for reverse, step backwards through array
  static int stepindex = 0;
  static int interruptcount_filter = 0;
  static int interruptcount_motor = 0;
  static int speed = 15;
  static int stop = 0;
  static int ramp_count = 0;
  
  static char digits[30];
  static int numdigits = 0;
  static long x;
  static int y; 
  static long ylng;
  static int d[3] = {32};
  static long dlng;
  static long a[2] =  {-25575, 10508};
  static long b[3] =  {329, 658, 329};

  
  // ALWAYS CLEAR THE FLAG FIRST!
  MCFLG |= 0x80;
 
  
  interruptcount_filter++;
  interruptcount_motor++;
  
  if(interruptcount_filter == 500){
    
    interruptcount_filter = 0;
    
    //IIR
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
    
  }
  
  //ramp up motor. 10000 interrupts = 1 sec
  if (ramp_count < 10000){
    ramp_count++;
    speed = 1500 / (ramp_count / 100);
  }
   
  
  if(y >60)
    stop = 1;
  else
    stop = 0;
  
  if(stop == 0) {
    if (interruptcount_motor >= speed){
    	PORTB = steparray[stepindex];
    	stepindex++;
    	if (stepindex> 8)
    		stepindex = 0;
    	interruptcount_motor = 0;
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
