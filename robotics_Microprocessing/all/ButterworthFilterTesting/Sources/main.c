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

void SetupDDR_Ports(void);

void HALF_STEP_FORWARD(int steps);    // move forward x number of steps
void RIGHT_STEP_FORWARD(void);
void LEFT_STEP_FORWARD(void);
void RAMP_BOTH_UP(void);

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

void main(void) {
    
    SYNR = 0x02;
	REFDV = 0x00;		//this code should change the clock to 24MHz
	while (!(CRGFLG & 0x08))
	{ }
	CLKSEL = 0x80;


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
  //CRGINT = 0b10000000;
 // RTICTL = 0b01111011;
  
  
	//set up MDCU for 0.1ms = 0.0001 seconds interrupt  
  	MCCTL = 0xCF;  //interrupt enable, load latest mod reg, mcen, pre=16
	MCCNT = 150;   //(0.0001-sec)(24000000-CLKperSEC)(16-CLKperCOUNT) = COUNT
	MCCTL = 0xCF;
	SetupDDR_Ports();
	
    CRGINT = 0b10000000;
    RTICTL = 0b01111011;
  
  
  //set up main timer 
 // TSCR1 = 0x80;
  
  //set up ATD
  ATD0CTL2 = 0x80; // power on ATD
  ATD0CTL3 = 0x18; // two conversions = 0x10 three conversions = 0x18
  ATD0CTL4 = 0x85; // was 0x84      8bit resolution = 0x85 - 10bit resolution = 0x05

	EnableInterrupts;

   for(;;)
   {
    // left justified, unsigned, scan = continuous conversion sequence, multichannel, start on channel 3
    ATD0CTL5 = 0x33; 
    while (!(ATD0STAT0 & 0x80));
     
		switch (state){
		    case 0:   // 30cm to 25cm
		                  HALF_STEP_FORWARD(drive_5cm);
		                  //PROBABLE_POS();
		                  //DISPLAY_POS();
		                  state = 4;
		                  break;
		                  
            case 1:   // 25cm to 20cm
                          HALF_STEP_FORWARD(drive_5cm);
		                  //PROBABLE_POS();
		                  //DISPLAY_POS();
		                  state = 2;
		                  break;

            case 2:   // 20cm to 15cm
                          HALF_STEP_FORWARD(drive_5cm);
		                  //PROBABLE_POS();
		                  //DISPLAY_POS();
		                  state = 3;
		                  break;		                  
		                  
            case 3:   // 15cm to 10cm
                          HALF_STEP_FORWARD(drive_5cm);
		                  //PROBABLE_POS();
		                  //DISPLAY_POS();
		                  state = 4;
		                  break;
		                  
		    case 4:   break;
		
		} // end of switch
    }// end for loop
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


//interrupt triggers every 0.1ms
#pragma TRAP_PROC
#pragma CODE_SEG __SHORT_SEG NON_BANKED
interrupt VectorNumber_Vtimmdcu void mdcuInterrupt () {  

	MCFLG |= 0x80;
	
	interruptcount_left=interruptcount_left+6;
	interruptcount_right=interruptcount_right+6;
    interruptcount_filter++;
	
    if(interruptcount_filter == 10 ){ //&& ((state == 0) || (state == 1) || (state == 2))
      interruptcount_filter = 0;
      
        //Clear display
		COMWRT4(0x01);
		MSDelay(1);
  
		x_s = ATD0DR1H;
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
      
    }//  end of sensor reading
 	
}//  end of MDCU

interrupt VectorNumber_Vrti void rtiInterrupt () {
  //static int rti_50 = 0;
  //static int values50[50] = {0};
  //static int avg50 = 0;
  static int ii;
  static char idigits[30];
  static int inumdigits = 0;
  //static int starttime, endtime;
  //static int counts = 0;
  //static int time = 0;
  //static long x;

  // ATD conversion
  static long ifx, rx, lx;
  static int ify;//, ry, ly 
  static long ifylng;//, rylng, lylng;
  static int ifd[3] = {32};
  //static int rd[3] ={32};
  //static int ld[3] ={32};
  static long ifdlng;//, rdlng, ldlng;


  static int ishift = 14;
  static long ia[2] =  {-25575, 10508};
  static long ib[3] =  {329, 658, 329}; 
  /*static float a[2] =  { -1.7786,0.8008};
  static float b[3] =  {0.0055427,0.011085, 0.0055427};  
  static float a[2] =  {-1.9556, 0.95654};
  static float b[3] =  {0.00024136,0.00048272, 0.00024136}; */
  
  // ALWAYS CLEAR THE FLAG FIRST!
  CRGFLG |= 0x80;
 
  
  //Clear display
  COMWRT4(0x01);
  MSDelay(1);
  
    // left sensor
    lx = ATD0DR0H;
/*		lx <<= 14;
      
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
  
  //send left sensor value to LCD 
  numdigits = sprintf(digits, "%d", ly);
  
  for(i = 0; i < numdigits; i++)
    DATWRT4(digits[i]); 
    
*/    
   // right sensor
    rx = ATD0DR2H;
/*		rx <<= 14;
      
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
    
  //Move cursor to right side of first line
  COMWRT4(0x85);
  
  //send left sensor value to LCD 
  numdigits = sprintf(digits, "%d", ry);
  
  for(i = 0; i < numdigits; i++)
    DATWRT4(digits[i]);

*/
  
  //DATWRT4('n');
  //DATWRT4('f');
  //DATWRT4('=');
   //Move cursor to firstline middle
  //COMWRT4(0x83); 
    
    // front sensor
    ifx = ATD0DR1H;
   
  //send fx to LCD 
  //numdigits = sprintf(digits, "%ld", fx);  
  //for(i = 0; i < numdigits; i++)
  //  DATWRT4(digits[i]);
  
  
    ifx <<= 14;
      
    ifdlng = ifx - ia[0] * ifd[1] - ia[1] * ifd[2];
    ifdlng <<= 2;
    ifdlng >>= 16;
    ifd[0] = ifdlng;
    ifylng = ib[0] * ifd[0] + ib[1] * ifd[1] + ib[2] * ifd[2];
    ifylng <<=2;
    ifylng >>= 16;
    ify = ifylng;
    ifd[2] = ifd[1];
    ifd[1] = ifd[0];
  
  //Move cursor to second line
  //COMWRT4(0xC0); 
  //DATWRT4('b');
  //DATWRT4('f');
  //DATWRT4('=');
  //Move cursor to second line
  //COMWRT4(0xC3);

  //send y to LCD 
  //inumdigits = sprintf(idigits, "%d", ify);
  
  //for(ii = 0; ii < inumdigits; ii++)
    //DATWRT4(idigits[ii]); 

 
  //TFLG2 = 0x80;

  

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

void RAMP_BOTH_UP(void){
	
	//current_speed = bottom_speed;
	while (current_speed >= top_speed){
  
		if ((interruptcount_right >= current_speed) && (interruptcount_left >= current_speed)){
			
			// this works for moving left & right wheel at same rate
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
		 
			current_speed = current_speed-1;
		}// end if
    
	}	// end while
	current_speed = top_speed;

}	// end RAMP_BOTH_UP

void SetupDDR_Ports(void){
  
	// set up 7-segment display  
	DDRH = 0xFF;
	// set up motor ports
	DDRB = 0xFF; //     PORTB0-PORTB3  as output 
	DDRP = 0x0F; //     PORTP0 and PORTP1 as output for 12EN=1 and 34EN=1
  
	PORTB=0b00000000;//  start with all off
	PTP=0b00001111; //  Turn on both 12EN and 34EN Enables for 754410 chip 
  
}