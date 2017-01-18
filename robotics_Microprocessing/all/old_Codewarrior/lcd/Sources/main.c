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
  
  DDRK = 0xFF;
  COMWRT4(0x33);
  USecDelay(1);
  COMWRT4(0x32);
  USecDelay(1);
  COMWRT4(0x28);
  
  USecDelay(1);
  COMWRT4(0x06);
  USecDelay(1);
  COMWRT4(0x0E);
  USecDelay(1);
  COMWRT4(0x01);
  USecDelay(1);
  COMWRT4(0x80);
  USecDelay(1);
  
  DATWRT4('G'); 
  USecDelay(1);
  DATWRT4('o'); 
  USecDelay(1);
  DATWRT4('l'); 
  USecDelay(1);
  DATWRT4('t'); 
  USecDelay(1); 
  DATWRT4('z'); 
  
  for (;;);
    
}

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