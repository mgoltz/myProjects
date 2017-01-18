#include <hidef.h>      // common defines and macros 
#include "derivative.h"   //    derivative-specific definitions 

void InitializeATD(void);
void SetupMDCU(void);
void SetupRTI(void);
void SetupDDR_Ports(void);
void HALF_STEP_FORWARD(int steps);
void RIGHT_STEP_FORWARD(void);
void LEFT_STEP_FORWARD(void);
void RAMP_BOTH_UP(void);
void RAMP_BOTH_DOWN(void);
void STATIONARY_180_TURN(int steps);
//void TURN_ONE_WHEEL(void);

volatile int Output_right[2] = { 0 };   //last two distance readings
volatile int Output_left[2] = { 0 };   //last two distance readings
volatile unsigned char OutputIndex = 0;	//points to the most recent distance Output location

 //motor wheel half steps
volatile unsigned char RightStepArray[] ={0xC0, 0x80, 0x90, 0x10, 0x30, 0x20, 0x60, 0x40};
volatile unsigned char RightStepArray_Rev[] ={0x30, 0x10, 0x90, 0x80, 0xC0, 0x40, 0x60, 0x20};
volatile unsigned char LeftStepArray[] ={0x03, 0x01, 0x09, 0x08, 0x0C, 0x04, 0x06, 0x02};

// motor timing global variable
volatile long a[2] =  {-25575, 10508};
volatile long b[3] =  {329, 658, 329};
volatile int interruptcount_filter = 0;
volatile int interruptcount_left = 0;
volatile int interruptcount_right = 0;
volatile unsigned char stepindex_left = 0;
volatile unsigned char stepindex_right = 0;
volatile int speed_left = 20;
volatile int speed_right = 15;
volatile int top_speed = 15;
volatile int current_speed = 120;
volatile int bottom_speed = 120;
volatile int speed_counter = 0;
volatile int count_both = 0;
volatile int state = 0;

// ATD conversion
volatile long x, rx, lx;
volatile int y, ry, ly; 
volatile long ylng, rylng, lylng;
volatile int d[3] = {32};
volatile int rd[3] ={32};
volatile int ld[3] ={32};
volatile long dlng, rdlng, ldlng;

// start of main
void main(void)
{
	SYNR = 0x02;
	REFDV = 0x00;		//this code should change the clock to 24MHz
	while (!(CRGFLG & 0x08))
	{ }
	CLKSEL = 0x80;
	 
	InitializeATD();
	//set up MDCU for 0.1ms = 0.0001 seconds interrupt
	//SetupMDCU();
	     //set up MDCU for 0.1ms = 0.0001 seconds interrupt
  MCCTL = 0xCF;  //interrupt enable, load latest mod reg, mcen, pre=16
  MCCNT = 150;   //(0.0001-sec)(24000000-CLKperSEC)(16-CLKperCOUNT) = COUNT
  MCCTL = 0xCF;
	SetupDDR_Ports();
	
	EnableInterrupts;
	
	for(;;) {
	  
      ATD0CTL5 = 0x33;
	  
    while (!(ATD0STAT0 & 0x80));
	  	  
	  if (state == 0){
	    RAMP_BOTH_UP();
	  } else if (state == 1){
	    HALF_STEP_FORWARD(800); // 400 steps = 1 revolution
	  } else if (state == 2){
        RAMP_BOTH_DOWN();	  
	  } else if (state == 3){
        STATIONARY_180_TURN(404);
	  }
	    
	
		
	}
}

//interrupt triggers every 0.1ms
#pragma TRAP_PROC
#pragma CODE_SEG __SHORT_SEG NON_BANKED
interrupt VectorNumber_Vtimmdcu void mdcuInterrupt () 
{

	MCFLG |= 0x80;
	
  interruptcount_filter++;
  interruptcount_left++;
  interruptcount_right++;
  

  //   fixed point math butterworth filter
  //   read sensors every millisecond = 1ms = 0.001sec
  if(interruptcount_filter == 10 ){ //&& ((state == 0) || (state == 1) || (state == 2))
    
    interruptcount_filter = 0;
   
    // right sensor
    x = ATD0DR2H;
		x <<= 14;
      
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
    rx <<= 14;
      
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
		lx <<= 14;
      
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
   
   
  }//  end of sensor reading
  
 
	
}//  end of MDCU

void HALF_STEP_FORWARD(int steps){
 

  while (count_both <= steps){
  if (y < 65){
    if ((interruptcount_right >= speed_right) || (interruptcount_left >= speed_left)){
      
      PTH = 0x79; // right one
      PORTB =  (RightStepArray[stepindex_right] | LeftStepArray[stepindex_left]);
      
      stepindex_right++;
      stepindex_left++;         
      
      // reset step index
      if (stepindex_right > 7)
        stepindex_right = 0;
      // reset step index
      if (stepindex_left > 7)
        stepindex_left = 0;
          
      if (interruptcount_right >= speed_right){
        // reset right wheel interrupt count
        interruptcount_right = 0;
      }
      
       if (interruptcount_left >= speed_left){  
          //reset left wheel interrupt count
          interruptcount_left = 0;
       }
      
      count_both++;
    } // end of if                  
    
 } else {
    count_both=0;
    state = 3; // u turn
 }
 
 } // end of while
    count_both=0;
    current_speed = top_speed;
    state = 2;
 
 
  
} // end of half step forward

void RIGHT_STEP_FORWARD(void){

    //right wheel speed
    if (interruptcount_right >= speed_right){
      PORTB =  (PORTB & ~0xF0) |  (RightStepArray[stepindex_right] & 0xF0);
      stepindex_right++;
          
      // reset step index
      if (stepindex_right > 7)
        stepindex_right = 0;
          
      // reset right wheel interrupt count
      interruptcount_right = 0;
     
    }
      
  
}
void LEFT_STEP_FORWARD(void){
  
    // left speed time
    if (interruptcount_left >= speed_left){
      PORTB =  (PORTB & ~0x0F) |  (LeftStepArray[stepindex_left] & 0x0F);
      stepindex_left++;

      // reset step index
      if (stepindex_left > 7)
        stepindex_left = 0;
          
      //reset left wheel interrupt count
      interruptcount_left = 0;
    }
  
}

void RAMP_BOTH_UP(void){
  
  while (current_speed > top_speed){
  
    if ((interruptcount_right >= current_speed) && (interruptcount_left >= current_speed)){
     
      
      PORTB =  (LeftStepArray[stepindex_left] | RightStepArray[stepindex_right]);
      
      stepindex_right++;
      stepindex_left++;         
      
      // reset step index
      if (stepindex_right > 7)
        stepindex_right = 0;
      // reset step index
      if (stepindex_left > 7)
        stepindex_left = 0;
          
      //reset left wheel interrupt count
      interruptcount_left = 0;
      // reset right wheel interrupt count
      interruptcount_right = 0;
     
     
      current_speed--;
    }
    
  }
  //current_speed = 0;
  state = 1;
  
}
void RAMP_BOTH_DOWN(void){
 
    while (current_speed < bottom_speed){
  
    if ((interruptcount_right >= current_speed) && (interruptcount_left >= current_speed)){
     
      PTH = 0x24;   //2
      PORTB =  (LeftStepArray[stepindex_left] | RightStepArray[stepindex_right]);
      
      stepindex_right++;
      stepindex_left++;         
      
      // reset step index
      if (stepindex_right > 7)
        stepindex_right = 0;
      // reset step index
      if (stepindex_left > 7)
        stepindex_left = 0;
          
      //reset left wheel interrupt count
      interruptcount_left = 0;
      // reset right wheel interrupt count
      interruptcount_right = 0;
     
     
      current_speed++;
    }
    
  }
  
  state = 0;
  
}

void STATIONARY_180_TURN(int steps){
  
  while (count_both <= steps){

    if ((interruptcount_right >= 15) && (interruptcount_left >= 15)){
      
      PTH = 0x30;   //3
      PORTB =  (RightStepArray_Rev[stepindex_right] | LeftStepArray[stepindex_left]);
      
      stepindex_right++;
      stepindex_left++;         
      
      // reset step index
      if (stepindex_right > 7)
        stepindex_right = 0;
      // reset step index
      if (stepindex_left > 7)
        stepindex_left = 0;
          
      //reset left wheel interrupt count
      interruptcount_left = 0;
      // reset right wheel interrupt count
      interruptcount_right = 0;
      
      count_both++;
    }                   
  } // end of while
  
    count_both=0;
    current_speed = bottom_speed;
    state = 0;
 
} // end of u-turn


void InitializeATD(void)
{
  ATD0CTL2 = 0x80; // power on ATD
  ATD0CTL3 = 0x18; // two conversions = 0x10 three conversions = 0x18
  ATD0CTL4 = 0x85; // was 0x84
}

void SetupMDCU(void){
     //set up MDCU for 0.1ms = 0.0001 seconds interrupt
  MCCTL = 0xCF;  //interrupt enable, load latest mod reg, mcen, pre=16
  MCCNT = 150;   //(0.0001-sec)(24000000-CLKperSEC)(16-CLKperCOUNT) = COUNT
  MCCTL = 0xCF;  
}

void SetupRTI(void){
 
  RTICTL = 0b01101011;	//sets prescaler of 82^10 resulting in interrupt every 49.15ms
  CRGINT = 0x80;			//tells that want interrupt given, not flag raised 
  
}

void SetupDDR_Ports(void){
  
  // set up 7-segment display  
  DDRH = 0xFF;
  // set up motor ports
  DDRB = 0xFF; //     PORTB0-PORTB3  as output 
  DDRP = 0x0F; //     PORTP0 and PORTP1 as output for 12EN=1 and 34EN=1
  
  PORTB=0b00000000;//  start with all off
  PTP=0b00001111; //  Turn on both 12EN and 34EN Enables for 754410 chip 
  
}