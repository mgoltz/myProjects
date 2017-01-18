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
                                                          //0       1       2        3        4     5       6       7       8       9
// turn control
volatile int G_Block_Step = 490; // steps for length of a block
volatile int G_90_Turn = 208; // steps for 90 degree turn    208
volatile int G_Current_Orient = 0; //0=N, 1=E, 2=S, 3=W
volatile int G_Turn = 0;  //0=N, 1=E, 2=S,  3=W
volatile int prev_explore = -1;
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

volatile int flag_this = 0;

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

/*numstack.c*/

void push(int x){
  if (root==NULL){
    root = (struct node *) malloc(sizeof(struct node));
    tail = root;
    root->x = x;
    root->next = NULL;
    root->prev = NULL;
  }else{
    struct node * temp = (struct node *) malloc(sizeof(struct node));
    tail->next = temp;
    temp->prev = tail;
    temp->x =x;
    temp->next = NULL;
    tail = temp;
  }
}

int pop(){
  if(root==NULL)return -1;
  else{
    struct node * temp = tail;
    int x = tail->x;
    tail = tail->prev;
    if(tail != NULL) tail->next = NULL;
    free(temp);
    return x;

  }
}

int peek(){
  if(root!=NULL)return tail->x;
  else return -1
;}

int isEmpty(){
  if (root == NULL) return 1;
  else return 0;
}




/*End numstack.c*/
/*Start floodfill.c*/
int fnum[MAZE_WIDTH][MAZE_HEIGHT]; //numbers assigned from floodfill
int walls[7][7] = {{0}};//0-no walls, 1-north wall, 2-east wall 3- north and east wall
	/*	{{2,0,0,1,1,1,2},
	     {2,2,2,0,1,1,3},
		 {0,3,2,2,2,1,2},
		 {2,0,3,0,1,2,2},
		 {2,3,1,3,2,2,2},
		 {0,1,1,2,2,2,2},
		 {3,1,1,3,1,1,3}};
    */


void initMaze(){
	int i, j = 0;
	for(i=0;i<MAZE_WIDTH;i++){
		for(j=0;j<MAZE_HEIGHT; j++){
			fnum[i][j]=(MAZE_HEIGHT*MAZE_WIDTH)+1;
		}
	}
}

void printMaze(){
	int i = 0;
	int j = 0;
	//printf("\n\n");
	for(i=MAZE_HEIGHT-1; i>=0; i--){
		for(j=0; j<MAZE_WIDTH; j++){
			//printf(" %2d ", fnum[i][j]);
		}
			//printf("\n");
	}
}

bool isWallWest(int x, int y){
	if(x==0){
		return true;
	}
	else if(walls[y][x-1]==2 || walls[y][x-1]==3){
		return true;
	}
	else return false;
}

bool isWallEast(int x, int y){
	if(x==MAZE_WIDTH-1){
		return true;
	}
	else if(walls[y][x]==2 || walls[y][x]==3){
		return true;
	}
	else return false;
}

bool isWallNorth(int x, int y){
	if(y==MAZE_HEIGHT-1){
		return true;
	}
	else if(walls[y][x]==1 || walls[y][x]==3){
		return true;
	}
	else return false;
}

bool isWallSouth(int x, int y){
	if(y==0){
		return true;
	}
	else if(walls[y-1][x]==1 || walls[y-1][x]==3){
		return true;
	}
	else return false;
}

int explore(int x, int y, int goal_x, int goal_y, int orientation, int front, int left, int right){
	/*********************************************
	*explore
	*
	*returns 
	*	-1 		- error
	*	0 - 3 	- direction (N,E,W,S)
	*	4 		- maze complete
	*
	* *******************************************/
	
	int cell_f_num = (MAZE_HEIGHT*MAZE_WIDTH)+1;
	int temp = (MAZE_HEIGHT*MAZE_WIDTH)+1;
	int f_num_dif = (MAZE_HEIGHT*MAZE_WIDTH)+1;
	int dir = -1;
	
	mapCell(x, y, orientation, front, left, right); //update wall array with new view;
	initMaze();
	floodfill(4,4,0);
	cell_f_num = getFNum(x,y);


	if(x==goal_x && y==goal_y){
		return 4; //explore success
	}
	/*Ideally, dependent on orientation. Change it in later update*/
	if(!isWallNorth(x,y)){
		dir=0; //default to north
		if((cell_f_num - getFNum(x,y+1))>=0)f_num_dif = cell_f_num - getFNum(x,y+1);
	}
	temp = cell_f_num - getFNum(x+1, y);
	if(!isWallEast(x,y)&& (temp<f_num_dif) && temp>=0){	//if no wall east and the num is lower, go east
		dir = 1;
		f_num_dif= temp;
	}
	temp = cell_f_num - getFNum(x, y-1);
	if(!isWallSouth(x,y) && temp<f_num_dif && temp>=0){	//if no wall east and the num is lower, go east
		f_num_dif = temp;
		dir = 2;
	}

	temp = cell_f_num - getFNum(x-1, y);
	if(!isWallWest(x,y)&& temp<f_num_dif && temp>=0){	//if no wall east and the num is lower, go east
		f_num_dif = temp;
		dir = 3;
	}


	if (dir>=0){
		if((peek()==((dir+2)%4))) {
			pop();
			//printMaze();
			////printf("\ndir-->%d, peek()-->%d", dir, peek());
			////printf("\npop");//pop(); //pop if opposite of the top of stack
		}
		else push(dir); //else push direction to stack
		return dir;

	}else{
		//error. nowhere to go...
		return -1;
	}

}

int mapCell(int x, int y, int orientation, int front, int left, int right){ //0-N, 1-E, 2-S, 3-W
	int l_wall=0;
	int f_wall=0;
	int r_wall=0;

	/*Get Walls. If wall exists, set l_wall, f_wall, r_wall
	*
	*******************************************************/

	//y, Output_left, Output_right are global variables 
	if(front>55){
		f_wall = 1;
	}

	if(left>60){
		l_wall = 1;
	}

	if(right>60){
		r_wall = 1;
	}

	switch(orientation){
		case 0:	//facing north
			if (l_wall)setWall(x,y,3);
			if (r_wall)setWall(x,y,1);
			if (f_wall)setWall(x,y,0);
			break;
		case 1:
			if (l_wall)setWall(x,y,0);
			if (r_wall)setWall(x,y,2);
			if (f_wall)setWall(x,y,1);
			break;
		case 2:
			if (l_wall)setWall(x,y,1);
			if (r_wall)setWall(x,y,3);
			if (f_wall)setWall(x,y,2);
			break;
		case 3:
			if (l_wall)setWall(x,y,2);
			if (r_wall)setWall(x,y,0);
			if (f_wall)setWall(x,y,3);
			break;
	}
}


int setWall(int x, int y, int dir){ //0-N, 1-E, 2-S, 3-W
	int in_bounds = 0;
	if(dir==0 && y<(MAZE_HEIGHT-1))in_bounds=1;
	if(dir==1 && x<(MAZE_WIDTH-1))in_bounds=1;
	if(dir==2 && y>0)in_bounds=1;
	if(dir==3 && x>0)in_bounds=1;

	if(in_bounds){
		if (dir==0){		//NORTH
			if(isWallEast(x,y))walls[y][x]=3;
			else walls[y][x]=1;

		}else if(dir==1){	//EAST
			if(isWallNorth(x,y))walls[y][x]=3;
			else walls[y][x]=2;
		}else if(dir==2){	//SOUTH
			if(isWallEast(x,y-1))walls[y-1][x]=3;
			else walls[y-1][x]=1;

		}else if(dir==3){	//WEST
			if(isWallNorth(x-1,y))walls[y][x-1]=3;
			else walls[y][x-1]=2;

		}
	}
}

// To keep consistency with maze
int getFNum(int x, int y){
/*helper function to keep consistency with wall array*/

	if(x<MAZE_WIDTH && y<MAZE_HEIGHT)return fnum[y][x];
	else return -1;
}

void setFNum(int x, int y, int num){
/*helper function to keep consistency with wall array*/
	fnum[y][x]=num;
}


void floodfill(int x, int y, int level){
/*recursive function which outputs to FNum array the minimum 
number of steps required to get to the goal from each cell. Takes
into account current state of wall array.

when calling function, level should be 0

inputs: goal x position 
		goal y position
		current level (0 for first run)
returns: void

 */
	setFNum(x,y,level);
	////printf("!isWallNorth")
//	//printf("!isWallNorth && ((level+1)< getFNum(x, y+1))-->%d", !isWallNorth && ((level+1)< getFNum(x, y+1)));
	if(!isWallNorth(x,y) && ((level+1)< getFNum(x, y+1))){
		////printf("Enter north flood\n");
		floodfill(x,y+1,level+1);
	}
	if(!isWallEast(x,y) && ((level+1)< getFNum(x+1, y))){
		////printf("Enter east flood\n");
		floodfill(x+1,y,level+1);
	}
	if(!isWallSouth(x,y) && ((level+1)< getFNum(x, y-1))){
		////printf("Enter south flood\n");
		floodfill(x,y-1,level+1);
	}
	if(!isWallWest(x,y) && ((level+1)< getFNum(x-1, y))){
		////printf("Enter west flood\n");
		floodfill(x-1,y,level+1);
	}
	return;
}

/*int main(){

	initMaze();
	int i = 0;
	int j = 0;
	int x = 0;
	int y = 0;
	int orient = 0;
	while(i!=4){

		i = explore(x,y,4,4,orient);
		switch(i){
			case 0:	y++;
					orient = 0;
					//printf("north | peek()-->%d\n", peek());
					break;
			case 1:	x++;
					orient = 1;
					//printf("east | peek()-->%d\n", peek());
					break;
			case 2:	y--;
					orient = 2;
					//printf("south | peek()-->%d\n", peek());
					break;
			case 3:	x--;
					orient = 3;
					//printf("west | peek()-->%d\n", peek());
					break;
		}

		////printf("x-->%d, y-->%d, turn-->%d", x,y,peek());
		
	}

	/*orient = explore(0,0,4,4,orient);
	//printf("\n\nx-->%d, y-->%d, turn-->%d", x,y,peek());
	orient = explore(0,1,4,4,orient);
	//printf("\n\nx-->%d, y-->%d, turn-->%d", x,y,peek());
	orient = explore(0,2,4,4,orient);
	//printf("\n\nx-->%d, y-->%d, turn-->%d", x,y,peek());
	orient = explore(1,2,4,4,orient);
	//printf("\n\nx-->%d, y-->%d, turn-->%d", x,y,peek());
*/
	//printf("\n");
	//printMaze();


//}
/*end floodfill.c*/




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
	
	//initMaze();
	
	EnableInterrupts;
	
	for(;;) {
	  
		ATD0CTL5 = 0x33;
		  
		while (!(ATD0STAT0 & 0x80));
	
		
/* * * * * * * * * * * * * * * * * * *
    START
    4/2/2014
    Merging Drive and wall array and floodfill

 * * * * * * * * * * * * * * * * * * */

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
      flag_this++;
  if (flag_this == 10000) {
    
                 
      G_Turn = explore(Current_x, Current_y, 4,4, G_Current_Orient, y, Output_left[OutputIndex], Output_right[OutputIndex]); // 4 finished, 0 -N, 1 -E, 2 -S, 3 -W, -1 -error
	  switch(G_Turn){
	    case 0: Current_y++;break;
	    case 1: Current_x++;break;
	    case 2: Current_y--;break;
	    case 3: Current_x--;break;
	  }
	  
	  G_Current_Orient=G_Turn;
      PTH = SEG7_DISPLAY[G_Turn];
      flag_this = 0;
    
  }

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