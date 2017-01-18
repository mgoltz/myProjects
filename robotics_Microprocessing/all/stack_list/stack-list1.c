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

int delete_from_end()
{
      struct node_L *temp;
      temp=last;
      if(temp->previous==NULL)
      {
           free(temp);
           head=NULL;
           last=NULL;
           return 0;
      }
      //printf("\nData deleted from list is %d \n",last->data);
      last=temp->previous;
      last->next=NULL;
      free(temp);
      return 0;
}

int backTrack()
{  
     int value;
     struct node *temp_L;
     temp_L=last;
     if(temp_L->previous==NULL)
      {
         //printf("Beginning reached");
      } else {
          
          value = temp_L->data;
          temp_L=temp_L->previous;
          return value;
     }
}


int main()
{
    head=NULL;

                 insert_end(1);
                 display();
                 
                 insert_end(2);
                 
                 insert_end(3);
                 
                 display();
                 display();

}