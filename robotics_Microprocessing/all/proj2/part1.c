/* * * *
Authors:    Michael Goltz
            Joshua Philpott
Date:       Jan 22, 2014
USF Class:  Control of Mobile Robots
Project 2:  

* * * * */

#include <hidef.h>      /* common defines and macros */
#include "derivative.h"      /* derivative-specific definitions */
#include <stdlib.h>
#include <stdio.h>

#define LCD_DATA PORTK
#define LCD_CTRL PORTK
#define RS 0x01
#define EN 0x02

int duty = 0;

//software delay
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
        LCD_DATA =LCD_DATA & ~0x3C;          //clear bits Pk5-Pk2
        LCD_DATA = LCD_DATA | x;          //sends high nibble to PORTK
        MSDelay(1);
        LCD_CTRL = LCD_CTRL & ~RS;         //set RS to command (RS=0)
        MSDelay(1);
        LCD_CTRL = LCD_CTRL | EN;          //rais enable
        MSDelay(5);
        LCD_CTRL = LCD_CTRL & ~EN;         //Drop enable to capture command
        MSDelay(15);                       //wait
        x = (command & 0x0F)<< 2;          // shift low nibble to center of byte for Pk5-Pk2
        LCD_DATA =LCD_DATA & ~0x3C;         //clear bits Pk5-Pk2
        LCD_DATA =LCD_DATA | x;             //send low nibble to PORTK
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

 void setup_lcd(){
 
        DDRK = 0xFF;   
        COMWRT4(0x33);   //reset sequence provided by data sheet
        MSDelay(1);
        COMWRT4(0x32);   //reset sequence provided by data sheet
        MSDelay(1);
        COMWRT4(0x28);   //Function set to four bit data length
                                         //2 line, 5 x 7 dot format
        MSDelay(1);
        COMWRT4(0x06);  //entry mode set, increment, no shift
        MSDelay(1);
        COMWRT4(0x0E);  //Display set, disp on, cursor on, blink off
        MSDelay(1);
        COMWRT4(0x01);  //Clear display
        MSDelay(1);
        COMWRT4(0x80);  //set start posistion, home position
        MSDelay(1); 
        
      	DATWRT4('A');   // A
        MSDelay(1);
        DATWRT4('v');   // v
        MSDelay(1);
        DATWRT4('g');   // g
        MSDelay(1);
        DATWRT4(':');   // :
        MSDelay(1);
        DATWRT4(' ');   // space
        MSDelay(1);
        
          // write to second line
        COMWRT4(0xC0);  // second line
        MSDelay(1);
      	DATWRT4('A');   // A
        MSDelay(1);
        DATWRT4('v');   // v
        MSDelay(1);
        DATWRT4('g');   // g
        MSDelay(1);
        DATWRT4(':');   // :
        MSDelay(1);
        DATWRT4(' ');   // space
        MSDelay(1);
        
}


void main(void) {
 
 
  // will talk about this later
/*  
  SYNR = 2;
  REFDV = 0;
  while (!(CRGFLG&0x08)){};
  CLKSEL |= 0x80;
*/
  
  setup_lcd();
  
  // set up 7-segment display  
  DDRH = 0xFF;

  // set up RTI
  RTICTL = 0b01111011; // RTI Prescale - 12*2^16 clk = 8MHz/(12*2^16) = 10.17Hz = 0.098304sec/int
  CRGINT = 0b10000000; // Turns on interrupt

	EnableInterrupts;
	
	
	// sensor setup
  ATD0CTL2 = 0x80;      // power on
  ATD0CTL3 = 0x18;      // three conversions
  ATD0CTL4 = 0x85;      // 8bit resolution

	EnableInterrupts;
	
	for(;;){
	    _FEED_COP();
       // wait for rtiInterrupt
	}
   
}

#pragma TRAP_PROC
#pragma CODE_SEG __SHORT_SEG NON_BANKED
interrupt VectorNumber_Vrti void rtiInterrupt () {
  
  char buffer[3];
  static int vals8[8]=0;
  static int i = -1;
	int avg8 = 0;
	static int sum8 = 0;
	int n = 0;
	
	static int vals50[50]=0;
	static int j = -1;
	int avg50 = 0;
	static int sum50 = 0;
	
  
  // ALWAYS CLEAR THE FLAG FIRST!
  CRGFLG |= 0x80;
  
  /* * * * * * * * * * * * * * *
  *  
  *  START - Read Sensor values
  *
  * * * * * * * * * * * * * * */
  
  	// writes to ATDCTL5 will abort current conversion
	  // sequence and start a new conversion sequence
	  ATD0CTL5 = 0b00100100;  // left justified, continuous conversion, one channel, start on channel 4
	  	  
	  // conversion sequence complete flag
	  while(!(ATD0STAT0 & 0x80)){
	 
	    // average digital output converted to ?? duty between 0-9
	    int dout = ((int)ATD0DR0H + (int)ATD0DR1H + (int)ATD0DR2H) / 3;
	    
	    
	    //round to the nearest 10s up to roughly 70
	    /*if(duty%10>=5){
	      duty = duty + (10-duty%10);
	    }else{
	      duty = duty - (duty%10);
	    } */
	    

	   
	   
	   i = (i+1) % 8;
	   sum8 -= vals8[i];
	   vals8[i] = dout;
	   sum8 += vals8[i];
	   avg8 = sum8 / 8;
	   
	   j = (j+1) % 50;
	   sum50 -= vals50[j];
	   vals50[j] = dout;
	   sum50 += vals50[j];
	   avg50 = sum50 / 50;
	    
	    
	        
	  }
	  
	  
	  /* * * * * * * * * * * * * * *
  *  
  *  END - Read Sensor values
  *
  * * * * * * * * * * * * * * */
  
  
  /* * * * * * * * * * * * * * *
  *  
  *  START - Update LCD values
  *
  * * * * * * * * * * * * * * */
  
 
  // error checking in count > 999
  n = sprintf(buffer, "%d", avg8);
  if (n == 1) {
    buffer[1] = ' ';
    buffer[2] = ' ';
  }else if( n == 2){
    buffer[2] = ' '; 
  }
  
  if (n>3){    
    COMWRT4(0x85);  // first line
    DATWRT4('E');   // insert char of first digit
    MSDelay(1); 
    DATWRT4('R');   // insert char of next digit
    MSDelay(1);
    DATWRT4('R');   // insert char of next digit
    MSDelay(1);  
    
  } else {
    
   
  //call an update function or do the same below
  // write to first line
  //COMWRT4(0x01);  //Clear display

  COMWRT4(0x85);  // first line
  DATWRT4(buffer[0]);   // insert char of first digit
  MSDelay(1); 
  DATWRT4(buffer[1]);   // insert char of next digit
  MSDelay(1);
  DATWRT4(buffer[2]);
  MSDelay(1);

  }
  
  n = sprintf(buffer, "%d", avg50);
  if (n == 1) {
    buffer[1] = ' ';
    buffer[2] = ' ';
  }else if( n == 2){
    buffer[2] = ' '; 
  }  
  
  if(n>3) {
    //error
    COMWRT4(0xC5);  // second line
    DATWRT4('E');   // insert char of first digit
    MSDelay(1); 
    DATWRT4('R');   // insert char of next digit
    MSDelay(1);
    DATWRT4('R');   // insert char of next digit
    MSDelay(1);  
  }else{
    
    // write to second line
    COMWRT4(0xC5);  // second line
    DATWRT4(buffer[0]);   // insert char of first digit
    MSDelay(1); 
    DATWRT4(buffer[1]);   // insert char of next digit
    MSDelay(1);
    DATWRT4(buffer[2]);
    MSDelay(1);

  }       
    
  
  /* * * * * * * * * * * * * * *
  *  
  *  END - Update LCD values
  *
  * * * * * * * * * * * * * * */
  
  
  /* * * * * * * * * * * * * * *
  *  
  *  START - 7segment control of brightness
  *
  * * * * * * * * * * * * * * */
  /*static int rti_count = 0;
  static int off_count = 20;
  static int temp = 0;
  
  rti_count++;
  if (rti_count >= 100) {
    rti_count = 0;
    temp = 0;
    //Turn PTH_PTH* on
    PTH = 0x00;
    
  }
  
  temp++;
  if(temp >= duty){
    //Turn PTH_PTH* OFF 
    PTH = 0xFF;
  }*/
  
  /* * * * * * * * * * * * * * *
  *  
  *  END - 7segment control of brightness
  *
  * * * * * * * * * * * * * * */

  
}
