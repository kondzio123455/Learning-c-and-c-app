#include "lab1.h"
#include "lab2.h"   // reverse() reuses swap_ints from Lab 2

// Q13: Count how many times value appears in the array.
// The game uses this to count how many of your group are left.
int frequencyCount(int size, int arr[], int value) {
    int count = 0;
    for (int i = 0; i < size; i++) {
        if (arr[i] == value) count++;
    }
    return count;
}

// Q14: Count duplicates by comparing every pair once.
// The inner loop starts at i + 1, so an element is never compared with itself.
// (This simple version counts matching pairs.)
// The game uses this to check the rack has no ball twice.
int countDuplicates(int size, int arr[]) {
    int count = 0;
    for (int i = 0; i < size; i++) {
        for (int j = i + 1; j < size; j++) {
            if (arr[i] == arr[j]) count++;
        }
    }
    return count;
}

// Q15: Reverse the array in place: swap the ends and work inwards to the middle.
void reverse(int size, int arr[]) {
    for (int i = 0; i < size / 2; i++) {
        swap_ints(&arr[i], &arr[size - 1 - i]);
    }
}

// Q16: Rotate left by one: {1, 2, 3, 4} becomes {2, 3, 4, 1}.
// The lab's starter returns an int, so this returns the element that moved
// to the end (or -1 if the array is empty).
int rotateLeft(int size, int arr[]) {
    if (size <= 0) return -1;

    int first = arr[0];
    for (int i = 0; i < size - 1; i++) {
        arr[i] = arr[i + 1];
    }
    arr[size - 1] = first;
    return first;
}

// Q17: Are there two different movies whose lengths add up to the flight length?
bool twoMovies(int flightLength, int movieLengths[], int size) {
    for (int i = 0; i < size; i++) {
        for (int j = i + 1; j < size; j++) {
            if (movieLengths[i] + movieLengths[j] == flightLength) return true;
        }
    }
    return false;
}

// Q18: Count words: a word starts where a letter comes after a non-letter.
int wordCounter(int size, char characters[]) {
    int count = 0;
    bool inWord = false;
    for (int i = 0; i < size; i++) {
        char c = characters[i];
        bool isLetter = (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
        if (isLetter && !inWord) count++;
        inWord = isLetter;
    }
    return count;
}
