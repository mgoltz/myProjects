#include <hidef.h>      // common defines and macros 
#include "derivative.h"   //    derivative-specific definitions

#define LCD_DATA PORTK
#define LCD_CTRL PORTK
#define RS 0x01
#define EN 0x02

void InitializeATD(void);
void SetupMDCU(void);
void SetupRTI(void);
void SetupDDR_Ports(void);
void HALF_STEP_FORWARD(int steps);    // move forward x number of steps
void RIGHT_STEP_FORWARD(void);
void LEFT_STEP_FORWARD(void);

void DISPLAY_POS(void);
void COMWRT4(unsigned char);
void DATWRT4(unsigned char);
void MSDelay(unsigned int);

volatile unsigned char OutputIndex = 0;	//points to the most recent distance Output location

//motor wheel half steps
volatile unsigned char RightStepArray[] ={0xC0, 0x80, 0x90, 0x10, 0x30, 0x20, 0x60, 0x40};
volatile unsigned char LeftStepArray[] ={0x0C, 0x04, 0x06, 0x02, 0x03, 0x01, 0x09, 0x08};
unsigned char SEG7_DISPLAY[10] = {0x40,0x79,0x24,0x30,0x19,0x12,0x02,0x78,0x00,0x10};
                                                          //0       1       2        3        4     5       6       7       8       9
// turn control
volatile int state = 0;
// motor timing global variable
volatile int interruptcount_left = 0;
volatile int interruptcount_right = 0;
volatile int stepindex_left = 0;
volatile int stepindex_right = 0;
volatile int speed_left = 30;       // int  26
volatile int speed_right = 30;     // int  26
volatile int top_speed = 30;      // 26
volatile int current_speed = 130;
volatile int count_both = 0;
volatile int drive_5cm = 160;

// ATD conversion
volatile long a[2] =  {-25575, 10508};
volatile long b[3] =  {329, 658, 329};
volatile int interruptcount_filter = 0;
volatile long x_s;
volatile int y_s; 
volatile long ylng;
volatile int d[3] = {32};
volatile long dlng;

volatile int numdigits = 0;
volatile int i;
volatile char digits[30];

/*
* This is code for calculating the drive probabilities based off sensor readings.
* We did not have time to debug this code so it currently does not work.
*
* However, we think you'll be able to see how we're attempting to implement calculating the probabilities at each step
 */

//Drive distances and probabilities for each based off our probability density function
volatile double drive_prob[12] = {0.03, 0.0, 0.8, 0.03, 0.05, 0.18, 0.15, 0.20, 0.15, 0.10, 0.03, 0.03};
volatile double drive_pos[12]  = {4.2,  4.3, 4.4, 4.5,  4.6,  4.7,  4.8,  4.9,  5.0,  5.1,  5.2,  5.3};
volatile double n = 0;


//Calculates the probability based off sensor value
void PROBABLE_POS(int sensor){
  static float prob_x_start = 1; /*probability x starts at 0,5,10,etc...)*/
  static float start_pos[10]={0};
  static float prob_start_pos[10]={1}; 
  static float prob_pos[120]={0};
  static float pos[120]={0};

  float sensor_prob[4] = {0};
  int i = 0;
  int j = 0;


  //Run for each possible start position
  for(i=0; i<10; i++){
    int prob_sum = 0;
    //Run for each possible drive distance
    for(j=0; j<12; j++){
      //Determine probability based off discrete sensor readings, drive probability, and start position probability
      if(start_pos[i]>=0 && start_pos[i]<=2.5){
        if(sensor==20) prob_pos[((i+1)*(j+1))-1] = (.05) * drive_prob[j] * prob_start_pos[i];
        else if(sensor==21) prob_pos[((i+1)*(j+1))-1] = (.35) * drive_prob[j] * prob_start_pos[i];
        else if(sensor==22) prob_pos[((i+1)*(j+1))-1] = (.30) * drive_prob[j] * prob_start_pos[i];
        else if(sensor==23) prob_pos[((i+1)*(j+1))-1] = (.30) * drive_prob[j] * prob_start_pos[i];
        else prob_pos[((i+1)*(j+1))-1] = 0;
      }else if(start_pos[i]>2.5 && start_pos[i]<=7.5){
        if(sensor==26) prob_pos[((i+1)*(j+1))-1] = (.05) * drive_prob[j] * prob_start_pos[i];
        else if(sensor==27) prob_pos[((i+1)*(j+1))-1] = (.75) * drive_prob[j] * prob_start_pos[i];
        else if(sensor==28) prob_pos[((i+1)*(j+1))-1] = (.20) * drive_prob[j] * prob_start_pos[i];
        else prob_pos[((i+1)*(j+1))-1] = 0;
      }else if(start_pos[i]>7.5 && start_pos[i]<=12.5){
        if(sensor==34) prob_pos[((i+1)*(j+1))-1] = (.05) * drive_prob[j] * prob_start_pos[i];
        else if(sensor==35) prob_pos[((i+1)*(j+1))-1] = (.35) * drive_prob[j] * prob_start_pos[i];
        else if(sensor==36) prob_pos[((i+1)*(j+1))-1] = (.30) * drive_prob[j] * prob_start_pos[i];
        else if(sensor==37) prob_pos[((i+1)*(j+1))-1] = (.30) * drive_prob[j] * prob_start_pos[i];
        else prob_pos[((i+1)*(j+1))-1] = 0;
      }else if(start_pos[i]>12.5 && start_pos[i]<=17.5){
        if(sensor==47) prob_pos[((i+1)*(j+1))-1] = (.10) * drive_prob[j] * prob_start_pos[i];
        else if(sensor==48) prob_pos[((i+1)*(j+1))-1] = (.35) * drive_prob[j] * prob_start_pos[i];
        else if(sensor==49) prob_pos[((i+1)*(j+1))-1] = (.40) * drive_prob[j] * prob_start_pos[i];
        else if(sensor==50) prob_pos[((i+1)*(j+1))-1] = (.5) * drive_prob[j] * prob_start_pos[i];
        else prob_pos[((i+1)*(j+1))-1] = 0;
      }

      //Calculate the position based off start position and drive distance
      pos[((i+1)*(j+1))-1] = start_pos[i] + drive_pos[j]; 
      //Sum probabilities used to calculate norm
      prob_sum +=  prob_pos[((i+1)*(j+1))-1];
    }

    //Calculate norm and multiply each of the previous 10 values by the norm
    int norm = 1;
    if(prob_sum != 0) norm = 1/prob_sum;

    for(j=0;j<12;j++){
      prob_pos[((i+1)*(j+1))-1] = prob_pos[((i+1)*(j+1))-1] * norm;
    }
  }

  int max_probs[10]={0};
  int max_prob_pos[10]={0};


  //Bubble sort 
  for(i=0; i<120; i++){
    for(j=0; j<119; j++){
      if(prob_start_pos[i]>prob_start_pos[i+1])
      {
        int temp = prob_start_pos[i+1];
        prob_start_pos[i+1] = prob_start_pos[i];
        prob_start_pos[i] = temp;
        temp = pos[i+1];
        pos[i+1]=pos[i];
        pos[i] = temp;
      }
    }
  }

  //Assign the greatest values start position and probability
  for(i=0;i<10;i++){
    start_pos[i] = pos[119-i];
    prob_start_pos[i] = prob_pos[119-i];

}

/* * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  void main (void)
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void main(void){
  

	SYNR = 0x02;
	REFDV = 0x00;		//this code should change the clock to 24MHz
	while (!(CRGFLG & 0x08))
	{ }
	CLKSEL = 0x80;
	 
	InitializeATD();
	//SetupMDCU();
	//set up MDCU for 0.1ms = 0.0001 seconds interrupt   
	MCCTL = 0xCF;  //interrupt enable, load latest mod reg, mcen, pre=16
	MCCNT = 150;   //(0.0001-sec)(24000000-CLKperSEC)(16-CLKperCOUNT) = COUNT
	MCCTL = 0xCF;
	SetupDDR_Ports();
	
    COMWRT4(0x33); //reset sequence provided by data sheet
    COMWRT4(0x32); //reset sequence provided by data sheet
    COMWRT4(0x28);
    COMWRT4(0x06);
    COMWRT4(0x0E);
    COMWRT4(0x01);
    COMWRT4(0x80);

	EnableInterrupts;
	
	for(;;) {
	  
		ATD0CTL5 = 0x33; // scan, no multichan, start at 4
		  
		while (!(ATD0STAT0 & 0x80));
     
		switch (state){
		    case 0:   // 30cm to 25cm
		                  HALF_STEP_FORWARD(drive_5cm);
		                  PROBABLE_POS(y_s);
		                  DISPLAY_POS();
		                  state = 4;
		                  break;
		                  
            case 1:   // 25cm to 20cm
                          HALF_STEP_FORWARD(drive_5cm);
		                  PROBABLE_POS(y_s);
		                  DISPLAY_POS();
		                  state = 2;
		                  break;

            case 2:   // 20cm to 15cm
                          HALF_STEP_FORWARD(drive_5cm);
		                  PROBABLE_POS(y_s);
		                  DISPLAY_POS();
		                  state = 3;
		                  break;		                  
		                  
            case 3:   // 15cm to 10cm
                          HALF_STEP_FORWARD(drive_5cm);
		                  PROBABLE_POS(y_s);
		                  DISPLAY_POS();
		                  state = 4;
		                  break;
		                  
		    case 4:   break;
		
		} // end of switch
    }// end for loop
} // end of main		                  


//interrupt triggers every 0.1ms
#pragma TRAP_PROC
#pragma CODE_SEG __SHORT_SEG NON_BANKED
interrupt VectorNumber_Vtimmdcu void mdcuInterrupt () 
{

	MCFLG |= 0x80;
	
	interruptcount_left++;
	interruptcount_right++;
    interruptcount_filter++;
	
    if(interruptcount_filter == 10 ){ //&& ((state == 0) || (state == 1) || (state == 2))
      interruptcount_filter = 0;
      
      		// front sensor
		x_s = ATD0DR0H;
		x_s <<= 14;
		  
		dlng = x_s - a[0] * d[1] - a[1] * d[2];
		dlng <<= 2;
		dlng >>= 16;
		d[0] = dlng;
		ylng = b[0] * d[0] + b[1] * d[1] + b[2] * d[2];
		ylng <<= 2;
		ylng >>= 16;
		y_s = ylng;   
		d[2] = d[1];     
		d[1] = d[0];
	
    }//  end of sensor reading
 	
}//  end of MDCU

void DISPLAY_POS(void) {
  PTH = SEG7_DISPLAY[0];
  
  
  		if (y_s >= 24)
		  PTH = SEG7_DISPLAY[1];
		else if (y_s >= 29)
		  PTH = SEG7_DISPLAY[2];
		else if (y_s >= 37)
		  PTH = SEG7_DISPLAY[3];
		else if (y_s >= 51)
          PTH = SEG7_DISPLAY[3];
  
		//Move cursor to second line
		COMWRT4(0xC0); 
		DATWRT4('b');
		DATWRT4('f');
		DATWRT4('=');
		//Move cursor to second line
		COMWRT4(0xC3);

		//send y to LCD 
		numdigits = sprintf(digits, "%d", y_s);
  
		for(i = 0; i < numdigits; i++)
				DATWRT4(digits[i]); 
  
}
void HALF_STEP_FORWARD(int steps){
 
	while (count_both <= steps){
	    
	     // go straight	
				speed_left = speed_right = top_speed;
				
				// right speed time
                if ((interruptcount_right >= speed_right)){
                  RIGHT_STEP_FORWARD();
                  count_both++;
                }// end of right port writing    
        
                // left speed time
                if (interruptcount_left >= speed_left){
                  LEFT_STEP_FORWARD();
                }// end of left port writing
                
	}// end of while
	count_both = 0;
	
} // end of half step forward

void RIGHT_STEP_FORWARD(void){

    //right wheel speed
		PORTB =  (PORTB & ~0xF0) |  (RightStepArray[stepindex_right] & 0xF0);
		stepindex_right++;
			  
		// reset step index
		if (stepindex_right > 7)
			stepindex_right = 0;
			  
		// reset right wheel interrupt count
		interruptcount_right = 0;
}
void LEFT_STEP_FORWARD(void){
  
    // left speed time
		PORTB =  (PORTB & ~0x0F) |  (LeftStepArray[stepindex_left] & 0x0F);
		stepindex_left++;

		// reset step index
		if (stepindex_left > 7)
			stepindex_left = 0;
          
		//reset left wheel interrupt count
		interruptcount_left = 0;
}

void InitializeATD(void)
{
	//ATD0CTL2 = 0x80; // power on ATD
	//ATD0CTL3 = 0x08; // one conversion = 0xo8, two conversions = 0x10 three conversions = 0x18
	//ATD0CTL4 = 0x85; // was 0x84
	
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