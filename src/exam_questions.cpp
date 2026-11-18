#include "exam_questions.h"
#include <cstdio>
#include <cstring>

// ============================================================
// The question bank. Descriptions and prototypes are copied from the labs.
// Lab 1 Q1-Q4 use the tests from Lab 1's own main; the rest use the same style.
// ============================================================

const char *INCLUDES =
    "#include <stdio.h>\n"
    "#include <stdbool.h>\n"
    "#include <string.h>\n"
    "#include <iostream>\n"
    "using namespace std;\n";

// Premade test helpers for the singly linked list (from the lecture notes)
const char *NODE_SUPPORT = R"CODE(struct Node {
    int data;      // Stores data
    Node* nextPtr; // Pointer to the next node
};

// Premade helpers for the tests
int valueAt(Node* headPtr, int index) {   // -1 if the list is too short
    Node* curNodePtr = headPtr;
    for (int i = 0; i < index && curNodePtr != nullptr; i++) curNodePtr = curNodePtr->nextPtr;
    if (curNodePtr == nullptr) return -1;
    return curNodePtr->data;
}
int listLength(Node* headPtr) {
    int length = 0;
    for (Node* curNodePtr = headPtr; curNodePtr != nullptr; curNodePtr = curNodePtr->nextPtr) length++;
    return length;
}
Node* makeList(int values[], int count) {
    Node* headPtr = nullptr;
    for (int i = count - 1; i >= 0; i--) {
        Node* newNode = new Node();
        newNode->data = values[i];
        newNode->nextPtr = headPtr;
        headPtr = newNode;
    }
    return headPtr;
})CODE";

const char *DOUBLY_SUPPORT = R"CODE(class DoublyLinkedNode {
public:
    int data;
    DoublyLinkedNode *prevPointer;
    DoublyLinkedNode *nextPointer;
    DoublyLinkedNode(int newData) {
        data = newData;
        prevPointer = nullptr;
        nextPointer = nullptr;
    }
};

class DoublyLinkedList {
public:
    DoublyLinkedNode *head;
    DoublyLinkedList() { head = nullptr; }
    void InsertAtBeginning(int newData);
    void InsertAtEnd(int newData);
    void InsertAtPosition(int newData, int position);
};

// Premade helpers for the tests
int valueAt(DoublyLinkedList *list, int index) {   // -1 if the list is too short
    DoublyLinkedNode *curNodePtr = list->head;
    for (int i = 0; i < index && curNodePtr != nullptr; i++) curNodePtr = curNodePtr->nextPointer;
    if (curNodePtr == nullptr) return -1;
    return curNodePtr->data;
}
int listLength(DoublyLinkedList *list) {
    int length = 0;
    for (DoublyLinkedNode *n = list->head; n != nullptr; n = n->nextPointer) length++;
    return length;
}
bool linksOK(DoublyLinkedList *list) {   // every prevPointer points back at the node before it
    if (list->head != nullptr && list->head->prevPointer != nullptr) return false;
    for (DoublyLinkedNode *n = list->head; n != nullptr; n = n->nextPointer) {
        if (n->nextPointer != nullptr && n->nextPointer->prevPointer != n) return false;
    }
    return true;
}
void buildList(DoublyLinkedList *list, int values[], int count) {   // links the nodes directly
    DoublyLinkedNode *last = nullptr;
    for (int i = 0; i < count; i++) {
        DoublyLinkedNode *n = new DoublyLinkedNode(values[i]);
        n->prevPointer = last;
        if (last == nullptr) list->head = n;
        else last->nextPointer = n;
        last = n;
    }
})CODE";

#define TEST(condition) "    if (" condition ") printf(\"yes\\n\"); else printf(\"no\\n\");\n"

ExamQuestion questions[EXAM_QUESTION_COUNT] = {
    // ==================== Lab 1 ====================
    { PART_LAB1, "Lab 1 Q1", "isLeapYear",
      "Check if an inputted year is a Leap Year.\nA leap year is divisible by 4, but not by 100 unless also divisible by 400.",
      "",
      "bool isLeapYear(int year) {\n    return true;\n}",
      "    printf(\"Leapyear\\n\");\n" TEST("!isLeapYear(1700)") TEST("isLeapYear(1600)") TEST("isLeapYear(2016)") TEST("!isLeapYear(2019)"),
      "bool isLeapYear(int year) {\n    if (year % 400 == 0) return true;\n    if (year % 100 == 0) return false;\n    return year % 4 == 0;\n}",
      false },
    { PART_LAB1, "Lab 1 Q2", "Reversed and isAPalindrome",
      "Reversed (helper): Reverse the digits of a number and return the reversed integer.\nE.g., 123 -> 321.\n\n"
      "isAPalindrome: Check Whether a Number is Palindrome or Not.\nA palindrome reads the same forwards and backwards (e.g., 121).",
      "",
      "int Reversed(int testNumber) {\n    return 0;\n}\n\nbool isAPalindrome(int testNumber) {\n    return false;\n}",
      "    printf(\"NumberReversed\\n\");\n" TEST("Reversed(123) == 321") TEST("Reversed(1) == 1") TEST("Reversed(1234) == 4321")
      "    printf(\"Palindrome\\n\");\n" TEST("isAPalindrome(1)") TEST("isAPalindrome(121)") TEST("!isAPalindrome(1213)"),
      "int Reversed(int testNumber) {\n    int reversed = 0;\n    while (testNumber > 0) {\n        reversed = reversed * 10 + testNumber % 10;\n"
      "        testNumber = testNumber / 10;\n    }\n    return reversed;\n}\n\nbool isAPalindrome(int testNumber) {\n    return Reversed(testNumber) == testNumber;\n}",
      false },
    { PART_LAB1, "Lab 1 Q3", "isAPrimeNumber",
      "Check Whether a Number is Prime or Not.\nA prime number is only divisible by itself and 1.\n\n"
      "(Lab 1's main tests 2147483647, so checking every number up to it takes too long: think about where you can stop.)",
      "",
      "bool isAPrimeNumber(int numbertoTest) {\n    return false;\n}",
      "    printf(\"Prime Number\\n\");\n" TEST("isAPrimeNumber(3)") TEST("isAPrimeNumber(2147483647)") TEST("!isAPrimeNumber(4)") TEST("!isAPrimeNumber(91)"),
      "bool isAPrimeNumber(int numbertoTest) {\n    if (numbertoTest < 2) return false;\n    for (int i = 2; i <= numbertoTest / i; i++) {\n"
      "        if (numbertoTest % i == 0) return false;\n    }\n    return true;\n}",
      false },
    { PART_LAB1, "Lab 1 Q4", "convertBinarytoDecimal",
      "Convert Binary Number to Decimal without using the shift operator.\nE.g., 110 -> 6.",
      "",
      "int convertBinarytoDecimal(int binaryNumber) {\n    return 0;\n}",
      "    printf(\"Binary to Decimal\\n\");\n" TEST("convertBinarytoDecimal(110) == 6") TEST("convertBinarytoDecimal(111) == 7")
      TEST("convertBinarytoDecimal(1) == 1") TEST("convertBinarytoDecimal(1010) == 10"),
      "int convertBinarytoDecimal(int binaryNumber) {\n    int decimal = 0;\n    int placeValue = 1;\n    while (binaryNumber > 0) {\n"
      "        decimal = decimal + (binaryNumber % 10) * placeValue;\n        placeValue = placeValue * 2;\n        binaryNumber = binaryNumber / 10;\n    }\n    return decimal;\n}",
      false },
    { PART_LAB1, "Lab 1 Q8", "find",
      "Find an element in an array and print the index where it was found,\nor -1 if not found.\n\n(The tests check the index find returns.)",
      "",
      "int find(int size, int arr[], int toFind) {\n    return -1;\n}",
      "    printf(\"find\\n\");\n    int arr[5] = { 4, 8, 15, 16, 23 };\n" TEST("find(5, arr, 15) == 2") TEST("find(5, arr, 4) == 0") TEST("find(5, arr, 99) == -1"),
      "int find(int size, int arr[], int toFind) {\n    for (int i = 0; i < size; i++) {\n        if (arr[i] == toFind) {\n"
      "            printf(\"%d\\n\", i);\n            return i;\n        }\n    }\n    printf(\"-1\\n\");\n    return -1;\n}",
      false },
    { PART_LAB1, "Lab 1 Q9", "find2ndLargest",
      "Find and return the second largest element in an array of positive\nintegers; returns -1 if it can't find one.",
      "",
      "int find2ndLargest(int size, int arr[]) {\n    return -1;\n}",
      "    printf(\"find2ndLargest\\n\");\n    int arr1[5] = { 3, 9, 2, 8, 5 };\n    int arr2[2] = { 10, 4 };\n    int arr3[1] = { 7 };\n"
      TEST("find2ndLargest(5, arr1) == 8") TEST("find2ndLargest(2, arr2) == 4") TEST("find2ndLargest(1, arr3) == -1"),
      "int find2ndLargest(int size, int arr[]) {\n    int largest = -1;\n    int second = -1;\n    for (int i = 0; i < size; i++) {\n"
      "        if (arr[i] > largest) {\n            second = largest;\n            largest = arr[i];\n"
      "        } else if (arr[i] > second && arr[i] != largest) {\n            second = arr[i];\n        }\n    }\n    return second;\n}",
      false },
    { PART_LAB1, "Lab 1 Q10", "copyArraytoArray",
      "Copy all elements from arr1 to arr2. Both arrays are the same size.",
      "",
      "void copyArraytoArray(int size, int arr1[], int arr2[]) {\n    return;\n}",
      "    printf(\"copyArraytoArray\\n\");\n    int arr1[5] = { 1, 2, 3, 4, 5 };\n    int arr2[5] = { 0, 0, 0, 0, 0 };\n    copyArraytoArray(5, arr1, arr2);\n"
      TEST("arr2[0] == 1 && arr2[2] == 3 && arr2[4] == 5") TEST("arr1[0] == 1 && arr1[4] == 5"),
      "void copyArraytoArray(int size, int arr1[], int arr2[]) {\n    for (int i = 0; i < size; i++) {\n        arr2[i] = arr1[i];\n    }\n}",
      false },
    { PART_LAB1, "Lab 1 Q11", "insertElement",
      "Insert an element at a specified index in the array, shifting elements\nto the right. 'count' tracks the number of elements and cannot exceed 'size'.\n"
      "Returns true if an element was inserted, otherwise false.",
      "",
      "bool insertElement(int& size, int& count, int arr[], int elementToInsert, int insertIndex) {\n    return false;\n}",
      "    printf(\"insertElement\\n\");\n    int size = 6;\n    int count = 4;\n    int arr[6] = { 1, 2, 4, 5 };\n"
      "    bool inserted = insertElement(size, count, arr, 3, 2);\n"
      TEST("inserted") TEST("arr[0] == 1 && arr[1] == 2 && arr[2] == 3 && arr[3] == 4 && arr[4] == 5") TEST("count == 5")
      "    int fullSize = 3;\n    int fullCount = 3;\n    int full[3] = { 7, 8, 9 };\n" TEST("!insertElement(fullSize, fullCount, full, 1, 0)")
      TEST("!insertElement(size, count, arr, 1, 9)"),
      "bool insertElement(int& size, int& count, int arr[], int elementToInsert, int insertIndex) {\n"
      "    if (count >= size || insertIndex < 0 || insertIndex > count) return false;\n"
      "    for (int i = count; i > insertIndex; i--) {\n        arr[i] = arr[i - 1];\n    }\n"
      "    arr[insertIndex] = elementToInsert;\n    count++;\n    return true;\n}",
      false },
    { PART_LAB1, "Lab 1 Q12", "deleteElement",
      "Delete an element at a specified position, shifting higher elements\nto the left by one. 'count' tracks the number of elements and cannot exceed\n"
      "'size'. Returns true if an element was deleted, otherwise false.",
      "",
      "bool deleteElement(int& size, int& count, int arr[], int deleteIndex) {\n    return true;\n}",
      "    printf(\"deleteElement\\n\");\n    int size = 5;\n    int count = 5;\n    int arr[5] = { 1, 2, 3, 4, 5 };\n"
      "    bool deleted = deleteElement(size, count, arr, 1);\n"
      TEST("deleted") TEST("arr[0] == 1 && arr[1] == 3 && arr[2] == 4 && arr[3] == 5") TEST("count == 4") TEST("!deleteElement(size, count, arr, 7)"),
      "bool deleteElement(int& size, int& count, int arr[], int deleteIndex) {\n"
      "    if (deleteIndex < 0 || deleteIndex >= count) return false;\n"
      "    for (int i = deleteIndex; i < count - 1; i++) {\n        arr[i] = arr[i + 1];\n    }\n    count--;\n    return true;\n}",
      false },
    { PART_LAB1, "Lab 1 Q13", "frequencyCount",
      "Count the frequency of a given element in an array.",
      "",
      "int frequencyCount(int size, int arr[], int value) {\n    return 0;\n}",
      "    printf(\"frequencyCount\\n\");\n    int arr[5] = { 1, 2, 2, 3, 2 };\n"
      TEST("frequencyCount(5, arr, 2) == 3") TEST("frequencyCount(5, arr, 1) == 1") TEST("frequencyCount(5, arr, 7) == 0"),
      "int frequencyCount(int size, int arr[], int value) {\n    int count = 0;\n    for (int i = 0; i < size; i++) {\n"
      "        if (arr[i] == value) count++;\n    }\n    return count;\n}",
      false },
    { PART_LAB1, "Lab 1 Q15", "reverse",
      "Reverse an array in place using only 1 array.",
      "",
      "void reverse(int size, int arr[]) {\n    return;\n}",
      "    printf(\"reverse\\n\");\n    int odd[5] = { 1, 2, 3, 4, 5 };\n    reverse(5, odd);\n"
      TEST("odd[0] == 5 && odd[1] == 4 && odd[2] == 3 && odd[3] == 2 && odd[4] == 1")
      "    int even[4] = { 1, 2, 3, 4 };\n    reverse(4, even);\n" TEST("even[0] == 4 && even[1] == 3 && even[2] == 2 && even[3] == 1"),
      "void reverse(int size, int arr[]) {\n    for (int i = 0; i < size / 2; i++) {\n        int temp = arr[i];\n"
      "        arr[i] = arr[size - 1 - i];\n        arr[size - 1 - i] = temp;\n    }\n}",
      false },
    { PART_LAB1, "Lab 1 Q17", "twoMovies",
      "Given a flight length and an array of movie lengths, return true if\nthere are exactly two movies whose runtimes sum to the flight length.\n"
      "Users watch exactly two movies and cannot watch the same movie twice.",
      "",
      "bool twoMovies(int flightLength, int movieLengths[], int size) {\n    return false;\n}",
      "    printf(\"twoMovies\\n\");\n    int movies[4] = { 90, 60, 30, 45 };\n    int one[2] = { 60, 50 };\n    int same[2] = { 50, 50 };\n"
      TEST("twoMovies(120, movies, 4)") TEST("!twoMovies(120, one, 2)") TEST("twoMovies(100, same, 2)") TEST("!twoMovies(500, movies, 4)"),
      "bool twoMovies(int flightLength, int movieLengths[], int size) {\n    for (int i = 0; i < size; i++) {\n"
      "        for (int j = i + 1; j < size; j++) {\n            if (movieLengths[i] + movieLengths[j] == flightLength) return true;\n"
      "        }\n    }\n    return false;\n}",
      false },
    { PART_LAB1, "Lab 1 Q18", "wordCounter",
      "Count the number of separate words in an array of chars, where a word\nis one or more characters in ranges A..Z or a..z separated by spaces.",
      "",
      "int wordCounter(int size, char characters[]) {\n    return 0;\n}",
      "    printf(\"wordCounter\\n\");\n    char two[] = \"hello world\";\n    char spaces[] = \"  Hi  there  you \";\n    char none[] = \"   \";\n"
      TEST("wordCounter(11, two) == 2") TEST("wordCounter(17, spaces) == 3") TEST("wordCounter(3, none) == 0"),
      "int wordCounter(int size, char characters[]) {\n    int count = 0;\n    bool inWord = false;\n    for (int i = 0; i < size; i++) {\n"
      "        char c = characters[i];\n        bool isLetter = (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');\n"
      "        if (isLetter && !inWord) count++;\n        inWord = isLetter;\n    }\n    return count;\n}",
      false },

    // ==================== Lab 2 Part 1 (no array indexing) ====================
    { PART_LAB2, "Lab 2 Q1", "swap_ints",
      "Swap the values of two integers using pointers.\n\nExample:\nint x = 5, y = 10;\nswap_ints(&x, &y); // Now x = 10, y = 5",
      "",
      "void swap_ints(int *a, int *b) {\n}",
      "    printf(\"swap_ints\\n\");\n    int x = 5, y = 10;\n    swap_ints(&x, &y);\n" TEST("x == 10 && y == 5")
      "    swap_ints(&x, &y);\n" TEST("x == 5 && y == 10"),
      "void swap_ints(int *a, int *b) {\n    int temp = *a;\n    *a = *b;\n    *b = temp;\n}",
      true },
    { PART_LAB2, "Lab 2 Q2", "swap_chars",
      "Swap the values of two characters using pointers.\n\nExample:\nchar a = 'A', b = 'B';\nswap_chars(&a, &b); // Now a = 'B', b = 'A'",
      "",
      "void swap_chars(char *a, char *b) {\n}",
      "    printf(\"swap_chars\\n\");\n    char a = 'A', b = 'B';\n    swap_chars(&a, &b);\n" TEST("a == 'B' && b == 'A'")
      "    char c = 'x', d = 'y';\n    swap_chars(&d, &c);\n" TEST("c == 'y' && d == 'x'"),
      "void swap_chars(char *a, char *b) {\n    char temp = *a;\n    *a = *b;\n    *b = temp;\n}",
      true },
    { PART_LAB2, "Lab 2 Q3", "find_max",
      "Find the maximum value in an integer array using pointers.\n\nExample:\nint arr[5] = {3, 9, 2, 8, 5};\nint max = find_max(arr, 5); // max = 9",
      "",
      "int find_max(int *arr, int size) {\n    return 0;\n}",
      "    printf(\"find_max\\n\");\n    int arr[5] = {3, 9, 2, 8, 5};\n    int negatives[3] = {-4, -2, -7};\n    int one[1] = {6};\n"
      TEST("find_max(arr, 5) == 9") TEST("find_max(negatives, 3) == -2") TEST("find_max(one, 1) == 6"),
      "int find_max(int *arr, int size) {\n    int max = *arr;\n    for (int i = 1; i < size; i++) {\n"
      "        if (*(arr + i) > max) max = *(arr + i);\n    }\n    return max;\n}",
      true },
    { PART_LAB2, "Lab 2 Q4", "reverse_array",
      "Reverse the contents of an integer array in-place using pointers.\n\nExample:\nint arr[5] = {1, 2, 3, 4, 5};\nreverse_array(arr, 5); // arr = {5, 4, 3, 2, 1}",
      "",
      "void reverse_array(int *arr, int size) {\n}",
      "    printf(\"reverse_array\\n\");\n    int arr[5] = {1, 2, 3, 4, 5};\n    reverse_array(arr, 5);\n"
      TEST("*arr == 5 && *(arr + 1) == 4 && *(arr + 2) == 3 && *(arr + 3) == 2 && *(arr + 4) == 1")
      "    int even[4] = {1, 2, 3, 4};\n    reverse_array(even, 4);\n" TEST("*even == 4 && *(even + 1) == 3 && *(even + 2) == 2 && *(even + 3) == 1"),
      "void reverse_array(int *arr, int size) {\n    int *start = arr;\n    int *end = arr + size - 1;\n    while (start < end) {\n"
      "        int temp = *start;\n        *start = *end;\n        *end = temp;\n        start++;\n        end--;\n    }\n}",
      true },
    { PART_LAB2, "Lab 2 Q5", "strlen_ptr",
      "Implement a function that calculates the length of a string (null-terminated\ncharacter array) using pointers.\n\n"
      "Example:\nchar str[] = \"Hello\";\nint length = strlen_ptr(str); // length = 5",
      "",
      "int strlen_ptr(char *str) {\n    return 0;\n}",
      "    printf(\"strlen_ptr\\n\");\n    char str[] = \"Hello\";\n    char longer[] = \"Hello, world!\";\n    char empty[] = \"\";\n"
      TEST("strlen_ptr(str) == 5") TEST("strlen_ptr(longer) == 13") TEST("strlen_ptr(empty) == 0"),
      "int strlen_ptr(char *str) {\n    int length = 0;\n    while (*str != '\\0') {\n        length++;\n        str++;\n    }\n    return length;\n}",
      true },
    { PART_LAB2, "Lab 2 Q6", "count_vowels",
      "Count the number of vowels in a string using pointers.\n\nExample:\nchar str[] = \"OpenAI\";\nint vowels = count_vowels(str); // vowels = 3 ('O', 'e', 'A')\n\n"
      "(Careful: the sheet's example missed the I at the end - \"OpenAI\" really has 4.)",
      "",
      "int count_vowels(char *str) {\n    return 0;\n}",
      "    printf(\"count_vowels\\n\");\n    char word[] = \"Programming\";\n    char all[] = \"AEIOU aeiou\";\n    char none[] = \"xyz\";\n"
      TEST("count_vowels(word) == 3") TEST("count_vowels(all) == 10") TEST("count_vowels(none) == 0"),
      "int count_vowels(char *str) {\n    int count = 0;\n    while (*str != '\\0') {\n        char c = *str;\n"
      "        if (c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u' ||\n            c == 'A' || c == 'E' || c == 'I' || c == 'O' || c == 'U') {\n"
      "            count++;\n        }\n        str++;\n    }\n    return count;\n}",
      true },
    { PART_LAB2, "Lab 2 Q7", "copy_string",
      "Copy the contents of one string to another using pointers (you may assume the\ndestination has enough space).\n\n"
      "Example:\nchar src[] = \"Hello\";\nchar dest[10];\ncopy_string(dest, src); // dest = \"Hello\"",
      "",
      "void copy_string(char *dest, char *src) {\n}",
      "    printf(\"copy_string\\n\");\n    char src[] = \"Hello\";\n    char dest[10] = \"xxxxxxxxx\";\n    copy_string(dest, src);\n"
      TEST("strcmp(dest, \"Hello\") == 0") "    char empty[] = \"\";\n    copy_string(dest, empty);\n" TEST("strcmp(dest, \"\") == 0"),
      "void copy_string(char *dest, char *src) {\n    while (*src != '\\0') {\n        *dest = *src;\n        dest++;\n        src++;\n    }\n    *dest = '\\0';\n}",
      true },
    { PART_LAB2, "Lab 2 Q8", "concat_strings",
      "Concatenate two strings using pointers (you may assume the destination has\nenough space).\n\n"
      "Example:\nchar dest[20] = \"Hello, \";\nchar src[] = \"World!\";\nconcat_strings(dest, src); // dest = \"Hello, World!\"",
      "",
      "void concat_strings(char *dest, char *src) {\n}",
      "    printf(\"concat_strings\\n\");\n    char dest[20] = \"Hello, \";\n    char src[] = \"World!\";\n    concat_strings(dest, src);\n"
      TEST("strcmp(dest, \"Hello, World!\") == 0") "    char start[10] = \"\";\n    char abc[] = \"abc\";\n    concat_strings(start, abc);\n"
      TEST("strcmp(start, \"abc\") == 0"),
      "void concat_strings(char *dest, char *src) {\n    while (*dest != '\\0') dest++;\n    while (*src != '\\0') {\n"
      "        *dest = *src;\n        dest++;\n        src++;\n    }\n    *dest = '\\0';\n}",
      true },
    { PART_LAB2, "Lab 2 Q9", "sum_array",
      "Compute the sum of all elements in an integer array using pointers.\n\nExample:\nint arr[4] = {1, 2, 3, 4};\nint sum = sum_array(arr, 4); // sum = 10",
      "",
      "int sum_array(int *arr, int size) {\n    return 0;\n}",
      "    printf(\"sum_array\\n\");\n    int arr[4] = {1, 2, 3, 4};\n    int mixed[3] = {-5, 5, 7};\n"
      TEST("sum_array(arr, 4) == 10") TEST("sum_array(mixed, 3) == 7") TEST("sum_array(arr, 0) == 0"),
      "int sum_array(int *arr, int size) {\n    int sum = 0;\n    for (int i = 0; i < size; i++) {\n        sum += *(arr + i);\n    }\n    return sum;\n}",
      true },
    { PART_LAB2, "Lab 2 Q10", "compare_strings",
      "Compare two strings lexicographically using pointers. Return 0 if they are\nequal, a negative value if the first string is lexicographically smaller, and a\n"
      "positive value if the first string is larger.\n\n"
      "Example:\nchar str1[] = \"apple\";\nchar str2[] = \"banana\";\nint cmp = compare_strings(str1, str2); // cmp < 0 since \"apple\" < \"banana\"",
      "",
      "int compare_strings(char *str1, char *str2) {\n    return 0;\n}",
      "    printf(\"compare_strings\\n\");\n    char str1[] = \"apple\";\n    char str2[] = \"banana\";\n    char same[] = \"apple\";\n    char longer[] = \"apples\";\n"
      TEST("compare_strings(str1, str2) < 0") TEST("compare_strings(str2, str1) > 0") TEST("compare_strings(str1, same) == 0")
      TEST("compare_strings(str1, longer) < 0"),
      "int compare_strings(char *str1, char *str2) {\n    while (*str1 != '\\0' && *str1 == *str2) {\n        str1++;\n        str2++;\n    }\n"
      "    return *str1 - *str2;\n}",
      true },

    // ==================== Lab 2 Part 2: linked lists ====================
    { PART_LISTS, "Lab 2 Part 2 (lecture notes)", "InsertAtEnd",
      "Implement the linkedlist list code from the lecture notes:\nInsertAtEnd adds a new node holding newData to the end of the list.\n"
      "If the list is empty, the new node becomes the head.",
      NODE_SUPPORT,
      "void InsertAtEnd(Node** ptrToHeadPtr, int newData) {\n}",
      "    printf(\"InsertAtEnd\\n\");\n    Node* headPtr = nullptr;\n    InsertAtEnd(&headPtr, 10);\n" TEST("listLength(headPtr) == 1 && valueAt(headPtr, 0) == 10")
      "    InsertAtEnd(&headPtr, 20);\n    InsertAtEnd(&headPtr, 30);\n"
      TEST("listLength(headPtr) == 3 && valueAt(headPtr, 1) == 20 && valueAt(headPtr, 2) == 30") TEST("valueAt(headPtr, 0) == 10"),
      "void InsertAtEnd(Node** ptrToHeadPtr, int newData) {\n    Node* newNode = new Node();\n    newNode->data = newData;\n    newNode->nextPtr = nullptr;\n"
      "    if (*ptrToHeadPtr == nullptr) {\n        *ptrToHeadPtr = newNode;\n        return;\n    }\n    Node* curNodePtr = *ptrToHeadPtr;\n"
      "    while (curNodePtr->nextPtr != nullptr) {\n        curNodePtr = curNodePtr->nextPtr;\n    }\n    curNodePtr->nextPtr = newNode;\n}",
      false },
    { PART_LISTS, "Lab 2 Part 2 (lecture notes)", "DeleteWithValue",
      "Implement the linkedlist list code from the lecture notes:\nDeleteWithValue deletes the first node that holds value, and frees its memory.\n"
      "If the head holds the value, the head moves to the next node.",
      NODE_SUPPORT,
      "void DeleteWithValue(Node** ptrToHeadPtr, int value) {\n}",
      "    printf(\"DeleteWithValue\\n\");\n    int values[4] = { 10, 20, 30, 40 };\n    Node* headPtr = makeList(values, 4);\n    DeleteWithValue(&headPtr, 20);\n"
      TEST("listLength(headPtr) == 3 && valueAt(headPtr, 1) == 30") "    DeleteWithValue(&headPtr, 10);\n" TEST("listLength(headPtr) == 2 && valueAt(headPtr, 0) == 30")
      "    DeleteWithValue(&headPtr, 99);\n" TEST("listLength(headPtr) == 2") "    DeleteWithValue(&headPtr, 40);\n" TEST("listLength(headPtr) == 1 && valueAt(headPtr, 0) == 30"),
      "void DeleteWithValue(Node** ptrToHeadPtr, int value) {\n    if (*ptrToHeadPtr == nullptr) {\n        cout << \"List is empty.\" << endl;\n        return;\n    }\n"
      "    Node* curNodePtr = *ptrToHeadPtr;\n    // If the head node itself holds the value to be deleted\n"
      "    if (curNodePtr != nullptr && curNodePtr->data == value) {\n        *ptrToHeadPtr = curNodePtr->nextPtr;\n        delete curNodePtr;\n        return;\n    }\n"
      "    // Search for the node, keeping track of the previous node\n    Node* prev = nullptr;\n"
      "    while (curNodePtr != nullptr && curNodePtr->data != value) {\n        prev = curNodePtr;\n        curNodePtr = curNodePtr->nextPtr;\n    }\n"
      "    if (curNodePtr == nullptr) {\n        cout << \"Value not found in the list.\" << endl;\n        return;\n    }\n"
      "    prev->nextPtr = curNodePtr->nextPtr;\n    delete curNodePtr;\n}",
      false },
    { PART_LISTS, "Lab 2 Part 2", "DoublyLinkedList::InsertAtBeginning",
      "Create a new classes called DoublyLinkedList and a DoublyLinkedNode. DoublyLinkedNode\nhas both a prevPointer and nextPointer.\n\n"
      "Implement InsertAtBeginning: add a new node holding newData at the front of the list.\n(The classes are premade above.)",
      DOUBLY_SUPPORT,
      "void DoublyLinkedList::InsertAtBeginning(int newData) {\n}",
      "    printf(\"InsertAtBeginning\\n\");\n    DoublyLinkedList list;\n    list.InsertAtBeginning(30);\n" TEST("listLength(&list) == 1 && valueAt(&list, 0) == 30")
      "    list.InsertAtBeginning(20);\n    list.InsertAtBeginning(10);\n"
      TEST("listLength(&list) == 3 && valueAt(&list, 0) == 10 && valueAt(&list, 1) == 20 && valueAt(&list, 2) == 30") TEST("linksOK(&list)"),
      "void DoublyLinkedList::InsertAtBeginning(int newData) {\n    DoublyLinkedNode *newNode = new DoublyLinkedNode(newData);\n"
      "    newNode->nextPointer = head;\n    if (head != nullptr) {\n        head->prevPointer = newNode;\n    }\n    head = newNode;\n}",
      false },
    { PART_LISTS, "Lab 2 Part 2", "DoublyLinkedList::InsertAtEnd",
      "Create a new classes called DoublyLinkedList and a DoublyLinkedNode. DoublyLinkedNode\nhas both a prevPointer and nextPointer.\n\n"
      "Implement InsertAtEnd: add a new node holding newData at the end of the list.\n(The classes are premade above.)",
      DOUBLY_SUPPORT,
      "void DoublyLinkedList::InsertAtEnd(int newData) {\n}",
      "    printf(\"InsertAtEnd\\n\");\n    DoublyLinkedList list;\n    list.InsertAtEnd(10);\n" TEST("listLength(&list) == 1 && valueAt(&list, 0) == 10")
      "    list.InsertAtEnd(20);\n    list.InsertAtEnd(30);\n"
      TEST("listLength(&list) == 3 && valueAt(&list, 1) == 20 && valueAt(&list, 2) == 30") TEST("linksOK(&list)"),
      "void DoublyLinkedList::InsertAtEnd(int newData) {\n    DoublyLinkedNode *newNode = new DoublyLinkedNode(newData);\n"
      "    if (head == nullptr) {\n        head = newNode;\n        return;\n    }\n    DoublyLinkedNode *curNodePtr = head;\n"
      "    while (curNodePtr->nextPointer != nullptr) {\n        curNodePtr = curNodePtr->nextPointer;\n    }\n"
      "    curNodePtr->nextPointer = newNode;\n    newNode->prevPointer = curNodePtr;\n}",
      false },
    { PART_LISTS, "Lab 2 Part 2", "DoublyLinkedList::InsertAtPosition",
      "Create a new classes called DoublyLinkedList and a DoublyLinkedNode. DoublyLinkedNode\nhas both a prevPointer and nextPointer.\n\n"
      "Implement InsertAtPosition: add a new node holding newData so it ends up at that position.\n"
      "(The lab doesn't say how positions are counted: here position 0 is the front, like an array.)",
      DOUBLY_SUPPORT,
      "void DoublyLinkedList::InsertAtPosition(int newData, int position) {\n}",
      "    printf(\"InsertAtPosition\\n\");\n    DoublyLinkedList list;\n    int values[2] = { 10, 30 };\n    buildList(&list, values, 2);\n"
      "    list.InsertAtPosition(20, 1);\n" TEST("listLength(&list) == 3 && valueAt(&list, 1) == 20 && valueAt(&list, 2) == 30") TEST("linksOK(&list)")
      "    list.InsertAtPosition(5, 0);\n" TEST("valueAt(&list, 0) == 5 && valueAt(&list, 1) == 10 && linksOK(&list)")
      "    list.InsertAtPosition(40, 4);\n" TEST("listLength(&list) == 5 && valueAt(&list, 4) == 40 && linksOK(&list)"),
      "void DoublyLinkedList::InsertAtPosition(int newData, int position) {\n    DoublyLinkedNode *newNode = new DoublyLinkedNode(newData);\n"
      "    if (position == 0 || head == nullptr) {\n        newNode->nextPointer = head;\n        if (head != nullptr) head->prevPointer = newNode;\n"
      "        head = newNode;\n        return;\n    }\n    // Walk to the node just before the position\n    DoublyLinkedNode *curNodePtr = head;\n"
      "    for (int i = 0; i < position - 1 && curNodePtr->nextPointer != nullptr; i++) {\n        curNodePtr = curNodePtr->nextPointer;\n    }\n"
      "    newNode->prevPointer = curNodePtr;\n    newNode->nextPointer = curNodePtr->nextPointer;\n"
      "    if (curNodePtr->nextPointer != nullptr) {\n        curNodePtr->nextPointer->prevPointer = newNode;\n    }\n    curNodePtr->nextPointer = newNode;\n}",
      false },
};

ExamQuestion *getExamQuestion(int index) {
    return &questions[index];
}

// ============================================================
// Building the program to compile
// ============================================================

// Adds text to the end of out
void addText(char *out, int size, const char *text) {
    int length = (int)strlen(out);
    snprintf(out + length, size - length, "%s", text);
}

void premadeCodeText(ExamQuestion *q, char *out, int size) {
    out[0] = '\0';
    addText(out, size, INCLUDES);
    if (q->support[0] != '\0') {
        addText(out, size, "\n");
        addText(out, size, q->support);
        addText(out, size, "\n");
    }
    addText(out, size, "\n// ---- your code goes here ----\n\nint main() {\n");
    addText(out, size, q->mainBody);
    addText(out, size, "    return 0;\n}");
}

void buildExamProgram(ExamQuestion *q, const char *yourCode, char *out, int size) {
    out[0] = '\0';
    // A watchdog stops code that runs forever (it isn't part of the exam)
    addText(out, size, "#include <cstdio>\n#include <thread>\n#include <chrono>\n#include <cstdlib>\n");
    addText(out, size, "static void pointerPoolWatchdog() {\n"
                       "    std::this_thread::sleep_for(std::chrono::seconds(10));\n"
                       "    printf(\"\\nEXAM-TIMEOUT\\n\");\n    fflush(stdout);\n    std::_Exit(3);\n}\n");
    addText(out, size, "#line 1 \"premade_code\"\n");
    addText(out, size, INCLUDES);
    addText(out, size, q->support);
    addText(out, size, "\n#line 1 \"your_code\"\n");
    addText(out, size, yourCode);
    addText(out, size, "\n#line 1 \"premade_main\"\nint main() {\n"
                       "    std::thread(pointerPoolWatchdog).detach();\n"
                       "    setvbuf(stdout, NULL, _IONBF, 0);\n");
    addText(out, size, q->mainBody);
    addText(out, size, "    printf(\"EXAM-DONE\\n\");\n    return 0;\n}\n");
}

int countTests(ExamQuestion *q) {
    int count = 0;
    const char *p = q->mainBody;
    while ((p = strstr(p, "printf(\"yes\\n\")")) != nullptr) {
        count++;
        p++;
    }
    return count;
}

// Lab 2 rule: arr[i] isn't allowed. Looks for [ with something inside it,
// skipping comments and strings. (delete[] is fine.)
bool examUsesIndexing(const char *code) {
    for (const char *p = code; *p != '\0'; p++) {
        if (p[0] == '/' && p[1] == '/') {
            while (*p != '\0' && *p != '\n') p++;
            if (*p == '\0') break;
        } else if (p[0] == '/' && p[1] == '*') {
            p += 2;
            while (*p != '\0' && !(p[0] == '*' && p[1] == '/')) p++;
            if (*p == '\0') break;
        } else if (*p == '"' || *p == '\'') {
            char quote = *p;
            p++;
            while (*p != '\0' && *p != quote) {
                if (*p == '\\' && p[1] != '\0') p++;
                p++;
            }
            if (*p == '\0') break;
        } else if (*p == '[') {
            const char *q = p + 1;
            while (*q == ' ') q++;
            if (*q != ']') return true;
        }
    }
    return false;
}
