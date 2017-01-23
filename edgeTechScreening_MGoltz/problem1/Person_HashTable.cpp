//problem 1 - EdgeTech
//Michael Goltz
//  Person_HashTable.cpp


#include "Person_HashTable.h"

// Constructs the empty Hash Table object.
// Array length is set by user input or default 13 for buckets of size 2 
PersonHashTable::PersonHashTable( int tableLength )
{
    array = new PersonLinkedList[ tableLength ];
    length = tableLength;
}

// Returns an array location for a given keyName.
// Hash function returns bucket value of first name initial's ascii value
// each bucket contains 2 ascii value
int PersonHashTable::hash( string keyName )
{
    int value = tolower(keyName[0]);
    return ((value - 97) / 2) % length;
}

// Adds an item to the Hash Table.
void PersonHashTable::insertPerson( Person * newPerson/*string keyName, bool isAlive, bool gender*/ )
{
    int index = hash( newPerson -> keyName );
    array[ index ].insertPerson( newPerson );
}

// Returns an item from the Hash Table by key.
// If the item isn't found, a null pointer is returned.
Person * PersonHashTable::getPerson( string keyName )
{
    int index = hash( keyName );
    return array[ index ].getPerson( keyName );
}

// Creates links between child and parents
void PersonHashTable::connectChildAndParent( string childName, string parentName1, string parentName2 )
{
    int index = hash( childName );
    Person * child = array[ index ].getPerson( childName );
    index = hash( parentName1 );
    Person * parent1 = array[ index ].getPerson( parentName1 );
    index = hash( parentName2 );
    Person * parent2 = array[ index ].getPerson( parentName2 );

    array[ index ].connectChildAndParent( child, parent1, parent2 );
}

// Prints parents of child, given a childs name
void PersonHashTable::printParents(string childName)
{
    int index = hash( childName );
    Person * child = array[ index ].getPerson( childName );
    array[ index ].printParents(child);
}

// Prints children of parent, given a parent name
void PersonHashTable::printChildren(string parentName)
{
    int index = hash( parentName );
    Person * parent = array[ index ].getPerson( parentName );
    array[ index ].printChildren(parent);
}

// Prints all grandparents of a person, given a persons name
void PersonHashTable::printGrandParents(string personName)
{
    int index = hash( personName );
    Person * person = array[ index ].getPerson( personName );
    array[ index ].printGrandParents(person);
}

// Prints all siblings of a person, given a persons name
void PersonHashTable::printSiblings(string personName)
{
    int index = hash( personName );
    Person * person = array[ index ].getPerson( personName );
    array[ index ].printSiblings(person);    
}

// Prints a statement of truth given a persons name and the parents gender
void PersonHashTable::isParentAlive(string personName, bool parentGender)
{
    int index = hash( personName );
    Person * person = array[ index ].getPerson( personName );
    array[ index ].isParentAlive(person, parentGender); 
}

// Prints a statement of truth given a persons name and the grandparents name
void PersonHashTable::isGrandParentAlive(string personName, string grandParentName)
{
    int index = hash( personName );
    Person * person = array[ index ].getPerson( personName );
    array[ index ].isGrandParentAlive(person, grandParentName); 
}

// Prints a statement of truth given a persons name and searches for parents siblings
void PersonHashTable::hasParentsSiblings(string personName, bool parentsSiblingsGender)
{
    int index = hash( personName );
    Person * person = array[ index ].getPerson( personName );
    array[ index ].hasParentsSiblings(person, parentsSiblingsGender); 
}

// Display the contents of the Hash Table to console window.
void PersonHashTable::printTable()
{
    cout << "\n\nHash Table:\n";
    for ( int i = 0; i < length; i++ )
    {
        cout << "Bucket " << i + 1 << ": ";
        array[i].printList();
    }
}

// Returns the number of locations in the Hash Table.
int PersonHashTable::getLength()
{
    return length;
}

// Returns the number of Items in the Hash Table.
int PersonHashTable::getNumberOfItems()
{
    int itemCount = 0;
    for ( int i = 0; i < length; i++ )
    {
        itemCount += array[i].getLength();
    }
    return itemCount;
}

// De-allocates all memory used for the Hash Table.
PersonHashTable::~PersonHashTable()
{
    delete [] array;
}
