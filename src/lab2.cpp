#include "lab2.h"
#include <iostream>
using namespace std;

// ============================================================
// Part 1: pointer functions (no arr[i] anywhere)
// ============================================================

// Q1: Swap two ints. The game uses this to switch whose turn it is.
void swap_ints(int *a, int *b) {
    int temp = *a;   // save a's value before it gets overwritten
    *a = *b;
    *b = temp;
}

// Q2: Swap two chars. The game uses this to switch the players' groups ('S' and 'T').
void swap_chars(char *a, char *b) {
    char temp = *a;
    *a = *b;
    *b = temp;
}

// Q3: Biggest value in the array. The game uses this for "highest ball left".
int find_max(int *arr, int size) {
    int max = *arr;                  // start with the first element
    for (int i = 1; i < size; i++) {
        if (*(arr + i) > max) max = *(arr + i);
    }
    return max;
}

// Q4: Reverse with two pointers that move towards each other.
// The game uses this to show your potted balls newest first.
void reverse_array(int *arr, int size) {
    int *start = arr;
    int *end = arr + size - 1;       // the last element
    while (start < end) {
        swap_ints(start, end);
        start++;
        end--;
    }
}

// Q5: Length of a string: count characters until the '\0' at the end.
// The game uses this to centre player names (every letter is the same width).
int strlen_ptr(char *str) {
    int length = 0;
    while (*str != '\0') {
        length++;
        str++;
    }
    return length;
}

// Q6: Count the vowels, upper or lower case.
int count_vowels(char *str) {
    int count = 0;
    while (*str != '\0') {
        char c = *str;
        if (c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u' ||
            c == 'A' || c == 'E' || c == 'I' || c == 'O' || c == 'U') {
            count++;
        }
        str++;
    }
    return count;
}

// Q7: Copy src into dest one character at a time, then end dest with '\0'.
// The game uses this to start the win message with the winner's name.
void copy_string(char *dest, char *src) {
    while (*src != '\0') {
        *dest = *src;
        dest++;
        src++;
    }
    *dest = '\0';
}

// Q8: Add src onto the end of dest: move to dest's '\0', then copy from there.
// The game uses this to add " wins!" to the win message.
void concat_strings(char *dest, char *src) {
    while (*dest != '\0') dest++;
    copy_string(dest, src);
}

// Q9: Add up the array. The game uses this for the points on the results screen.
int sum_array(int *arr, int size) {
    int sum = 0;
    for (int i = 0; i < size; i++) {
        sum += *(arr + i);
    }
    return sum;
}

// Q10: 0 if the strings are equal, negative if str1 comes first, positive if after.
// The game's answer checker uses this to compare your answer with the right one.
int compare_strings(char *str1, char *str2) {
    while (*str1 != '\0' && *str1 == *str2) {
        str1++;
        str2++;
    }
    return *str1 - *str2;
}

// ============================================================
// Part 2: the singly linked list from the lecture notes
// ============================================================

void InsertAtEnd(Node **ptrToHeadPtr, int newData) {
    Node *newNode = new Node();
    newNode->data = newData;
    newNode->nextPtr = nullptr;

    // Empty list: the new node becomes the head
    if (*ptrToHeadPtr == nullptr) {
        *ptrToHeadPtr = newNode;
        return;
    }

    // Otherwise walk to the last node and link the new one after it
    Node *curNodePtr = *ptrToHeadPtr;
    while (curNodePtr->nextPtr != nullptr) {
        curNodePtr = curNodePtr->nextPtr;
    }
    curNodePtr->nextPtr = newNode;
}

void PrintList(Node *headPtr) {
    Node *curNodePtr = headPtr;
    while (curNodePtr != nullptr) {
        cout << curNodePtr->data << " ";
        curNodePtr = curNodePtr->nextPtr;
    }
    cout << endl;
}

// Deletes the first node that holds value (from the lecture notes)
void DeleteWithValue(Node **ptrToHeadPtr, int value) {
    if (*ptrToHeadPtr == nullptr) {
        cout << "List is empty." << endl;
        return;
    }
    Node *curNodePtr = *ptrToHeadPtr;

    // If the head node itself holds the value to be deleted
    if (curNodePtr != nullptr && curNodePtr->data == value) {
        *ptrToHeadPtr = curNodePtr->nextPtr;   // Change head to the next node
        delete curNodePtr;                     // Free memory
        return;
    }

    // Search for the node that holds the value, and keep track of the previous node
    Node *prev = nullptr;
    while (curNodePtr != nullptr && curNodePtr->data != value) {
        prev = curNodePtr;
        curNodePtr = curNodePtr->nextPtr;
    }
    if (curNodePtr == nullptr) {
        cout << "Value not found in the list." << endl;
        return;
    }
    prev->nextPtr = curNodePtr->nextPtr;   // Unlink the node from the linked list
    delete curNodePtr;                     // Free memory
}

// ============================================================
// Part 2: doubly linked list
// ============================================================

DoublyLinkedNode::DoublyLinkedNode(int value) {
    data = value;
    text[0] = '\0';
    prevPointer = nullptr;
    nextPointer = nullptr;
}

DoublyLinkedList::DoublyLinkedList() {
    head = nullptr;
}

DoublyLinkedList::~DoublyLinkedList() {
    clear();
}

void DoublyLinkedList::InsertAtBeginning(int newData) {
    DoublyLinkedNode *newNode = new DoublyLinkedNode(newData);
    newNode->nextPointer = head;          // new node points forward at the old head
    if (head != nullptr) {
        head->prevPointer = newNode;      // old head points back at the new node
    }
    head = newNode;
}

void DoublyLinkedList::InsertAtEnd(int newData) {
    DoublyLinkedNode *newNode = new DoublyLinkedNode(newData);
    if (head == nullptr) {
        head = newNode;
        return;
    }

    DoublyLinkedNode *curNodePtr = head;
    while (curNodePtr->nextPointer != nullptr) {
        curNodePtr = curNodePtr->nextPointer;
    }
    curNodePtr->nextPointer = newNode;    // last node points forward at the new one
    newNode->prevPointer = curNodePtr;    // new node points back at the old last node
}

// Position 0 is the front. A position past the end adds the node at the end.
void DoublyLinkedList::InsertAtPosition(int newData, int position) {
    if (position <= 0 || head == nullptr) {
        InsertAtBeginning(newData);
        return;
    }

    // Walk to the node just before the position
    DoublyLinkedNode *curNodePtr = head;
    for (int i = 0; i < position - 1 && curNodePtr->nextPointer != nullptr; i++) {
        curNodePtr = curNodePtr->nextPointer;
    }

    // The new node goes between curNodePtr and the node after it
    DoublyLinkedNode *newNode = new DoublyLinkedNode(newData);
    newNode->prevPointer = curNodePtr;
    newNode->nextPointer = curNodePtr->nextPointer;
    if (curNodePtr->nextPointer != nullptr) {
        curNodePtr->nextPointer->prevPointer = newNode;
    }
    curNodePtr->nextPointer = newNode;
}

// Deletes every node, so no memory is leaked
void DoublyLinkedList::clear() {
    DoublyLinkedNode *current = head;
    while (current != nullptr) {
        DoublyLinkedNode *temp = current;
        current = current->nextPointer;
        delete temp;
    }
    head = nullptr;
}
