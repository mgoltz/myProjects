//problem 1 - EdgeTech
//Michael Goltz

#include <stdbool.h>
#include "Person_LinkedList.h"
using namespace std;

int main()
{
	// person data structure
	// name, isAlive ALIVE=true, gender MALE=true, FEMALE=false
	Person * p1 = new Person {"Flo", !ALIVE, FEMALE, NULL};		// grandparent
	Person * p2 = new Person {"Louis", !ALIVE, MALE, NULL};		// grandparent
	Person * p3 = new Person {"Dave", ALIVE, MALE, NULL};		// grandparent
	Person * p4 = new Person {"Monica", !ALIVE, FEMALE, NULL};	// grandparent
	Person * p5 = new Person { "Lorelei", ALIVE, FEMALE, NULL};// parent/ child of grandparent
	Person * p6 = new Person { "Randel", ALIVE, MALE, NULL};	// parent/ child of grandparent
	Person * p7 = new Person { "Bob", ALIVE, MALE, NULL};		// parent/ child of grandparent
	Person * p8 = new Person { "Michael", ALIVE, MALE, NULL};	// child
	Person * p9 = new Person { "Sarah", ALIVE, FEMALE, NULL};	// child
	Person * p10 = new Person { "Brian", ALIVE, MALE, NULL};	// child
	Person * p11 = new Person { "Peter", !ALIVE, MALE, NULL};	// parent
	Person * p12 = new Person { "Jerrie", !ALIVE, FEMALE, NULL}; // child of grandparent
	Person * p13 = new Person { "Francis", ALIVE, MALE, NULL};	// child of grandparent

	//**************************************************************
	// Linked list implementation
	//**************************************************************
	PersonLinkedList myTreeList;

	myTreeList.insertPerson(p1);
	myTreeList.insertPerson(p2);
	myTreeList.insertPerson(p3);
	myTreeList.insertPerson(p4);
	myTreeList.insertPerson(p5);
	myTreeList.insertPerson(p6);
	myTreeList.insertPerson(p7);
	myTreeList.insertPerson(p8);
	myTreeList.insertPerson(p9);
	myTreeList.insertPerson(p10);
	myTreeList.insertPerson(p11);
	myTreeList.insertPerson(p12);
	myTreeList.insertPerson(p13);

	// search people in list and set parent and child links
	myTreeList.connectChildAndParent( "Michael", "Lorelei", "Randel" );	// chidlren and parents
	myTreeList.connectChildAndParent( "Brian", "Lorelei", "Bob" );		// children and parents
	myTreeList.connectChildAndParent( "Lorelei", "Dave", "Monica" );	// grandparents
	myTreeList.connectChildAndParent( "Randel", "Louis", "Flo" );		// grandparents
	myTreeList.connectChildAndParent( "Sarah", "Lorelei", "Peter" );	// children and parents
	myTreeList.connectChildAndParent( "Jerrie", "Dave", "Monica" );		// child of grandparents
	myTreeList.connectChildAndParent( "Francis", "Flo", "Louis" );		// child of grandparents
    
    // print list of people
    myTreeList.printList();
    cout << endl;

    // prints parents related to name
	myTreeList.printParents( "Michael" );
	myTreeList.printParents( "Lorelei" );
	myTreeList.printParents( "Flo" );
	cout << endl;

	// prints children related to name
	myTreeList.printChildren( "Lorelei" );
	myTreeList.printChildren( "Bob" );
	cout << endl;

	// prints grandparents related to name
	myTreeList.printGrandParents( "Michael" );
	cout << endl;

	// prints siblings related to name
	myTreeList.printSiblings( "Michael" );
	myTreeList.printSiblings( "Sarah" );
	cout << endl;

	// prints alive truth statement about parents given name and gender
	myTreeList.isParentAlive( "Sarah", MALE );
	myTreeList.isParentAlive( "Sarah", FEMALE );
	myTreeList.isParentAlive( "Flo", FEMALE );
	cout << endl;

	// prints alive truth statement about grandparents given name and grandparent name
	myTreeList.isGrandParentAlive( "Brian", "Monica" );
	myTreeList.isGrandParentAlive( "Brian", "Dave" );
	myTreeList.isGrandParentAlive( "Monica", "Flo" );
	myTreeList.isGrandParentAlive( "Brian", "Sue" );
	cout << endl;

	// prints confirmsation of first occurance of aunt or uncle
	myTreeList.hasParentsSiblings( "Michael", MALE );
	myTreeList.hasParentsSiblings( "Michael", FEMALE );

}