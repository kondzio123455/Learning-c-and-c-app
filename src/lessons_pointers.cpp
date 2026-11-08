#include "questions.h"

// ============================================================
// Chapters 1 to 4: the Pointers lecture and Lab 2 Part 1.
// Every task uses the lecturer's own code and names.
// Mistake messages explain what went wrong without giving the answer;
// the three hints get more specific, and are only shown when asked for.
// ============================================================

void setupPointerLessons() {
    Level *level;
    Task *task;

    // =========================================================
    // Chapter 1: Pointer basics (Pointers lecture)
    // =========================================================

    // ---------- Level 1 ----------
    level = startLevel(1, 1, "Addresses with &", "&", "Pointers lecture, slides 3-8: Computer Memory, Addressing Concept, Twin Operators");
    level->explanation = "Memory is a row of slots, and every slot has an address (slide 3). A pointer is a variable that "
                         "stores the address of another variable - not its value (slide 6). The address-of operator & "
                         "gets a variable's address (slide 7).";
    level->example = "int x = 10;\n"
                     "int *ptr;\n"
                     "ptr = &x;    // ptr now holds the address of x";
    level->inGame = "Under the table, the balls are shown in memory with their addresses. Soon you'll point at them.";
    level->slideTitle = "Pointers lecture, slides 3, 5 and 7";
    level->slideText = "Slide 3: Computer Memory Revisited\n"
                       "- Computers store data in memory slots\n"
                       "- Each slot has an unique address\n"
                       "\n"
                       "Slide 5: Addressing Concept\n"
                       "- Pointer stores the address of another entity\n"
                       "- It refers to a memory location\n"
                       "int i = 5;\n"
                       "int* ptr; /* declare a pointer variable */\n"
                       "ptr = &i; /* store address-of i to ptr */\n"
                       "*ptr = 10; /* refer to contents a that ptr address*/\n"
                       "\n"
                       "Slide 7: Twin Operators\n"
                       "- &: Address-of operator\n"
                       "  - Get the address of an entity\n"
                       "  - e.g. ptr = &j;";

    task = startTask(level, 0, TASK_TYPE, "Store the address of i in ptr.",
                     "int i = 5;\n"
                     "int* ptr;   /* declare a pointer variable */\n"
                     "____        /* store address-of i to ptr */");
    addAnswer(task, "ptr = &i;");
    addMistake(task, "ptr = i;", "That copies the value 5 into ptr. A pointer has to hold an address, not a value.");
    addMistake(task, "*ptr = &i;", "*ptr means 'the thing ptr points at', but ptr doesn't point anywhere yet. Store into ptr itself.");
    addMistake(task, "int* ptr = &i;", "ptr is already declared on the line above, so don't write its type again.");
    addMistake(task, "&ptr = i;", "&ptr is the address of ptr itself, and you can't assign to it. Which side needs the &?");
    setHints(task, "A pointer stores an address, not a value.",
                   "Slide 7 lists the twin operators. The first is &: 'Address-of operator - get the address of an entity'.",
                   "Put that operator in front of i, and store the result in ptr.");

    task = startTask(level, 1, TASK_TYPE, "Make ptr point at j, like slide 7.",
                     "int i = 40, j = 33;\n"
                     "int* ptr;\n"
                     "____   // ptr should hold the address of j");
    addAnswer(task, "ptr = &j;");
    addMistake(task, "ptr = &i;", "That points at i. The task wants j.");
    addMistake(task, "ptr = j;", "That copies j's value (33) into ptr. The pointer needs j's address.");
    addMistake(task, "*ptr = &j;", "*ptr means 'the thing ptr points at', but ptr doesn't point anywhere yet. Store into ptr itself.");
    setHints(task, "ptr needs to hold where j lives in memory.",
                   "The address-of operator gets that.",
                   "Store &... into ptr.");

    // ---------- Level 2 ----------
    level = startLevel(2, 1, "Dereferencing with *", "*", "Pointers lecture, slides 6-8: What actually ptr is?, Twin Operators");
    level->explanation = "Put * in front of a pointer to reach the variable it points at. This is called dereferencing (slide 8). "
                         "If ptr = &i, then *ptr IS i: reading *ptr gives i's value, and *ptr = 99; changes i. "
                         "ptr itself still holds the same address.";
    level->example = "int x = 10;\n"
                     "int *ptr = &x;\n"
                     "int y = *ptr;   // y gets the value of x (10)";
    level->inGame = "The game reads your shot power through a pointer: int *powerPtr = &power; (look under the power bar).";
    level->slideTitle = "Pointers lecture, slides 6 and 8";
    level->slideText = "Slide 6: What actually ptr is?\n"
                       "- ptr is a variable storing an address\n"
                       "- ptr is NOT storing the actual value of i\n"
                       "int i = 5;\n"
                       "int* ptr;\n"
                       "ptr = &i;\n"
                       "printf(\"i = %d\\n\", i);\n"
                       "printf(\"*ptr = %d\\n\", *ptr);\n"
                       "printf(\"ptr = %p\\n\", ptr);\n"
                       "(the slide's output is left out here, because it answers the trace question)\n"
                       "\n"
                       "Slide 8: Twin Operators\n"
                       "- *: De-reference operator\n"
                       "  - Refer to the content of the referee\n"
                       "  - e.g. *ptr = 99;";

    task = startTask(level, 0, TASK_TRACE, "This is slide 6. What number does the last line print for *ptr?",
                     "int i = 5;\n"
                     "int* ptr;\n"
                     "ptr = &i;\n"
                     "printf(\"i = %d\\n\", i);\n"
                     "printf(\"*ptr = %d\\n\", *ptr);");
    addAnswer(task, "5");
    addAnswer(task, "*ptr=5");
    addMistake(task, "effff5e0", "That's what ptr itself holds: an address. *ptr reads the value stored at that address.");
    setHints(task, "ptr holds the address of i.",
                   "*ptr means 'the int stored at the address in ptr'.",
                   "So here *ptr is the same thing as i.");
    task->layout = "Line 3: ptr = &i; so ptr holds the address of i.\n"
                   "*ptr means: the int stored at that address.\n"
                   "So *ptr is the same int as i, which is ...";

    task = startTask(level, 1, TASK_TYPE, "Use ptr to change j to 99. (Don't write j = 99.)",
                     "int i = 40, j = 33;\n"
                     "int* ptr;\n"
                     "ptr = &j;\n"
                     "____   // j should now be 99");
    addAnswer(task, "*ptr = 99;");
    addMistake(task, "ptr = 99;", "Without the * you'd change the address stored in ptr, not j.");
    addMistake(task, "j = 99;", "That works, but the task is to change j through the pointer.");
    addMistake(task, "&ptr = 99;", "& gives an address, and you can't assign to it. Which operator reaches the variable ptr points at?");
    setHints(task, "You want to change the variable that ptr points at.",
                   "Slide 8: '*: De-reference operator - refer to the content of the referee'.",
                   "Put that operator in front of ptr, then = 99;");

    // ---------- Level 3: the game's call line ----------
    level = startLevel(3, 1, "Call your ball", "call ball", "Pointers lecture, slide 24: ptr = &(a[2])");
    level->explanation = "All 16 balls live in one array: Ball balls[16];. Just like a[2] is an element of an array, balls[14] is "
                         "ball 14, and &balls[14] is its address. Slide 24 does the same with ptr = &(a[2]);. A pointer to a "
                         "Ball has the type Ball *. Pointing target at a ball is how you call your shot.";
    level->example = "float a[4];\n"
                     "float* ptr;\n"
                     "ptr = &(a[2]);   // ptr points at element 2 (slide 24)";
    level->inGame = "From now on you call a ball before every shot (except the break). Pot the ball you called and you shoot "
                    "again. Your call line stays in its box, so next time you only change the number.";
    level->slideTitle = "Pointers lecture, slide 24";
    level->slideText = "Slide 24: Pointer Arithmetic and Array\n"
                       "float a[4];\n"
                       "float* ptr;\n"
                       "ptr = &(a[2]);\n"
                       "*ptr = 3.14;\n"
                       "ptr++;\n"
                       "*ptr = 9.0;\n"
                       "\n"
                       "- ptr = &(a[2]); stores the address of element 2 of the array in ptr.\n"
                       "- In the game, Ball *target = &balls[3]; does the same with the balls array.";

    task = startTask(level, 0, TASK_TYPE, "Call the ball you want to pot: make a Ball pointer called target that points at it. "
                     "Hover over a ball to see its number. On an open table any ball except the 8 is allowed.",
                     "Ball balls[16];   // balls[0] is the cue ball\n"
                     "____");
    addAnswer(task, "Ball *target = &balls[#];");
    addAnswer(task, "Ball *target = &(balls[#]);");
    addMistake(task, "Ball *target = balls[#];", "balls[N] is the ball itself, not where it lives. A pointer needs an address.");
    addMistake(task, "Ball target = &balls[#];", "target has to be a pointer, so its type needs a *.");
    addMistake(task, "target = &balls[#];", "target hasn't been declared yet. Start with its type.");
    setHints(task, "You need a pointer to a Ball that points at one element of the array.",
                   "Slide 24 does this for a float array: ptr = &(a[2]);",
                   "Ball *target = the address of balls[your ball's number];");

    task = startTask(level, 1, TASK_TYPE, "Call the next ball you want to pot by pointing target at it.",
                     "Ball balls[16];   // balls[0] is the cue ball\n"
                     "____");
    addAnswer(task, "Ball *target = &balls[#];");
    addAnswer(task, "Ball *target = &(balls[#]);");
    addMistake(task, "Ball *target = balls[#];", "balls[N] is the ball itself, not where it lives. A pointer needs an address.");
    addMistake(task, "Ball target = &balls[#];", "target has to be a pointer, so its type needs a *.");
    addMistake(task, "target = &balls[#];", "target hasn't been declared yet. Start with its type.");
    setHints(task, "A pointer to a Ball, pointing at one element of balls.",
                   "Get an element's address with &.",
                   "Ball *target = the address of balls[your ball's number];");

    // The call line, used before every shot after Level 3
    task = getCallTask(false);
    task->kind = TASK_TYPE;
    addAnswer(task, "Ball *target = &balls[#];");
    addAnswer(task, "Ball *target = &(balls[#]);");
    addMistake(task, "Ball *target = balls[#];", "balls[N] is the ball itself, not where it lives. A pointer needs an address.");
    addMistake(task, "Ball target = &balls[#];", "target has to be a pointer, so its type needs a *.");
    addMistake(task, "target = &balls[#];", "target hasn't been declared yet. Start with its type.");

    task = getCallTask(true);
    task->kind = TASK_TYPE;
    addAnswer(task, "Ball *target = balls + #;");
    addAnswer(task, "Ball *target = (balls + #);");
    addAnswer(task, "Ball *target = # + balls;");
    addMistake(task, "Ball *target = *(balls + #);", "*(balls + N) is the ball itself. target needs its address.");
    addMistake(task, "Ball target = balls + #;", "target has to be a pointer, so its type needs a *.");
    addMistake(task, "Ball *target = &(balls + #);", "balls + N is already an address, so you don't need the &.");
    addMistake(task, "target = balls + #;", "target hasn't been declared yet. Start with its type.");

    // ---------- Level 4 ----------
    level = startLevel(4, 1, "Tracing a pointer", "trace ptr", "Pointers lecture, slides 10-15: An Illustration");
    level->explanation = "A pointer can be moved to point at a different variable. What *ptr = ... changes depends on where ptr "
                         "points at that moment. To trace code like this, keep a table of every variable and update it after "
                         "each line, exactly like the Data Table on slides 10-15.";
    level->example = "int a = 1, b = 2;\n"
                     "int* p = &a;\n"
                     "*p = 5;    // a is 5\n"
                     "p = &b;    // p now points at b\n"
                     "*p = 6;    // b is 6, a is still 5";
    level->inGame = "Not used by the game, but tracing like this is a classic test question.";
    level->slideTitle = "Pointers lecture, slides 10-15: An Illustration";
    level->slideText = "int i = 5, j = 10;\n"
                       "int* ptr;\n"
                       "ptr = &i;\n"
                       "*ptr = 3;\n"
                       "ptr = &j;\n"
                       "*ptr = -2;\n"
                       "\n"
                       "Data Table on slide 12 (after ptr = &i;):\n"
                       "Name   Type     Description                  Value\n"
                       "i      int      integer variable             5\n"
                       "j      int      integer variable             10\n"
                       "ptr    int *    integer pointer variable     address of i\n"
                       "*ptr   int      de-reference of ptr          5\n"
                       "\n"
                       "(slides 13-15 fill in the rest of the table, which would give the answers away)";

    task = startTask(level, 0, TASK_TRACE, "This is the code from slides 10-15. What is the value of i at the end?",
                     "int i = 5, j = 10;\n"
                     "int* ptr;\n"
                     "ptr = &i;\n"
                     "*ptr = 3;\n"
                     "ptr = &j;\n"
                     "*ptr = -2;");
    addAnswer(task, "3");
    addAnswer(task, "i=3");
    addMistake(task, "5", "i doesn't keep its starting value: one of the lines changes it through ptr.");
    addMistake(task, "-2", "By the time *ptr = -2 runs, ptr has been moved to point at j.");
    addMistake(task, "10", "10 is j's starting value. The question is about i.");
    setHints(task, "Make a table with i, j and ptr, and update it after every line.",
                   "*ptr = 3 runs while ptr points at i.",
                   "After ptr = &j; nothing changes i again.");
    task->layout = "after ptr = &i;    i = 5    j = 10   ptr -> i\n"
                   "after *ptr = 3;    i = ?    j = 10   ptr -> i\n"
                   "after ptr = &j;    i = ?    j = 10   ptr -> j\n"
                   "after *ptr = -2;   (this one changes j)";

    task = startTask(level, 1, TASK_TRACE, "Same code from slides 10-15. What is the value of j at the end?",
                     "int i = 5, j = 10;\n"
                     "int* ptr;\n"
                     "ptr = &i;\n"
                     "*ptr = 3;\n"
                     "ptr = &j;\n"
                     "*ptr = -2;");
    addAnswer(task, "-2");
    addAnswer(task, "j=-2");
    addMistake(task, "10", "j doesn't keep its starting value: the last line changes it through ptr.");
    addMistake(task, "3", "3 went into i, while ptr pointed at i.");
    setHints(task, "Make a table with i, j and ptr, and update it after every line.",
                   "ptr = &j; moves the pointer to j.",
                   "The last line writes through ptr, which points at j by then.");
    task->layout = "after ptr = &i;    i = 5   j = 10   ptr -> i\n"
                   "after *ptr = 3;    i = 3   j = 10   ptr -> i\n"
                   "after ptr = &j;    i = 3   j = 10   ptr -> j\n"
                   "after *ptr = -2;   i = 3   j = ?    ptr -> j";

    // ---------- Level 5 ----------
    level = startLevel(5, 1, "Pass by value and by pointer", "pass ptr", "Pointers lecture, slide 9; Pointers & Linked List lecture: Pass-by-Value vs Pass-by-Reference");
    level->explanation = "When you pass a normal variable to a function, the function gets a copy, so the original doesn't change "
                         "(pass by value). Pass the variable's address instead, and the function can change the original "
                         "through the pointer. Careful with brackets: *numPtr++ moves the pointer, it doesn't change the value.";
    level->example = "void f(int *ptr) {   // slide 9\n"
                     "    *ptr = 5;\n"
                     "}\n"
                     "int i = 3;\n"
                     "f(&i);              // i is now 5";
    level->inGame = "Every time you pot one of your balls, the game calls addPoint(&player.score), which works like increment.";
    level->slideTitle = "Pointers lecture, slide 9: Example: Pass by Pointer";
    level->slideText = "- Modify behaviour in argument passing\n"
                       "\n"
                       "void f(int j)              void f(int *ptr)\n"
                       "{                          {\n"
                       "  j = 5;                     *ptr = 5;\n"
                       "}                          }\n"
                       "void g()                   void g()\n"
                       "{                          {\n"
                       "  int i = 3;                 int i = 3;\n"
                       "  f(i);                      f(&i);\n"
                       "}                          }\n"
                       "\n"
                       "The slide asks: i = 3?   i = 5?";

    task = startTask(level, 0, TASK_TRACE, "Slide 9: this f takes an int, not a pointer. What is i after the call f(i)?",
                     "void f(int j) {\n"
                     "    j = 5;\n"
                     "}\n"
                     "void g() {\n"
                     "    int i = 3;\n"
                     "    f(i);\n"
                     "    // what is i here?\n"
                     "}");
    addAnswer(task, "3");
    addAnswer(task, "i=3");
    addMistake(task, "5", "f was given a copy of i (pass by value). Changing j inside f doesn't touch i.");
    setHints(task, "Look at f's parameter: is it an int or an int *?",
                   "With pass by value, the function works on a copy.",
                   "So i keeps the value it had before the call.");
    task->layout = "f(i) gives f a copy:   inside f, j = 3\n"
                   "j = 5;                 inside f, j = 5   (only the copy changes)\n"
                   "back in g:             i = ?";

    task = startTask(level, 1, TASK_TYPE, "Finish increment (from the lecture) so it adds 1 to the caller's variable.",
                     "void increment(int *numPtr) {\n"
                     "    ____   // modify the actual variable\n"
                     "}\n"
                     "int x = 5;\n"
                     "increment(&x);   // x should be 6");
    addAnswer(task, "(*numPtr)++;");
    addAnswer(task, "*numPtr = *numPtr + 1;");
    addAnswer(task, "*numPtr += 1;");
    addAnswer(task, "++(*numPtr);");
    addAnswer(task, "++*numPtr;");
    addMistake(task, "*numPtr++;", "The ++ happens to numPtr before the *, so this moves the pointer instead of changing x.");
    addMistake(task, "numPtr++;", "That moves the pointer to the next address. You want to change the value it points at.");
    addMistake(task, "x++;", "x isn't visible inside increment. The function can only reach it through numPtr.");
    setHints(task, "You need to change the int that numPtr points at.",
                   "Dereference first, then add 1 - and think about which happens first.",
                   "Put brackets around the dereference, then add 1.");

    // ---------- Level 6 ----------
    level = startLevel(6, 1, "Pointer to a pointer", "**", "Pointers lecture, slides 16-21: Double Indirection");
    level->explanation = "A pointer can point at another pointer: int **pptr = &ptr; stores the address of ptr. Then *pptr IS ptr "
                         "(so *pptr = &i; moves ptr), and **pptr is the int that ptr points at. Trace it with a data table, "
                         "one line at a time.";
    level->example = "int i = 1;\n"
                     "int *ptr = &i;\n"
                     "int **pptr = &ptr;\n"
                     "**pptr = 4;    // i is now 4";
    level->inGame = "Linked lists use this later: Node **ptrToHeadPtr lets a function change your head pointer.";
    level->slideTitle = "Pointers lecture, slides 16-21: Double Indirection";
    level->slideText = "int i = 5, j = 10;\n"
                       "int *ptr;\n"
                       "int **pptr;   /* declare a pointer-to-pointer-to-integer variable */\n"
                       "ptr = &i;\n"
                       "pptr = &ptr;  /* store address-of ptr to pptr */\n"
                       "*ptr = 3;\n"
                       "**pptr = 7;\n"
                       "ptr = &j;\n"
                       "**pptr = 9;\n"
                       "*pptr = &i;\n"
                       "*ptr = -2;\n"
                       "\n"
                       "Data Table on slide 17 (after pptr = &ptr;):\n"
                       "ptr    int *    integer pointer variable           address of i\n"
                       "pptr   int **   integer pointer pointer variable   address of ptr\n"
                       "*pptr  int *    de-reference of pptr               value of ptr (address of i)\n"
                       "\n"
                       "(slides 18-20 fill in the rest, and slide 21 is the exercise)";

    task = startTask(level, 0, TASK_TRACE, "This is the code from slide 16. What is j at the end?",
                     "int i = 5, j = 10;\n"
                     "int *ptr;\n"
                     "int **pptr;\n"
                     "ptr = &i;\n"
                     "pptr = &ptr;\n"
                     "*ptr = 3;\n"
                     "**pptr = 7;\n"
                     "ptr = &j;\n"
                     "**pptr = 9;\n"
                     "*pptr = &i;\n"
                     "*ptr = -2;");
    addAnswer(task, "9");
    addAnswer(task, "j=9");
    addMistake(task, "10", "j does change: look at the line straight after ptr = &j;.");
    addMistake(task, "-2", "By the last line, *pptr = &i; has pointed ptr back at i.");
    addMistake(task, "7", "7 went into i. **pptr goes to ptr, then to whatever ptr points at at that moment.");
    setHints(task, "Keep a table for i, j, ptr and pptr.",
                   "**pptr means: go to ptr, then to what ptr points at.",
                   "After ptr = &j; the next **pptr = 9 writes into j, and nothing changes j after that.");
    task->layout = "ptr = &i; pptr = &ptr;   ptr -> i\n"
                   "*ptr = 3; **pptr = 7;    i = 7\n"
                   "ptr = &j;                ptr -> j\n"
                   "**pptr = 9;              j = ?\n"
                   "*pptr = &i; *ptr = -2;   (these two only change ptr and i)";

    task = startTask(level, 1, TASK_TRACE, "This is the exercise on slide 21. What is i at the end?",
                     "int i = 2, j = 8;\n"
                     "int* ptr;\n"
                     "int** pptr;\n"
                     "ptr = &i;\n"
                     "pptr = &ptr;\n"
                     "*ptr = 7;\n"
                     "**pptr = 12;\n"
                     "ptr = &j;\n"
                     "**pptr = 34;\n"
                     "*pptr = &i;\n"
                     "*ptr = -25;");
    addAnswer(task, "-25");
    addAnswer(task, "i=-25");
    addMistake(task, "12", "Not the end yet: *pptr = &i; points ptr back at i before the last line.");
    addMistake(task, "2", "i is changed several times through ptr and pptr.");
    addMistake(task, "34", "34 went into j, while ptr pointed at j.");
    setHints(task, "Track ptr carefully: it moves twice.",
                   "*pptr = &i; changes ptr, because *pptr IS ptr.",
                   "So the last line writes through a pointer that points at i again.");
    task->layout = "ptr = &i; pptr = &ptr;   ptr -> i\n"
                   "*ptr = 7; **pptr = 12;   i = 12\n"
                   "ptr = &j; **pptr = 34;   j = 34\n"
                   "*pptr = &i;              ptr -> i again\n"
                   "*ptr = -25;              i = ?";

    // ---------- Level 7 ----------
    level = startLevel(7, 1, "Pointing at nothing: nullptr", "nullptr", "Pointers lecture, slide 39: Pointer to nullptr");
    level->explanation = "An un-initialised pointer could contain any value, so it's safer to point it at nothing on purpose: "
                         "nullptr. Before using a pointer that might be empty, check it. In C you'll see NULL instead of nullptr.";
    level->example = "int *p = nullptr;      // points at nothing yet\n"
                     "if (p != nullptr) {\n"
                     "    cout << *p;        // only safe if p points somewhere\n"
                     "}";
    level->inGame = "lastPotted points at the last ball potted. Until a ball goes down it's nullptr - look under the table.";
    level->slideTitle = "Pointers lecture, slide 39: Pointer to nullptr";
    level->slideText = "int *ptr;\n"
                       "//un-initialised pointer could contain any value\n"
                       "..\n"
                       "//set pointer = to nullptr value.\n"
                       "ptr =nullptr;\n"
                       "......\n"
                       "//check if pointer points to anything...\n"
                       "If (ptr== nullptr )\n"
                       "{\n"
                       "}";

    task = startTask(level, 0, TASK_TYPE, "Slide 39: set ptr to point at nothing.",
                     "int *ptr;\n"
                     "// un-initialised pointer could contain any value\n"
                     "____   // set pointer = to nullptr");
    addAnswer(task, "ptr = nullptr;");
    addAnswer(task, "ptr = NULL;");
    addMistake(task, "*ptr = nullptr;", "*ptr is the int that ptr points at, and ptr doesn't point anywhere safe yet. Set the pointer itself.");
    addMistake(task, "ptr == nullptr;", "== compares two things. To set the pointer, use a single =.");
    addMistake(task, "int *ptr = nullptr;", "ptr is already declared above, so don't write its type again.");
    setHints(task, "You want to change the pointer itself, not what it points at.",
                   "The 'points at nothing' value in C++ is nullptr.",
                   "ptr = ...;");

    task = startTask(level, 1, TASK_TYPE, "Type the if line that checks whether ptr points at nothing.",
                     "____   // check if pointer points to anything\n"
                     "{\n"
                     "    cout << \"ptr points at nothing\";\n"
                     "}");
    addAnswer(task, "if (ptr == nullptr)");
    addAnswer(task, "if (ptr == NULL)");
    addAnswer(task, "if (nullptr == ptr)");
    addAnswer(task, "if (!ptr)");
    addMistake(task, "if (ptr = nullptr)", "A single = would SET ptr to nullptr. To compare, use ==.");
    addMistake(task, "if (*ptr == nullptr)", "*ptr reads through the pointer, which crashes if it's empty. Compare the pointer itself.");
    addMistake(task, "if (ptr != nullptr)", "!= checks that it DOES point at something. The task asks for the opposite.");
    setHints(task, "Compare the pointer with nullptr.",
                   "Comparing uses ==, not =.",
                   "if ( ... ) - the { is already on the next line.");

    // ---------- Level 8 ----------
    level = startLevel(8, 1, "Pass by reference vs pointer", "reference", "Pointers lecture, slides 40-41: Comparison of pass by reference and pass by pointer");
    level->explanation = "C++ can also pass by reference: void swap(int &x, int &y). Inside, x and y are used like normal variables "
                         "(no *), but they ARE the caller's variables. The two are very similar and references give neater "
                         "code, but only a pointer can be null: a reference must always refer to a real variable (slide 41).";
    level->example = "int a = 45; int b = 35;\n"
                     "swap(&a, &b);   // pass by pointer:   now a=35 b=45\n"
                     "swap(a, b);     // pass by reference: now a=45 b=35";
    level->inGame = "Not used by the game, but slide 41's differences could easily come up in a test.";
    level->slideTitle = "Pointers lecture, slides 40-41";
    level->slideText = "void swap(int* x, int* y)\n"
                       "{\n"
                       "  int z = *x; *x=*y; *y=z;\n"
                       "}\n"
                       "void swap(int &x, int& y)\n"
                       "{\n"
                       "  int z = x; x=y; y=z;\n"
                       "}\n"
                       "\n"
                       "Slide 41: Comparison with pass by reference and pass by pointer\n"
                       "- Very Similar\n"
                       "- Pass by reference seen as neater code\n"
                       "- Use pass by reference when not dealing with null values.\n"
                       "- Generally pass by reference parameter should be a const.\n"
                       "- Pass by pointer can pass a null pointer, pass by reference cannot.";

    task = startTask(level, 0, TASK_TYPE, "Finish the pass-by-reference swap from slide 40.",
                     "void swap(int &x, int &y) {\n"
                     "    int z = x;\n"
                     "    ____\n"
                     "    y = z;\n"
                     "}");
    addAnswer(task, "x = y;");
    addMistake(task, "*x = *y;", "x and y are references, not pointers. You use them like normal variables, with no *.");
    addMistake(task, "y = x;", "That copies the wrong way: x has to get y's value now (z is already saving x).");
    addMistake(task, "x = z;", "z holds x's old value. Right now x needs y's value.");
    setHints(task, "z has saved x's value.",
                   "So x can safely take y's value now.",
                   "References need no * - write it like normal variables.");

    task = startTask(level, 1, TASK_TRACE, "Slide 41: which one can be passed a null value? Type pointer or reference.",
                     "void f(int *p);   // pass by pointer\n"
                     "void g(int &r);   // pass by reference");
    addAnswer(task, "pointer");
    addAnswer(task, "Pointer");
    addAnswer(task, "apointer");
    addAnswer(task, "passbypointer");
    addMistake(task, "reference", "A reference must always refer to a real variable, so it can't be null.");
    addMistake(task, "Reference", "A reference must always refer to a real variable, so it can't be null.");
    setHints(task, "One of them can hold nullptr.",
                   "A reference is just another name for a variable that already exists.",
                   "So only one of them can be 'nothing'.");
    task->layout = "int *p = nullptr;   // allowed: a pointer can be nullptr\n"
                   "int &r = ...;       // a reference must name a real variable\n"
                   "So the answer is one word: the one that can hold nullptr.";

    // =========================================================
    // Chapter 2: Lab 2 swaps
    // =========================================================

    // ---------- Level 9 ----------
    level = startLevel(9, 2, "swap_ints", "swap_ints", "Lab 2 Q1");
    level->explanation = "Lab 2 Q1 asks for swap_ints(int *a, int *b). To swap two values you need a third variable to hold "
                         "one of them for a moment, or it gets overwritten. Because a and b point at the caller's variables, "
                         "swapping the values they point at swaps the caller's x and y.";
    level->example = "int x = 5, y = 10;\n"
                     "swap_ints(&x, &y);   // Now x = 10, y = 5";
    level->inGame = "When your turn ends, the game calls swap_ints(&currentPlayer, &waitingPlayer).";
    level->slideTitle = "Lab 2, Q1: swap_ints";
    level->slideText = "1. swap_ints\n"
                       "Description: Swap the values of two integers using pointers.\n"
                       "Prototype:\n"
                       "void swap_ints(int *a, int *b);\n"
                       "Example:\n"
                       "int x = 5, y = 10;\n"
                       "swap_ints(&x, &y); // Now x = 10, y = 5\n"
                       "\n"
                       "Note: No use of array indexing (arr[i]) in any function -\n"
                       "use only pointer arithmetic.";

    task = startTask(level, 0, TASK_ORDER, "Build the body of swap_ints: click the lines in the order they should run. "
                     "Two of the lines are wrong, so leave them out.",
                     "void swap_ints(int *a, int *b) {\n"
                     "    ____\n"
                     "}");
    addTile(task, "*b = temp;");
    addTile(task, "int temp = *a;");
    addTile(task, "*b = *a;");
    addTile(task, "*a = *b;");
    addTile(task, "int temp = a;");
    addAnswer(task, "int temp = *a; *a = *b; *b = temp;");
    addMistake(task, "int temp = *a; *a = *b; *b = *a;", "By the last line, *a has already been overwritten with b's value, so b gets its own value back.");
    addMistake(task, "*a = *b; int temp = *a; *b = temp;", "temp has to save a's value BEFORE it gets overwritten.");
    addMistake(task, "int temp = a; *a = *b; *b = temp;", "temp is an int, but a is an address. temp should hold the value a points at.");
    setHints(task, "Save one value first, so it isn't lost.",
                   "Save *a, then copy *b into *a, then use the saved value.",
                   "Start with int temp = *a;");
    task->layout = "1  int temp = *a;\n"
                   "2  *a = ...;\n"
                   "3  *b = ...;";

    task = startTask(level, 1, TASK_TYPE, "Exam style: write the whole body of swap_ints from memory, using a variable called temp.",
                     "void swap_ints(int *a, int *b) {\n"
                     "    ____\n"
                     "}");
    addAnswer(task, "int temp = *a; *a = *b; *b = temp;");
    addAnswer(task, "int temp = *b; *b = *a; *a = temp;");
    addMistake(task, "int temp = *a; *a = *b; *b = *a;", "By the last line, *a has already been overwritten with b's value, so b gets its own value back.");
    addMistake(task, "int temp = a; a = b; b = temp;", "That swaps the function's own copies of the addresses, so the caller's variables don't change. Swap the values they point at.");
    addMistake(task, "*a = *b; *b = *a;", "Without saving a's value first, it's lost on the first line.");
    setHints(task, "Three lines: save, copy, restore.",
                   "temp should hold the value a points at.",
                   "Start with int temp = *a; then two more lines using *.");

    // ---------- Level 10 ----------
    level = startLevel(10, 2, "swap_chars", "swap_chars", "Lab 2 Q2");
    level->explanation = "Pointers work the same way for every type. A char * points at a char, so swap_chars is the same idea as "
                         "swap_ints. Just think about what type temp needs to be.";
    level->example = "char a = 'A', b = 'B';\n"
                     "swap_chars(&a, &b);   // Now a = 'B', b = 'A'";
    level->inGame = "Groups are stored as chars: 'S' for solids and 'T' for stripes. After the first pot, the game calls "
                    "swap_chars if the shooter needs the other group.";
    level->slideTitle = "Lab 2, Q2: swap_chars";
    level->slideText = "2. swap_chars\n"
                       "Description: Swap the values of two characters using pointers.\n"
                       "Prototype:\n"
                       "void swap_chars(char *a, char *b);\n"
                       "Example:\n"
                       "char a = 'A', b = 'B';\n"
                       "swap_chars(&a, &b); // Now a = 'B', b = 'A'";

    task = startTask(level, 0, TASK_TYPE, "Write the body of swap_chars, using a variable called temp.",
                     "void swap_chars(char *a, char *b) {\n"
                     "    ____\n"
                     "}");
    addAnswer(task, "char temp = *a; *a = *b; *b = temp;");
    addAnswer(task, "char temp = *b; *b = *a; *a = temp;");
    addMistake(task, "int temp = *a; *a = *b; *b = temp;", "So close, but temp holds a char here. Give it the matching type.");
    addMistake(task, "char temp = a; a = b; b = temp;", "That swaps the function's own copies of the addresses. Swap the chars they point at.");
    addMistake(task, "char temp = *a; *a = *b; *b = *a;", "*a has been overwritten by the time the last line runs.");
    setHints(task, "It's swap_ints with a different type.",
                   "temp holds one of the chars for a moment.",
                   "char temp = *a; and then two more lines.");

    task = startTask(level, 1, TASK_TYPE, "Type the call that swaps a and b, as in the Lab 2 example.",
                     "char a = 'A', b = 'B';\n"
                     "____   // Now a = 'B', b = 'A'");
    addAnswer(task, "swap_chars(&a, &b);");
    addAnswer(task, "swap_chars(&b, &a);");
    addMistake(task, "swap_chars(a, b);", "swap_chars takes char * parameters: it needs to know where a and b live, not their values.");
    addMistake(task, "swap_chars(*a, *b);", "a and b aren't pointers, so * doesn't work on them.");
    setHints(task, "swap_chars needs two addresses.",
                   "The address-of operator gets them.",
                   "swap_chars(&..., &...);");

    // =========================================================
    // Chapter 3: Pointer arithmetic (Pointers lecture and Lab 2)
    // =========================================================

    // ---------- Level 11 ----------
    level = startLevel(11, 3, "Pointer arithmetic", "ptr + 1", "Pointers lecture, slides 22-23 and 33");
    level->noIndexing = true;
    level->explanation = "Adding to a pointer moves it through memory: ptr + 1 is the next element and ptr - 1 the previous one. "
                         "ptr * 2 and ptr / 2 are invalid (slide 23). An array's name is the address of its first element, so "
                         "a[2] is *(a + 2), and &(a[2]) is just a + 2 (slide 33). From here on Lab 2's rule applies: no [ ].";
    level->example = "int arr[] = {4, 6, 7};\n"
                     "int *p = arr;     // same as &arr[0]\n"
                     "p = p + 1;        // p now points at the 6";
    level->inGame = "The balls are an array too, so soon you'll call a ball with balls + N instead of &balls[N].";
    level->slideTitle = "Pointers lecture, slides 23 and 33";
    level->slideText = "Slide 23: Pointer Arithmetic\n"
                       "- What's ptr + 1?  -> The next memory location!\n"
                       "- What's ptr - 1?  -> The previous memory location!\n"
                       "- What's ptr * 2 and ptr / 2?  -> Invalid operations!!!\n"
                       "\n"
                       "Slide 33:\n"
                       "- Type of a is float *\n"
                       "- a[2] -> *(a + 2)\n"
                       "  ptr = &(a[2])\n"
                       "  -> ptr = &(*(a + 2))\n"
                       "  -> ptr = a + 2\n"
                       "- a is a memory address constant\n"
                       "- ptr is a pointer variable";

    task = startTask(level, 0, TASK_TYPE, "Slide 33: write ptr = &(a[2]); another way, without & or [ ].",
                     "float a[4];\n"
                     "float* ptr;\n"
                     "// ptr = &(a[2]); written another way:\n"
                     "____");
    addAnswer(task, "ptr = a + 2;");
    addAnswer(task, "ptr = 2 + a;");
    addMistake(task, "ptr = *(a + 2);", "*(a + 2) is the value stored in a[2]. ptr needs its address.");
    addMistake(task, "ptr = a;", "a on its own is the address of element 0.");
    addMistake(task, "ptr = &a + 2;", "a is already an address, so no & is needed.");
    setHints(task, "The array name a is the address of a[0].",
                   "Adding 2 to an address moves it 2 elements along.",
                   "ptr = a ...;");

    task = startTask(level, 1, TASK_TRACE, "What does the last line print?",
                     "int arr[] = {4, 6, 7, 8, 9};\n"
                     "int* ptr = arr;\n"
                     "cout << *(ptr + 3);");
    addAnswer(task, "8");
    addMistake(task, "7", "Count from 0: ptr + 0 is the 4, so ptr + 3 is three steps after it.");
    addMistake(task, "9", "That would be ptr + 4. Count the steps from the first element.");
    addMistake(task, "4", "That's *ptr. The + 3 moves three elements along first.");
    setHints(task, "ptr points at the first element.",
                   "ptr + 3 points three elements further along.",
                   "Count 4 (step 0), 6 (step 1), ...");
    task->layout = "arr:    4   6   7   8   9\n"
                   "index:  0   1   2   3   4\n"
                   "ptr + 3 points at index 3, so *(ptr + 3) is ...";

    // ---------- Level 12 ----------
    level = startLevel(12, 3, "Tracing pointer arithmetic", "ptr++", "Pointers lecture, slides 24-32: Pointer Arithmetic and Array");
    level->noIndexing = true;
    level->explanation = "ptr++ moves ptr to the next element, ptr = ptr - 3 moves it back three, and ptr += 2 moves it forward two. "
                         "*ptr = ... always writes to whichever element ptr points at right then. Keep a data table like slides "
                         "24-32 and update it line by line.";
    level->example = "int b[3];\n"
                     "int *p = b;    // p -> b[0]\n"
                     "p++;           // p -> b[1]\n"
                     "*p = 5;        // b[1] is 5";
    level->inGame = "Not used by the game, but this exact slide is great test practice.";
    level->slideTitle = "Pointers lecture, slides 24-32: Pointer Arithmetic and Array";
    level->slideText = "float a[4];\n"
                       "float* ptr;\n"
                       "ptr = &(a[2]);\n"
                       "*ptr = 3.14;\n"
                       "ptr++;\n"
                       "*ptr = 9.0;\n"
                       "ptr = ptr - 3;\n"
                       "*ptr = 6.0;\n"
                       "ptr += 2;\n"
                       "*ptr = 7.0;\n"
                       "\n"
                       "Data Table on slide 26 (after *ptr = 3.14;):\n"
                       "a[0]   float     ?\n"
                       "a[1]   float     ?\n"
                       "a[2]   float     3.14\n"
                       "a[3]   float     ?\n"
                       "ptr    float *   address of a[2]\n"
                       "\n"
                       "(slides 27-32 fill in the rest, which would give the answers away)";

    task = startTask(level, 0, TASK_TRACE, "This is the code from slides 24-32. What is a[0] at the end?",
                     "float a[4];\n"
                     "float* ptr;\n"
                     "ptr = &(a[2]);\n"
                     "*ptr = 3.14;\n"
                     "ptr++;\n"
                     "*ptr = 9.0;\n"
                     "ptr = ptr - 3;\n"
                     "*ptr = 6.0;\n"
                     "ptr += 2;\n"
                     "*ptr = 7.0;");
    addAnswer(task, "6");
    addAnswer(task, "6.0");
    addAnswer(task, "a[0]=6");
    addAnswer(task, "a[0]=6.0");
    addMistake(task, "7", "7.0 is written after ptr += 2 has moved ptr away from a[0].");
    addMistake(task, "3.14", "3.14 goes into a[2], where ptr starts.");
    addMistake(task, "9", "9.0 goes into a[3], straight after ptr++.");
    setHints(task, "ptr starts at a[2].",
                   "ptr++ goes to a[3], and ptr - 3 goes back to a[0].",
                   "What is written while ptr points at a[0]?");
    task->layout = "ptr = &(a[2])    a[2] = 3.14\n"
                   "ptr++            ptr -> a[3]   a[3] = 9.0\n"
                   "ptr = ptr - 3    ptr -> a[0]\n"
                   "*ptr = 6.0       a[0] = ?\n"
                   "ptr += 2         ptr -> a[2]   (7.0 goes there)";

    task = startTask(level, 1, TASK_TRACE, "Same code from slides 24-32. What is a[2] at the end?",
                     "float a[4];\n"
                     "float* ptr;\n"
                     "ptr = &(a[2]);\n"
                     "*ptr = 3.14;\n"
                     "ptr++;\n"
                     "*ptr = 9.0;\n"
                     "ptr = ptr - 3;\n"
                     "*ptr = 6.0;\n"
                     "ptr += 2;\n"
                     "*ptr = 7.0;");
    addAnswer(task, "7");
    addAnswer(task, "7.0");
    addAnswer(task, "a[2]=7");
    addAnswer(task, "a[2]=7.0");
    addMistake(task, "3.14", "a[2] gets 3.14 first, but ptr comes back to a[2] later and overwrites it.");
    addMistake(task, "6", "6.0 goes into a[0].");
    setHints(task, "a[2] is written to twice.",
                   "After ptr = ptr - 3 (a[0]), ptr += 2 moves back to a[2].",
                   "The last value written to a[2] is the one that stays.");
    task->layout = "ptr = &(a[2])    a[2] = 3.14\n"
                   "ptr++            a[3] = 9.0\n"
                   "ptr = ptr - 3    a[0] = 6.0\n"
                   "ptr += 2         ptr -> a[?]\n"
                   "*ptr = 7.0       ...";

    // ---------- Level 13 ----------
    level = startLevel(13, 3, "Pointer steps and sizeof", "sizeof", "Pointers lecture, slides 34-35: More Pointer Arithmetic");
    level->noIndexing = true;
    level->explanation = "+ and - move by the size of the type the pointer points at. On slide 35 a double takes 4 memory slots, "
                         "so if ptr is at address 1000, ptr + 1 is at 1000 + sizeof(double) = 1004, not 1001. In this game "
                         "each Ball is 24 bytes, so balls + 14 is at 1000 + 14 * 24 = 1336.";
    level->example = "// an int takes 4 bytes, and arr starts at address 2000\n"
                     "int *p = arr;\n"
                     "// p + 1 is at 2004, p + 2 is at 2008";
    level->inGame = "Your call line changes now: [ ] is locked, so call your ball with pointer arithmetic: Ball *target = balls + N;";
    level->slideTitle = "Pointers lecture, slides 34-35: More Pointer Arithmetic";
    level->slideText = "Slide 34:\n"
                       "- What if a is a double array?\n"
                       "- A double may occupy more memory slots!\n"
                       "  - Given double *ptr = a;\n"
                       "  - What's ptr + 1 then?\n"
                       "\n"
                       "Addr  Content        Addr  Content\n"
                       "1000  a[0]: 37.9     1001  ...\n"
                       "1004  a[1]: 1.23     1005  ...\n"
                       "1008  a[2]: 3.14     1009  ...\n"
                       "\n"
                       "Slide 35:\n"
                       "- Arithmetic operators + and - auto-adjust the address offset\n"
                       "- According to the type of the pointer:\n"
                       "  1000 + sizeof(double) = 1000 + 4 = 1004";

    task = startTask(level, 0, TASK_TRACE, "Slide 35: double a[3] starts at address 1000, and each double takes 4 slots. "
                     "What address does ptr + 1 hold?",
                     "double a[3];        // starts at address 1000\n"
                     "double *ptr = a;    // ptr holds 1000\n"
                     "// what address does ptr + 1 hold?");
    addAnswer(task, "1004");
    addMistake(task, "1001", "+ 1 moves by one whole double, not one slot.");
    addMistake(task, "1008", "That would be ptr + 2.");
    setHints(task, "Pointer arithmetic counts in elements, not slots.",
                   "Slide 35: '+ and - auto-adjust the address offset, according to the type of the pointer.' A double is 4 slots there.",
                   "1000 + 1 * 4");
    task->layout = "address of ptr + n  =  1000 + n * (slots per double)\n"
                   "slots per double on slide 35 = 4\n"
                   "n = 1";

    task = startTask(level, 1, TASK_TRACE, "balls starts at address 1000 and each Ball is 24 bytes. What address does target hold?",
                     "Ball balls[16];              // starts at address 1000\n"
                     "Ball *target = balls + 14;\n"
                     "// what address does target hold?");
    addAnswer(task, "1336");
    addMistake(task, "1014", "+ 14 moves 14 whole Balls, and each one is 24 bytes.");
    addMistake(task, "1024", "That's balls + 1.");
    setHints(task, "Each step is sizeof(Ball) = 24 bytes.",
                   "14 steps is 14 * 24 bytes.",
                   "1000 + 14 * 24");
    task->layout = "address of balls + n  =  1000 + n * sizeof(Ball)\n"
                   "sizeof(Ball) = 24\n"
                   "n = 14";

    // ---------- Level 14 ----------
    level = startLevel(14, 3, "Printing with a pointer", "print ptr", "Pointers lecture, slide 36 exercise; Lab 2 sample main");
    level->noIndexing = true;
    level->explanation = "To visit every element without [ ], add i to the pointer and dereference it: *(ptr + i) is element i. "
                         "Or move a pointer along with ptr++ until it reaches the end, which is arr + len (one past the "
                         "last element).";
    level->example = "// from the Lab 2 sample main:\n"
                     "for (int i = 0; i < 5; i++) {\n"
                     "    std::cout << *(arr + i) << \" \";\n"
                     "}";
    level->inGame = "Not used by the game, but slide 36 is one of the lecture's exercises.";
    level->slideTitle = "Pointers lecture, slide 36; Lab 2 sample main";
    level->slideText = "Slide 36 - Exercise:\n"
                       "- Write code to output the 5 integers in an array using pointer arithmetic.\n"
                       "int arr[] = {4,6,7,8,9};\n"
                       "int* ptr= arr;\n"
                       "int len=5;\n"
                       "\n"
                       "Lab 2 sample main:\n"
                       "for(int i = 0; i < 5; i++) {\n"
                       "    std::cout << *(arr + i) << \" \";\n"
                       "}";

    task = startTask(level, 0, TASK_TYPE, "Slide 36 exercise: print the 5 integers using pointer arithmetic. Fill in what to print.",
                     "int arr[] = {4, 6, 7, 8, 9};\n"
                     "int* ptr = arr;\n"
                     "int len = 5;\n"
                     "for (int i = 0; i < len; i++) {\n"
                     "    cout << ____ << \" \";\n"
                     "}");
    addAnswer(task, "*(ptr + i)");
    addAnswer(task, "*(arr + i)");
    addAnswer(task, "*(i + ptr)");
    addAnswer(task, "*(i + arr)");
    addMistake(task, "ptr + i", "That's an address, so it would print hex numbers like 0x7ff7.... Read the value stored there.");
    addMistake(task, "*ptr + i", "* happens before +, so this reads the first element and adds i: it prints 4 5 6 7 8.");
    addMistake(task, "*ptr", "ptr never moves here, so this prints the first element five times.");
    setHints(task, "ptr + i is the address of element i.",
                   "Put * in front to read the value there.",
                   "Use brackets so the + happens before the *.");

    task = startTask(level, 1, TASK_TYPE, "This version moves p along the array. Fill in the loop condition so it stops after the last element.",
                     "int arr[] = {4, 6, 7, 8, 9};\n"
                     "int len = 5;\n"
                     "for (int* p = arr; ____; p++) {\n"
                     "    cout << *p << \" \";\n"
                     "}");
    addAnswer(task, "p < arr + len");
    addAnswer(task, "p < len + arr");
    addAnswer(task, "p != arr + len");
    addAnswer(task, "p < arr + 5");
    addMistake(task, "p < len", "p is an address but len is a count, so comparing them doesn't make sense. Compare p with an address.");
    addMistake(task, "*p < len", "That compares the values in the array, not where p is.");
    addMistake(task, "p <= arr + len", "arr + len is one past the end, so p mustn't reach it.");
    setHints(task, "p is an address, so compare it with an address.",
                   "The last element is at arr + len - 1.",
                   "Keep going while p is before arr + len.");

    // ---------- Level 15 ----------
    level = startLevel(15, 3, "sum_array", "sum_array", "Lab 2 Q9; Pointers lecture, slides 37-38 exercises");
    level->noIndexing = true;
    level->explanation = "Lab 2 Q9: int sum_array(int *arr, int size) adds up all the elements using pointer arithmetic. "
                         "Slide 38 shows another style: a pointer that moves along the array until it reaches a 0.";
    level->example = "int total = 0;\n"
                     "total += *(arr + 0);   // add element 0\n"
                     "total += *(arr + 1);   // add element 1";
    level->inGame = "At the end of a game, the results screen totals each player's potted ball numbers with sum_array.";
    level->slideTitle = "Lab 2 Q9; Pointers lecture, slides 37-38";
    level->slideText = "Lab 2, 9. sum_array\n"
                       "Description: Compute the sum of all elements in an integer array using pointers.\n"
                       "Prototype: int sum_array(int *arr, int size);\n"
                       "Example:   int arr[4] = {1, 2, 3, 4};\n"
                       "           int sum = sum_array(arr, 4); // sum = 10\n"
                       "\n"
                       "Slide 37 - Exercise: write a function called int sum(int* ptr, int numValues)\n"
                       "that uses pointer arithmetic to return the sum of numValues number of values.\n"
                       "\n"
                       "Slide 38 - Exercise: write code that adds the array elements until it reaches a 0.\n"
                       "int a[]={4,6,7,8,9,0};\n"
                       "int sum=0;\n"
                       "for(int *ptr=a;;....)\n"
                       "{\n"
                       "  sum +=*ptr;\n"
                       "}";

    task = startTask(level, 0, TASK_TYPE, "Type the line inside the loop that adds element i onto sum, without [ ].",
                     "int sum_array(int *arr, int size) {\n"
                     "    int sum = 0;\n"
                     "    for (int i = 0; i < size; i++) {\n"
                     "        ____\n"
                     "    }\n"
                     "    return sum;\n"
                     "}");
    addAnswer(task, "sum += *(arr + i);");
    addAnswer(task, "sum = sum + *(arr + i);");
    addAnswer(task, "sum += *(i + arr);");
    addMistake(task, "sum += arr + i;", "arr + i is an address. You want the number stored there.");
    addMistake(task, "sum += *arr + i;", "This adds i to the first element every time round.");
    addMistake(task, "sum = *(arr + i);", "That replaces sum each time instead of adding to it.");
    setHints(task, "Element i lives at arr + i.",
                   "Read it with *, using brackets.",
                   "Add it onto sum with +=.");

    task = startTask(level, 1, TASK_TYPE, "Slide 38 exercise: add the elements until reaching a 0. Fill in the loop condition.",
                     "int a[] = {4, 6, 7, 8, 9, 0};\n"
                     "int sum = 0;\n"
                     "for (int *ptr = a; ____; ptr++) {\n"
                     "    sum += *ptr;\n"
                     "}");
    addAnswer(task, "*ptr != 0");
    addAnswer(task, "*ptr");
    addAnswer(task, "0 != *ptr");
    addMistake(task, "ptr != 0", "That checks the address, not the number stored there.");
    addMistake(task, "*ptr == 0", "That's false straight away (the first number is 4), so the loop never runs.");
    addMistake(task, "ptr != nullptr", "ptr is never nullptr here. The loop should stop at the 0 in the array.");
    setHints(task, "Stop when you reach the element that is 0.",
                   "The current element is *ptr.",
                   "Keep going while the current element is not 0.");

    // ---------- Level 16 ----------
    level = startLevel(16, 3, "find_max", "find_max", "Lab 2 Q3");
    level->noIndexing = true;
    level->explanation = "Lab 2 Q3: int find_max(int *arr, int size). Start with the first element as the biggest so far, then "
                         "compare every other element with it and keep the larger one. No [ ] allowed.";
    level->example = "// the opposite idea: finding the SMALLEST\n"
                     "int smallest = *arr;\n"
                     "if (*(arr + 1) < smallest) smallest = *(arr + 1);";
    level->inGame = "Under the table you can now see find_max: the highest-numbered ball still on the table.";
    level->slideTitle = "Lab 2, Q3: find_max";
    level->slideText = "3. find_max\n"
                       "Description: Find the maximum value in an integer array using pointers.\n"
                       "Prototype:\n"
                       "int find_max(int *arr, int size);\n"
                       "Example:\n"
                       "int arr[5] = {3, 9, 2, 8, 5};\n"
                       "int max = find_max(arr, 5); // max = 9";

    task = startTask(level, 0, TASK_TYPE, "Type the if line that updates max when element i is bigger.",
                     "int find_max(int *arr, int size) {\n"
                     "    int max = *arr;\n"
                     "    for (int i = 1; i < size; i++) {\n"
                     "        ____\n"
                     "    }\n"
                     "    return max;\n"
                     "}");
    addAnswer(task, "if (*(arr + i) > max) max = *(arr + i);");
    addAnswer(task, "if (*(arr + i) > max) { max = *(arr + i); }");
    addAnswer(task, "if (max < *(arr + i)) max = *(arr + i);");
    addMistake(task, "if (*(arr + i) < max) max = *(arr + i);", "< keeps the smaller one, so this finds the minimum.");
    addMistake(task, "if (arr + i > max) max = arr + i;", "arr + i is an address. Compare the value stored there.");
    addMistake(task, "max = *(arr + i);", "Without an if, max just ends up as the last element.");
    setHints(task, "Compare element i with max.",
                   "Element i is *(arr + i).",
                   "If it's bigger, max becomes element i.");

    task = startTask(level, 1, TASK_TYPE, "Type the first line of find_max: start max at the first element, without [ ].",
                     "int find_max(int *arr, int size) {\n"
                     "    ____\n"
                     "    for (int i = 1; i < size; i++) {\n"
                     "        if (*(arr + i) > max) max = *(arr + i);\n"
                     "    }\n"
                     "    return max;\n"
                     "}");
    addAnswer(task, "int max = *arr;");
    addAnswer(task, "int max = *(arr + 0);");
    addMistake(task, "int max = 0;", "Starting at 0 goes wrong if every number is negative. Start with the first element instead.");
    addMistake(task, "int max = arr;", "arr is the address of the first element, not its value.");
    addMistake(task, "max = *arr;", "max doesn't exist yet, so it needs declaring.");
    setHints(task, "max should start as the first element's value.",
                   "The first element is at the address arr.",
                   "int max = ...;");

    // ---------- Level 17 ----------
    level = startLevel(17, 3, "reverse_array", "rev_array", "Lab 2 Q4");
    level->noIndexing = true;
    level->explanation = "Lab 2 Q4: reverse the array in place with two pointers: start at the first element and end at the last "
                         "(arr + size - 1). Swap what they point at, then move them towards each other until they meet. You "
                         "can reuse swap_ints, because start and end are already addresses.";
    level->example = "int *start = arr;\n"
                     "int *end = arr + size - 1;   // the last element\n"
                     "// swap, then move both towards the middle";
    level->inGame = "The game uses reverse_array to show your potted balls newest first.";
    level->slideTitle = "Lab 2, Q4: reverse_array";
    level->slideText = "4. reverse_array\n"
                       "Description: Reverse the contents of an integer array in-place using pointers.\n"
                       "Prototype:\n"
                       "void reverse_array(int *arr, int size);\n"
                       "Example:\n"
                       "int arr[5] = {1, 2, 3, 4, 5};\n"
                       "reverse_array(arr, 5); // arr = {5, 4, 3, 2, 1}";

    task = startTask(level, 0, TASK_ORDER, "Build the loop body of reverse_array. Two of the lines are wrong, so leave them out.",
                     "void reverse_array(int *arr, int size) {\n"
                     "    int *start = arr;\n"
                     "    int *end = arr + size - 1;\n"
                     "    while (start < end) {\n"
                     "        ____\n"
                     "    }\n"
                     "}");
    addTile(task, "end--;");
    addTile(task, "swap_ints(start, end);");
    addTile(task, "start--;");
    addTile(task, "start++;");
    addTile(task, "swap_ints(&start, &end);");
    addAnswer(task, "swap_ints(start, end); start++; end--;");
    addAnswer(task, "swap_ints(start, end); end--; start++;");
    addMistake(task, "start++; end--; swap_ints(start, end);", "If you move first, the two end elements are never swapped.");
    addMistake(task, "swap_ints(&start, &end); start++; end--;", "start and end are already addresses. &start would be the address of the pointer itself.");
    setHints(task, "Swap first, then move.",
                   "start moves forward and end moves backward.",
                   "start and end are already pointers, so swap_ints can take them as they are.");
    task->layout = "1  swap_ints(..., ...);\n"
                   "2  start...;\n"
                   "3  end...;";

    task = startTask(level, 1, TASK_TYPE, "Type the line that makes end point at the last element.",
                     "void reverse_array(int *arr, int size) {\n"
                     "    int *start = arr;\n"
                     "    ____\n"
                     "    while (start < end) {\n"
                     "        swap_ints(start, end);\n"
                     "        start++;\n"
                     "        end--;\n"
                     "    }\n"
                     "}");
    addAnswer(task, "int *end = arr + size - 1;");
    addAnswer(task, "int *end = arr + (size - 1);");
    addAnswer(task, "int *end = size - 1 + arr;");
    addMistake(task, "int *end = arr + size;", "arr + size is one past the end of the array.");
    addMistake(task, "int *end = *(arr + size - 1);", "That's the value of the last element. end needs its address.");
    addMistake(task, "int end = arr + size - 1;", "end has to be a pointer, so its type needs a *.");
    setHints(task, "end is a pointer to the last element.",
                   "The first element is at arr. How far along is the last one?",
                   "The last element of size elements is size - 1 steps from the start.");

    // =========================================================
    // Chapter 4: Lab 2 strings (no [ ])
    // =========================================================

    // ---------- Level 18 ----------
    level = startLevel(18, 4, "strlen_ptr", "strlen_ptr", "Lab 2 Q5");
    level->noIndexing = true;
    level->explanation = "A C string is an array of chars that ends with a hidden '\\0'. Lab 2 Q5: to find the length, walk a "
                         "pointer along the string and count, until the char it points at is '\\0'.";
    level->example = "char word[] = \"Hi\";   // stored as H i \\0\n"
                     "char *p = word;      // p points at 'H'\n"
                     "p++;                 // now p points at 'i'";
    level->inGame = "Every letter in this font is the same width, so the game centres player names using strlen_ptr.";
    level->slideTitle = "Lab 2, Q5: strlen_ptr";
    level->slideText = "5. strlen_ptr\n"
                       "Description: Implement a function that calculates the length of a string\n"
                       "(null-terminated character array) using pointers.\n"
                       "Prototype:\n"
                       "int strlen_ptr(char *str);\n"
                       "Example:\n"
                       "char str[] = \"Hello\";\n"
                       "int length = strlen_ptr(str); // length = 5";

    task = startTask(level, 0, TASK_TYPE, "Type the while line that keeps going until str reaches the '\\0' at the end.",
                     "int strlen_ptr(char *str) {\n"
                     "    int length = 0;\n"
                     "    ____\n"
                     "        length++;\n"
                     "        str++;\n"
                     "    }\n"
                     "    return length;\n"
                     "}");
    addAnswer(task, "while (*str != '\\0') {");
    addAnswer(task, "while (*str != 0) {");
    addAnswer(task, "while (*str) {");
    addMistake(task, "while (str != '\\0') {", "str is the address. Compare the character it points at.");
    addMistake(task, "while (*str == '\\0') {", "== '\\0' is only true at the very end, so the loop would never run.");
    addMistake(task, "while (*str != \"\\0\") {", "Double quotes make a string. One character needs single quotes.");
    setHints(task, "Keep going while you haven't reached the end of the string.",
                   "The end is marked by the char '\\0'.",
                   "Compare the char str points at with '\\0'.");

    task = startTask(level, 1, TASK_TYPE, "Type the line inside the loop that moves str on to the next character.",
                     "int strlen_ptr(char *str) {\n"
                     "    int length = 0;\n"
                     "    while (*str != '\\0') {\n"
                     "        length++;\n"
                     "        ____\n"
                     "    }\n"
                     "    return length;\n"
                     "}");
    addAnswer(task, "str++;");
    addAnswer(task, "++str;");
    addAnswer(task, "str = str + 1;");
    addAnswer(task, "str += 1;");
    addMistake(task, "*str++;", "That works, but the * reads a char and throws it away. Just move the pointer.");
    addMistake(task, "(*str)++;", "That changes the character itself ('H' would become 'I'!) instead of moving the pointer.");
    addMistake(task, "length++;", "length has already been counted on the line above. Now the pointer has to move.");
    setHints(task, "The pointer has to move one char along.",
                   "That's pointer arithmetic on str itself.",
                   "Add 1 to str.");

    // ---------- Level 19 ----------
    level = startLevel(19, 4, "count_vowels", "vowels", "Lab 2 Q6");
    level->noIndexing = true;
    level->explanation = "Lab 2 Q6: walk the string the same way as strlen_ptr, but check each char before moving on. To check "
                         "for several letters, compare *str with each one and join the checks with ||. Note: the lab sheet says "
                         "\"OpenAI\" has 3 vowels (O, e, A), but it forgot the I at the end.";
    level->example = "// counting the letter x, either case:\n"
                     "if (*str == 'x' || *str == 'X') {\n"
                     "    count++;\n"
                     "}";
    level->inGame = "Not used by the game, but it's Lab 2 Q6.";
    level->slideTitle = "Lab 2, Q6: count_vowels";
    level->slideText = "6. count_vowels\n"
                       "Description: Count the number of vowels in a string using pointers.\n"
                       "Prototype:\n"
                       "int count_vowels(char *str);\n"
                       "Example:\n"
                       "char str[] = \"OpenAI\";\n"
                       "int vowels = count_vowels(str); // vowels = 3 ('O', 'e', 'A')\n"
                       "\n"
                       "(Careful: the sheet missed the I at the end.)";

    task = startTask(level, 0, TASK_TYPE, "Type the if line that adds 1 to count when *str is a small vowel. Check them in the order a, e, i, o, u.",
                     "int count_vowels(char *str) {\n"
                     "    int count = 0;\n"
                     "    while (*str != '\\0') {\n"
                     "        ____\n"
                     "        str++;\n"
                     "    }\n"
                     "    return count;\n"
                     "}");
    addAnswer(task, "if (*str == 'a' || *str == 'e' || *str == 'i' || *str == 'o' || *str == 'u') count++;");
    addAnswer(task, "if (*str == 'a' || *str == 'e' || *str == 'i' || *str == 'o' || *str == 'u') { count++; }");
    addMistake(task, "if (*str == 'a' || 'e' || 'i' || 'o' || 'u') count++;", "Each check needs its own *str ==. On its own, 'e' counts as true, so this counts every character.");
    addMistake(task, "if (*str == 'a' && *str == 'e' && *str == 'i' && *str == 'o' && *str == 'u') count++;", "&& needs ALL of them to be true at once, which can never happen.");
    addMistake(task, "if (str == 'a' || str == 'e' || str == 'i' || str == 'o' || str == 'u') count++;", "str is the address. Compare the char it points at.");
    setHints(task, "Compare the current char with each vowel.",
                   "Each comparison looks like *str == 'a', and they're joined with 'or'.",
                   "'Or' in C is ||. Then count++;");

    task = startTask(level, 1, TASK_TRACE, "The full Lab 2 version counts small and capital vowels. What does count_vowels(\"OpenAI\") return?",
                     "char str[] = \"OpenAI\";\n"
                     "int vowels = count_vowels(str);\n"
                     "// how many vowels?");
    addAnswer(task, "4");
    addMistake(task, "3", "That's what the Lab 2 sheet says, but it missed one. Check every letter, including the last.");
    addMistake(task, "2", "Capital letters count too: O, A and I are vowels.");
    setHints(task, "Go letter by letter: O, p, e, n, A, I.",
                   "Capital vowels count as well.",
                   "The sheet forgot the I at the end.");
    task->layout = "O    p    e    n    A    I\n"
                   "yes  no   yes  no   ?    ?\n"
                   "Add up the yeses.";

    // ---------- Level 20 ----------
    level = startLevel(20, 4, "copy_string", "copy_str", "Lab 2 Q7");
    level->noIndexing = true;
    level->explanation = "Lab 2 Q7: copy one char at a time from src to dest, moving both pointers on each time. Stop at src's "
                         "'\\0', then put a '\\0' at the end of dest so it's a proper string too.";
    level->example = "char word[] = \"cat\";\n"
                     "char *p = word;\n"
                     "*p = 'b';            // word is now \"bat\"";
    level->inGame = "When someone wins, the game starts the win message by copying their name with copy_string.";
    level->slideTitle = "Lab 2, Q7: copy_string";
    level->slideText = "7. copy_string\n"
                       "Description: Copy the contents of one string to another using pointers\n"
                       "(you may assume the destination has enough space).\n"
                       "Prototype:\n"
                       "void copy_string(char *dest, char *src);\n"
                       "Example:\n"
                       "char src[] = \"Hello\";\n"
                       "char dest[10];\n"
                       "copy_string(dest, src); // dest = \"Hello\"";

    task = startTask(level, 0, TASK_TYPE, "Type the line inside the loop that copies one character from src to dest.",
                     "void copy_string(char *dest, char *src) {\n"
                     "    while (*src != '\\0') {\n"
                     "        ____\n"
                     "        dest++;\n"
                     "        src++;\n"
                     "    }\n"
                     "    *dest = '\\0';\n"
                     "}");
    addAnswer(task, "*dest = *src;");
    addMistake(task, "dest = src;", "That copies the address, so dest would just point at src. Copy the character instead.");
    addMistake(task, "*src = *dest;", "That copies the wrong way: dest should get src's char.");
    addMistake(task, "dest* = src*;", "The * goes in front of the pointer.");
    setHints(task, "Copy the char src points at into the place dest points at.",
                   "Both sides need to be dereferenced.",
                   "*dest = ...;");

    task = startTask(level, 1, TASK_TYPE, "Type the line after the loop that ends dest with '\\0'.",
                     "void copy_string(char *dest, char *src) {\n"
                     "    while (*src != '\\0') {\n"
                     "        *dest = *src;\n"
                     "        dest++;\n"
                     "        src++;\n"
                     "    }\n"
                     "    ____\n"
                     "}");
    addAnswer(task, "*dest = '\\0';");
    addAnswer(task, "*dest = 0;");
    addMistake(task, "dest = '\\0';", "That changes the pointer. You want to write a char to where it points.");
    addMistake(task, "*dest = \"\\0\";", "Double quotes make a string. One character needs single quotes.");
    addMistake(task, "*src = '\\0';", "src already ends with '\\0'. It's dest that needs one.");
    setHints(task, "dest needs the end-of-string marker.",
                   "That marker is the char '\\0'.",
                   "Write it to where dest points.");

    // ---------- Level 21 ----------
    level = startLevel(21, 4, "concat_strings", "concat", "Lab 2 Q8");
    level->noIndexing = true;
    level->explanation = "Lab 2 Q8: to add one string onto the end of another, first move dest along to its '\\0'. Then copy src "
                         "from there - and you already have a function that copies strings.";
    level->example = "char dest[20] = \"Hello, \";\n"
                     "// 1. move dest to the '\\0' after \"Hello, \"\n"
                     "// 2. copy \"World!\" there";
    level->inGame = "The game adds \" wins!\" to the winner's name with concat_strings.";
    level->slideTitle = "Lab 2, Q8: concat_strings";
    level->slideText = "8. concat_strings\n"
                       "Description: Concatenate two strings using pointers\n"
                       "(you may assume the destination has enough space).\n"
                       "Prototype:\n"
                       "void concat_strings(char *dest, char *src);\n"
                       "Example:\n"
                       "char dest[20] = \"Hello, \";\n"
                       "char src[] = \"World!\";\n"
                       "concat_strings(dest, src); // dest = \"Hello, World!\"";

    task = startTask(level, 0, TASK_TYPE, "Type the while line that moves dest along to the end of its string.",
                     "void concat_strings(char *dest, char *src) {\n"
                     "    ____\n"
                     "    copy_string(dest, src);\n"
                     "}");
    addAnswer(task, "while (*dest != '\\0') dest++;");
    addAnswer(task, "while (*dest != '\\0') { dest++; }");
    addAnswer(task, "while (*dest) dest++;");
    addAnswer(task, "while (*dest != 0) dest++;");
    addMistake(task, "while (dest != '\\0') dest++;", "Compare the char dest points at, not the address.");
    addMistake(task, "while (*dest != '\\0') *dest++;", "Nearly, but the * isn't needed: just move the pointer.");
    addMistake(task, "while (*src != '\\0') src++;", "It's dest that has to move to its end, so src can be copied there.");
    setHints(task, "Move dest forward until it reaches its '\\0'.",
                   "It's the same loop as strlen_ptr, without the counting.",
                   "while (...) dest++;");

    task = startTask(level, 1, TASK_TYPE, "Type the line that copies src onto the end of dest, reusing a Lab 2 function.",
                     "void concat_strings(char *dest, char *src) {\n"
                     "    while (*dest != '\\0') dest++;\n"
                     "    ____\n"
                     "}");
    addAnswer(task, "copy_string(dest, src);");
    addMistake(task, "copy_string(src, dest);", "The destination comes first in copy_string(dest, src).");
    addMistake(task, "copy_string(*dest, *src);", "copy_string takes pointers, not chars.");
    addMistake(task, "copy_string(&dest, &src);", "dest and src are already pointers. Pass them as they are.");
    setHints(task, "Lab 2 Q7 already copies a string.",
                   "dest now points at the end of the first string.",
                   "copy_string(destination, source);");

    // ---------- Level 22 ----------
    level = startLevel(22, 4, "compare_strings", "compare", "Lab 2 Q10");
    level->noIndexing = true;
    level->explanation = "Lab 2 Q10: walk both strings together while their chars match. Stop when they differ or the first "
                         "string ends, and return the difference between the two chars: negative if str1 comes first, 0 if "
                         "they're equal, positive if it comes after.";
    level->example = "'c' - 'c'   // is 0: the same char\n"
                     "'d' - 'c'   // is 1: d comes after c";
    level->inGame = "When you click Run, the game's answer checker compares your answer with the right ones using compare_strings.";
    level->slideTitle = "Lab 2, Q10: compare_strings";
    level->slideText = "10. compare_strings\n"
                       "Description: Compare two strings lexicographically using pointers. Return 0 if\n"
                       "they are equal, a negative value if the first string is lexicographically\n"
                       "smaller, and a positive value if the first string is larger.\n"
                       "Prototype:\n"
                       "int compare_strings(char *str1, char *str2);\n"
                       "Example:\n"
                       "char str1[] = \"apple\";\n"
                       "char str2[] = \"banana\";\n"
                       "int cmp = compare_strings(str1, str2); // cmp < 0 since \"apple\" < \"banana\"";

    task = startTask(level, 0, TASK_TYPE, "Type the return line: the difference between the two current characters.",
                     "int compare_strings(char *str1, char *str2) {\n"
                     "    while (*str1 != '\\0' && *str1 == *str2) {\n"
                     "        str1++;\n"
                     "        str2++;\n"
                     "    }\n"
                     "    ____\n"
                     "}");
    addAnswer(task, "return *str1 - *str2;");
    addMistake(task, "return str1 - str2;", "That subtracts the addresses. Use the chars they point at.");
    addMistake(task, "return *str2 - *str1;", "That's the wrong way round: \"apple\" vs \"banana\" must give a negative number.");
    addMistake(task, "return 0;", "That says every pair of strings is equal.");
    setHints(task, "Return how far apart the two current chars are.",
                   "Use the chars, not the pointers.",
                   "First char minus second char.");

    task = startTask(level, 1, TASK_TRACE, "Using the Lab 2 version, what does compare_strings(\"apple\", \"banana\") return?",
                     "char str1[] = \"apple\";\n"
                     "char str2[] = \"banana\";\n"
                     "int cmp = compare_strings(str1, str2);");
    addAnswer(task, "-1");
    addMistake(task, "1", "It's the first char minus the second: 'a' - 'b'.");
    addMistake(task, "0", "0 means equal, but these strings differ at the very first letter.");
    setHints(task, "The loop stops straight away: 'a' and 'b' differ.",
                   "It returns *str1 - *str2.",
                   "What is 'a' - 'b'?");
    task->layout = "str1:  a  p  p  l  e\n"
                   "str2:  b  a  n  a  n  a\n"
                   "They differ straight away, so it returns 'a' - 'b'.\n"
                   "'a' is 97 and 'b' is 98.";
}
