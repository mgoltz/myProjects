// Gregory Alway
// Michael Goltz
// February 13, 2014
// Control of Mobile Robots
// Project 4-1:  Control the speed of a motor using the filtered
//                output of the front proximity sensor. When the
//                obstacle is more than 20 cm, run the motor at full
//                speed. As the distance to the obstacle decreases,
//                decrease the motor speed. The speed of the motor
//                should be directly proportional to the distance from
//                the obstacle. When the obstacle is closer than 6 cm,
//                stop the motor.

#include <hidef.h>      /* common defines and macros */
#include "derivative.h"      /* derivative-specific definitions */
#include <stdio.h>

// GLOBAL Variables
volatile int NEXT_MOVE = 0;
volatile int ON = 0; 

void main(void) {

  SYNR = 2;
  REFDV = 0;
  while (!(CRGFLG&0x08)){};
  CLKSEL |= 0x80;

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
  MCCNT = 150;  // 0.1ms delay
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
  //static unsigned char steparray[] = {0x3C,0x18, 0x99, 0x81,0xC3, 0x42, 0x66, 0x24}; //for reverse, step backwards through array
  // bot 3
  static unsigned char steparray[] = {0xC3,0x81, 0x99, 0x18,0x3C, 0x24, 0x66, 0x42}; //for reverse, step backwards through array

  static int stepindex = 0;
  static int interruptcount_filter = 0;
  static int interruptcount_motor = 0;
  static int speed = 15;
  static int stop = 0;
  
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
  
  if(interruptcount_filter == 10){
    
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
  
  
  if(y > 65)
    stop = 1;
  else if (y > 60) {
    speed = 24;
    stop = 0;
  }
  else if (y > 55) {
    speed = 23;
    stop = 0;
  }
  else if (y > 50) {
    speed = 22;
    stop = 0;
  }
  else if (y > 45) {
    speed = 21;
    stop = 0;
  }
  else if (y > 40) {
    speed = 20;
    stop = 0;
  }
  else if (y > 35) {
    speed = 19;
    stop = 0;
  }
  else if (y > 30) {
    speed = 18;
    stop = 0;
  }
  else if (y > 25) {
    speed = 17;
    stop = 0;
  }
  else if (y > 20) {
    speed = 16;
    stop = 0;
  }
  else {
    speed = 15; 
    stop = 0;    
  }
  
  
  if(stop == 0) {
    if (interruptcount_motor >= speed){    // speed at 1.5/.1s
    	PORTB = steparray[stepindex];
    	stepindex++;
    	if (stepindex > 8)
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