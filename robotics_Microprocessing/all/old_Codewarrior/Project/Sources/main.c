#include <hidef.h>      /* common defines and macros */
#include "derivative.h"      /* derivative-specific definitions */


#define LCD_DATA PORTK
#define LCD_CTRL PORTK
#define RS 0x01
#define EN 0x02

void COMWRT4(unsigned char);
void DATWRT4(unsigned char);
void USecDelay(unsigned int);

void main(void) {

// LCD Initial SETUP
  DDRK = 0xFF;
  COMWRT4(0x33);
  USecDelay(1);
  COMWRT4(0x32);
  USecDelay(1);
  COMWRT4(0x28);
  
// LCD additional setup  
  USecDelay(1);
  COMWRT4(0x06);
  USecDelay(1);
  COMWRT4(0x0E);
  USecDelay(1);
  COMWRT4(0x01);
  USecDelay(1);
  COMWRT4(0x80);
  USecDelay(1);
  
// LCD output
  DATWRT4('G'); 
  USecDelay(1);
  DATWRT4('o'); 
  USecDelay(1);
  DATWRT4('l'); 
  USecDelay(1);
  DATWRT4('t'); 
  USecDelay(1); 
  DATWRT4('z'); 
  
// 7 Segment Display counter 
  unsigned char output[] = {0x40,0x79,0x24,0x30,0x19,0x12,0x02,0x78,0x00,0x10};
  unsigned char index = 0;
  
  DDRH = 255;
  
  while (1) {
      
    if (index<10) {
      
      PTH = output[index];
      index++;
      wait();
      
    } else {
      index = 0;
    }
  }; // end of while
  
  for (;;);
}

void wait() {
  int i, j;
  for (i=0; i<400; i++) {
    
    for (j=0; j<1700; j++) {
    }
  }
} // end of wait

void COMWRT4(unsigned char command){
  unsigned char x;
  
  x = (command & 0xF0)>>2; 
  LCD_DATA = LCD_DATA & ~0x3C;
  LCD_DATA = LCD_DATA | x;
  USecDelay(1); 
  LCD_CTRL = LCD_CTRL & ~RS;
  USecDelay(1); 
  LCD_CTRL = LCD_CTRL | EN;
  USecDelay(1); 
  LCD_CTRL = LCD_CTRL & ~EN;
  USecDelay(1); 
  
  x = (command & 0x0F)<<2;  
  LCD_DATA = LCD_DATA & ~0x3C;
  LCD_DATA = LCD_DATA | x;
  LCD_CTRL = LCD_CTRL | EN;
  USecDelay(1); 
  LCD_CTRL = LCD_CTRL & ~EN;
  USecDelay(1); 
  
}

void DATWRT4(unsigned char data) {
  
  unsigned char x;
  
  x = (command & 0xF0)>>2; 
  LCD_DATA = LCD_DATA & ~0x3C;
  LCD_DATA = LCD_DATA | x;
  USecDelay(1); 
  LCD_CTRL = LCD_CTRL | RS;
  USecDelay(1); 
  LCD_CTRL = LCD_CTRL | EN;
  USecDelay(1); 
  LCD_CTRL = LCD_CTRL & ~EN;
  USecDelay(1);   
  
  x = (command & 0x0F)<<2;  
  LCD_DATA = LCD_DATA & ~0x3C;
  LCD_DATA = LCD_DATA | x;
  LCD_CTRL = LCD_CTRL | EN;
  USecDelay(1); 
  LCD_CTRL = LCD_CTRL & ~EN;
  USecDelay(1);
    
}

void USecDelay(unsigned int itime){
  
  unsigned int i,j; 
  
  for(i=0;i<itime;i++)
    for (j=0;j<4000;j++);
  
}
