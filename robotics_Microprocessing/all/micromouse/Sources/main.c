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

void change_buzzer(int dout);
void setup_buzzer(void);
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


void PIVOT_TURN(int steps, int direction);  // pivot turn calls subfunctions
void RAMP_ONE_UP(int steps, int direction);  // pivot turn
void RAMP_ONE_DOWN(int steps, int direction);  // pivot turn


volatile double Output_right[2] = { 0 };   //last two distance readings
volatile double Output_left[2] = { 0 };   //last two distance readings
volatile unsigned char OutputIndex = 0;	//points to the most recent distance Output location

//motor wheel half steps
volatile unsigned char RightStepArray[] ={0xC0, 0x80, 0x90, 0x10, 0x30, 0x20, 0x60, 0x40};
volatile unsigned char LeftStepArray[] ={0x0C, 0x04, 0x06, 0x02, 0x03, 0x01, 0x09, 0x08};
unsigned char SEG7_DISPLAY[10] = {0x40,0x79,0x24,0x30,0x19,0x12,0x02,0x78,0x00,0x10};
                                                          //0       1       2        3        4     5       6       7       8       9
// turn control
volatile int G_Block_Step = 520; // steps for length of a block
volatile int G_90_Turn = 208; // steps for 90 degree turn    208
volatile int G_Current_Orient = 0; //0=N, 1=E, 2=S, 3=W
volatile int G_Turn = 0;  //0=N, 1=E, 2=S,  3=W
volatile int prev_explore = -1;
volatile int Temp_New_Direction = 0;
volatile int G_Steps = 0;
volatile int state = 0;
volatile int direction = 0;	// 0 = left, 1 = right


// motor timing global variable
volatile long a[2] =  {-25575, 10508};
volatile long b[3] =  {329, 658, 329};
volatile double kp = 0.7; // 1.0
volatile double kd = 0.2;  // 0.9
volatile double error_r = 0;		// represents our distance from desired location
volatile double errorDot_r = 0;	// represents our change in distance from last reading
volatile double error_l = 0;		// represents our distance from desired location
volatile double errorDot_l = 0;	// represents our change in distance from last reading
volatile int diff = 0;
volatile int interruptcount_filter = 0;
volatile int interruptcount_left = 0;
volatile int interruptcount_right = 0;
volatile int stepindex_left = 0;
volatile int stepindex_right = 0;
volatile double speed_left = 30;       // int  26
volatile double speed_right = 30;     // int  26
volatile double top_speed = 30;      // 26
volatile double current_speed = 130;
volatile double bottom_speed = 130;
volatile int speed_counter = 0;
volatile int count_both = 0;
volatile int count_one = 0;


// ATD conversion
volatile long x_s, rx, lx;
volatile int y_s, ry, ly; 
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
volatile short L_wall = 1;
volatile short R_wall = 1;
volatile short turn = 0;
volatile short rolling_flag = 0;

/* * * * *  * * * *  * * * ** * * * * * * * * * * * * * * *

    start of maze solving algorithm
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

volatile int visited[MAZE_WIDTH][MAZE_HEIGHT] = {{0}};
volatile int v_x = 0;
volatile int v_y = 0;

/* * * * *  * * * *  * * * ** * * * * * * * * * * * * * * *

    increment part of explore
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int increment_v(int x, int y){
/*
 * set v_y, v_x to new explore position.
 *
 * return 1 if everything is visited
 * return 0 if still exploring
 * 
 */
    
    //Find next v with lowest floodnumber
    int vcell=0;
    int vcell_n=0;
    int vcell_e=0;
    int vcell_s=0;
    int vcell_w=0;
    int i = 0;
    int j = 0;
    int flag = 0;
    int low_x = 0;
    int low_y = 0;
    int low_f = (MAZE_HEIGHT*MAZE_WIDTH)+1;

    initMaze();
    floodfill(x,y,0);

    for(i=0;i<MAZE_WIDTH;i++){
        for(j=0;j<MAZE_HEIGHT;j++){

            //j will be x,
            //i will be y
            vcell = visited[i][j];
            if(i < (MAZE_HEIGHT-1)) vcell_n = visited[i+1][j];
            else vcell_n = 1;

            if(j < (MAZE_WIDTH-1)) vcell_e = visited[i][j+1];
            else vcell_e =  1;

            if(i>0)vcell_s = visited[i-1][j];
            else vcell_s = 1;

            if(j>0) vcell_w = visited[i][j-1];
            else vcell_w = 1;

            if(vcell_n && vcell_e && vcell_s && vcell_w){
                visited[i][j]=1;
                vcell = 1;
            }


            if(!vcell){
                flag = 1;
                if(getFNum(j,i)<low_f){
                    low_f = getFNum(j,i);
                    low_x = j;
                    low_y = i;
                }
            }
        }
    }

    if(flag){
        v_x = low_x;
        v_y = low_y;
        return 0;
    }else{
        return 1;
    }
}
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


/* * * * *  * * * *  * * * ** * * * * * * * * * * * * * * *

    list node functions
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
void insert_end(int value)
{
     struct node_L *var,*temp;
     var=(struct node_L *)malloc(sizeof(struct node_L));
             var->data=value;
     if(head==NULL)
     {
          head=var;
          head->previous=NULL;
          head->next=NULL;
          last=head;
     }
     else
     {
         last=head;
         while(last!=NULL)
         {
             temp=last;
             last=last->next;
         }
     last=var;
     temp->next=last;
     last->previous=temp;
     last->next=NULL;
     }
} 


void delete_from_end()
{
      struct node_L *temp;
      temp=last;
      if(temp->previous==NULL)
      {
           free(temp);
           head=NULL;
           last=NULL;
           //return 0;
      }
      //printf("\nData deleted from list is %d \n",last->data);
      last=temp->previous;
      last->next=NULL;
      free(temp);
      //return 0;
}

int peek(){
  if(head!=NULL)return last->data;
  else return -1;
}

int backTrack()
{  
     int value;


     if(temp_L==NULL)
      {
          return 4;  //printf("Beginning reached");
      } else {          
          value = temp_L->data;
          temp_L=temp_L->previous;
          return value;
     }
    //printf("Beginning reached");

}
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/* * * numstack.c * * 

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
  else return -1;
}

int isEmpty(){
  if (root == NULL) return 1;
  else return 0;
}
* * *                                     * */
/* * * End numstack.c * * */



/* * * Start floodfill.c * * */
volatile int fnum[MAZE_WIDTH][MAZE_HEIGHT]; //numbers assigned from floodfill
volatile int walls[7][7] = {{2,0,0,0,0,0,0},
                                         {0,0,0,0,0,0,0},
                                         {0,0,0,0,0,0,0},
                                         {0,0,0,0,0,0,0},
                                         {0,0,0,0,0,0,0},
                                         {0,0,0,0,0,0,0},
                                         {0,0,0,0,0,0,0}

                                                                  };
  //0-no walls, 1-north wall, 2-east wall 3- north and east wall
		/*{{2,0,0,1,1,1,2},
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

/* * * * *  * * * *  * * * ** * * * * * * * * * * * * * * *

    Explore the maze and goal seek
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int explore(int x, int y, int goal_x, int goal_y, int orientation, int front, int left, int right){
    /*********************************************
    *explore
    *
    *returns 
    *   -1      - error
    *   0 - 3   - direction (N,E,W,S)
    *   4       - maze complete
    *
    * *******************************************/
    int cell_f_num = (MAZE_HEIGHT*MAZE_WIDTH)+1;
    int temp = (MAZE_HEIGHT*MAZE_WIDTH)+1;
    int f_num_dif = (MAZE_HEIGHT*MAZE_WIDTH)+1;
    int dir = -1;
    static int found_goal=0;
    static int maze_explore_complete=0;
    static int maze_return_start=0;
    if(!maze_explore_complete)mapCell(x, y, orientation, front, left, right); //update wall array with new view;

    visited[y][x] = 1;

    if (x==4 && y==4)found_goal=1;

    if(found_goal){
        if(increment_v(x,y) && !maze_explore_complete){
            maze_explore_complete=1;
          //  printf("Explore Complete! Heading back to start...\n");
        }
        goal_x = v_x;
        goal_y = v_y;
    }

    if(maze_explore_complete){
        if(x==0 && y==0){
         //   printf("back at start! Heading to finish...\n");
            maze_return_start=1;
        }else{
            goal_x=0;
            goal_y=0;
        }
        if(maze_return_start){
            if(x==4 && y==4)return 4;
            rolling_flag=1;
            goal_x=4;
            goal_y=4;
        }

    }

    initMaze();
    floodfill(goal_x,goal_y,0);
    cell_f_num = getFNum(x,y);

    /*Ideally, dependent on orientation. Change it in later update*/
    if(!isWallNorth(x,y)){
        dir=0; //default to north
        if((cell_f_num - getFNum(x,y+1))>=0)f_num_dif = cell_f_num - getFNum(x,y+1);
    }
    temp = cell_f_num - getFNum(x+1, y);
    if(!isWallEast(x,y)&& (temp<f_num_dif) && temp>=0){ //if no wall east and the num is lower, go east
        dir = 1;
        f_num_dif= temp;
    }
    temp = cell_f_num - getFNum(x, y-1);
    if(!isWallSouth(x,y) && temp<f_num_dif && temp>=0){ //if no wall east and the num is lower, go east
        f_num_dif = temp;
        dir = 2;
    }

    temp = cell_f_num - getFNum(x-1, y);
    if(!isWallWest(x,y)&& temp<f_num_dif && temp>=0){   //if no wall east and the num is lower, go east
        f_num_dif = temp;
        dir = 3;
    }


    if (dir>=0){
        return dir;

    }else{
        //error. nowhere to go...
        return -1;
    }

}

/*int explore(int x, int y, int goal_x, int goal_y, int orientation, int front, int left, int right){
    /*********************************************
    *explore
    *
    *returns 
    *   -1      - error
    *   0 - 3   - direction (N,E,W,S)
    *   4       - maze complete
    *
    * *******************************************
    int cell_f_num = (MAZE_HEIGHT*MAZE_WIDTH)+1;
    int temp = (MAZE_HEIGHT*MAZE_WIDTH)+1;
    int f_num_dif = (MAZE_HEIGHT*MAZE_WIDTH)+1;
    int dir = -1;
    static int found_goal=0;
    static int maze_explore_complete=0;
    static int maze_return_start=0;
    mapCell(x, y, orientation, front, left, right); //update wall array with new view;

    visited[y][x] = 1;

    if (x==4 && y==4)found_goal=1;

    if(found_goal){
        if(increment_v(x,y) && !maze_explore_complete){
            maze_explore_complete=1;
            //printf("Explore Complete! Heading back to start...\n");
        }else{
            goal_x = v_x;
            goal_y = v_y;
        }
    }

    if(maze_explore_complete){
        if(x==0 && y==0){
            //printf("back at start! Heading to finish...\n");
            maze_return_start=1;
        }else{
            goal_x=0;
            goal_y=0;
        }
        if(maze_return_start){
            if(x==4 && y==4)return 4;
            rolling_flag=1;
            goal_x=4;
            goal_y=4;
        }

    }

    if(!maze_explore_complete){
        initMaze();
        floodfill(goal_x,goal_y,0);
    }
    cell_f_num = getFNum(x,y);

    /*Ideally, dependent on orientation. Change it in later update
    if(!isWallNorth(x,y)){
        dir=0; //default to north
        if((cell_f_num - getFNum(x,y+1))>=0)f_num_dif = cell_f_num - getFNum(x,y+1);
    }
    temp = cell_f_num - getFNum(x+1, y);
    if(!isWallEast(x,y)&& (temp<f_num_dif) && temp>=0){ //if no wall east and the num is lower, go east
        dir = 1;
        f_num_dif= temp;
    }
    temp = cell_f_num - getFNum(x, y-1);
    if(!isWallSouth(x,y) && temp<f_num_dif && temp>=0){ //if no wall east and the num is lower, go east
        f_num_dif = temp;
        dir = 2;
    }

    temp = cell_f_num - getFNum(x-1, y);
    if(!isWallWest(x,y)&& temp<f_num_dif && temp>=0){   //if no wall east and the num is lower, go east
        f_num_dif = temp;
        dir = 3;
    }


    if (dir>=0){
        return dir;

    }else{
        //error. nowhere to go...
        return -1;
    }

} */

/*int explore(int x, int y, int goal_x, int goal_y, int orientation, int front, int left, int right){
    // *********************************************
    // *explore
    // *
    // *returns 
    // *   -1      - error
    // *   0 - 3   - direction (N,E,W,S)
    // *   4       - maze complete
    // *
    // * *******************************************
    int cell_f_num = (MAZE_HEIGHT*MAZE_WIDTH)+1;
    int temp = (MAZE_HEIGHT*MAZE_WIDTH)+1;
    int f_num_dif = (MAZE_HEIGHT*MAZE_WIDTH)+1;
    int dir = -1;
    static int found_goal=0;
    mapCell(x, y, orientation, front, left, right); //update wall array with new view;

    visited[y][x] = 1;

    if (x==4 && y==4)found_goal=1;

    if(found_goal){
        if(increment_v(x,y))return 4;
        goal_x = v_x;
        goal_y = v_y;
    }


    // *if((x==goal_x && y==goal_y)||found_goal){
        //we have reached the goal, but we havent explored the entire maze. 
        //we will loop through the visited array. if 0, explore to it. if all visited around block, mark as visited.
      //  if(increment_v()==4)return 4;//find next unvisited position.
    //}
    initMaze();
    floodfill(goal_x,goal_y,0);
    cell_f_num = getFNum(x,y);

    // *Ideally, dependent on orientation. Change it in later update
    if(!isWallNorth(x,y)){
        dir=0; //default to north
        if((cell_f_num - getFNum(x,y+1))>=0)f_num_dif = cell_f_num - getFNum(x,y+1);
    }
    temp = cell_f_num - getFNum(x+1, y);
    if(!isWallEast(x,y)&& (temp<f_num_dif) && temp>=0){ //if no wall east and the num is lower, go east
        dir = 1;
        f_num_dif= temp;
    }
    temp = cell_f_num - getFNum(x, y-1);
    if(!isWallSouth(x,y) && temp<f_num_dif && temp>=0){ //if no wall east and the num is lower, go east
        f_num_dif = temp;
        dir = 2;
    }

    temp = cell_f_num - getFNum(x-1, y);
    if(!isWallWest(x,y)&& temp<f_num_dif && temp>=0){   //if no wall east and the num is lower, go east
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
        //
        //printf("wall_north-->%d\nwall_east-->%d\nwall_south-->%d\nwall_west-->%d\n", isWallNorth(x,y), isWallEast(x,y), 
        //        isWallSouth(x,y), isWallWest(x,y));
        return -1;

    }

} */
  
/* * * * *  * * * *  * * * ** * * * * * * * * * * * * * * *

    end of exploring maze and goal seeking
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/* *
int explore(int x, int y, int goal_x, int goal_y, int orientation, int front, int left, int right){
	// *********************************************
	// *explore
	// *
	// *returns 
	// *	-1 		- error
	// *	0 - 3 	- direction (N,E,W,S)
	// *	4 		- maze complete
	// *
	// * *******************************************
	
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
	// *Ideally, dependent on orientation. Change it in later update
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



/* * * * *  * * * *  * * * ** * * * * * * * * * * * * * * *

    map cell walls
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int mapCell(int x, int y, int orientation, int front, int left, int right){ //0-N, 1-E, 2-S, 3-W
	int l_wall=0;
	int f_wall=0;
	int r_wall=0;

	/*Get Walls. If wall exists, set l_wall, f_wall, r_wall
	*
	*******************************************************/

	//y, Output_left, Output_right are global variables 
	if(front>40){       // 55
		f_wall = 1;
	}

	if(left>45){          // 60
		l_wall = 1;
	}

	if(right>45){        // 60
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

/* * * * *  * * * *  * * * ** * * * * * * * * * * * * * * *
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
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
	setup_buzzer();
	
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
             
switch (state){
		    case 0:  
		                  RAMP_BOTH_UP();
		                  //HALF_STEP_FORWARD(100);
		                  /*
		                  G_Turn = explore(Current_x, Current_y, 4,4, G_Current_Orient, y_s, Output_left[OutputIndex], Output_right[OutputIndex]); // 4 finished, 0 -N, 1 -E, 2 -S, 3 -W, -1 -error
                          SWITCH_DIRECTION();
                          G_Steps = 390;
                          RAMP_BOTH_UP();
                          HALF_STEP_FORWARD(G_Steps);
                          PTH = SEG7_DISPLAY[0];
                          
                          */
                          //if (rolling_flag) state = 6;
			              //else 
			              state = 8;
			              
                          break;
			case 1 ://  PTH = SEG7_DISPLAY[Current_y];
			             
			             // for rolling turn state = 6 or for stationary turn set new speed 
			              if (rolling_flag){
			                speed_left = 22;       // int  26
                            speed_right = 22;     // int  26
                            top_speed = 22;      // 26
			                //state = 6; break;
			              }
			              
			              turn = 0;
			              G_Turn = explore(Current_x, Current_y, 4,4, G_Current_Orient, y_s, Output_left[OutputIndex], Output_right[OutputIndex]); // 4 finished, 0 -N, 1 -E, 2 -S, 3 -W, -1 -error
			             
			              PTH = SEG7_DISPLAY[G_Turn];
			              if (G_Turn == -1) break;
			              else if (G_Turn == 4) state = 3;
			              else if (G_Turn == G_Current_Orient){  // remain going straight
                              SWITCH_DIRECTION();
			                  G_Steps = G_Block_Step;
			                  
                              HALF_STEP_FORWARD(G_Steps);
                            
			              } else {  // turn or u-turn, ramp down, stationary turn, ramp up
                              SWITCH_DIRECTION();
                              state = 2;                             

			              }
			              
			              break;

			case 2:   turn = 1;
			              STATIONARY_TURN(G_Steps, direction);	// ramp down TURN - U-TURN
			              
			              G_Steps = 360; // 360
			              
			              RAMP_BOTH_UP();
                          HALF_STEP_FORWARD(G_Steps);
                          state = 1; //1
                          break;
                          
			case 3:  PTH = SEG7_DISPLAY[4];
			              change_buzzer(0);
			              break;
			
            case 4:   
            
                          state = 1;
                          //PTH = SEG7_DISPLAY[5];                          
                          break;
                          
            case 5 :  // for pivot turns
                          PIVOT_TURN(350, direction);
                          state = 1;
						  //NEXT_STATE();
						  break;	

        	case 6 :  // for rolling turns
        	              turn = 0;
			              G_Turn = explore(Current_x, Current_y, 4,4, G_Current_Orient, y_s, Output_left[OutputIndex], Output_right[OutputIndex]); // 4 finished, 0 -N, 1 -E, 2 -S, 3 -W, -1 -error
			             
			              PTH = SEG7_DISPLAY[6];
			              if (G_Turn == -1) break;
			              else if (G_Turn == 4) state = 3;
			              else if (G_Turn == G_Current_Orient){  // remain going straight
                              SWITCH_DIRECTION();
			                  G_Steps = G_Block_Step;
			                  
                              HALF_STEP_FORWARD(G_Steps);
                            
			              } else {  // turn or u-turn, ramp down, stationary turn, ramp up
                              SWITCH_DIRECTION();
                             //if (G_Turn == G_90_Turn*2) state = 2;
                             //else 
                             state = 7;                              
			              }
			              
			              break;
						  
            case 7:  //ROLLING_TURN (set both speeds 1:3 ratio)
						  ROLLING_TURN(580, direction);
						  //G_Steps = 50;
						  state = 6;
						  break;
						  
            case 8:  break;
		
} // end of switch

/* * * * * * * * * * * * * * * * * * *
    END
    4/2/2014
    Merging Drive and wall array and floodfill

 * * * * * * * * * * * * * * * * * * */
	}
} // end of main

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
		x_s = ATD0DR1H;
		x_s <<= 14;
		  
		dlng = x_s - a[0] * d[1] - a[1] * d[2];
		dlng <<= 2;
		dlng >>= 16;
		d[0] = dlng;
		ylng = b[0] * d[0] + b[1] * d[1] + b[2] * d[2];
		ylng <<= 2;
		ylng >>= 16;
		y_s = ylng;   
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
	    
	     // go straight	
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
/*	    
        if(y_s>60){       // 55
          count_both = 0;
		  return;
        }    
	    
	   /* if (!turn){  // not out of a turn
	      
	    if ((Output_left[OutputIndex] < 50) && L_wall){     // left wall disappears
	      L_wall = 0;
	      count_both = 0;
	      HALF_STEP_FORWARD(260);
	      state = 4;
	      return;
	    } else if ((Output_right[OutputIndex] < 50) && R_wall){  // right wall disappears
          R_wall = 0;
	      count_both = 0;
	      HALF_STEP_FORWARD(260);
	      state = 4;
	      return;	      	      
	    } else if ((Output_left[OutputIndex] >= 50) && !L_wall){ // wall appears
	      L_wall = 1;
	      count_both = 0;
	      HALF_STEP_FORWARD(260);
	      state = 4;
	      return;     	      	      
	    } else if ((Output_right[OutputIndex] >= 50) && !R_wall){ // wall appears
	      R_wall = 1;
	      count_both = 0;
	      HALF_STEP_FORWARD(260);
	      state = 4;
	      return;	      	      
	    }
	    }       // *

        if (state == 1 || state == 0 || state == 2 || state==6) {
			//PD CONTROL
			diff = ((Output_left[OutputIndex] > Output_right[OutputIndex]) ? (
			Output_left[OutputIndex] - Output_right[OutputIndex]) : (Output_right[OutputIndex] - Output_left[OutputIndex]));
            
            if ((Output_left[OutputIndex] < 50) && (Output_right[OutputIndex] < 50)){
              
                // go straight	
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
              
            }
            // left wall present correct right speed					
			else if ((Output_left[OutputIndex] > Output_right[OutputIndex]) && (diff > 10)){
					
				//PTH = 0x79; // right one
					
				error_r = 72.0 - Output_left[OutputIndex];
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
			} else if ((Output_right[OutputIndex] > Output_left[OutputIndex]) && (diff > 10)) {
				
				//PTH = 0x4F; // left one	  
				error_l = 72.0 - Output_right[OutputIndex];
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
					  
			} else if (diff <= 10) {   // go straight
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
	
	*/
	}// end of while
	count_both = 0;
	  //state = 0;

 

} // end of half step forward



void SWITCH_DIRECTION(void){
  switch (G_Current_Orient){
    case 0:   G_Current_Orient = G_Turn;
                  if (G_Turn == 0) Current_y++;  // straight
                  else if (G_Turn == 1){ // right turn
                    Current_x++;
                    direction = 1;
                    G_Steps = G_90_Turn;
                  }
                  else if (G_Turn == 2) {// u-turn
                    Current_y--;
                    G_Steps = G_90_Turn*2;
                  }
                  else if (G_Turn == 3){ // left turn
                    Current_x--;
                    direction = 0;
                    G_Steps = G_90_Turn;
                  }
               break;
    case 1: { switch (G_Turn) { // East to
                    case 0: G_Current_Orient=0; Current_y++; direction = 0; G_Steps = G_90_Turn; break; // North - left turn
                    case 1: G_Current_Orient=1; Current_x++; break; // East - Straight
                    case 2: G_Current_Orient=2; Current_y--; direction = 1; G_Steps = G_90_Turn; break; // South - right turn 
                    case 3: G_Current_Orient=3; Current_x--; G_Steps = G_90_Turn*2; break; // West - u-turn
                    }
    }break;
  
    case 2:  { switch (G_Turn) { // South to
                    case 0: G_Current_Orient=0; Current_y++; G_Steps = G_90_Turn*2; break; // North - u-turn
                    case 1: G_Current_Orient=1; Current_x++; direction = 0; G_Steps = G_90_Turn; break; // East - left turn
                    case 2: G_Current_Orient=2; Current_y--; break; // South - Straight
                    case 3: G_Current_Orient=3; Current_x--; direction = 1; G_Steps = G_90_Turn; break; // South - right turn 
                    } 
    }break;
               
    case 3:  { switch (G_Turn) { // West to
                    case 0: G_Current_Orient=0; Current_y++; direction = 1; G_Steps = G_90_Turn; break; // North - right turn 
                    case 1: G_Current_Orient=1; Current_x++; G_Steps = G_90_Turn*2; break; // East - u-turn
                    case 2: G_Current_Orient=2; Current_y--; direction = 0; G_Steps = G_90_Turn; break; // South - left turn
                    case 3: G_Current_Orient=3; Current_x--; break; // South
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
		 
			current_speed = current_speed-1;
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
/* */
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
  /* */
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
	//PWME=0x20; 	   //Enable chan 5 PWM 
  
}

void change_buzzer(int dout){
 
     if(dout == 0){
      PWMSCLA=10; //10
      PWME=0x20; 	   //Enable chan 5 PWM
    }else if(dout == 1){
      PWMSCLA=30; // 30
      PWME=0x20; 	   //Enable chan 5 PWM
    }else if(dout == 2){
      PWMSCLA=50; // 50
      PWME=0x20; 	   //Enable chan 5 PWM
    }else if(dout == 3){
      PWMSCLA=70; //70
      PWME=0x20; 	   //Enable chan 5 PWM
    }else if(dout == 4){
      PWMSCLA=90; //90
      PWME=0x20; 	   //Enable chan 5 PWM
    } 
  
}