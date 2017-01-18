#include <hidef.h>      /* common defines and macros */
#include "derivative.h"      /* derivative-specific definitions */


 void incMSDelay(unsigned int itime)
 {
    int time = 0;
    unsigned int i; unsigned int j;
    for(i=0;i<itime;i++)
      for(j=0;j<4000;j++){
       
        time++;
        PWMSCLA = time;  
        
      }
 }
 
 void decMSDelay(unsigned int itime)
 {
    unsigned int i; unsigned int j;
    for(i=0;i<itime;i++)
      for(j=0;j<4000;j++);
 }
 
  

void main(void) {
  /* put your own code here */
  

  PWMPRCLK=0x04; //ClockA=Fbus/2**4=24MHz/16=1.5MHz	
	PWMSCLA=100; 	 //ClockSA=1.5MHz/2x125=6000 Hz
	PWMCLK=0b00100000; 	 //ClockSA for chan 5
	PWMPOL=0x20; 		     //high then low for polarity
	PWMCAE=0x0; 		     //left aligned
	PWMCTL=0x0;	         //8-bit chan, pwm during freeze and wait
	PWMPER5=100; 	 //PWM_Freq=ClockSA/100=6000Hz/100=60Hz. CHANGE THIS
	PWMDTY5=50; 	 //50% duty cycle                        AND THIS TO GET DIFFERENT SOUND
	PWMCNT5=0;		 //clear initial counter. This is optional
	PWME=0x20; 	   //Enable chan 5 PWM
  while(1){
    
    //incMSDelay(100);
    
  };      //stay here forever
  
}