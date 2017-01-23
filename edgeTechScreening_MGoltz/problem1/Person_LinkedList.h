//problem 1 - EdgeTech
//Michael Goltz
//  Person_LinkedList.h

//  This header file contains the Person Linked List class declaration.
//  If using the Hash Table for faster searching in large family tree, 
//  array elements consist of Linked List objects.

#ifndef PersonLinkedList_h
#define PersonLinkedList_h

#include <iostream>
#include <string>
#include <vector>
#include <stdbool.h>
#include <stdlib.h>
using namespace std;

#define MALE true
#define FEMALE false
#define ALIVE true


//*****************************************************************
// Person having a unique keyName, info, and vectors of relatives
// and pointer to maintain linked list
//*****************************************************************
struct Person
{
    string keyName;
    bool isAlive;
    bool gender;
    
    Person * next;
    vector<Person *> parents;
    vector<Person *> children;
};

//*****************************************************************
// Person Linked lists store a variable number of items.
//*****************************************************************
class PersonLinkedList
{
private:
    // Head is a reference to a list of data nodes/people
    Person * head;
    
    // Length is the number of data nodes/people in list
    int length;
    
public:
    // Constructs the empty linked list object.
    // Creates the head node and sets length to zero.
    PersonLinkedList();
    
    // Inserts a person at the end of the list.
    void insertPerson( Person * newPerson/*string name, bool isAlive, bool gender*/ );

    // Creates links between child and parents
    void connectChildAndParent( Person * child, Person * parent1, Person * parent2 );
    void connectChildAndParent( string childName, string parentName1, string parentName2 );

    // Prints parents of child, given a childs name
    void printParents(string childName);
    void printParents(Person * child);

    // Prints children of parent, given a parent name
    void printChildren(string parentName);
    void printChildren(Person * parent);

    // Prints all grandparents of a person, given a persons name
    void printGrandParents(string personName);
    void printGrandParents(Person * person);

    // Prints all siblings of a person, given a persons name
    void printSiblings(string personName);
    void printSiblings(Person * person);

    // Prints a statement of truth given a persons name and the parents gender
    void isParentAlive(string personName, bool parentGender);
    void isParentAlive(Person * person, bool parentGender);

    // Prints a statement of truth given a persons name and the grandparents name
    void isGrandParentAlive(string personName, string grandParentName);
    void isGrandParentAlive(Person * person, string grandParentName);

    // Prints a statement of truth given a persons name and searches for parents siblings
    void hasParentsSiblings(string personName, bool parentsSiblingsGender);
    void hasParentsSiblings(Person * person, bool parentsSiblingsGender);
    
    // search through list to find person name and return a reference
    Person * getPerson( string personName );
    
    // Displays list contents to the console window.
    void printList();
    
    // Returns the length of the list.
    int getLength();
    
    // De-allocates list memory when the program terminates.
    ~PersonLinkedList();
};

#endif