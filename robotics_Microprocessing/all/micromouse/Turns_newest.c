#include <hidef.h>      /* common defines and macros */
#include "derivative.h"      /* derivative-specific definitions */
//#include "Turns.h" //includes right and left rolling/stationary/pivot 90deg and Uturn

#define MAX_SPEED 12
#define CRUISE_SPEED 20
#define MIN_SPEED 60
#define STOP_SPEED 65
#define LEFT_WALL 1
#define RIGHT_WALL 2
#define NO_WALL 0


int a1[3] = {0x4000, 0x63E7, 0x290B};
int b1[3] = {0x0524, 0x0A48, 0x0524};
unsigned long int dR[3] = {0};
unsigned long int dL[3] = {0};
unsigned long int dF[3] = {0};
int yL, yR, yF;

//unsigned char leftStepArray[] = {0x40, 0x60, 0x20, 0x30, 0x10, 0x90, 0x80, 0xC0};
//unsigned char rightStepArray[] = {0x04, 0x06, 0x02, 0x03, 0x01, 0x09, 0x08, 0x0C};
unsigned char leftStepArray[] = {0x0C, 0x04, 0x06, 0x02, 0x03, 0x01, 0x09, 0x08};
unsigned char rightStepArray[] = {0xC0, 0x80, 0x90, 0x10, 0x30, 0x20, 0x60, 0x40};


int leftStepIndex = -1;
int rightStepIndex = -1;
int interruptCount = 0;
int rightStepCount = 0;
int leftStepCount = 0;
int avgStepCount = 0;

int count = 0;
int i = -1;
int x=100;
int state=0;
int hold=0;
int stop =0;
int turnDistance [] = {1068, 935, 668, 852}; //40cm, 35cm, 25cm, 30cm
int sensorDistance [] ={60, 50, 30, 80};
int z = 0;
  

void main(void) {
/* main block
*	
* Currently sets up MDCU, ADC, system clock, and port B DDR
*
* More things need to be moved into main as we progress. 
*/
	DDRB = 0xFF;

	//System Clock set to 24MHz
	SYNR = 2;
	REFDV = 0;
	while (!(CRGFLG&0x08)){};
	CLKSEL |= 0x80;

	//Turn on ATD and configure
	ATD0CTL2 = 0x80; //Turns on ATD converter
	ATD0CTL3 = 0x18; //Length=3 FIFO=0
	ATD0CTL4 |= 0x80; //8-bit resolution

	// set up MDCU and configure for .1ms interrupts
	MCCTL = 0xCC;   //Prescaler = 0;
	MCCNT = 2399; //.1 ms interrupt
	MCCTL = 0xCC;
	
	ATD0CTL5 = 0xB3; //Right Justified, Unsigned, SCAN=1, MULTI=1, START=3

	EnableInterrupts;

  for(;;) {
    _FEED_COP(); /* feeds the dog */
  } 
  
  while(1); //Just in case _FEED_COP() exits
}

int butterworth(unsigned long int input, unsigned long int d[3], int a[3], int b[3]){
/*	Butterworth filter using fixed point math. 
*	Note, that A[1] should be negated in the input array
*	
*	returns filtered value
*/	

	unsigned long int fact1_d, fact2_d, fact1_y, fact2_y, fact3_y , result_d, z;
	unsigned long int x = 0; //Q8.0

	//Calculating factors involved in computing d[0]
	fact1_d = a[1] * d[1]; //Q2.14 * Q12.4 = Q14.18		
	fact2_d = a[2] * d[2]; //Q2.14 * Q12.4 = Q14.18	
	
	//Left shift input 18 to get Q8.18 for subtraction / addition
	x = input << 18;//Q8.0 --> Q8.18		
	//		  8.18 - 14.18 - 14.18
	
	//Compute D - Note that a[1] has been converted to positive in the a[1] constant
	result_d = (x + fact1_d - fact2_d); //Q14.18
	
	//avoid sign errors 
	if (result_d > 0x80000000){
	    result_d = 0;   
	}
	
	//convert back to Q12.4
	d[0]= (result_d<<2)>>16; //Q12.4
	
	//Calculate factors involved in computing y
	fact1_y = b[0] * d[0];//Q0.16 *  12.4 == 12.20	//d[0] = 12.20
	fact2_y = b[1] * d[1];//Q0.16 *  12.4 			//d[1] = 12.20
	fact3_y = b[2] * d[2];//Q0.16 *  12.4 			//d[2] = 12.20
	
	//Compute y
	z = fact1_y + fact2_y + fact3_y; // Q12.20

	//get just the integer portion of Q12.20
	z = z >> 20;

	//shift d
	d[2]=d[1];
	d[1]=d[0];

   	return z;
}

int UTurn(){
/*UTurn function
*	returns 1 if UTurn is completed. 0 if UTurn still in progress.
*
*/
    static int counter=0;
    static int steps = 0;

    counter++;

	//2ms per step for 420 steps
    if(counter%20==0 && steps<390){
        steps++;
        leftStepIndex=(leftStepIndex+1)%8;
        rightStepIndex= rightStepIndex-1;
        if(rightStepIndex<0) rightStepIndex=7;
    } 

    PORTB = (leftStepArray[leftStepIndex]|rightStepArray[rightStepIndex]);  
                        
    if(steps>=390){
        //state=0;
        counter=0;
        steps = 0;
        return 1;
    } else {
        return 0;   
    }
}

int StationaryRightTurn(){
/*Stationary Right Turn function
*	returns 1 if STurn is completed. 0 if STurn still in progress.
*
*/
    static int counter=0;
    static int steps = 0;

    counter++;

	//3ms per step for 210 steps
    if(counter%30==0 && steps<180){
        steps++;
        leftStepIndex=(leftStepIndex+1)%8;
        rightStepIndex= rightStepIndex-1;
        if(rightStepIndex<0) rightStepIndex=7;
    } 

    PORTB = (leftStepArray[leftStepIndex]|rightStepArray[rightStepIndex]);  
 
    if(steps>=180){
        //stop=0;
        counter=0;
        steps = 0;
        return 1;
    } else {
        return 0;   
    }
}

int StationaryLeftTurn(){
/*Stationary Left Turn function
*	returns 1 if STurn is completed. 0 if STurn still in progress.
*
*/
    static int counter=0;
    static int steps = 0;

    counter++;

	//2ms per step for 210 steps
    if(counter%30==0 && steps<180){
        steps++;
        rightStepIndex=(rightStepIndex+1)%8;
        leftStepIndex= leftStepIndex-1;
        if(leftStepIndex<0) leftStepIndex=7;
    } 

    PORTB = (leftStepArray[leftStepIndex]|rightStepArray[rightStepIndex]);  
 
    if(steps>=180){
        //state=0;
        counter=0;
        steps = 0;
        return 1;
    } else {
        return 0;   
    }
}

int PivotLeftTurn(){
/*Pivot LeftTurn function
*	returns 1 if STurn is completed. 0 if STurn still in progress.
*
*/
    static int counter=0;
    static int steps = 0;

    counter++;

	//2ms per step for 435 steps
    if(counter%30==0 && steps<420){
        steps++;
        rightStepIndex=(rightStepIndex+1)%8;
    } 

    PORTB = (rightStepArray[rightStepIndex]);  
 
    if(steps>=420){
        //state=0;
        counter=0;
        steps = 0;
        return 1;
    } else {
        return 0;   
    }
}

int PivotRightTurn(){
/*Pivot Right Turn function
*	returns 1 if STurn is completed. 0 if STurn still in progress.
*
*/
    static int counter=0;
    static int steps = 0;

    counter++;

	//2ms per step for 210 steps
    if(counter%30==0 && steps<420){//420
        steps++;
        leftStepIndex=(leftStepIndex+1)%8;
    } 

    PORTB = (leftStepArray[leftStepIndex]);  
 
    if(steps>=420){//420
        //state=0;
        counter=0;
        steps = 0;
        return 1;
    } else {
        return 0;   
    }
}

int RollingRightTurn(){
/*Rolling Right Turn function
*	returns 1 if STurn is completed. 0 if STurn still in progress.
*
*/
    static int counter=0;
    static int rightsteps = 0;
    static int leftsteps = 0;

    counter++;

	//2ms per step for 210 steps
    if(counter%30==0 && leftsteps<=619){
        leftsteps++;
        leftStepIndex=(leftStepIndex+1)%8;
    }
    
    if(counter%80==0 && rightsteps<=220){
      rightsteps++;
      rightStepIndex=(rightStepIndex+1)%8;
    }

    PORTB = (leftStepArray[leftStepIndex]|rightStepArray[rightStepIndex]);  
 
    if(leftsteps>619 && rightsteps>220){
        //state=0;
        counter=0;
        leftsteps = 0;
        rightsteps = 0;
        return 1;
    } else {
        return 0;   
    }
}



int RollingLeftTurn(){
/*Rolling Left Turn function
*	returns 1 if STurn is completed. 0 if STurn still in progress.
*
*/
    static int counter=0;
    static int rightsteps = 0;
    static int leftsteps = 0;

    counter++;

	//2ms per step for 210 steps
    if(counter%30==0 && rightsteps<=619){
        rightsteps++;
        rightStepIndex=(rightStepIndex+1)%8;
    }
    
    if(counter%80==0 && leftsteps<=220){
      leftsteps++;
      leftStepIndex=(leftStepIndex+1)%8;
    }

    PORTB = (leftStepArray[leftStepIndex]|rightStepArray[rightStepIndex]);  
 
    if(rightsteps>619 && leftsteps>220){
        //state=0;
        counter=0;
        leftsteps = 0;
        rightsteps = 0;
        return 1;
    } else {
        return 0;   
    }
}


                                   

#pragma TRAP_PROC
#pragma CODE_SEG __SHORT_SEG NON_BANKED
interrupt VectorNumber_Vtimmdcu void mdcuInterrupt () {
/* MDCU interrupt
*
* 	Sends steps to stepper motor / controls ramping and wall follow control / calls UTurn
*
*/	

  static int turn = 0;
	static int leftStepCount = 0;
	static int rightStepCount = 0;
	static int avgStepCount = 0;
	static int doneturning = 0; 
	
	//PD Controller Constants
	float P = .15; 
	float D = .3; 
	
	//Speed Control Variables
	//S*_G is the goal speed of the right/left wheel.
	static int SR_G = MAX_SPEED;
	static int SL_G = MAX_SPEED;
	static int SR = MIN_SPEED;
	static int SL = MIN_SPEED;
	
	//Ramping constant. 
	int Rc = 50;	//Controls number of interrupts needed to increase speed
	static int Rsc = 1;	//Variable controlling the speed increment for ramping
	static int R_cnt = 0;	//Ramp counts
	
	static int wall = 0; //0->no wall | 1-> left | 2->right
	static int read_count = 0;
	
	//Error variables
	static int delta_e = 0;
	static int prev_e =0;
	static int e =0;
	
	//misc
	static int hold;
 
	// Clearing the flag
	MCFLG |= 0x80;
 
	interruptCount++;
	
	//stop==0 -> stopped/uturn 
	//stop==1 -> stopping
	//stop==2 -> not stopping
  

if(stop==1 || stop==0){	//if not stopping

    //Read new proximity values every millisecond
    if(interruptCount%10 == 0){  
        
        hold = ATD0DR0L;
        yL = butterworth(hold, dL, a1, b1);

        hold = ATD0DR1L;
        yR = butterworth(hold, dR, a1, b1);
        
        hold = ATD0DR2L;
        yF = butterworth(hold, dF, a1, b1);   
    }  
    
    if(avgStepCount>=turnDistance[z] && stop!=2){	//stop!=2 is probably redundant here
        SR_G = STOP_SPEED;             //Read the sensors to make sure your not too close to a wall before turning?
        SL_G = STOP_SPEED;
        stop = 1;
        avgStepCount = 0;
        rightStepCount = 0;
        leftStepCount = 0;
        z = (z+1)%4; 
    }                      
   
	if(stop!=1){   //Dont run if ramping down
		
		//picking the wall
		e = 70-yR;
		wall = RIGHT_WALL;
		 
		if(yR < 50){  
			if(yL < 50){
				wall = NO_WALL;
				SR_G = CRUISE_SPEED;
				SL_G = CRUISE_SPEED;
				SR = CRUISE_SPEED;
				SL = CRUISE_SPEED;
			}else{
				e = 70-yL;
				wall = LEFT_WALL;
			}
		}

		
		delta_e = e-prev_e;

		//delta_e < 7 helps avoid problems when both walls disappear(but filter takes time to respond)
		//if we get a large spike in changein error, we ignore it. We shouldnt move towards the wall
		//at a rate that would be greater than this in .1ms
		
		if(wall && delta_e<7){	
			SR_G =CRUISE_SPEED;
			SL_G =CRUISE_SPEED;
			 
			
			if(wall==LEFT_WALL){   
				SR_G = CRUISE_SPEED - P*e - D*(e-prev_e);  	
			 }else if(wall==RIGHT_WALL){
				SR_G = CRUISE_SPEED + P*e + D*(e-prev_e);        
			}    
						  
			if(SR_G>MIN_SPEED)SR_G = MIN_SPEED;
			if(SL_G>MIN_SPEED)SL_G = MIN_SPEED;
			if(SR_G<MAX_SPEED)SR_G = MAX_SPEED;
			if(SL_G<MAX_SPEED)SL_G = MAX_SPEED; 
		  
			}
		}
   
		//Ramping control
		if(R_cnt++ == Rc){

			if(SR < SR_G)SR+=Rsc;
			else if(SR > SR_G)SR-=Rsc;
			
			if(SL < SL_G)SL+=Rsc;
			else if(SL > SL_G) SL-=Rsc;
			
			R_cnt = 0;     
		} 

		if(interruptCount%SR == 0){
		  rightStepCount++;
		  rightStepIndex = (rightStepIndex+1)%8;
		}
		if(interruptCount%SL == 0){
		  leftStepCount++;
		  leftStepIndex = (leftStepIndex+1)%8;
		}
		
		avgStepCount = (leftStepCount + rightStepCount)/2;
		  
		if(SR!=STOP_SPEED && SL!=STOP_SPEED) PORTB = (rightStepArray[rightStepIndex]|leftStepArray[leftStepIndex]);
		if (SR==STOP_SPEED && SL==STOP_SPEED) stop=2; turn = 1;
}else{
	if(turn){
	  if(state==0)doneturning = StationaryRightTurn();
    else if(state==1)doneturning = PivotRightTurn();
    else if(state==2)doneturning = RollingRightTurn();
    else if(state==3)doneturning = UTurn();
    else if(state==4)doneturning = StationaryLeftTurn();
    else if(state==5)doneturning = PivotLeftTurn();
    else if(state==6)doneturning = RollingLeftTurn();
    else if(state==7)doneturning = UTurn();
	}
	 if(doneturning){
	  doneturning = 0;
	  turn = 0;
	  state = (state+1)%8;
		SR_G = MAX_SPEED;
		SL_G = MAX_SPEED;
		SR = MIN_SPEED;
		SL = MIN_SPEED;
		//Flush front proximity filter to 0
		dF[0]=0;dF[1]=0;dF[2]=0;
		yF = 0;
		stop = 0;
		//flush filter to goal value.
		dR[0]=13500;dR[1]=13500;dR[2]=13500;
		dL[0]=13500;dL[1]=13500;dL[2]=13500;
		yR = 70;
		yL = 70;
		//Clear sensor values
	  leftStepCount = 0;
	  rightStepCount = 0;
	}
}


}