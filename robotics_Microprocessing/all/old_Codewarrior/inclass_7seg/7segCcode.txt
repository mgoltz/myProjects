/*
*   Michael Goltz
*   September 9, 2013
*   Microprocessor Interfacing - CIS 4930
*   Homework#2
*
*   7-segment repetitive counter 0-9 with one second interval
*
 */

#include <hidef.h>      /* common defines and macros */
#include "derivative.h"      /* derivative-specific definitions */


void wait() {
  int i, j;
  for (i=0; i<400; i++) {
    
    for (j=0; j<1700; j++) {
    }
  }
} // end of wait

void main(void) {
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
}
