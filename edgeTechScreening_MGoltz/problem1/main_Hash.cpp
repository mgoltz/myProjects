//problem 1 - EdgeTech
//Michael Goltz

#include <stdbool.h>
#include "Person_LinkedList.h"
#include "Person_HashTable.h"
using namespace std;

int main()
{
    // person data structure
    // name, isAlive ALIVE=true, gender MALE=true, FEMALE=false
    Person * p1 = new Person {"Flo", !ALIVE, FEMALE, NULL};     // grandparent
    Person * p2 = new Person {"Louis", !ALIVE, MALE, NULL};     // grandparent
    Person * p3 = new Person {"Dave", ALIVE, MALE, NULL};       // grandparent
    Person * p4 = new Person {"Monica", !ALIVE, FEMALE, NULL};  // grandparent
    Person * p5 = new Person { "Lorelei", ALIVE, FEMALE, NULL};// parent/ child of grandparent
    Person * p6 = new Person { "Randel", ALIVE, MALE, NULL};    // parent/ child of grandparent
    Person * p7 = new Person { "Bob", ALIVE, MALE, NULL};       // parent/ child of grandparent
    Person * p8 = new Person { "Michael", ALIVE, MALE, NULL};   // child
    Person * p9 = new Person { "Sarah", ALIVE, FEMALE, NULL};   // child
    Person * p10 = new Person { "Brian", ALIVE, MALE, NULL};    // child
    Person * p11 = new Person { "Peter", !ALIVE, MALE, NULL};   // parent
    Person * p12 = new Person { "Jerrie", !ALIVE, FEMALE, NULL}; // child of grandparent
    Person * p13 = new Person { "Francis", ALIVE, MALE, NULL};  // child of grandparent

    //**************************************************************
    // Hash list implementation
    //**************************************************************
    PersonHashTable myTreeHash;

    myTreeHash.insertPerson(p1);
    myTreeHash.insertPerson(p2);
    myTreeHash.insertPerson(p3);
    myTreeHash.insertPerson(p4);
    myTreeHash.insertPerson(p5);
    myTreeHash.insertPerson(p6);
    myTreeHash.insertPerson(p7);
    myTreeHash.insertPerson(p8);
    myTreeHash.insertPerson(p9);
    myTreeHash.insertPerson(p10);
    myTreeHash.insertPerson(p11);
    myTreeHash.insertPerson(p12);
    myTreeHash.insertPerson(p13);

    myTreeHash.printTable();
    cout << endl;

    // search people in list and set parent and child links
    myTreeHash.connectChildAndParent( p8 -> keyName,  p5 -> keyName, p6 -> keyName );   // chidlren and parents
    myTreeHash.connectChildAndParent( p10 -> keyName, p5 -> keyName, p7 -> keyName );       // children and parents
    myTreeHash.connectChildAndParent( p5 -> keyName,  p3 -> keyName, p4 -> keyName );   // grandparents
    myTreeHash.connectChildAndParent( p6 -> keyName,  p2 -> keyName, p1 -> keyName );       // grandparents
    myTreeHash.connectChildAndParent( p9 -> keyName,  p5 -> keyName, p11 -> keyName );  // children and parents
    myTreeHash.connectChildAndParent( p12 -> keyName, p3 -> keyName, p4 -> keyName );       // child of grandparents
    myTreeHash.connectChildAndParent( p13 -> keyName, p1 -> keyName, p2 -> keyName );       // child of grandparents

    // prints parents related to name
    myTreeHash.printParents( "Michael" );
    myTreeHash.printParents( "Lorelei" );
    myTreeHash.printParents( "Flo" );
    cout << endl;

    // prints children related to name
    myTreeHash.printChildren( "Lorelei" );
    myTreeHash.printChildren( "Bob" );
    cout << endl;

    // prints grandparents related to name
    myTreeHash.printGrandParents( "Michael" );
    cout << endl;

    // prints siblings related to name
    myTreeHash.printSiblings( "Michael" );
    myTreeHash.printSiblings( "Sarah" );
    cout << endl;

    // prints alive truth statement about parents given name and gender
    myTreeHash.isParentAlive( "Sarah", MALE );
    myTreeHash.isParentAlive( "Sarah", FEMALE );
    myTreeHash.isParentAlive( "Flo", FEMALE );
    cout << endl;

    // prints alive truth statement about grandparents given name and grandparent name
    myTreeHash.isGrandParentAlive( "Brian", "Monica" );
    myTreeHash.isGrandParentAlive( "Brian", "Dave" );
    myTreeHash.isGrandParentAlive( "Monica", "Flo" );
    myTreeHash.isGrandParentAlive( "Brian", "Sue" );
    cout << endl;

    // prints confirmsation of first occurance of aunt or uncle
    myTreeHash.hasParentsSiblings( "Michael", MALE );
    myTreeHash.hasParentsSiblings( "Michael", FEMALE );

}