#include <hidef.h>      /* common defines and macros */
#include "derivative.h"      /* derivative-specific definitions */
#define LCD_DATA PORTK
#define LCD_CTRL PORTK
#define RS 0x01
#define EN 0x02

  /* put your own code here */
  /************************************************************************************************/
/*                                                                                        						 	*/
/*  CIS 4930 - Microprocessor Interfacing                   							*/
/*  LCD Code                                                                     							*/
/*                                                                                          							*/
/* Printed from:  http://www.microdigitaled.com/HCS12/Hardware/Dragon12-Plus-Support.htm, by way of 		*/
/* http://www.evbplus.com/hcs12_9s12_resources/hc11_68hc11_hc12_68hc12_9s12_hcs12_sites.html  		*/
/************************************************************************************************/



void COMWRT4(unsigned char);
void DATWRT4(unsigned char);
void MSDelay(unsigned int);
void main(void) 
{
	
	ATD0DIEN = 0x70;
	ATD0CTL2 = 0x80;
	ATD0CTL3 = 0x08;
	ATD0CTL4 = 0x85;
	ATD0CTL5 = 0x14;
	
	
	
	while(!(ATD0STAT0 && 0x80));
	
	

	
	for(;;);       //stay here 
}

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
	LCD_CTRL = LCD_CTRL | EN;          //raise enable
	MSDelay(5);
	LCD_CTRL = LCD_CTRL & ~EN;         //Drop enable to capture command
	MSDelay(15);                       //wait
	
	x = (command & 0x0F)<< 2;          // shift low nibble to center of byte for Pk5-Pk2
	LCD_DATA =LCD_DATA & ~0x3C;         //clear bits Pk5-Pk2
	LCD_DATA =LCD_DATA | x;             //send low nibble to PORTK
	LCD_CTRL = LCD_CTRL | EN;          //raise enable
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

 
