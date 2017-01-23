#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#define boolean

struct Tree {
	struct Person *person;
};

struct Person{
  char name[12];
  bool isAlive;

  struct Person * mother;
  struct Person * father;
  struct Person * siblings;
  struct Person * children;
};

struct Tree *tree = NULL;

void insertPerson(char *name, bool isAlive ) {
	struct Person *new_person = (struct Person *) malloc(sizeof(struct Person));
	strcpy(new_person->name, name);
	new_person->isAlive = isAlive;
	printf("reached before tree assignment\n");
	if (tree == NULL) printf("tree null\n" );
	tree->person = new_person;
	printf("reached after tree assignment\n");
}
void printPerson(){
	printf("%s\n", tree->person->name);
};

int main(void) {

	//struct Person tree;
	insertPerson("Mike", true);
	printPerson();



	return 0;
}