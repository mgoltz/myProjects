#ifndef MAZE_HEIGHT
#define MAZE_HEIGHT 7
#endif
#ifndef MAZE_WIDTH
#define MAZE_WIDTH 7
#endif

typedef enum { false, true } bool;
void initMaze(void);
bool isWallEast(int x, int y);
bool isWallWest(int x, int y);
bool isWallNorth(int x, int y);
bool isWallSouth(int x, int y);
int explore(int x, int y, int goal_x, int goal_y, int orientation, int front, int left, int right);
int mapCell(int x, int y, int orientation, int front, int left, int right);
int setWall(int x, int y, int dir);
int getFNum(int x,int y);
void setFNum(int x, int y, int num);
void floodfill(int x, int y, int level);
int increment_v(int x, int y);