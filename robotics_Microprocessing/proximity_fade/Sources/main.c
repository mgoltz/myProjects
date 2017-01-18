#include <hidef.h>      /* common defines and macros */
#include "derivative.h"      /* derivative-specific definitions */

int duty = 0;

void main(void) {
 
 
  // will talk about this later
/*  
  SYNR = 2;
  REFDV = 0;
  while (!(CRGFLG&0x08)){};
  CLKSEL |= 0x80;
*/
  
  // set up 7-segment display  
  DDRH = 0xFF;

  // set up RTI
  RTICTL = 0b00010000; // RTI Prescale - 2^10 clk = 8MHz/2^10 = 
  CRGINT = 0b10000000; // Turns on interrupt

	EnableInterrupts;
	
	
	
  ATD0CTL2 = 0x80;
  ATD0CTL3 = 0x18;      //prev 0x40
  ATD0CTL4 = 0x85;   

	EnableInterrupts;
	
	while(1){
	  ATD0CTL5 = 0b00100100;
	  while(!(ATD0STAT0 & 0x80)){
	  
	    int dout = ((int)ATD0DR0H + (int)ATD0DR1H + (int)ATD0DR2H) / 3;
	    duty = dout / 2;
	    
	    if(duty%10>=5){
	      duty = duty + (10-duty%10);
	    }else{
	      duty = duty - (duty%10);
	    } 
	  }
	}
   
}

#pragma TRAP_PROC
#pragma CODE_SEG __SHORT_SEG NON_BANKED
interrupt VectorNumber_Vrti void rtiInterrupt () {
  static int rti_count = 0;
  static int off_count = 20;
  static int temp = 0;
  
  // ALWAYS CLEAR THE FLAG FIRST!
  CRGFLG |= 0x80;
  
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
  }
  
}

