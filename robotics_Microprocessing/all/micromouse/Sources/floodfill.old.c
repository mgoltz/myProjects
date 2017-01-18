#include "floodfill.h"
/*#include "numstack.h"
#ifndef FLOODFILL_H
#define FLOODFILL_H
#include "floodfill.h" 
#endif
#ifndef NUMSTACK_H
#define NUMSTACK_H
#include "numstack.h"
#endif
   */
   
  
int fnum[MAZE_WIDTH][MAZE_HEIGHT]; //numbers assigned from floodfill
int walls[7][7] = //0-no walls, 1-north wall, 2-east wall 3- north and east wall
		{{2,0,0,1,1,1,2},
	     {2,2,2,0,1,1,3},
		 {0,3,2,2,2,1,2},
		 {2,0,3,0,1,2,2},
		 {2,3,1,3,2,2,2},
		 {0,1,1,2,2,2,2},
		 {3,1,1,3,1,1,3}};



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
