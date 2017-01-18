#include <hidef.h>      /* common defines and macros */
#include "derivative.h"      /* derivative-specific definitions */

#define LCD_DATA PORTK
#define LCD_CTRL PORTK
#define RS 0x01
#define EN 0x02


  void MSDelay(unsigned int itime)
  {
    unsigned int i; unsigned int j;
    for(i=0;i<itime;i++)
      for(j=0;j<4000;j++);
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
  
}


void setup_buzzer(){
  
  PWMPRCLK=0x04; //ClockA=Fbus/2**4=24MHz/16=1.5MHz	
	PWMSCLA=10; 	 //ClockSA=1.5MHz/2x125=6000 Hz
	PWMCLK=0b00100000; 	 //ClockSA for chan 5
	PWMPOL=0x20; 		     //high then low for polarity
	PWMCAE=0x0; 		     //left aligned
	PWMCTL=0x0;	         //8-bit chan, pwm during freeze and wait
	PWMPER5=100; 	 //PWM_Freq=ClockSA/100=6000Hz/100=60Hz. CHANGE THIS
	PWMDTY5=50; 	 //50% duty cycle                        AND THIS TO GET DIFFERENT SOUND
	PWMCNT5=0;		 //clear initial counter. This is optional
	PWME=0x20; 	   //Enable chan 5 PWM 
  
}
void change_buzzer(int dout){
 
     if(dout > 138){
      PWMSCLA=10; 
      PWME=0x20; 	   //Enable chan 5 PWM
    }else if(dout > 102){
      PWMSCLA=20; 
      PWME=0x20; 	   //Enable chan 5 PWM
    }else if(dout > 79){
      PWMSCLA=30;
      PWME=0x20; 	   //Enable chan 5 PWM
    }else if(dout > 64){
      PWMSCLA=40;
      PWME=0x20; 	   //Enable chan 5 PWM    
    }else if(dout > 58){
      PWMSCLA=50;
      PWME=0x20; 	   //Enable chan 5 PWM
    }else if(dout > 46){
      PWMSCLA=60;
      PWME=0x20; 	   //Enable chan 5 PWM
    }else if(dout > 41){
      PWMSCLA=70;
      PWME=0x20; 	   //Enable chan 5 PWM
    }else if(dout > 38){
      PWMSCLA=80;
      PWME=0x20; 	   //Enable chan 5 PWM
    }else if(dout > 35){
      PWMSCLA=90;
      PWME=0x20; 	   //Enable chan 5 PWM
    }else if(dout > 30){
      PWMSCLA=100;
      PWME=0x20; 	   //Enable chan 5 PWM
    }else if(dout <= 30){
      PWME=0x00; 	   //Enable chan 5 PWM
    } 
  
}
void output_seven_seg(int dout){
    unsigned char output[10] = {0x40,0x79,0x24,0x30,0x19,0x12,0x02,0x78,0x00,0x10};

    if(dout > 138){
      PTH = output[0];
      COMWRT4(0x01);  //Clear display
      MSDelay(1);
	    DATWRT4('0');
      MSDelay(1); 
    }else if(dout > 102){
      PTH = output[1]; 
      COMWRT4(0x01);  //Clear display
      MSDelay(1);
	    DATWRT4('1');
      MSDelay(1); 
    }else if(dout > 79){
      PTH = output[2];
      COMWRT4(0x01);  //Clear display
      MSDelay(1);
	    DATWRT4('2');
      MSDelay(1); 
    }else if(dout > 64){
      PTH = output[3]; 
      COMWRT4(0x01);  //Clear display
      MSDelay(1);
	    DATWRT4('3');
      MSDelay(1); 
    }else if(dout > 58){
      PTH = output[4]; 
      COMWRT4(0x01);  //Clear display
      MSDelay(1);
	    DATWRT4('4');
      MSDelay(1); 
    }else if(dout > 46){
      PTH = output[5]; 
      COMWRT4(0x01);  //Clear display
      MSDelay(1);
	    DATWRT4('5');
      MSDelay(1); 
    }else if(dout > 41){
      PTH = output[6]; 
      COMWRT4(0x01);  //Clear display
      MSDelay(1);
	    DATWRT4('6');
      MSDelay(1); 
    }else if(dout > 38){
      PTH = output[7]; 
      COMWRT4(0x01);  //Clear display
      MSDelay(1);
	    DATWRT4('7');
      MSDelay(1); 
    }else if(dout > 35){
      PTH = output[8]; 
      COMWRT4(0x01);  //Clear display
      MSDelay(1);
	    DATWRT4('8');
      MSDelay(1); 
    }else if(dout > 30){
      PTH = output[9]; 
      COMWRT4(0x01);  //Clear display
      MSDelay(1);
	    DATWRT4('9');
      MSDelay(1); 
    }else if(dout <= 30){
      PTH = 0xFF;
      COMWRT4(0x01);  //Clear display
      MSDelay(1);
    }              
}

void proximity_setup(){
/*********************************************
 *  Setup ATD0 with proximity sensor
 *
 *
 *********************************************/
  ATD0CTL2 = 0x80;
  ATD0CTL3 = 0x48;  
  ATD0CTL4 = 0x85;   
  
}

int proximity_read(){
/*********************************************************
 *
 *   Read a digital value from the front proximity sensor
 *
 *   Returns average of front 8 conversions from ATD0
 *
 ********************************************************/
    int dout;
    ATD0CTL5 = 0b00100100;
	  while(!(ATD0STAT0 & 0x80)){
	  
      dout = ((int)ATD0DR0H + (int)ATD0DR1H + (int)ATD0DR2H +
              (int)ATD0DR3H + (int)ATD0DR4H + (int)ATD0DR5H +
              (int)ATD0DR6H + (int)ATD0DR7H) / 8;
 	  }
    return dout;
}


void main(void) {
  int dout = 0;
 
  proximity_setup();   //Setup ATD0CTL
  DDRH = 0xFF;         //Setup for 7-seg

  setup_buzzer();
  setup_lcd();
	
	while(1){
	  dout = proximity_read();
	  output_seven_seg(dout);
	  change_buzzer(dout);
	}    
}





