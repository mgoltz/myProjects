#ifndef NUMSTACK_H
#define NUMSTACK_H
#include "numstack.h"
#endif
   

//#include "numstack.h"
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
