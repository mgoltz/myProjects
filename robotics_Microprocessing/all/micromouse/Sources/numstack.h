#include <stdlib.h>

// stack node

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
// end of stack

// list node
struct node_L
{
     struct node_L *previous;
     int data;
     struct node_L *next;
}*head, *last; 
 
void insert_end(int value);
int delete_from_end();
int backTrack();
// end of list node

/*
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


*/
