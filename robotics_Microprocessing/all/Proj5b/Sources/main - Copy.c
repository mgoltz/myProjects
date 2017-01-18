/*
Michael Goltz
Matthew Ro
CDA 4621
Project 5 Part B
*/
#include <hidef.h>      /* common defines and macros */
#include "derivative.h"      /* derivative-specific definitions */

void InitializeATD(void);
void HALF_STEP_FORWARD(void);
void STATIONARY_180_TURN(void);
void RAMP_UP(void);
void RAMP_DOWN(void);

volatile float Output_right[2] = { 0 };  // last two distance readings
volatile float Output_left[2] = { 0 };  // last two distance readings
volatile unsigned char OutputIndex = 0;	//points to the most recent distance Output location

// motor wheel half steps
volatile unsigned char LeftStepArray[] ={0xC0, 0x80, 0x90, 0x10, 0x30, 0x20, 0x60, 0x40}; //really right wheel
volatile unsigned char LeftStepArray_Rev[] ={0x30, 0x10, 0x90, 0x80, 0xC0, 0x40, 0x60, 0x20}; //really right wheel
volatile unsigned char RightStepArray[] ={0x03, 0x01, 0x09, 0x08, 0x0C, 0x04, 0x06, 0x02};//really left wheel

// motor timing global variable
volatile long a[2] =  {-25575, 10508};
volatile long b[3] =  {329, 658, 329};
volatile int interruptcount_filter = 0;
volatile int interruptcount_left = 0;
volatile int interruptcount_right = 0;
volatile unsigned char stepindex_left = 7;
volatile unsigned char stepindex_right = 0;
volatile int speed_left = 11;
volatile int speed_right = 11;
volatile int stop = 1;
volatile int state = 0;
volatile int speed = 11;
volatile int y;
volatile int ramp_count = 1;

// stationary 180 turn
volatile int stepsRight = 0;
volatile int stepsLeft = 0;
volatile int i = 0;

volatile int timer = 0;

// start of main
void main(void)
{
	SYNR = 0x02;
	REFDV = 0x00;		//this code should change the clock to 24MHz
	while (!(CRGFLG & 0x08))
	{ }
	CLKSEL |= 0x80;
	
	InitializeATD();
	
	// set up 7-segment display  
  DDRH = 0xFF;
  // set up motor ports
  DDRB = 0xFF;      //PORTB0-PORTB3  as output 
  DDRP = 0x0F;      //PORTP0 and PORTP1 as output for 12EN=1 and 34EN=1
  
  PORTB=0b00000000; // start with all off
  PTP=0b00001111;   //Turn on both 12EN and 34EN Enables for 754410 chip 
	
  // set up MDCU for 0.1ms = 0.0001 seconds interrupt
  MCCTL = 0xCF; // interrupt enable, load latest mod reg, mcen, pre=16
  MCCNT = 150;  // (0.0001-sec)*(24000000-CLKperSEC)/(16-CLKperCOUNT) = COUNT
  MCCTL = 0xCF;
	
	//RTICTL = 0b01101011;	//sets prescaler of 8*2^10 resulting in interrupt every 49.15ms
	//CRGINT = 0x80;			//tells that want interrupt given, not flag raised
	
	EnableInterrupts;

	for(;;) {
	
	  ATD0CTL5 = 0x33; // 
    while (!(ATD0STAT0 & 0x80));
	//	_FEED_COP(); /* feeds the dog */	
		
	}
}

//interrupt should trigger once every 0.25ms
#pragma TRAP_PROC
#pragma CODE_SEG __SHORT_SEG NON_BANKED
interrupt VectorNumber_Vtimmdcu void mdcuInterrupt () 
{

  //	assigning kd a larger value seems to cause the bot to overreact to a change in distance
	static const float kd = 30;	
	static const float kp = 0.5; // play with this number a bit 0.6 was good also
	static float error_r = 0;			//represents our distance from desired location
	static float errorDot_r = 0;		//represents our change in distance from last reading
	static float error_l = 0;			//represents our distance from desired location
	static float errorDot_l = 0;		//represents our change in distance from last reading
  
  // ATD conversion
  static long x, rx, lx;
  static int ry, ly; 
  static long ylng, rylng, lylng;
  static int d[3] = {32};
  static int rd[3] ={32};
  static int ld[3] ={32};
  static long dlng, rdlng, ldlng;

  
  // stationary turn
  // r = 2.38cm
  // d = 9.5cm
  //static int sL, sR; // 2*pi*r*ticks/200
	
	MCFLG |= 0x80;
	
	interruptcount_filter++;
  interruptcount_left++;
  interruptcount_right++;
  
    // fixed point math butterworth filter
    // read sensors every millisecond = 1ms = 0.001sec
  if(interruptcount_filter == 10){
    
    interruptcount_filter = 0;
   
    // right sensor
    x = ATD0DR2H;
		x<<= 14;
      
    dlng = x - a[0] * d[1] - a[1] * d[2];
    dlng <<= 2;
    dlng >>= 16;
    d[0] = dlng;
    ylng = b[0] * d[0] + b[1] * d[1] + b[2] * d[2];
    ylng <<=2;
    ylng >>= 16;
    
		OutputIndex++;
		if (OutputIndex == 2)
			OutputIndex = 0;
		
		Output_right[OutputIndex] = ylng;
    d[2] = d[1];
    d[1] = d[0];
    
    // front sensor
    rx = ATD0DR1H;
    rx<<= 14;
      
    rdlng = rx - a[0] * rd[1] - a[1] * rd[2];
    rdlng <<= 2;
    rdlng >>= 16;
    rd[0] = rdlng;
    rylng = b[0] * rd[0] + b[1] * rd[1] + b[2] * rd[2];
    rylng <<=2;
    rylng >>= 16;
    y = rylng;
    rd[2] = rd[1];
    rd[1] = rd[0];
    
    // left sensor
    lx = ATD0DR0H;
		lx<<= 14;
      
    ldlng = lx - a[0] * ld[1] - a[1] * ld[2];
    ldlng <<= 2;
    ldlng >>= 16;
    ld[0] = ldlng;
    lylng = b[0] * ld[0] + b[1] * ld[1] + b[2] * ld[2];
    lylng <<=2;
    lylng >>= 16;
		
		Output_left[OutputIndex] = lylng;
    ld[2] = ld[1];
    ld[1] = ld[0];
   
   //
  } // end of sensor reading
  
 if (state == 0){
//	if (!done)
	PTH = 0x40;
	RAMP_UP(); //reset done and change to state 1
	HALF_STEP_FORWARD();
 }
 else if (state == 1){
	PTH = 0x79;
	//done = 0;
	if (y > 45)
		state = 2;
	else {
	  //PD Control
	  error_r = 80 - Output_right[OutputIndex];		//represents our error (distance from where we want to be)
	  errorDot_r = Output_right[OutputIndex] - Output_right[(OutputIndex - 1 == -1? 1 : 0)];	//represents change in position over last two readings
	  speed_right = speed - (int)(kp * error_r - kd * errorDot_r);  //controls speed of left wheel
	  	
	  if (speed_right < 9)
		  speed_right = 9;	  
	  	  
	  error_l = 80 - Output_left[OutputIndex];		//represents our error (distance from where we want to be)
	  errorDot_l = Output_left[OutputIndex] - Output_left[(OutputIndex - 1 == -1? 1 : 0)];	//represents change in position over last two readings
	  speed_left = speed - (int)(kp * error_l - kd * errorDot_l);
	  
    if (speed_left < 9)
		  speed_left = 9;
	  
	    
		HALF_STEP_FORWARD();
	}
 }
 else if (state == 2){
	PTH = 0x24;
	RAMP_DOWN(); //reset done and change to state 3
	HALF_STEP_FORWARD();
 }
 else if (state == 3){
	 PTH = 0x30;
	 STATIONARY_180_TURN(); //reset done and change to state 0
 }

} // end of MDCU

//triggered every 49.15ms
interrupt VectorNumber_Vrti void rtiInterrupt()
{    
	CRGFLG |= 0x80; 
}

void HALF_STEP_FORWARD(void){
 
  // right wheel speed
   if (interruptcount_left >= speed_left){
    PORTB =  (PORTB & ~0xF0) |  (LeftStepArray[stepindex_left] & 0xF0);
    stepindex_left++;
          
    // reset step index
    if (stepindex_left > 7)
      stepindex_left = 0;
          
    // reset right wheel interrupt count
    interruptcount_left = 0;
  }
    
  // left speed time
  if (interruptcount_right >= speed_right){
    PORTB =  (PORTB & ~0x0F) |  (RightStepArray[stepindex_right] & 0x0F);
    stepindex_right++;

    // reset step index
    if (stepindex_right > 7)
      stepindex_right = 0;
          
    // reset left wheel interrupt count
    interruptcount_right = 0;
  } 
  
}

void STATIONARY_180_TURN(void){
  
   //turn robot 180 degrees
  
  if (i < 5000) {
     
    if (interruptcount_left >= 11){
      PORTB =  (PORTB & ~0xF0) |  (LeftStepArray_Rev[stepindex_left] & 0xF0);
      
      stepindex_left++;
          
       //reset step index
      if (stepindex_left > 7)
        stepindex_left = 0;
          
       //reset left wheel interrupt count
      interruptcount_left = 0;
    }
    
     //right speed time
    if (interruptcount_right >= 11){
      PORTB =  (PORTB & ~0x0F) |  (RightStepArray[stepindex_right] & 0x0F);
      stepindex_right++;

       //reset step index
      if (stepindex_right > 7)
        stepindex_right = 0;
          
      // reset right wheel interrupt count
      interruptcount_right = 0;
      
    } 
    i++;
    
  }
  else{
    i = 0;
    state = 0; // uturn done now ramp_up
  }
  
}

void RAMP_UP(void){
  
    //ramp up motor. 10000 interrupts = 1 sec
    if (ramp_count < 9000){
      ramp_count = ramp_count + 3;
      speed_left = speed_right = 1000 / (ramp_count / 100);
    } else {
      ramp_count = 9000;
      state = 1;  // back to PD controller  
    }
  
}
void RAMP_DOWN(void){
  

    if (ramp_count > 1){
      ramp_count = ramp_count - 2;
      speed_left = speed_right = 1000 / (ramp_count / 100);
    } else {
      ramp_count = 1;
      state = 3; //  arbituary state value  
    }  
  
}
void InitializeATD(void)
{
  ATD0CTL2 = 0x80; // power on ATD
  ATD0CTL3 = 0x18; // two conversions = 0x10 three conversions = 0x18
  ATD0CTL4 = 0x85; // was 0x84
}