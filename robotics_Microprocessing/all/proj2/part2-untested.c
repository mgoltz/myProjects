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
      	DATWRT4('T');   // A
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
  EnableInterrupts;
	

  // set up RTI
  RTICTL = 0b01111011; // RTI Prescale - 12*2^16 clk = 8MHz/(12*2^16) = 10.17Hz = 0.098304sec/int
  CRGINT = 0b10000000; // Turns on interrupt


	
	// sensor setup
  ATD0CTL2 = 0x80;      // power on
  ATD0CTL3 = 0x18;      // three conversions
  ATD0CTL4 = 0x85;      // 8bit resolution
	
	for(;;){
	    _FEED_COP();
       // wait for rtiInterrupt
	}
   
}

#pragma TRAP_PROC
#pragma CODE_SEG __SHORT_SEG NON_BANKED
interrupt VectorNumber_Vrti void rtiInterrupt () {
  
  char buffer[3];
  static int vals[10]=0;
  static int i = -1;
	int avg6 = 0;
	static int sum6 = 0;
	int n = 0;
	int start = 0;
	int end = 0;
	int time = 0;
	
  int max1, max2 = 0;
  int min1, min2 = 1000;
  int j;

	
  
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
	    
     TSCR2 = 0x00;    //Start main timer and clear overflow flag!
     TSCR1 = 0x80;
     TFLG2 = 0x00;
     start = TCNT;
     
     i = (i+1)%10;
     vals[i] = dout;
     sum6 = 0;
    
     
     for ( j = 0; j < 10; j++){   
        if (vals[j] > max1){
        	max2 = max1;
        	max1 = vals[j];
        }else if (vals[j] > max2){
        	max2= vals[j];
        }
        
        if(vals[j] < min1){
        	min2 = min1;
        	min1 = vals[j];
        }else if(vals[j]< min2){
        	min2 = vals[j];
        }
        sum6 += vals[j];
     }
	   
	   if(min1 == 1000 & min2 == 1000){
	     sum6 = 0;
	   }else{
	     sum6 -= (min1 + min2 + max1 + max2);
	   }
	   avg6 = sum6 / 6;
	   avg6 += 20; //get rid of negative values
	   
	   end = TCNT;
	   
	   if (TFLG2 == 0x80){
	    time = (65536-start)+ end;
	   }else{
	    time = end - start;
	   }
	   
	   time = time / 24;    //convert to 1microsecond
    
 	        
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
  n = sprintf(buffer, "%d", avg6);
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
  
  
  n = sprintf(buffer, "%d", time);
  COMWRT4(0xC2);  // second line
  for (i = 0; i<n; i++){
    DATWRT4(buffer[i]);   //
	  MSDelay(1);
  }
  


  }
  
}
