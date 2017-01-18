#include <stdlib.h>


struct node{
  int x;
  struct node * next;
  struct node * prev;
};

struct node * root;
struct node * tail;

void push(int);
int pop();
int peek();
int isEmpty();

struct node_L
{
     struct node_L *previous;
     int data;
     struct node_L *next;
}*head, *last; 
 
void insert_end(int value);
int delete_from_end();
void backTrack();
