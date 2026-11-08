#ifndef LAB2_H
#define LAB2_H

// Lab 2 (Coding Exam 2): pointers.
// Lab rule: no array indexing like arr[i] -- pointer arithmetic only.
// These are the real versions of the code the player types in Chapters 2, 4, 5 and 6.

// ---------- Part 1 ----------
void swap_ints(int *a, int *b);
void swap_chars(char *a, char *b);
int find_max(int *arr, int size);
void reverse_array(int *arr, int size);
int strlen_ptr(char *str);
int count_vowels(char *str);
void copy_string(char *dest, char *src);
void concat_strings(char *dest, char *src);
int sum_array(int *arr, int size);
int compare_strings(char *str1, char *str2);

// ---------- Part 2: the linked list from the lecture notes ----------
struct Node {
    int data;
    Node *nextPtr;
};

void InsertAtEnd(Node **ptrToHeadPtr, int newData);
void PrintList(Node *headPtr);
void DeleteWithValue(Node **ptrToHeadPtr, int value);

// ---------- Part 2: doubly linked list ----------
// Each node points forwards (nextPointer) and backwards (prevPointer).
// The game keeps its shot log in one of these, so 'text' holds what happened.
class DoublyLinkedNode {
public:
    int data;                        // the shot number
    char text[100];                  // what happened on that shot
    DoublyLinkedNode *prevPointer;
    DoublyLinkedNode *nextPointer;

    DoublyLinkedNode(int value);
};

class DoublyLinkedList {
public:
    DoublyLinkedNode *head;          // public so the game can walk the list

    DoublyLinkedList();
    ~DoublyLinkedList();             // frees every node with delete

    void InsertAtBeginning(int newData);
    void InsertAtEnd(int newData);
    void InsertAtPosition(int newData, int position);
    void clear();
};

#endif
