//problem 1 - EdgeTech
//Michael Goltz
//  Person_HashTable.h

//  This header file contains the Hash Table class declaration.
//  Hash Table array elements consist of Linked List objects.

#ifndef PersonHashTable_h
#define PersonHashTable_h

#include <ctype.h>
#include "Person_LinkedList.h"

//*****************************************************************
// Hash Table objects store a fixed number of Linked Lists.
//*****************************************************************
class PersonHashTable
{
private:
    
    // Array is a reference to an array of Linked Lists.
    PersonLinkedList * array;
    
    // Length is the size of the Hash Table array.
    int length;
    
    // Returns an array location for a given item key.
    int hash( string keyName );
    
public:
    
    // Constructs the empty Hash Table object.
    // Array length is set to 13 by default.
    PersonHashTable( int tableLength = 13 );
    
    // Adds an item to the Hash Table.
    void insertPerson( Person * newPerson/*string keyName, bool isAlive, bool gender*/ );

    // Returns an item from the Hash Table by key.
    // If the item isn't found, a null pointer is returned.
    Person * getPerson( string keyName );
    
    // Creates links between child and parents
    void connectChildAndParent( string childName, string parentName1, string parentName2 );

    // Prints parents of child, given a childs name
    void printParents(string childName);

    // Prints children of parent, given a parent name
    void printChildren(string parentName);

    // Prints all grandparents of a person, given a persons name
    void printGrandParents(string personName);

    // Prints all siblings of a person, given a persons name
    void printSiblings(string personName);

    // Prints a statement of truth given a persons name and the parents gender
    void isParentAlive(string personName, bool parentGender);

    // Prints a statement of truth given a persons name and the grandparents name
    void isGrandParentAlive(string personName, string grandParentName);

    // Prints a statement of truth given a persons name and searches for parents siblings
    void hasParentsSiblings(string personName, bool parentsSiblingsGender);

    // Display the contents of the Hash Table to console window.
    void printTable();
    
    // Returns the number of locations in the Hash Table.
    int getLength();
    
    // Returns the number of Items in the Hash Table.
    int getNumberOfItems();
    
    // De-allocates all memory used for the Hash Table.
    ~PersonHashTable();
};

#endif