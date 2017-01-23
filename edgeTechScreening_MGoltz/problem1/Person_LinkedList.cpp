//problem 1 - EdgeTech
//Michael Goltz
//  Person_LinkedList.cpp

//  This is the implementation file containing the Person Linked List class declaration.

#include "Person_LinkedList.h"

// Constructs the empty linked list object.
// Creates the head node and sets length to zero.
PersonLinkedList::PersonLinkedList()
{
    head = new Person;
    head -> next = NULL;
    length = 0;
}

// Inserts a person at the end of the list.
void PersonLinkedList::insertPerson( Person * newPerson/*string name, bool isAlive, bool gender*/ )
{
    
    //Person * newPerson = new Person {name, isAlive, gender, NULL};

    if (!head -> next)
    {
        head -> next = newPerson;
        length++;
        return;
    }
    Person * p = head;
    Person * q = head;
    while (q)
    {
        p = q;
        q = p -> next;
    }
    p -> next = newPerson;
    newPerson -> next = NULL;
    length++;
}

// Creates links between child and parents
void PersonLinkedList::connectChildAndParent( Person * child, Person * parent1, Person * parent2 )
{
    // link parents to child
    child -> parents.push_back(parent1);
    child -> parents.push_back(parent2);
    
    // link child to parents
    parent1 -> children.push_back(child);
    parent2 -> children.push_back(child);
}
void PersonLinkedList::connectChildAndParent( string childName, string parentName1, string parentName2 )
{
    // get links to each person
    Person * child = getPerson(childName);
    Person * parent1 = getPerson(parentName1);
    Person * parent2 = getPerson(parentName2);

    // link parents to child
    child -> parents.push_back(parent1);
    child -> parents.push_back(parent2);
    
    // link child to parents
    parent1 -> children.push_back(child);
    parent2 -> children.push_back(child);
}

// Prints parents of child, given a childs name
// Searches for a person by its keyName.
// iterators parent vector for parent's keyName
void PersonLinkedList::printParents(string childName)
{
    Person * child = getPerson(childName);
    
    if (child -> parents.size() > 0)
    {
        cout << child -> keyName << "'s Parents: \t{ ";
        cout << child -> parents.at(0) -> keyName << ", " ;
        cout << child -> parents.at(1) -> keyName;
        cout << " }" << endl;
    } else cout << child -> keyName << "'s parents are unknown." << endl;
}
void PersonLinkedList::printParents(Person * child)
{
    if (child -> parents.size() > 0)
    {
        cout << child -> keyName << "'s Parents: \t{ ";
        cout << child -> parents.at(0) -> keyName << ", " ;
        cout << child -> parents.at(1) -> keyName;
        cout << " }" << endl;
    } else cout << child -> keyName << "'s parents are unknown." << endl;
}

// Prints children of parent, given a parent name
// Searches for a person by its keyName.
// iterators children to get keyName
void PersonLinkedList::printChildren(string parentName)
{
    Person * parent = getPerson(parentName);

    cout << parent -> keyName << "'s Children: \t{ ";
    for (int i = 0; i < parent -> children.size(); i++)
    {
        cout << parent -> children.at(i) -> keyName;
        if (i < parent -> children.size()-1)
        {
            cout << ", ";
        }
    }
    cout << " }" << endl;
}
void PersonLinkedList::printChildren(Person * parent)
{
    cout << parent -> keyName << "'s Children: \t{ ";
    for (int i = 0; i < parent -> children.size(); i++)
    {
        cout << parent -> children.at(i) -> keyName;
        if (i < parent -> children.size()-1)
        {
            cout << ", ";
        }
    }
    cout << " }" << endl;
}

// Prints all grandparents of a person, given a persons name
// Searches for a person by its keyName.
// iterators through parents to get their parent's keyName
void PersonLinkedList::printGrandParents(string personName)
{
    Person * person = getPerson(personName);

    cout << personName << "'s Grandparents: \t{ ";
    for (int i = 0; i < person -> parents.size(); i++)
    {
        for (int j = 0; j < person -> parents.at(i) -> parents.size(); j++)
        {
            cout << person -> parents.at(i) -> parents.at(j) -> keyName;
            if (j < person -> parents.at(i) -> parents.size()-1)
            {
                cout << ", ";
            }
        }
        if (i < person -> parents.size()-1)
        {
            cout << ", ";
        }
    }
    cout << " }\n" << endl;
}
void PersonLinkedList::printGrandParents(Person * person)
{
    cout << person -> keyName << "'s Grandparents: \t{ ";
    for (int i = 0; i < person -> parents.size(); i++)
    {
        for (int j = 0; j < person -> parents.at(i) -> parents.size(); j++)
        {
            cout << person -> parents.at(i) -> parents.at(j) -> keyName;
            if (j < person -> parents.at(i) -> parents.size()-1)
            {
                cout << ", ";
            }
        }
        if (i < person -> parents.size()-1)
        {
            cout << ", ";
        }
    }
    cout << " }" << endl;
}

// Prints all siblings of a person, given a persons name
// Searches for a person by its keyName.
// references one parent and iterators through children to print keyNames
void PersonLinkedList::printSiblings(string personName)
{
    Person * person = getPerson(personName);

    cout << person -> keyName << "'s Siblings: \t{ ";
    if (person -> parents.at(0) -> children.size() > 1)
    {
        for (int i = 0; i < person -> parents.at(0) -> children.size(); i++)
        {
            if (person -> parents.at(0) -> children.at(i) -> keyName != person -> keyName)
            {
                cout << person -> parents.at(0) -> children.at(i) -> keyName;
                if (i < person -> parents.at(0) -> children.size()-1)
                {
                    cout << ", ";
                }
            }
        }
        cout << " }" << endl;
    }
}
void PersonLinkedList::printSiblings(Person * person)
{
    cout << person -> keyName << "'s Siblings: \t{ ";
    if (person -> parents.at(0) -> children.size() > 1)
    {
        for (int i = 0; i < person -> parents.at(0) -> children.size(); i++)
        {
            if (person -> parents.at(0) -> children.at(i) -> keyName != person -> keyName)
            {
                cout << person -> parents.at(0) -> children.at(i) -> keyName;
                if (i < person -> parents.at(0) -> children.size()-1)
                {
                    cout << ", ";
                }
            }
        }
        cout << " }" << endl;
    }
}

// Prints a statement of truth given a persons name and the parents gender
// Searches for a person by its keyName.
// iterators parent vector to print the right parents gender and isAlive status
void PersonLinkedList::isParentAlive(string personName, bool parentGender)
{
    Person * person = getPerson(personName);

    if (person -> parents.size() > 0)
    {
        cout << person -> keyName << "'s ";
        for (int i = 0; i < person -> parents.size(); i++)
        {
            if (person -> parents.at(i) -> gender == parentGender)
            {
                if (parentGender == MALE)
                {
                    cout << "Father is ";
                    if (person -> parents.at(i) -> isAlive == ALIVE) cout << "alive." << endl;
                    else cout << "deceased." << endl;
                } else {
                    cout << "Mother is ";
                    if (person -> parents.at(i) -> isAlive == ALIVE) cout << "alive." << endl;
                    else cout << "deceased." << endl;
                }

            }
        }
    } else cout << person -> keyName << " parents are unknown." << endl;
}
void PersonLinkedList::isParentAlive(Person * person, bool parentGender)
{
    if (person -> parents.size() > 0)
    {
        cout << person -> keyName << "'s ";
        for (int i = 0; i < person -> parents.size(); i++)
        {
            if (person -> parents.at(i) -> gender == parentGender)
            {
                if (parentGender == MALE)
                {
                    cout << "Father is ";
                    if (person -> parents.at(i) -> isAlive == ALIVE) cout << "alive." << endl;
                    else cout << "deceased." << endl;
                } else {
                    cout << "Mother is ";
                    if (person -> parents.at(i) -> isAlive == ALIVE) cout << "alive." << endl;
                    else cout << "deceased." << endl;
                }

            }
        }
    } else cout << person -> keyName << " parents are unknown." << endl;
}

// Prints a statement of truth given a persons name and the grandparents name
// Searches for a person by its keyName.
// iterators parent vector of parents to find grandparent's name that matches input
void PersonLinkedList::isGrandParentAlive(string personName, string grandParentName)
{
    Person * person = getPerson(personName);

    bool found = false;

    if (person -> parents.size() > 0)
    {
        cout << person -> keyName << "'s ";
        for (int i = 0; i < person -> parents.size(); i++)
        {
            for (int j = 0; j < person -> parents.at(i) -> parents.size(); j++)
            {
                if (person -> parents.at(i) -> parents.at(j) -> keyName == grandParentName)
                {
                    found = true;
                    if (person -> parents.at(i) -> parents.at(j) -> gender == MALE)
                    {
                        cout << "Grandfather " << grandParentName << " is ";
                        if (person -> parents.at(i) -> parents.at(j) -> isAlive == ALIVE) cout << "alive." << endl;
                        else cout << "deceased." << endl;
                    } else {
                        cout << "Grandmother " << grandParentName << " is ";
                        if (person -> parents.at(i) -> parents.at(j) -> isAlive == ALIVE) cout << "alive." << endl;
                        else cout << "deceased." << endl;
                    }

                }
            }
        }
        if (found == false) cout << "grandparent " << grandParentName << " does not exist."  << endl;
    } else cout << person -> keyName << "'s parents are unknown." << endl;
}
void PersonLinkedList::isGrandParentAlive(Person * person, string grandParentName)
{
    bool found = false;

    if (person -> parents.size() > 0)
    {
        cout << person -> keyName << "'s ";
        for (int i = 0; i < person -> parents.size(); i++)
        {
            for (int j = 0; j < person -> parents.at(i) -> parents.size(); j++)
            {
                if (person -> parents.at(i) -> parents.at(j) -> keyName == grandParentName)
                {
                    found = true;
                    if (person -> parents.at(i) -> parents.at(j) -> gender == MALE)
                    {
                        cout << "Grandfather " << grandParentName << " is ";
                        if (person -> parents.at(i) -> parents.at(j) -> isAlive == ALIVE) cout << "alive." << endl;
                        else cout << "deceased." << endl;
                    } else {
                        cout << "Grandmother " << grandParentName << " is ";
                        if (person -> parents.at(i) -> parents.at(j) -> isAlive == ALIVE) cout << "alive." << endl;
                        else cout << "deceased." << endl;
                    }

                }
            }
        }
        if (found == false) cout << "grandparent " << grandParentName << " does not exist."  << endl;
    } else cout << person -> keyName << "'s parents are unknown." << endl;
}

// Prints a statement of truth given a persons name and searches for parents siblings
// Searches for a person by its keyName.
void PersonLinkedList::hasParentsSiblings(string personName, bool parentsSiblingsGender)
{
    Person * person = getPerson(personName);

    bool found = false;

    if (person -> parents.size() > 0)
    {
        cout << person -> keyName << "'s ";
        for (int i = 0; i < person -> parents.size(); i++)
        {
            for (int k = 0; k < person -> parents.at(i) -> parents.at(0) -> children.size(); k++)
            {
                if (person -> parents.at(i) -> parents.at(0) -> children.at(k) != person -> parents.at(i))
                {
                    if (person -> parents.at(i) -> parents.at(0) -> children.at(k) -> gender == parentsSiblingsGender)
                    {
                        found = true;
                        string name = person -> parents.at(i) -> parents.at(0) -> children.at(k) -> keyName;
                        if (person -> parents.at(i) -> parents.at(0) -> children.at(k) -> gender == MALE)
                        {
                            cout << "has atleast an Uncle " << name << endl;
                        } else {
                            cout << "has atleast an Aunt " << name << endl;
                        }
                    }
                }
            }
        }
        if (found == false) cout << " Parent's siblings do not exist." << endl;
    } else cout << person -> keyName << "'s parents are unknown." << endl;
}
void PersonLinkedList::hasParentsSiblings(Person * person, bool parentsSiblingsGender)
{
    bool found = false;

    if (person -> parents.size() > 0)
    {
        cout << person -> keyName << "'s ";
        for (int i = 0; i < person -> parents.size(); i++)
        {
            for (int k = 0; k < person -> parents.at(i) -> parents.at(0) -> children.size(); k++)
            {
                if (person -> parents.at(i) -> parents.at(0) -> children.at(k) != person -> parents.at(i))
                {
                    if (person -> parents.at(i) -> parents.at(0) -> children.at(k) -> gender == parentsSiblingsGender)
                    {
                        found = true;
                        string name = person -> parents.at(i) -> parents.at(0) -> children.at(k) -> keyName;
                        if (person -> parents.at(i) -> parents.at(0) -> children.at(k) -> gender == MALE)
                        {
                            cout << "has atleast an Uncle " << name << endl;
                        } else {
                            cout << "has atleast an Aunt " << name << endl;
                        }
                    }
                }
            }
        }
        if (found == false) cout << " Parent's siblings do not exist." << endl;
    } else cout << person -> keyName << "'s parents are unknown." << endl;
}


// Searches for a person by its keyName.
// Returns a reference to first match.
// Returns a NULL pointer if no match is found.
Person * PersonLinkedList::getPerson( string personName )
{
    Person * p = head;
    Person * q = head;
    while (q)
    {
        p = q;
        if ((p != head) && (p -> keyName == personName))
            return p;
        q = p -> next;
    }
    return NULL;
}

// Displays list contents to the console window.
void PersonLinkedList::printList()
{
    if (length == 0)
    {
        cout << "\n{ }\n";
        return;
    }
    Person * p = head;
    Person * q = head;
    cout << "\n{ ";
    while (q)
    {
        p = q;
        if (p != head)
        {
            cout << p -> keyName;
            if (p -> next) cout << ", ";
            else cout << " ";
        }
        q = p -> next;
    }
    cout << "}\n" << endl;
}

// Returns the length of the list.
int PersonLinkedList::getLength()
{
    return length;
}

// De-allocates list memory when the program terminates.
PersonLinkedList::~PersonLinkedList()
{
    Person * p = head;
    Person * q = head;
    while (q)
    {
        p = q;
        q = p -> next;
        if (q) delete p;
    }
}
