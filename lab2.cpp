#include "lab2.h"

// Lab 2 Q1: Swap the values of two integers using pointers.
// Example: int x = 5, y = 10; swap_ints(&x, &y);  // now x = 10, y = 5
void swap_ints(int *a, int *b) {
    int temp = *a;   // save the value a points to before it gets overwritten
    *a = *b;         // a now gets b's value
    *b = temp;       // b gets a's old value from temp
}
