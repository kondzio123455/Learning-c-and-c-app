#include "questions.h"

// ============================================================
// Chapters 5 and 6: the Memory Management lecture, and the
// pointer parts of the Classes and Operator Overloading notes.
// ============================================================

void setupMemoryLessons() {
    Level *level;
    Task *task;

    // =========================================================
    // Chapter 5: Memory management
    // =========================================================

    // ---------- Level 23 ----------
    level = startLevel(23, 5, "Scope and dangling pointers", "dangling", "Memory Management lecture, slides 2-11");
    level->explanation = "A variable only exists while it's in scope. When the block it was declared in ends, its memory can be "
                         "reused (slide 3). A pointer that still points at that memory is a dangling pointer: reading through "
                         "it gives an undefined value (slide 8). So a function must never return the address of its own local "
                         "variable (slides 9-11).";
    level->example = "int main() {\n"
                     "  if (true) {\n"
                     "    int x = 5;\n"
                     "  }\n"
                     "  // x now out of scope, memory it used can be reused\n"
                     "}";
    level->inGame = "Not used by the game, but dangling pointers are a favourite test question.";
    level->slideTitle = "Memory Management lecture, slides 3-11";
    level->slideText = "Slide 3: Scoping and Memory\n"
                       "- When a variable goes out of scope, that memory is no longer\n"
                       "  guaranteed to store the variable's value\n"
                       "int main() {\n"
                       "  if (true) {\n"
                       "    int x = 5;\n"
                       "  }\n"
                       "  // x now out of scope, memory it used to occupy can be reused\n"
                       "}\n"
                       "\n"
                       "Slide 8: Here, p has become a dangling pointer\n"
                       "(points to memory whose contents are undefined)\n"
                       "\n"
                       "Slide 9: A Problematic Task. Incorrect implementation:\n"
                       "int* getPtrToFive() {\n"
                       "  int x = 5;\n"
                       "  return &x;\n"
                       "}";

    task = startTask(level, 0, TASK_BUG, "Slide 8: which line uses a dangling pointer? Type the line number.",
                     "int main() {\n"
                     "  int *p;\n"
                     "  if (true) {\n"
                     "    int x = 5;\n"
                     "    p = &x;\n"
                     "  }\n"
                     "  cout << *p << endl;\n"
                     "}");
    addAnswer(task, "7");
    addMistake(task, "5", "On line 5 x still exists, so p = &x; is fine at that moment.");
    addMistake(task, "4", "Declaring x is fine. The problem is using its address later.");
    addMistake(task, "6", "x goes out of scope on line 6, but nothing reads through p there.");
    setHints(task, "Where does x stop existing?",
                   "x goes out of scope at the } that ends the if block.",
                   "Which line reads through p after that?");
    task->layout = "Line 4: x is created inside the if block.\n"
                   "Line 6: the } ends the if block, so x is gone.\n"
                   "Which later line still reads through p?";

    task = startTask(level, 1, TASK_TYPE, "Slide 17: fix getPtrToFive so the memory stays allocated after the function returns.",
                     "int *getPtrToFive() {\n"
                     "  ____   // memory that outlives the function\n"
                     "  *x = 5;\n"
                     "  return x;\n"
                     "}");
    addAnswer(task, "int *x = new int;");
    addAnswer(task, "int *x = new int();");
    addMistake(task, "int x = 5;", "That's the incorrect version from slide 9: x disappears when the function returns.");
    addMistake(task, "int *x;", "That's a pointer to nowhere, so *x = 5; would write into random memory.");
    addMistake(task, "int *x = &x;", "x can't point at itself, and a local would still disappear when the function returns.");
    setHints(task, "Local variables disappear when the function returns.",
                   "Slide 12: new is 'another way to allocate memory, where the memory will remain allocated until you manually de-allocate it'.",
                   "int *x = ...;");

    // ---------- Level 24 ----------
    level = startLevel(24, 5, "new and delete", "new/delete", "Memory Management lecture, slides 12-18");
    level->explanation = "new allocates memory that stays allocated until you delete it, and returns a pointer to it (slide 12). "
                         "You can give it a starting value with round or curly brackets (slide 14). A plain int x; lives on the "
                         "stack; memory from new lives on the heap (slide 15). delete gives heap memory back (slide 16).";
    level->example = "int *x = new int;\n"
                     "// use memory allocated by new\n"
                     "delete x;";
    level->inGame = "Every entry in the shot log is made with new, and freed with delete when a new game starts.";
    level->slideTitle = "Memory Management lecture, slides 12-18";
    level->slideText = "Slide 12: The new operator\n"
                       "- Another way to allocate memory, where the memory will remain\n"
                       "  allocated until you manually de-allocate it\n"
                       "- Returns a pointer to the newly allocated memory\n"
                       "      int *x = new int;\n"
                       "\n"
                       "Slide 14: The new operator and Brackets!\n"
                       "      int *x = new int(5);\n"
                       "      int *x = new int{5};\n"
                       "      int *x = new int[5];   // see later what [] does\n"
                       "\n"
                       "Slide 15: int x; is allocated on the stack. new int is allocated on the heap.\n"
                       "\n"
                       "Slide 16: The delete operator\n"
                       "- De-allocates memory that was previously allocated using new\n"
                       "- Takes a pointer to the memory location\n"
                       "      delete x;";

    task = startTask(level, 0, TASK_TYPE, "Slide 14: allocate a new int that starts as 5.",
                     "int *x = ____;");
    addAnswer(task, "new int(5)");
    addAnswer(task, "new int{5}");
    addMistake(task, "new int[5]", "Square brackets make an array of 5 ints (slide 14). Use round or curly brackets to give one int a starting value.");
    addMistake(task, "new int", "That makes an int, but doesn't set it to 5.");
    addMistake(task, "5", "x is a pointer, so it needs an address from new, not a number.");
    setHints(task, "new returns a pointer to a new int.",
                   "Slide 14 ('The new operator and Brackets') shows new int with round brackets, curly brackets, and square brackets (an array).",
                   "new int, then the value in brackets.");

    task = startTask(level, 1, TASK_TYPE, "Slide 18: you've finished with the int. Free its memory.",
                     "int *p = getPtrToNewInt();\n"
                     "cout << *p << endl; // 5\n"
                     "____");
    addAnswer(task, "delete p;");
    addMistake(task, "delete *p;", "delete takes the pointer itself, not the value it points at.");
    addMistake(task, "delete[] p;", "delete[] is for arrays made with new[]. This was one int.");
    addMistake(task, "free(p);", "free goes with malloc. Memory from new goes back with delete.");
    addMistake(task, "p = nullptr;", "That forgets the address but doesn't free the memory: it's a leak.");
    setHints(task, "Memory from new has to be given back.",
                   "Slide 16: 'The delete operator de-allocates memory that was previously allocated using new. Takes a pointer to the memory location.'",
                   "delete, then the pointer.");

    // ---------- Level 25 ----------
    level = startLevel(25, 5, "Memory leaks", "leaks", "Memory Management lecture, slides 19-33");
    level->explanation = "If you allocate with new and never delete, the memory is wasted: that's a memory leak (slide 23). In a "
                         "loop, every new needs its own delete, so the delete goes inside the loop. One delete after the loop "
                         "only frees the memory from the last time round (slide 25).";
    level->example = "// slide 19: the leak\n"
                     "for (int i = 0; i < 3; i++) {\n"
                     "    p = getPtrToFive();    // new int each time\n"
                     "    cout << *p << endl;\n"
                     "}   // three ints allocated, none deleted";
    level->inGame = "Not used by the game, but the slides spend a lot of time on this, so expect it.";
    level->slideTitle = "Memory Management lecture, slides 19-26";
    level->slideText = "Slide 23:\n"
                       "- When your program allocates memory but is unable to\n"
                       "  de-allocate it, this is a memory leak\n"
                       "int main() {\n"
                       "  int *p;\n"
                       "  for (int i = 0; i < 3; ++i) {\n"
                       "    p = getPtrToFive();\n"
                       "    cout << *p << endl;\n"
                       "  }\n"
                       "}\n"
                       "\n"
                       "Slide 24: Does adding a delete after the loop fix this memory leak?\n"
                       "(slide 25 gives the answer, so it's left out here)\n"
                       "\n"
                       "Slide 26: To fix the memory leak, de-allocate memory within the loop";

    task = startTask(level, 0, TASK_TYPE, "Slide 26: fix the memory leak.",
                     "int main() {\n"
                     "  int *p;\n"
                     "  for (int i = 0; i < 3; ++i) {\n"
                     "    p = getPtrToFive();\n"
                     "    cout << *p << endl;\n"
                     "    ____\n"
                     "  }\n"
                     "}");
    addAnswer(task, "delete p;");
    addMistake(task, "delete[] p;", "getPtrToFive allocates one int, not an array.");
    addMistake(task, "delete *p;", "delete takes the pointer, not the value it points at.");
    addMistake(task, "p = nullptr;", "That forgets the address, but the int is still allocated.");
    setHints(task, "Each time round, getPtrToFive allocates a new int.",
                   "Each one has to be freed before p moves on to the next.",
                   "Free it inside the loop, after it's printed.");

    task = startTask(level, 1, TASK_TRACE, "Slide 24 asks: does adding delete p; AFTER the loop fix this leak? Type yes or no.",
                     "int *p;\n"
                     "for (int i = 0; i < 3; ++i) {\n"
                     "  p = getPtrToFive();\n"
                     "  cout << *p << endl;\n"
                     "}\n"
                     "delete p;");
    addAnswer(task, "no");
    addAnswer(task, "No");
    addAnswer(task, "NO");
    addMistake(task, "yes", "It only frees the memory from the last time round. The first two ints are still lost.");
    addMistake(task, "Yes", "It only frees the memory from the last time round. The first two ints are still lost.");
    setHints(task, "How many times does new run?",
                   "How many times does delete run?",
                   "Three news, one delete.");
    task->layout = "new runs 3 times (once each time round the loop).\n"
                   "delete runs 1 time (after the loop).\n"
                   "Can 1 delete free 3 allocations?";

    // ---------- Level 26 ----------
    level = startLevel(26, 5, "Memory mistakes", "mistakes", "Memory Management lecture, slides 34-39");
    level->explanation = "Three rules from the slides: don't use memory after you've deleted it (slide 34), don't delete the same "
                         "memory twice (slide 36), and only delete memory that came from new (slide 38), never the address of a "
                         "normal variable.";
    level->example = "int *x = getPtrToFive();\n"
                     "cout << *x << endl; // 5\n"
                     "delete x;           // use it, then delete it once";
    level->inGame = "Not used by the game, but spot-the-bug questions like these are common in tests.";
    level->slideTitle = "Memory Management lecture, slides 34-39";
    level->slideText = "Slide 35: Don't Use Memory After Deletion. The correct version:\n"
                       "int main() {\n"
                       "  int *x = getPtrToFive();\n"
                       "  cout << *x << endl; // 5\n"
                       "  delete x;\n"
                       "}\n"
                       "\n"
                       "Slide 37: Don't delete memory twice. The correct version:\n"
                       "  cout << *x << endl; // 5\n"
                       "  delete x;\n"
                       "}\n"
                       "\n"
                       "Slide 39: Only delete if memory was allocated by new. The correct version:\n"
                       "int main() {\n"
                       "  int x = 5;\n"
                       "  int *xPtr = &x;\n"
                       "  cout << *xPtr << endl;\n"
                       "}";

    task = startTask(level, 0, TASK_BUG, "Slide 34: which line is wrong? Type the line number.",
                     "int main() {\n"
                     "  int *x = getPtrToFive();\n"
                     "  delete x;\n"
                     "  cout << *x << endl;\n"
                     "}");
    addAnswer(task, "4");
    addMistake(task, "3", "Deleting is right. Look at what happens after it.");
    addMistake(task, "2", "Getting the pointer is fine.");
    setHints(task, "Follow x from top to bottom.",
                   "After delete, the memory isn't yours any more.",
                   "Which line still reads it?");
    task->layout = "Line 3 frees the memory that x points at.\n"
                   "After that, any line that reads *x is using freed memory.\n"
                   "Which line is that?";

    task = startTask(level, 1, TASK_BUG, "Slide 38: which line is wrong? Type the line number.",
                     "int main() {\n"
                     "  int x = 5;\n"
                     "  int *xPtr = &x;\n"
                     "  cout << *xPtr << endl;\n"
                     "  delete xPtr;\n"
                     "}");
    addAnswer(task, "5");
    addMistake(task, "3", "Pointing at x is fine.");
    addMistake(task, "4", "Reading through xPtr is fine: x still exists.");
    setHints(task, "Where did the memory xPtr points at come from?",
                   "x is a normal variable, not memory from new.",
                   "Which line tries to give it back?");
    task->layout = "Line 3: xPtr = &x, and x is a normal variable (not from new).\n"
                   "Only memory from new can be deleted.\n"
                   "Which line deletes?";

    // ---------- Level 27 ----------
    level = startLevel(27, 5, "Arrays with new[]", "new[]", "Memory Management lecture, slides 40-45");
    level->explanation = "A normal array needs a constant size, so int arr[numItems]; is not allowed when numItems is read in "
                         "(slide 40). new[] allocates an array of any size while the program runs (slide 41), and it must be "
                         "freed with delete[] (slide 45).";
    level->example = "int *arr = new int[10];\n"
                     "arr[0] = 7;\n"
                     "delete[] arr;";
    level->inGame = "Not used by the game, but slide 45 is the lecture's exercise.";
    level->slideTitle = "Memory Management lecture, slides 40-45";
    level->slideText = "Slide 40: When allocating arrays on the stack (using \"int arr[SIZE]\"),\n"
                       "size must be a constant\n"
                       "      int arr[numItems]; // not allowed\n"
                       "\n"
                       "Slide 41: If we use new[] to allocate arrays, they can have variable size\n"
                       "      int *arr = new int[numItems];\n"
                       "\n"
                       "Slide 45: Ex: Storing values input by the user\n"
                       "  int *arr = new int[numItems];\n"
                       "  for (int i = 0; i < numItems; ++i) {\n"
                       "    cout << \"enter item \" << i << \": \";\n"
                       "    cin >> arr[i];\n"
                       "  }\n"
                       "  for (int i = 0; i < numItems; ++i) {\n"
                       "    cout << arr[i] << endl;\n"
                       "  }\n"
                       "  delete[] arr;";

    task = startTask(level, 0, TASK_TYPE, "Slide 41: allocate an array of numItems ints.",
                     "int numItems;\n"
                     "cout << \"how many items?\";\n"
                     "cin >> numItems;\n"
                     "____");
    addAnswer(task, "int *arr = new int[numItems];");
    addMistake(task, "int arr[numItems];", "Slide 40: an array like this needs a constant size, and numItems is only known when the program runs.");
    addMistake(task, "int *arr = new int(numItems);", "Round brackets make ONE int with that value. An array needs the other brackets.");
    addMistake(task, "int arr = new int[numItems];", "new[] returns a pointer, so arr needs a * in its type.");
    setHints(task, "The size is only known when the program runs.",
                   "Slide 41: 'If we use new[] to allocate arrays, they can have variable size.'",
                   "int *arr = new int...;");

    task = startTask(level, 1, TASK_TYPE, "Slide 45: you've finished with the array. Free it.",
                     "int *arr = new int[numItems];\n"
                     "// ... read in and print the items ...\n"
                     "____");
    addAnswer(task, "delete[] arr;");
    addMistake(task, "delete arr;", "Memory from new[] needs the array version of delete.");
    addMistake(task, "delete[] *arr;", "delete[] takes the pointer itself.");
    addMistake(task, "free(arr);", "free goes with malloc, not new.");
    setHints(task, "Every new[] needs a matching delete.",
                   "Arrays use the array version of delete.",
                   "delete[] followed by the pointer.");

    // ---------- Level 28 ----------
    level = startLevel(28, 5, "2D arrays with int**", "int**", "Memory Management lecture, slides 46-48");
    level->explanation = "A 2D array of any size is an array of pointers, one per row: int** arr2D = new int*[rows];. Then each "
                         "row gets its own array of ints. To free it, delete each row first, then the array of row pointers "
                         "(slide 48).";
    level->example = "int** arr2D = new int*[rows];   // the row pointers\n"
                     "// each arr2D[row] then gets its own array of ints,\n"
                     "// and arr2D[row][col] is one element";
    level->inGame = "Not used by the game, but it's on slides 46-48.";
    level->slideTitle = "Memory Management lecture, slides 46-48: 2D Arrays";
    level->slideText = "int** arr2D = new int*[rows];\n"
                       "for (int row = 0; row < rows; row++) {\n"
                       "    arr2D[row] = new int[cols];\n"
                       "}\n"
                       "int counter = 1;\n"
                       "for (int row = 0; row < rows; row++) {\n"
                       "    for (int col = 0; col < cols; col++) {\n"
                       "        arr2D[row][col] = counter++;\n"
                       "    }\n"
                       "}\n"
                       "// Deallocate memory\n"
                       "for (int row = 0; row < rows; row++) {\n"
                       "    delete[] arr2D[row];\n"
                       "}\n"
                       "delete[] arr2D;";

    task = startTask(level, 0, TASK_TYPE, "Slide 47: give each row its own array of cols ints.",
                     "int** arr2D = new int*[rows];\n"
                     "for (int row = 0; row < rows; row++) {\n"
                     "    ____\n"
                     "}");
    addAnswer(task, "arr2D[row] = new int[cols];");
    addMistake(task, "arr2D = new int[cols];", "That would replace the whole array of row pointers. Each row needs its own array.");
    addMistake(task, "arr2D[row] = new int*[cols];", "Each row holds ints, not pointers.");
    addMistake(task, "int *arr2D[row] = new int[cols];", "arr2D[row] already exists, so don't declare it again.");
    setHints(task, "arr2D[row] is one row pointer.",
                   "Each row needs an array of cols ints.",
                   "arr2D[row] = new ...;");

    task = startTask(level, 1, TASK_TYPE, "Slide 48: the rows are freed. Now free the array of row pointers.",
                     "for (int row = 0; row < rows; row++) {\n"
                     "    delete[] arr2D[row];\n"
                     "}\n"
                     "____");
    addAnswer(task, "delete[] arr2D;");
    addMistake(task, "delete arr2D;", "arr2D was made with new int*[rows], which is an array.");
    addMistake(task, "delete[] arr2D[row];", "The rows are already freed (and row doesn't exist out here). Free arr2D itself.");
    setHints(task, "arr2D itself was made with new[] too.",
                   "Arrays use delete[].",
                   "delete[] followed by arr2D.");

    // ---------- Level 29 ----------
    level = startLevel(29, 5, "malloc and free (C)", "malloc", "Memory Management lecture, slides 49-51: C Memory Management");
    level->explanation = "C doesn't have new and delete. It uses malloc to allocate (with sizeof to get the right number of "
                         "bytes) and free to give the memory back. Always check whether malloc returned NULL, because "
                         "allocation can fail, and set the pointer to NULL after freeing it (slide 51). The (int*) cast on the "
                         "slides is optional in C, but C++ needs it.";
    level->example = "int *arr = (int*)malloc(size * sizeof(int));   // slide 50\n"
                     "if (arr == NULL) {\n"
                     "    printf(\"Memory allocation failed!\\n\");\n"
                     "    return 1;\n"
                     "}";
    level->inGame = "Not used by the game (it's C++), but your test is on C pointers too.";
    level->slideTitle = "Memory Management lecture, slides 49-51: C Memory Management";
    level->slideText = "int *ptr = (int*)malloc(sizeof(int));\n"
                       "if (ptr == NULL) {\n"
                       "    printf(\"Memory allocation failed!\\n\");\n"
                       "    return 1;\n"
                       "}\n"
                       "*ptr = 42;\n"
                       "free(ptr);\n"
                       "\n"
                       "int *arr = (int*)malloc(size * sizeof(int));   // slide 50\n"
                       "\n"
                       "Slide 51: Key Points to Remember\n"
                       "- Always check if malloc() returns NULL - allocation can fail\n"
                       "- Always call free() for every successful malloc() to avoid memory leaks\n"
                       "- Use sizeof() to get the correct size of the data type\n"
                       "- Don't use memory after freeing it - this causes undefined behavior\n"
                       "- Set pointer to NULL after freeing (good practice)";

    task = startTask(level, 0, TASK_TYPE, "Slide 49: allocate memory for a single integer with malloc.",
                     "int *ptr = ____;");
    addAnswer(task, "(int*)malloc(sizeof(int))");
    addAnswer(task, "malloc(sizeof(int))");
    addMistake(task, "malloc(4)", "Use sizeof(int) (slide 51): an int isn't the same size on every computer.");
    addMistake(task, "new int", "That's C++. In C, memory comes from malloc.");
    addMistake(task, "(int*)malloc(int)", "malloc needs a number of bytes, and sizeof gives you that.");
    setHints(task, "C allocates memory with malloc.",
                   "malloc needs the number of bytes: use sizeof.",
                   "Slide 49 puts (int*) in front of malloc, to turn its result into an int pointer.");
    task->layout = "(int*)malloc(sizeof(___))";

    task = startTask(level, 1, TASK_TYPE, "Slide 51: free the memory, then set ptr to NULL.",
                     "*ptr = 42;\n"
                     "printf(\"Value: %d\\n\", *ptr);\n"
                     "____");
    addAnswer(task, "free(ptr); ptr = NULL;");
    addMistake(task, "delete ptr; ptr = NULL;", "delete goes with new. Memory from malloc goes back with free.");
    addMistake(task, "free(ptr);", "Freed, but slide 51 also says to set the pointer to NULL afterwards.");
    addMistake(task, "ptr = NULL; free(ptr);", "Setting ptr to NULL first loses the address, so free can't give the memory back.");
    addMistake(task, "free(ptr); ptr = nullptr;", "nullptr is C++. The C slides use NULL.");
    setHints(task, "Two statements: give the memory back, then clear the pointer.",
                   "free(...) gives malloc's memory back.",
                   "Then set ptr to NULL.");

    // ---------- Level 30 ----------
    level = startLevel(30, 5, "Function pointers", "func ptr", "Memory Management lecture, slides 55-56");
    level->explanation = "A function has an address too. You can store it in a function pointer and call the function through it. "
                         "The pointer must have the same return type and parameter types as the function, like "
                         "int (*functionPtr)(int, int);. Function pointers can only be assigned and called: no arithmetic (slide 55).";
    level->example = "int (*functionPtr)(int, int);   // can point at any function that\n"
                     "                                // takes two ints and returns an int";
    level->inGame = "Not used by the game, but it's on slides 55-56.";
    level->slideTitle = "Memory Management lecture, slides 55-56: Function Pointers";
    level->slideText = "- We can obtain the address of a C function, store it in a function\n"
                       "  pointer, and then call the function indirectly through that pointer.\n"
                       "- The manipulation of function pointers is limited to assignment and\n"
                       "  indirection; we cannot do arithmetic on function pointers.\n"
                       "\n"
                       "int Add(int num1, int num2)\n"
                       "{\n"
                       "    return num1 + num2;\n"
                       "}\n"
                       "int main()\n"
                       "{\n"
                       "    //must have the same return type and parameter types as function\n"
                       "    int (*functionPtr)(int , int );\n"
                       "    functionPtr = Add;\n"
                       "    int result = functionPtr(20, 10);\n"
                       "}";

    task = startTask(level, 0, TASK_TYPE, "Slide 56: make functionPtr point at Add.",
                     "int Add(int num1, int num2) {\n"
                     "    return num1 + num2;\n"
                     "}\n"
                     "int (*functionPtr)(int, int);\n"
                     "____");
    addAnswer(task, "functionPtr = Add;");
    addAnswer(task, "functionPtr = &Add;");
    addMistake(task, "functionPtr = Add();", "Add() calls the function. To point at it, use its name without the brackets.");
    addMistake(task, "*functionPtr = Add;", "Assign to the pointer itself.");
    addMistake(task, "functionPtr = Add(20, 10);", "That calls Add and stores its result (30), not the function.");
    setHints(task, "A function's name gives you its address.",
                   "Store that address in functionPtr.",
                   "functionPtr = ...;");

    task = startTask(level, 1, TASK_TYPE, "Slide 56: call Add through functionPtr with 20 and 10.",
                     "functionPtr = Add;\n"
                     "int result = ____;");
    addAnswer(task, "functionPtr(20, 10)");
    addAnswer(task, "(*functionPtr)(20, 10)");
    addMistake(task, "Add(20, 10)", "That calls Add directly. The task is to call it through the pointer.");
    addMistake(task, "functionPtr", "That's the pointer itself. Calling it needs the arguments in brackets.");
    addMistake(task, "*functionPtr(20, 10)", "The ( ) happens before the *, so this needs brackets round *functionPtr, or no * at all.");
    setHints(task, "You call a function pointer just like a function.",
                   "Use the pointer's name where you'd write Add.",
                   "Then 20 and 10 in brackets.");

    // ---------- Level 31 ----------
    level = startLevel(31, 5, "Global variables", "globals", "Memory Management lecture, slides 52-54");
    level->explanation = "A global variable is declared outside every function, so every function can use it. But if a function "
                         "declares a local variable with the same name, the local one hides the global one inside that "
                         "function, and changing it doesn't touch the global (slide 54).";
    level->example = "int count = 0;     // global\n"
                     "void f() {\n"
                     "    count = 5;     // changes the global\n"
                     "}";
    level->inGame = "Not used by the game, but slides 52-54 make a good tracing question.";
    level->slideTitle = "Memory Management lecture, slides 52-54: Global Variables";
    level->slideText = "int glVar = 10;\n"
                       "int main() {\n"
                       "   int *p;\n"
                       "   for (int i = 0; i < 3; ++i) {\n"
                       "     p = getPtrToFive();\n"
                       "     cout << *p << endl;\n"
                       "   }\n"
                       "   glVar=33;\n"
                       "}\n"
                       "int *getPtrToFive() {\n"
                       "    int *x = new int;\n"
                       "    *x = 5;\n"
                       "    glVar=20;          // glVar is accessible everywhere\n"
                       "    return x;\n"
                       "}\n"
                       "\n"
                       "Slide 53 asks: what happens if getPtrToFive() also declares its own int glVar;?\n"
                       "(slide 54 answers it, so it's left out here)";

    task = startTask(level, 0, TASK_TRACE, "Slides 53-54: getPtrToFive() has just run. What is the global glVar now?",
                     "int glVar = 10;\n"
                     "int *getPtrToFive() {\n"
                     "    int glVar;          // a local glVar\n"
                     "    int *x = new int;\n"
                     "    *x = 5;\n"
                     "    glVar = 20;\n"
                     "    return x;\n"
                     "}");
    addAnswer(task, "10");
    addAnswer(task, "glVar=10");
    addMistake(task, "20", "The local glVar hides the global one inside the function, so only the local copy became 20.");
    setHints(task, "Look at the first line inside getPtrToFive.",
                   "There are two variables called glVar.",
                   "glVar = 20; changes the one declared inside the function.");
    task->layout = "Inside getPtrToFive, int glVar; makes a NEW local glVar.\n"
                   "glVar = 20; changes that local one.\n"
                   "The global glVar was 10 and ...";

    task = startTask(level, 1, TASK_TRACE, "Slide 52: no local glVar this time. getPtrToFive() has just run. What is the global glVar now?",
                     "int glVar = 10;\n"
                     "int *getPtrToFive() {\n"
                     "    int *x = new int;\n"
                     "    *x = 5;\n"
                     "    glVar = 20;\n"
                     "    return x;\n"
                     "}");
    addAnswer(task, "20");
    addAnswer(task, "glVar=20");
    addMistake(task, "10", "There's no local glVar this time, so glVar = 20; changes the global one.");
    setHints(task, "Is there a local variable called glVar?",
                   "The global glVar is accessible everywhere.",
                   "So glVar = 20; changes the global.");
    task->layout = "There's no local glVar inside getPtrToFive.\n"
                   "So glVar = 20; changes the global glVar,\n"
                   "which was 10 before the call.";

    // =========================================================
    // Chapter 6: Pointers and classes
    // =========================================================

    // ---------- Level 32 ----------
    level = startLevel(32, 6, "Passing objects", "objects", "Classes lecture, slides 28-29 and 38-39");
    level->explanation = "Objects can be passed to functions. Pass by value gives the function a copy, so changes are lost (slide "
                         "28). To change the original, pass by reference (Vector &v) or by pointer (Vector *v). With a pointer "
                         "you reach the fields with -> and pass the object's address (slide 39).";
    level->example = "void offsetVector(Vector &v, double offsetX, double offsetY);   // by reference\n"
                     "offsetVector(vec, 1.0, 1.5);";
    level->inGame = "Not used by the game, but it's in the Classes lecture.";
    level->slideTitle = "Classes lecture, slides 28-29 and 39";
    level->slideText = "Slide 28: Passing by value passes a copy of the class instance\n"
                       "to the function; changes aren't preserved\n"
                       "void offsetPoint(Point p, double x, double y) { // does nothing\n"
                       "\n"
                       "Slide 29: When a class instance is passed by reference,\n"
                       "changes are reflected in the original\n"
                       "void offsetPoint(Point &p, double x, double y) { // works\n"
                       "\n"
                       "Slide 39: Pass objects by pointer if they need to be modified\n"
                       "void offsetVector(Vector *v, double offsetX, double offsetY) {\n"
                       "  v->start.x +=offsetX;\n"
                       "  v->end.x +=offsetX;\n"
                       "  v->start.y +=offsetY;\n"
                       "  v->end.y += offsetY;\n"
                       "}\n"
                       "offsetVector(&vec, 1.0, 1.5);";

    task = startTask(level, 0, TASK_TYPE, "Slide 39: call the pointer version of offsetVector to move vec by 1.0 and 1.5.",
                     "void offsetVector(Vector *v, double offsetX, double offsetY);\n"
                     "\n"
                     "Vector vec;\n"
                     "____");
    addAnswer(task, "offsetVector(&vec, 1.0, 1.5);");
    addMistake(task, "offsetVector(vec, 1.0, 1.5);", "This version takes a Vector *, so it needs vec's address.");
    addMistake(task, "offsetVector(*vec, 1.0, 1.5);", "vec isn't a pointer, so * doesn't work on it.");
    setHints(task, "The parameter is a Vector *.",
                   "Pass the address of vec.",
                   "offsetVector(&..., 1.0, 1.5);");

    task = startTask(level, 1, TASK_TRACE, "Slide 28: offsetPoint takes a Point by value. What is p.x after the call?",
                     "void offsetPoint(Point p, double x, double y) {\n"
                     "    p.x += x;\n"
                     "    p.y += y;\n"
                     "}\n"
                     "Point p;\n"
                     "p.x = 3.0;\n"
                     "p.y = 4.0;\n"
                     "offsetPoint(p, 1.0, 2.0);");
    addAnswer(task, "3");
    addAnswer(task, "3.0");
    addAnswer(task, "p.x=3");
    addAnswer(task, "p.x=3.0");
    addMistake(task, "4", "Passing by value gives the function a copy, so the original p doesn't change.");
    addMistake(task, "4.0", "Passing by value gives the function a copy, so the original p doesn't change.");
    setHints(task, "Is the parameter a Point, a Point & or a Point *?",
                   "Pass by value works on a copy.",
                   "So the original p keeps its x.");
    task->layout = "offsetPoint(Point p, ...) gets a COPY of p.\n"
                   "The copy's x becomes 3.0 + 1.0.\n"
                   "The original p.x is still ...";

    // ---------- Level 33 ----------
    level = startLevel(33, 6, "A pointer to an object", "->", "Classes lecture, slide 57: Using a pointer to an object");
    level->explanation = "A pointer can point at an object: pPtr = &p;. Use -> to reach a member through the pointer, so "
                         "pPtr->x is the same variable as p.x (slide 57). There's only one object, so changing one changes the other.";
    level->example = "Point* pPtr;   // pointer to a Point. No instance created.\n"
                     "Point p;\n"
                     "pPtr = &p;\n"
                     "cout << pPtr->x;   // same as p.x";
    level->inGame = "The game reaches a ball's fields through pointers in exactly this way, for example cue->x.";
    level->slideTitle = "Classes lecture, slide 57: Using a pointer to an object";
    level->slideText = "int main() {\n"
                       "  Point* pPtr; // Pointer to Point object. No instance created.\n"
                       "  Point p;\n"
                       "  // p.x is 0.0, p.y is 0.0\n"
                       "  pPtr = &p;\n"
                       "  cout << p.x << \" \" << p.y;\n"
                       "  cout << pPtr->x << \" \" << pPtr->y;\n"
                       "  pPtr->x=10;\n"
                       "  p.y=20;\n"
                       "  // use the '->' to access the member data/methods when the\n"
                       "  // variable points to an instance\n"
                       "  cout << p.x << \" \" << p.y;\n"
                       "  cout << pPtr->x << \" \" << pPtr->y;\n"
                       "}";

    task = startTask(level, 0, TASK_TYPE, "Slide 57: set x to 10 through pPtr.",
                     "Point* pPtr;\n"
                     "Point p;\n"
                     "pPtr = &p;\n"
                     "____");
    addAnswer(task, "pPtr->x = 10;");
    addAnswer(task, "(*pPtr).x = 10;");
    addMistake(task, "pPtr.x = 10;", "pPtr is a pointer, so . doesn't work on it.");
    addMistake(task, "p->x = 10;", "p is an object, not a pointer. Here the pointer is pPtr.");
    addMistake(task, "*pPtr.x = 10;", "The . happens before the *, so this doesn't work.");
    setHints(task, "You're going through a pointer to reach a member.",
                   "Slide 57: 'use the -> to access the member data/methods when the variable points to an instance'.",
                   "pPtr ... x = 10;");

    task = startTask(level, 1, TASK_TRACE, "Slide 57: what does the last line print?",
                     "Point* pPtr;\n"
                     "Point p;        // p.x is 0.0, p.y is 0.0\n"
                     "pPtr = &p;\n"
                     "pPtr->x = 10;\n"
                     "p.y = 20;\n"
                     "cout << pPtr->y;");
    addAnswer(task, "20");
    addMistake(task, "0", "pPtr points at p, so pPtr->y and p.y are the same variable.");
    addMistake(task, "10", "10 went into x. The question is about y.");
    setHints(task, "How many Point objects are there?",
                   "pPtr points at p.",
                   "So p.y = 20 also changes pPtr->y.");
    task->layout = "pPtr = &p;   so pPtr->y IS p.y\n"
                   "p.y = 20;\n"
                   "cout << pPtr->y;   prints ...";

    // ---------- Level 34 ----------
    level = startLevel(34, 6, "Creating objects with new", "new Point", "Classes lecture, slides 58-60: Using New");
    level->explanation = "Point* p; only makes a pointer: no object exists yet. new creates an instance on the heap and gives back "
                         "its address, which you store in the pointer (slide 58). Two pointers can point at the same object, and "
                         "then a change through one is seen through the other (slides 59-60).";
    level->example = "Point* p = new Point();\n"
                     "Point* pPtr;\n"
                     "pPtr = p;    // two pointers, ONE object";
    level->inGame = "Not used by the game, but slides 58-60 are classic test material.";
    level->slideTitle = "Classes lecture, slides 58-60: Using New";
    level->slideText = "Slide 58: Use new to create an instance\n"
                       "int main() {\n"
                       "  Point* p; // Pointer to Point object. No instance created.\n"
                       "  p = new Point(); //instance created and p points to it\n"
                       "  // p->x is 0.0, p->y is 0.0\n"
                       "  p->x =10;\n"
                       "  cout << p->x << \" \" << p->y;\n"
                       "}\n"
                       "\n"
                       "Slide 60: Using a pointer to an object created with new\n"
                       "  Point* p;\n"
                       "  p = new Point();\n"
                       "  Point* pPtr;\n"
                       "  pPtr = p;\n"
                       "  pPtr->x=10;\n"
                       "  p->y=20;";

    task = startTask(level, 0, TASK_TYPE, "Slide 58: create a Point instance and make p point at it.",
                     "Point* p; // Pointer to Point object. No instance created.\n"
                     "____");
    addAnswer(task, "p = new Point();");
    addAnswer(task, "p = new Point;");
    addMistake(task, "Point p = new Point();", "p is already declared as a Point*, so just assign to it.");
    addMistake(task, "p = Point();", "That makes a temporary Point, not one on the heap.");
    addMistake(task, "*p = new Point();", "new returns an address, so store it in p itself.");
    setHints(task, "No object exists yet, so one has to be created.",
                   "Objects on the heap are made with new.",
                   "p = new ...;");

    task = startTask(level, 1, TASK_TRACE, "Slides 59-60: what does the last line print?",
                     "Point* p = new Point();   // x and y start as 0\n"
                     "Point* pPtr;\n"
                     "pPtr = p;\n"
                     "pPtr->x = 10;\n"
                     "p->y = 20;\n"
                     "cout << p->x;");
    addAnswer(task, "10");
    addMistake(task, "0", "p and pPtr point at the same object, so setting pPtr->x also sets p->x.");
    addMistake(task, "20", "20 went into y. The question is about x.");
    setHints(task, "How many Point objects does new create here?",
                   "pPtr = p; copies the address, not the object.",
                   "So pPtr->x and p->x are the same variable.");
    task->layout = "pPtr = p;   both point at the SAME Point\n"
                   "pPtr->x = 10;   sets that Point's x\n"
                   "cout << p->x;   reads the same x";

    // ---------- Level 35 ----------
    level = startLevel(35, 6, "The this pointer", "this", "Classes lecture, slide 61; Operator Overloading notes");
    level->explanation = "Inside a method, this is a pointer to the object the method was called on, so this->x is the object's own "
                         "x (slide 61). Because this is a pointer, *this is the object itself: the overloaded ++ in the Operator "
                         "Overloading notes returns the Clock that way.";
    level->example = "Box operator+(const Box& b) {   // Operator Overloading notes\n"
                     "    Box box;\n"
                     "    box.length = this->length + b.length;\n"
                     "    return box;\n"
                     "}";
    level->inGame = "Not used by the game, but this appears in both the Classes and Operator Overloading notes.";
    level->slideTitle = "Classes lecture, slide 61; Operator Overloading notes";
    level->slideText = "Slide 61: The \"this\" pointer\n"
                       "class Point {\n"
                       "public:\n"
                       "  double x, y;\n"
                       "  Point() {\n"
                       "    this->x = 0.0; this->y = 0.0;\n"
                       "    cout << \"default constructor\" << endl;\n"
                       "  }\n"
                       "};\n"
                       "\n"
                       "Operator Overloading notes:\n"
                       "Clock Clock::operator++() // prefix form\n"
                       "{\n"
                       "  tick();        // increment\n"
                       "  return *this;  // return itself for assignment\n"
                       "}";

    task = startTask(level, 0, TASK_TYPE, "Slide 61: set x to 0.0 using the this pointer.",
                     "Point() {\n"
                     "    ____\n"
                     "    this->y = 0.0;\n"
                     "    cout << \"default constructor\" << endl;\n"
                     "}");
    addAnswer(task, "this->x = 0.0;");
    addAnswer(task, "this->x = 0;");
    addAnswer(task, "(*this).x = 0.0;");
    addMistake(task, "this.x = 0.0;", "this is a pointer, so . doesn't work on it.");
    addMistake(task, "*this.x = 0.0;", "The . happens before the *, so this doesn't work.");
    addMistake(task, "x = 0.0;", "That works too, but the task is to use the this pointer.");
    setHints(task, "this points at the current object.",
                   "Reach a member through a pointer with ->.",
                   "Copy the style of the line below the gap.");

    task = startTask(level, 1, TASK_TYPE, "Operator Overloading notes: the prefix ++ must return the Clock object itself.",
                     "Clock Clock::operator++() {   // prefix form\n"
                     "    tick();                   // increment\n"
                     "    ____                      // return itself for assignment\n"
                     "}");
    addAnswer(task, "return *this;");
    addMistake(task, "return this;", "this is a pointer (Clock *), but the function returns a Clock.");
    addMistake(task, "return Clock;", "Clock is the type, not an object.");
    addMistake(task, "return *Clock;", "Clock is the type. The object is reached through this.");
    setHints(task, "The function returns a Clock, not a pointer.",
                   "this is a pointer to the current object.",
                   "Dereference this.");
    task->layout = "return ___;   // the object itself, not a pointer to it";

    // ---------- Level 36 ----------
    level = startLevel(36, 6, "Copies and copy constructors", "copies", "Classes lecture, slides 26, 64 and 71-72");
    level->explanation = "Assigning one object to another copies every field (slide 26). That's fine for numbers, but if a field is a "
                         "pointer, only the address is copied, so both objects share the same memory: a shallow copy (slide 71). "
                         "A custom copy constructor can make a real copy of what the pointer points at (slide 72).";
    level->example = "Point(Point &o) {   // custom copy constructor (slide 64)\n"
                     "    x = o.x;\n"
                     "    y = o.y;\n"
                     "}\n"
                     "Point r = q;        // this calls it";
    level->inGame = "Not used by the game, but slide 71's surprise makes a great test question.";
    level->slideTitle = "Classes lecture, slides 71-72";
    level->slideText = "Slide 71: Why make a copy constructor? Assigning all fields\n"
                       "(default copy constructor) may not be what you want\n"
                       "class MITStudent {\n"
                       "public:\n"
                       "   int studentID;\n"
                       "   char *name;\n"
                       "   MITStudent() { studentID = 0; name = NULL; }\n"
                       "};\n"
                       "MITStudent student2 = student1;\n"
                       "student2.name[0] = 'b';\n"
                       "cout << student1.name;   (the slide's output is left out: it's the trace answer)\n"
                       "\n"
                       "Slide 72: strdup() is a c function that duplicates a string.\n"
                       "MITStudent(MITStudent &o) {\n"
                       "  studentID = o.studentID;\n"
                       "  name = _strdup(o.name);\n"
                       "}";

    task = startTask(level, 0, TASK_TRACE, "Slide 71: what does the last line print?",
                     "MITStudent student1;\n"
                     "student1.studentID = 98;\n"
                     "char n[] = \"foo\";\n"
                     "student1.name = n;           // name is a char *\n"
                     "MITStudent student2 = student1;\n"
                     "student2.name[0] = 'b';\n"
                     "cout << student1.name;");
    addAnswer(task, "boo");
    addMistake(task, "foo", "The default copy copied the pointer, not the chars, so both students share the same name array.");
    setHints(task, "name is a char *, so copying the object copies only the address.",
                   "student1.name and student2.name point at the same array.",
                   "Changing student2.name[0] changes that shared array.");
    task->layout = "student2 = student1 copies the pointer name, not the chars.\n"
                   "So both names point at the array n, which holds \"foo\".\n"
                   "student2.name[0] = 'b' changes n[0], so student1.name is now ...";

    task = startTask(level, 1, TASK_TYPE, "Slide 72: in the copy constructor, give the new student its own copy of the name.",
                     "MITStudent(MITStudent &o) {\n"
                     "    studentID = o.studentID;\n"
                     "    ____\n"
                     "}");
    addAnswer(task, "name = _strdup(o.name);");
    addAnswer(task, "name = strdup(o.name);");
    addMistake(task, "name = o.name;", "That copies the pointer again (a shallow copy), so the two students would still share one name.");
    addMistake(task, "*name = *o.name;", "That copies one char into memory that was never allocated.");
    setHints(task, "Copying the pointer isn't enough.",
                   "Slide 72: 'strdup() is a c function that duplicates a string.' The slide writes it as _strdup.",
                   "name = _strdup(...);");

    // ---------- Level 37 ----------
    level = startLevel(37, 6, "Arrays of object pointers", "Point**", "Classes lecture, slides 76-78");
    level->explanation = "An array can hold pointers to objects: Point* pointPointers[20];. Each element then needs its own object "
                         "made with new, and each one must be deleted (slide 77). If the array of pointers itself comes from new "
                         "(Point** pointPointers = new Point*[30];), free it with delete[] at the end (slide 78).";
    level->example = "Point* points = new Point[30];   // an array of objects (slide 76)\n"
                     "// ...\n"
                     "delete[] points;";
    level->inGame = "Not used by the game, but slides 76-78 bring new, delete and ** together.";
    level->slideTitle = "Classes lecture, slides 76-78";
    level->slideText = "Slide 76: Using new to create the array\n"
                       "Point* points = new Point[30];\n"
                       "...\n"
                       "delete[] points;\n"
                       "\n"
                       "Slide 77: Array of pointers\n"
                       "Point* pointPointers[20];\n"
                       "for (int i = 0; i < 20; i++)\n"
                       "    pointPointers[i] = new Point(i);\n"
                       "...\n"
                       "for (int i = 0; i < 20; i++)\n"
                       "    delete pointPointers[i];\n"
                       "\n"
                       "Slide 78: Using new for the array of pointers\n"
                       "Point** pointPointers = new Point*[30];\n"
                       "...\n"
                       "delete[] pointPointers;";

    task = startTask(level, 0, TASK_TYPE, "Slide 77: make element i point at a new Point(i).",
                     "Point* pointPointers[20];\n"
                     "for (int i = 0; i < 20; i++) {\n"
                     "    ____\n"
                     "}");
    addAnswer(task, "pointPointers[i] = new Point(i);");
    addMistake(task, "pointPointers[i] = Point(i);", "Each element is a Point *, so it needs an object made with new.");
    addMistake(task, "pointPointers = new Point(i);", "That's the whole array. Fill in one element at a time.");
    addMistake(task, "*pointPointers[i] = new Point(i);", "new returns an address, so store it in the element itself.");
    setHints(task, "Each element is a pointer to a Point.",
                   "Create each Point on the heap.",
                   "pointPointers[i] = new ...;");

    task = startTask(level, 1, TASK_TYPE, "Slide 78: each Point has been deleted. Now free the array of pointers.",
                     "Point** pointPointers = new Point*[30];\n"
                     "// ...\n"
                     "for (int i = 0; i < 30; i++) {\n"
                     "    delete pointPointers[i];\n"
                     "}\n"
                     "____");
    addAnswer(task, "delete[] pointPointers;");
    addMistake(task, "delete pointPointers;", "pointPointers was made with new Point*[30], which is an array.");
    addMistake(task, "delete[] pointPointers[i];", "The Points are already deleted. Now free the array that held them.");
    setHints(task, "pointPointers itself was made with new[].",
                   "Arrays need delete[].",
                   "delete[] followed by the array's name.");

    // ---------- Level 38 ----------
    level = startLevel(38, 6, "Destructors", "~", "Classes lecture, slide 79: Destructors");
    level->explanation = "A destructor is called automatically when an object goes out of scope. Its name is the class name with a "
                         "~ in front. If the constructor allocated memory with new, the destructor is where you delete it, so the "
                         "object never leaks (slide 79).";
    level->example = "class TwentyThousandPoints {\n"
                     "public:\n"
                     "    Point* points;\n"
                     "    TwentyThousandPoints() {\n"
                     "        points = new Point[20000];\n"
                     "    }\n"
                     "};";
    level->inGame = "The game's own DoublyLinkedList has a destructor that deletes every node.";
    level->slideTitle = "Classes lecture, slide 79: Destructors";
    level->slideText = "A destructor is called when an object goes out of Scope\n"
                       "~ClassName()\n"
                       "\n"
                       "class TwentyThousandPoints\n"
                       "{\n"
                       "public:\n"
                       "    Point* points;\n"
                       "    TwentyThousandPoints()\n"
                       "    {\n"
                       "        points = new Point[20000];\n"
                       "    }\n"
                       "    ~TwentyThousandPoints()\n"
                       "    {\n"
                       "        delete[] points;\n"
                       "    }\n"
                       "};";

    task = startTask(level, 0, TASK_TYPE, "Slide 79: finish the destructor so the 20000 points are freed.",
                     "~TwentyThousandPoints() {\n"
                     "    ____\n"
                     "}");
    addAnswer(task, "delete[] points;");
    addMistake(task, "delete points;", "points was made with new Point[20000], which is an array.");
    addMistake(task, "delete[] *points;", "delete[] takes the pointer itself.");
    addMistake(task, "points = nullptr;", "That forgets the address but leaves the memory allocated: a leak.");
    setHints(task, "The constructor used new[].",
                   "Every new[] needs a matching delete[].",
                   "delete[] followed by the pointer.");

    task = startTask(level, 1, TASK_TYPE, "Type the destructor's first line: just its name and brackets.",
                     "class TwentyThousandPoints {\n"
                     "public:\n"
                     "    Point* points;\n"
                     "    ____ {\n"
                     "        delete[] points;\n"
                     "    }\n"
                     "};");
    addAnswer(task, "~TwentyThousandPoints()");
    addMistake(task, "TwentyThousandPoints()", "That's the constructor's name. A destructor has one extra symbol in front.");
    addMistake(task, "void ~TwentyThousandPoints()", "A destructor has no return type, not even void.");
    setHints(task, "A destructor has the same name as the class.",
                   "With one extra symbol in front.",
                   "~...()");
    task->layout = "~___()";
}
