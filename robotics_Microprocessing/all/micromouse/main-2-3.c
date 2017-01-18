#include <hidef.h>      // common defines and macros 
#include "derivative.h"   //    derivative-specific definitions
#ifndef FLOODFILL_H
#define FLOODFILL_H
#include "floodfill.h" 
#endif
#ifndef NUMSTACK_H
#define NUMSTACK_H
#include "numstack.h"
#endif

void InitializeATD(void);
void SetupMDCU(void);
void SetupRTI(void);
void SetupDDR_Ports(void);
void NEXT_STATE(void);
void SWITCH_DIRECTION(void);

void HALF_STEP_FORWARD(int steps);    // move forward x number of steps
void RIGHT_STEP_FORWARD(void);
void LEFT_STEP_FORWARD(void);


void STATIONARY_TURN(int steps, int direction); // stationary turn calls subfunctions
void RAMP_BOTH_DOWN(void);
void RAMP_BOTH_UP_REV(int steps, int direction);  // stationary turn
void HALF_STEP_REV(int steps, int direction);  // stationary turn
void RAMP_BOTH_DOWN_REV(int steps, int direction);  // stationary turn
void RAMP_BOTH_UP(void);
void ROLLING_TURN(int steps, int direction);  

//Maze functions
void UPDATE_WALL_ARRAY(void);

/*
void PIVOT_TURN(int steps, int direction);  // pivot turn calls subfunctions
void RAMP_ONE_UP(int steps, int direction);  // pivot turn
void RAMP_ONE_DOWN(int steps, int direction);  // pivot turn
*/

volatile int Output_right[2] = { 0 };   //last two distance readings
volatile int Output_left[2] = { 0 };   //last two distance readings
volatile unsigned char OutputIndex = 0;	//points to the most recent distance Output location

//motor wheel half steps
volatile unsigned char RightStepArray[] ={0xC0, 0x80, 0x90, 0x10, 0x30, 0x20, 0x60, 0x40};
volatile unsigned char LeftStepArray[] ={0x0C, 0x04, 0x06, 0x02, 0x03, 0x01, 0x09, 0x08};
unsigned char SEG7_DISPLAY[10] = {0x40,0x79,0x24,0x30,0x19,0x12,0x02,0x78,0x00,0x10};

// turn control
volatile int G_Block_Step = 490; // steps for length of a block
volatile int G_90_Turn = 208; // steps for 90 degree turn    208
volatile int G_Current_Orient = 0; //0=N, 1=E, 2=S, 3=W
volatile int G_Turn = 0;  //0=Forward, 1=Right, 2=U-Turn,  3=Left
volatile int Temp_New_Direction = 0;
volatile int G_Steps = 0;
volatile int state = 0;
volatile int direction = 0;	// 0 = left, 1 = right
//volatile int G_Center=0;

// motor timing global variable
volatile long a[2] =  {-25575, 10508};
volatile long b[3] =  {329, 658, 329};
volatile short kp = 2;
volatile short kd = 1;
volatile int error_r = 0;		// represents our distance from desired location
volatile int errorDot_r = 0;	// represents our change in distance from last reading
volatile int error_l = 0;		// represents our distance from desired location
volatile int errorDot_l = 0;	// represents our change in distance from last reading
volatile int diff = 0;
volatile int interruptcount_filter = 0;
volatile int interruptcount_left = 0;
volatile int interruptcount_right = 0;
volatile int stepindex_left = 0;
volatile int stepindex_right = 0;
volatile int speed_left = 27;
volatile int speed_right = 27;
volatile int top_speed = 27;
volatile int current_speed = 130;
volatile int bottom_speed = 130;
volatile int speed_counter = 0;
volatile int count_both = 0;
volatile int count_one = 0;


// ATD conversion
volatile long x, rx, lx;
volatile int y, ry, ly; 
volatile long ylng, rylng, lylng;
volatile int d[3] = {32};
volatile int rd[3] ={32};
volatile int ld[3] ={32};
volatile long dlng, rdlng, ldlng;

// maze solving
//volatile int visited[3][3] = 
volatile int Current_x = 0;
volatile int Current_y = 0;
volatile int G_Finished = 1;
//volatile int visited[][] = {{0}};

// start of main
void main(void){
  

	SYNR = 0x02;
	REFDV = 0x00;		//this code should change the clock to 24MHz
	while (!(CRGFLG & 0x08))
	{ }
	CLKSEL = 0x80;
	 
	InitializeATD();
	//set up MDCU for 0.1ms = 0.0001 seconds interrupt
	//SetupMDCU();
	//set up MDCU for 0.1ms = 0.0001 seconds interrupt
	MCCTL = 0xCF;  //interrupt enable, load latest mod reg, mcen, pre=16
	MCCNT = 150;   //(0.0001-sec)(24000000-CLKperSEC)(16-CLKperCOUNT) = COUNT
	MCCTL = 0xCF;
	SetupDDR_Ports();
	
	initMaze();
	
	EnableInterrupts;
	
	for(;;) {
	  
		ATD0CTL5 = 0x33;
		  
		while (!(ATD0STAT0 & 0x80));
	
		
/* * * * * * * * * * * * * * * * * * *
    START
    4/2/2014
    Merging Drive and wall array and floodfill

 * * * * * * * * * * * * * * * * * * */


switch (state){
		    case 0:   SWITCH_DIRECTION();
                          PTH = SEG7_DISPLAY[G_Current_Orient];  
                          RAMP_BOTH_UP();
                          G_Steps=405;         // 450
                          state=1;
                          break;
			case 1 : G_Turn = explore(Current_x, Current_y, 4,4, G_Current_Orient, y, Output_left[OutputIndex], Output_right[OutputIndex]); // 4 finished, 0 -N, 1 -E, 2 -S, 3 -W, -1 -error
			              if (G_Turn == 4) state = 5;
			              else if (G_Turn == -1){
			                PTH = 0x3F;
			                state = 5;
			              }
			              else SWITCH_DIRECTION();
			              state = 2;
			              break;
			case 2:  switch (G_Current_Orient){
                          case 0: G_Steps = G_Block_Step; state=3; break;                    // go forward
                          case 1: direction = 1; G_Steps = G_90_Turn; state=4; break;  // turn right
                          case 2: G_Steps = G_90_Turn*2; state=4; break;                    // u-turn
                          case 3: direction = 0; G_Steps = G_90_Turn; state=4; break; // turn left
			}break;                        
            case 3:   HALF_STEP_FORWARD(G_Steps);
                          state = 1;
                          break;
			case 4:  STATIONARY_TURN(G_Steps, direction);	// 90-TURN - ramp up
                          state = 0;
                          break;
			case 5:  break;
		
}

/* * * * * * * * * * * * * * * * * * *
    END
    4/2/2014
    Merging Drive and wall array and floodfill

 * * * * * * * * * * * * * * * * * * *
  if (!G_Finished) {
    
		switch (state){
		    case 0:{
			            SWITCH_DIRECTION();
			            PTH = SEG7_DISPLAY[G_Current_Orient];  
						RAMP_BOTH_UP();
						G_Steps=405;         // 450
						//state=3;
						state=1;
			}break;
			case 1 :{    
                        
						Temp_New_Direction = HALF_STEP_FORWARD(G_Steps);     // returns next turn
						//state=3;
                        switch (Temp_New_Direction){
                            case 0: G_Steps = G_Block_Step; state=1; break;                    // go forward
                            case 1: direction = 1; G_Steps = G_90_Turn; state=2; break;  // turn right
                            case 2: G_Steps = G_90_Turn*2; state=2; break;                    // u-turn
                            case 3: direction = 0; G_Steps = G_90_Turn; state=2; break; // turn left
                        } 
                    } break;                            
			case 2 :
						STATIONARY_TURN(G_Steps, direction);	// 90-TURN - ramp up
                        state = 0;
                        break;
            case 4 : break;
          
			case 5 :	PTH = 0x19;  // 4
						PIVOT_TURN(350, direction);
						NEXT_STATE();
						break;	
             
		}
		
  }else {
        switch (state){
		    case 0:{
			            SWITCH_DIRECTION();
			            PTH = SEG7_DISPLAY[G_Current_Orient];  
						RAMP_BOTH_UP();
						G_Steps=205;         // 205
						//state=3;
						state=1;
			}break;
			case 1 :{    
                        
						Temp_New_Direction = HALF_STEP_FORWARD(G_Steps);     // returns next turn
						//state=4;
                        switch (Temp_New_Direction){
                            case 0: G_Steps = G_Block_Step; state=1; break;                    // go forward
                            case 1: direction = 1; G_Steps = G_90_Turn; state=3; break;  // turn right
                            case 2: G_Steps = G_90_Turn*2; state=2; break;                    // u-turn
                            case 3: direction = 0; G_Steps = G_90_Turn; state=3; break; // turn left
                        } 
                    } break;                            
			case 2 :
						STATIONARY_TURN(G_Steps, direction);	// 90-TURN - ramp up
                        state = 0;
                        break;
        	case 3 :
						ROLLING_TURN(580, direction);
						G_Steps = 50;
						state = 1;
						break;	//ROLLING_TURN (set both speeds 1:3 ratio)
            case 4 : break;
    
        }
  }      */
	}
}

//interrupt triggers every 0.1ms
#pragma TRAP_PROC
#pragma CODE_SEG __SHORT_SEG NON_BANKED
interrupt VectorNumber_Vtimmdcu void mdcuInterrupt () 
{

	MCFLG |= 0x80;
	
	interruptcount_filter++;
	interruptcount_left++;
	interruptcount_right++;
  

	//   fixed point math butterworth filter
	//   read sensors every millisecond = 1ms = 0.001sec
	if(interruptcount_filter == 10 ){ //&& ((state == 0) || (state == 1) || (state == 2))
    
		interruptcount_filter = 0;
	   
		// right sensor
		rx = ATD0DR2H;
		rx <<= 14;
		  
		rdlng = rx - a[0] * rd[1] - a[1] * rd[2];
		rdlng <<= 2;
		rdlng >>= 16;
		rd[0] = rdlng;
		rylng = b[0] * rd[0] + b[1] * rd[1] + b[2] * rd[2];
		rylng <<= 2;
		rylng >>= 16;
		
		OutputIndex++;
		if (OutputIndex == 2)
			OutputIndex = 0;
			
		Output_right[OutputIndex] = rylng;
		rd[2] = rd[1];
		rd[1] = rd[0];
		
		// front sensor
		x = ATD0DR1H;
		x <<= 14;
		  
		dlng = x - a[0] * d[1] - a[1] * d[2];
		dlng <<= 2;
		dlng >>= 16;
		d[0] = dlng;
		ylng = b[0] * d[0] + b[1] * d[1] + b[2] * d[2];
		ylng <<= 2;
		ylng >>= 16;
		y = ylng;   
		d[2] = d[1];     
		d[1] = d[0];
		
		// left sensor
		lx = ATD0DR0H;
		lx <<= 14;
		  
		ldlng = lx - a[0] * ld[1] - a[1] * ld[2];
		ldlng <<= 2;
		ldlng >>= 16;
		ld[0] = ldlng;
		lylng = b[0] * ld[0] + b[1] * ld[1] + b[2] * ld[2];
		lylng <<= 2;
		lylng >>= 16;
			
		Output_left[OutputIndex] = lylng;
		ld[2] = ld[1];
		ld[1] = ld[0];
   
	}//  end of sensor reading
 	
}//  end of MDCU

void HALF_STEP_FORWARD(int steps){
 
	while (count_both <= steps){

		if ((state == 1 ) && (y > 55)){	// intermediate value theorem - Matt (haven't tested)
			count_both = steps + 1;
			//return (G_Turn = 2);
			//state = 8; // u turn
		} else {
			//PD CONTROL
			diff = ((Output_left[OutputIndex] > Output_right[OutputIndex]) ? (
			Output_left[OutputIndex] - Output_right[OutputIndex]) : (Output_right[OutputIndex] - Output_left[OutputIndex]));
            
            // left wall present correct right speed					
			if ((Output_left[OutputIndex] > Output_right[OutputIndex]) && (diff > 5)){
					
				//PTH = 0x79; // right one
					
				error_r = 72 - Output_left[OutputIndex];
				errorDot_r = Output_left[OutputIndex] - Output_left[(OutputIndex - 1 == -1 ? 1 : 0)];
			  
				speed_right = top_speed - (kp * error_r - kd * errorDot_r);
			
				if (speed_right < top_speed - 7)
					speed_right = top_speed - 7;
				
				// right speed time
                if ((interruptcount_right >= speed_right)){
                  RIGHT_STEP_FORWARD();
                  count_both++;
                }// end of right port writing    
        
                // left speed time
                if (interruptcount_left >= speed_left){
                  LEFT_STEP_FORWARD();
                  //count_both++;
                }// end of left port writing
               
               // right wall present correct left speed          		
			} else if ((Output_right[OutputIndex] > Output_left[OutputIndex]) && (diff > 5)) {
				
				//PTH = 0x4F; // left one	  
				error_l = 72 - Output_right[OutputIndex];
				errorDot_l = Output_right[OutputIndex] - Output_right[(OutputIndex - 1 == -1 ? 1 : 0)];
			  
				speed_left = top_speed - (kp * error_l - kd * errorDot_l);
			
				if (speed_left < top_speed - 7)
					speed_left = top_speed - 7;
				
				// right speed time
                if ((interruptcount_right >= speed_right)){
                  RIGHT_STEP_FORWARD();
                  //count_both++;
                }// end of right port writing    
        
                // left speed time
                if (interruptcount_left >= speed_left){
                  LEFT_STEP_FORWARD();
                  count_both++;
                }// end of left port writing
					  
			} else if (diff <= 5) {   // go straight
				// go straight
				//PTH = 0x49; // both one	
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
				
			}// end go straight 
			
		}// end of PD controller             
	
	}// end of while
	count_both = 0;
	  //state = 0;

 
 
 /*
// -1=Left, 0=Forward, 1=Right, 2=U-Turn
if  (Output_left[OutputIndex] < 60) { // no left
   
   G_Turn = 3;
    // call Change_G_Direction
} else if (Output_right[OutputIndex] < 60) { // no right
    G_Turn = 1;
} else if (y > 55) { // dead end
    G_Turn = 2;
} else  G_Turn = 0; // keep going straight

   */
    

   // return G_Turn;

} // end of half step forward



void SWITCH_DIRECTION(void){
  switch (G_Current_Orient){
    case 0: G_Current_Orient = G_Turn;
               break;
    case 1: { switch (G_Turn) { // East to
                    case 0: G_Current_Orient=1;  break; // straight - East
                    case 1: G_Current_Orient=2;  break; // South
                    case 2: G_Current_Orient=3;  break; // West
                    case 3: G_Current_Orient=0;  break; // North
                    }
    }break;
  
    case 2:  { switch (G_Turn) { // South to
                    case 0: G_Current_Orient=2;  break; // straight - south
                    case 1: G_Current_Orient=3;  break; // West
                    case 2: G_Current_Orient=0;  break; // North
                    case 3: G_Current_Orient=1;  break; // East
                    } 
    }break;
               
    case 3:  { switch (G_Turn) { // South to
                    case 0: G_Current_Orient=3;  break; // straight - West
                    case 1: G_Current_Orient=0;  break; // North
                    case 2: G_Current_Orient=1;  break; // East
                    case 3: G_Current_Orient=2;  break; // South
                  }
    }break;
  }
}

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

void STATIONARY_TURN(int steps, int direction){	
	
	// slow down robot to a stop
	RAMP_BOTH_DOWN();
	
	// rotate in place
	RAMP_BOTH_UP_REV(steps/4, direction);
	HALF_STEP_REV(steps/2, direction); 
	RAMP_BOTH_DOWN_REV(steps/4, direction);
	
	// accelerate to top speed
	// RAMP_BOTH_UP();		
 
} // end of u-turn

void RAMP_BOTH_UP_REV(int steps, int direction){

	while (count_both <= steps){
		
		if (current_speed >= top_speed) {
			if ((interruptcount_right >= current_speed) && (interruptcount_left >= current_speed)){
			  
				PORTB =  (LeftStepArray[stepindex_left] | RightStepArray[stepindex_right]);
				
				if (direction){	// if direction is right
					stepindex_right--;
					stepindex_left++;
				} else {		// if direction is left
					stepindex_right++;
					stepindex_left--;
				}
				  
				// reset step index
				if (stepindex_right > 7)
					stepindex_right = 0;
				if (stepindex_right < 0)
					stepindex_right = 7;
				// reset step index
				if (stepindex_left > 7)
					stepindex_left = 0;
				if (stepindex_left < 0)
					stepindex_left = 7;
					  
				// reset left wheel interrupt count
				interruptcount_left = 0;
				// reset right wheel interrupt count
				interruptcount_right = 0;
			 
				if (current_speed > top_speed)
					current_speed--;
				
				count_both++;
			}
		}
		   
	}	// end while
		
	current_speed = top_speed;
	count_both = 0;

}// end of RAMP_BOTH_UP_REV

void HALF_STEP_REV(int steps, int direction){	// used for stationary turning
     
	while (count_both <= steps){
		if ((interruptcount_right >= top_speed) && (interruptcount_left >= top_speed)){
			  
			PORTB = (RightStepArray[stepindex_right] | LeftStepArray[stepindex_left]);
			
			if (direction){ // direction is right
				stepindex_right--;
				stepindex_left++; 
			} else {
				stepindex_right++;
				stepindex_left--;
			}
			  
			// reset step index
			if (stepindex_right > 7)
				stepindex_right = 0;
			if (stepindex_right < 0)
				stepindex_right = 7;
			// reset step index
			if (stepindex_left > 7)
				stepindex_left = 0;
			if (stepindex_left < 0)
				stepindex_left = 7;
				  
			//reset left wheel interrupt count
			interruptcount_left = 0;
			// reset right wheel interrupt count
			interruptcount_right = 0;
			  
			count_both++;
		}  
	}
	count_both = 0;
}// end of HALF_STEP_REV

void RAMP_BOTH_DOWN_REV(int steps, int direction){

	while (count_both <= steps){
		
		if (current_speed <= bottom_speed) {
			if ((interruptcount_right >= current_speed) && (interruptcount_left >= current_speed)){
			  
				PORTB = (LeftStepArray[stepindex_left] | RightStepArray[stepindex_right]);
				
				if (direction){	// if direction is right
					stepindex_right--;
					stepindex_left++;
				} else {		// if direction is left
					stepindex_right++;
					stepindex_left--;
				}
				  
				// reset step index
				if (stepindex_right > 7)
					stepindex_right = 0;
				if (stepindex_right < 0)
				  stepindex_right = 7;
				// reset step index
				if (stepindex_left > 7)
					stepindex_left = 0;
				if (stepindex_left < 0)
					stepindex_left = 7;
					  
				// reset left wheel interrupt count
				interruptcount_left = 0;
				// reset right wheel interrupt count
				interruptcount_right = 0;
			 
				if (current_speed < bottom_speed)
					current_speed++;
			  
				count_both++;
			}
		}

	}// end of RAMP_BOTH_DOWN_REV
		
		current_speed = bottom_speed;
		count_both = 0;

} // end of 


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
		 
			current_speed--;
		}
    
	}
	
	current_speed = top_speed;
	
	
}	// end RAMP_BOTH_UP

void RAMP_BOTH_DOWN(void){
 
    while (current_speed <= bottom_speed){
	
		if ((interruptcount_right >= current_speed) && (interruptcount_left >= current_speed)){
		 
			//PTH = 0x24;   //2
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
			  
			current_speed++;
		}
    
	}
  
	current_speed = bottom_speed;
	//state = 0;
  
}
/*
void RAMP_ONE_DOWN(int steps, int direction){	// might want to control the amount of steps it takes
	
  while (count_one <= steps){
	
		  
		if (direction){	// if direction = 1 (turning right)
		  if (current_speed < bottom_speed){
			if (interruptcount_right >= current_speed){	// slow down right wheel (turning right)
			 
                RIGHT_STEP_FORWARD();
				current_speed = current_speed + 2;
				
			}
		  }
			if (interruptcount_left >= top_speed){	// constant speed
				
                LEFT_STEP_FORWARD();
				count_one++;
			}		  
		} else {	// if direction is false
		  if (current_speed < bottom_speed){
			if (interruptcount_left >= current_speed){	// slow down left wheel (turning left)
				
                LEFT_STEP_FORWARD();
				current_speed = current_speed + 2;
			
			}
		  }
			if (interruptcount_right >= top_speed){	// constant speed
			 
                RIGHT_STEP_FORWARD();
				count_one++;
			}
		}
  }

	count_one = 0;
	current_speed = bottom_speed;
	
}

void RAMP_ONE_UP(int steps, int direction){	// might want to control the amount of steps it takes
	
	while (count_one <= steps){
		  
		if (direction){	// if direction = 1 (turning right)
		  if (current_speed > top_speed){
			if (interruptcount_right >= current_speed){	// accelerate right wheel (turning right)
			 
        RIGHT_STEP_FORWARD();
				current_speed = current_speed - 2;
				
			}
		  } else {
		    RIGHT_STEP_FORWARD();
		  }
			if (interruptcount_left >= top_speed){	// constant speed
				
                LEFT_STEP_FORWARD();
				count_one++;
			}		  
		} else {	// if direction is false
		  if (current_speed > top_speed){
			if (interruptcount_left >= current_speed){	// accelerate left wheel (turning left)
				
                LEFT_STEP_FORWARD();
				current_speed = current_speed - 2;
			
			}
		  } else {
		    LEFT_STEP_FORWARD();
		  }
			if (interruptcount_right >= top_speed){	// constant speed
			 
                RIGHT_STEP_FORWARD();
				count_one++;
			}
		}
  }

	count_one = 0;
	current_speed = top_speed;
}

void PIVOT_TURN(int steps, int direction){

	// slow down one wheel
	RAMP_ONE_DOWN(steps, direction);
	
	// rev up from stop the one wheel
	RAMP_ONE_UP(steps/2, direction);
	
}
  */
void ROLLING_TURN(int steps, int direction){

count_one = 0;
while (count_one <= steps) { // change steps to discrete value
  
	// set speeds
	speed_left = (direction) ? top_speed : (3*top_speed);
	speed_right = (direction) ? (3*top_speed) : top_speed;
	
	if (direction){
	  
  	if (interruptcount_right >= speed_right)
  	  RIGHT_STEP_FORWARD();
  	if (interruptcount_left >= speed_left){
  	  LEFT_STEP_FORWARD();
  	  count_one++;
  	}
	} else {
	  if (interruptcount_right >= speed_right){
  	  RIGHT_STEP_FORWARD();
  	  count_one++;
	  }
  	if (interruptcount_left >= speed_left)
  	  LEFT_STEP_FORWARD();
	}	
}
  count_one = 0;
	speed_left = speed_right = top_speed;
	
}

void NEXT_STATE(void){
  
  switch (state){
  	case 0 : state = 1; break;
  	case 1 : state = 2; break;
  	case 2 : state = 3; break;
  	case 3 : state = 4; break;
  	case 4 : state = 5; break;
  	case 5 : state = 6; break;
  	case 6 : state = 7; break;
    case 7 : state = 8; break;
  	case 8 : state = 1; break;

  } 
  
}

void InitializeATD(void)
{
	ATD0CTL2 = 0x80; // power on ATD
	ATD0CTL3 = 0x18; // two conversions = 0x10 three conversions = 0x18
	ATD0CTL4 = 0x85; // was 0x84
}

void SetupMDCU(void){
    //set up MDCU for 0.1ms = 0.0001 seconds interrupt
	MCCTL = 0xCF;  //interrupt enable, load latest mod reg, mcen, pre=16
	MCCNT = 150;   //(0.0001-sec)(24000000-CLKperSEC)(16-CLKperCOUNT) = COUNT
	MCCTL = 0xCF;  
}

void SetupRTI(void){
 
	RTICTL = 0b01101011;	//sets prescaler of 82^10 resulting in interrupt every 49.15ms
	CRGINT = 0x80;			//tells that want interrupt given, not flag raised 
	
}

void SetupDDR_Ports(void){
  
	// set up 7-segment display  
	DDRH = 0xFF;
	// set up motor ports
	DDRB = 0xFF; //     PORTB0-PORTB3  as output 
	DDRP = 0x0F; //     PORTP0 and PORTP1 as output for 12EN=1 and 34EN=1
  
	PORTB=0b00000000;//  start with all off
	PTP=0b00001111; //  Turn on both 12EN and 34EN Enables for 754410 chip 
  
}