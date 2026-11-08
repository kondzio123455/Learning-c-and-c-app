#include "practice.h"
#include "lab1.h"
#include "lab2.h"
#include "raylib.h"
#include <cstdio>
#include <cstring>
#include <cstdlib>

// ============================================================
// Made-up practice questions, in the same style as the slides and labs.
//
// Each makeLevelN function builds the code line by line with new values,
// and works out the answer by running the same steps for real (or by
// calling the real lab function), so the answer always matches the code.
// ============================================================

// ---------- Small helpers ----------

int randomInt(int low, int high) {
    return GetRandomValue(low, high);
}

// Fills values[] with 'count' different random numbers between low and high
void differentNumbers(int values[], int count, int low, int high) {
    for (int i = 0; i < count; i++) {
        bool used = true;
        while (used) {
            values[i] = randomInt(low, high);
            used = false;
            for (int k = 0; k < i; k++) {
                if (values[k] == values[i]) used = true;
            }
        }
    }
}

// Adds a line to the end of some text
void addLine(char *text, int size, const char *newLine) {
    int length = (int)strlen(text);
    snprintf(text + length, size - length, "%s\n", newLine);
}

int countLinesIn(const char *text) {
    int lines = 0;
    while (*text != '\0') {
        if (*text == '\n') lines++;
        text++;
    }
    return lines;
}

void begin(MadeUpQuestion *q) {
    memset(q, 0, sizeof(MadeUpQuestion));   // start with every piece of text empty
}

void def(MadeUpQuestion *q, const char *text) { addLine(q->defs, sizeof(q->defs), text); }
void line(MadeUpQuestion *q, const char *text) { addLine(q->body, sizeof(q->body), text); }
void check(MadeUpQuestion *q, const char *text) { snprintf(q->check, sizeof(q->check), "%s", text); }
void note(MadeUpQuestion *q, const char *text) { snprintf(q->note, sizeof(q->note), "Learner note: %s", text); }
void layout(MadeUpQuestion *q, const char *text) { snprintf(q->layout, sizeof(q->layout), "%s", text); }

void answer(MadeUpQuestion *q, const char *text) {
    for (int i = 0; i < 4; i++) {
        if (q->answers[i][0] == '\0') {
            snprintf(q->answers[i], sizeof(q->answers[i]), "%s", text);
            return;
        }
    }
}

// A wrong answer and why it's wrong. Skipped if it's the same as an accepted answer.
void mistake(MadeUpQuestion *q, const char *text, const char *why) {
    for (int i = 0; i < 4; i++) {
        if (strcmp(q->answers[i], text) == 0) return;
    }
    for (int i = 0; i < 4; i++) {
        if (strcmp(q->mistakeAnswers[i], text) == 0) return;   // already there
        if (q->mistakeAnswers[i][0] == '\0') {
            snprintf(q->mistakeAnswers[i], sizeof(q->mistakeAnswers[i]), "%s", text);
            snprintf(q->mistakeWhy[i], sizeof(q->mistakeWhy[i]), "%s", why);
            return;
        }
    }
}

void hints(MadeUpQuestion *q, const char *h1, const char *h2, const char *h3) {
    snprintf(q->hints[0], sizeof(q->hints[0]), "%s", h1);
    snprintf(q->hints[1], sizeof(q->hints[1]), "%s", h2);
    snprintf(q->hints[2], sizeof(q->hints[2]), "%s", h3);
}

// A number answer, also accepted as "name=value" (spaces don't matter)
void numberAnswer(MadeUpQuestion *q, const char *name, int value) {
    answer(q, TextFormat("%d", value));
    answer(q, TextFormat("%s=%d", name, value));
}

// Joins numbers with spaces: {4, 7, 9} -> "4 7 9"
void joinNumbers(int values[], int count, char *out, int size) {
    out[0] = '\0';
    for (int i = 0; i < count; i++) {
        int length = (int)strlen(out);
        if (i == 0) snprintf(out + length, size - length, "%d", values[i]);
        else snprintf(out + length, size - length, " %d", values[i]);
    }
}

// A list of numbers is accepted with spaces or commas between them
void listAnswer(MadeUpQuestion *q, int values[], int count) {
    char spaced[200];
    joinNumbers(values, count, spaced, sizeof(spaced));
    answer(q, spaced);
    char commas[200] = "";
    for (int i = 0; i < count; i++) {
        int length = (int)strlen(commas);
        if (i == 0) snprintf(commas + length, sizeof(commas) - length, "%d", values[i]);
        else snprintf(commas + length, sizeof(commas) - length, ",%d", values[i]);
    }
    answer(q, commas);
}

// Puts the code together and points the Task at all the text
void finish(MadeUpQuestion *q, TaskKind kind, const char *taskText) {
    snprintf(q->taskText, sizeof(q->taskText), "%s", taskText);
    q->code[0] = '\0';
    if (q->defs[0] != '\0') {
        snprintf(q->code, sizeof(q->code), "%s", q->defs);
    }
    int length = (int)strlen(q->code);
    snprintf(q->code + length, sizeof(q->code) - length, "%s", q->body);
    length = (int)strlen(q->code);
    if (length > 0 && q->code[length - 1] == '\n') q->code[length - 1] = '\0';   // no blank line at the end

    Task *t = &q->task;
    t->kind = kind;
    t->task = q->taskText;
    t->code = q->code;
    for (int i = 0; i < 4; i++) {
        if (q->answers[i][0] != '\0') t->answers[i] = q->answers[i];
        if (q->mistakeAnswers[i][0] != '\0') {
            t->mistakes[i].answer = q->mistakeAnswers[i];
            t->mistakes[i].why = q->mistakeWhy[i];
        }
    }
    for (int i = 0; i < 3; i++) t->hints[i] = q->hints[i];
    if (q->layout[0] == '\0' && kind == TASK_TYPE) makeLayout(q->answers[0], q->layout, sizeof(q->layout));
    t->layout = q->layout;
    t->note = q->note;
}

const char *names[4] = { "i", "j", "k", "m" };   // the variable names slide 3 uses

// ============================================================
// Chapter 1: Pointer basics
// ============================================================

void makeLevel1(MadeUpQuestion *q) {
    int v[4];
    differentNumbers(v, 4, 1, 60);
    int target = randomInt(0, 3);
    int other = (target + 1 + randomInt(0, 2)) % 4;

    line(q, TextFormat("int i = %d, j = %d, k = %d, m = %d;", v[0], v[1], v[2], v[3]));
    line(q, "int* ptr;");
    line(q, TextFormat("____   // ptr should hold the address of %s", names[target]));

    answer(q, TextFormat("ptr = &%s;", names[target]));
    mistake(q, TextFormat("ptr = %s;", names[target]), TextFormat("That copies %s's value (%d) into ptr. A pointer has to hold an address.", names[target], v[target]));
    mistake(q, TextFormat("*ptr = &%s;", names[target]), "*ptr means 'the thing ptr points at', but ptr doesn't point anywhere yet.");
    mistake(q, TextFormat("ptr = &%s;", names[other]), TextFormat("That points at %s. The question asks for %s.", names[other], names[target]));
    hints(q, "A pointer stores an address, not a value.",
             "Slide 7: '&: Address-of operator - get the address of an entity'.",
             "Put & in front of the variable, and store the result in ptr.");
    note(q, "& in front of a variable gives its address, and a pointer stores that address. "
            "For example: int x = 10; int *p; p = &x; makes p point at x.");
    finish(q, TASK_TYPE, TextFormat("Store the address of %s in ptr.", names[target]));
}

void makeLevel2(MadeUpQuestion *q) {
    int v[4];
    differentNumbers(v, 4, 1, 60);

    if (randomInt(0, 1) == 0) {
        // Trace: change one variable through the pointer, then ask about one of them
        bool pointAtJ = randomInt(0, 1) == 1;
        bool askJ = randomInt(0, 1) == 1;
        int i = v[0], j = v[1];
        int *ptr = &i;
        if (pointAtJ) ptr = &j;
        *ptr = v[2];
        int result = i;
        if (askJ) result = j;

        line(q, TextFormat("int i = %d, j = %d;", v[0], v[1]));
        line(q, "int* ptr;");
        line(q, pointAtJ ? "ptr = &j;" : "ptr = &i;");
        line(q, TextFormat("*ptr = %d;", v[2]));
        check(q, askJ ? "cout << j;" : "cout << i;");
        numberAnswer(q, askJ ? "j" : "i", result);
        if (pointAtJ == askJ) {
            mistake(q, TextFormat("%d", askJ ? v[1] : v[0]), "That's its starting value, but *ptr = ... writes into the variable ptr points at - this one.");
        } else {
            mistake(q, TextFormat("%d", v[2]), "That value went into the other variable: *ptr writes to whatever ptr points at.");
        }
        hints(q, "Which variable does ptr point at?",
                 "*ptr = ... changes the variable ptr points at.",
                 "Any other variable keeps its starting value.");
        layout(q, TextFormat("ptr = %s;   so *ptr is %s\n*ptr = %d;   so %s = %d\nThe question asks about %s.",
                             pointAtJ ? "&j" : "&i", pointAtJ ? "j" : "i", v[2], pointAtJ ? "j" : "i", v[2], askJ ? "j" : "i"));
        note(q, "*ptr is the variable ptr points at. If ptr = &x, then *ptr = 7; changes x, and int y = *ptr; reads x.");
        finish(q, TASK_TRACE, TextFormat("What is %s at the end?", askJ ? "j" : "i"));
    } else {
        // Type: change a variable through the pointer
        int target = randomInt(0, 2);
        line(q, TextFormat("int i = %d, j = %d, k = %d;", v[0], v[1], v[2]));
        line(q, "int* ptr;");
        line(q, TextFormat("ptr = &%s;", names[target]));
        line(q, TextFormat("____   // %s should now be %d", names[target], v[3]));
        answer(q, TextFormat("*ptr = %d;", v[3]));
        mistake(q, TextFormat("ptr = %d;", v[3]), "Without the * you'd change the address stored in ptr.");
        mistake(q, TextFormat("%s = %d;", names[target], v[3]), "That works, but the task is to change it through the pointer.");
        mistake(q, TextFormat("&ptr = %d;", v[3]), "& gives an address, and you can't assign to it.");
        hints(q, "You want to change the variable ptr points at.",
                 "Slide 8: '*: De-reference operator - refer to the content of the referee'.",
                 "* in front of ptr, then = and the new value.");
        note(q, "*ptr = value; changes the variable that ptr points at. The pointer itself still holds the same address.");
        finish(q, TASK_TYPE, TextFormat("Use ptr to change %s to %d.", names[target], v[3]));
    }
}

void makeLevel3(MadeUpQuestion *q) {
    int ball = randomInt(1, 15);
    if (ball == 8) ball = 9;
    line(q, "Ball balls[16];   // balls[0] is the cue ball");
    line(q, "____");
    answer(q, TextFormat("Ball *target = &balls[%d];", ball));
    answer(q, TextFormat("Ball *target = &(balls[%d]);", ball));
    mistake(q, TextFormat("Ball *target = balls[%d];", ball), "balls[N] is the ball itself. A pointer needs its address.");
    mistake(q, TextFormat("Ball target = &balls[%d];", ball), "target has to be a pointer, so its type needs a *.");
    hints(q, "You need a pointer to one element of the balls array.",
             "Slide 24 does it for a float array: ptr = &(a[2]);",
             "Ball *target = the address of that element;");
    note(q, "&array[n] is the address of element n. Slide 24: ptr = &(a[2]); points ptr at element 2.");
    finish(q, TASK_TYPE, TextFormat("Make a Ball pointer called target that points at ball %d.", ball));
}

void makeLevel4(MadeUpQuestion *q) {
    int v[4];
    differentNumbers(v, 4, -20, 60);
    int variant = randomInt(0, 2);
    bool askI = randomInt(0, 1) == 1;

    // Run the same steps for real while writing them out
    int i = v[0], j = v[1];
    int *ptr;
    line(q, TextFormat("int i = %d, j = %d;", v[0], v[1]));
    line(q, "int* ptr;");
    if (variant == 0) {
        line(q, "ptr = &i;");                       ptr = &i;
        line(q, TextFormat("*ptr = %d;", v[2]));    *ptr = v[2];
        line(q, "ptr = &j;");                       ptr = &j;
        line(q, TextFormat("*ptr = %d;", v[3]));    *ptr = v[3];
    } else if (variant == 1) {
        line(q, "ptr = &j;");                       ptr = &j;
        line(q, TextFormat("*ptr = %d;", v[2]));    *ptr = v[2];
        line(q, "ptr = &i;");                       ptr = &i;
        line(q, TextFormat("*ptr = %d;", v[3]));    *ptr = v[3];
    } else {
        line(q, "ptr = &i;");                       ptr = &i;
        line(q, TextFormat("*ptr = %d;", v[2]));    *ptr = v[2];
        line(q, "ptr = &j;");                       ptr = &j;
        line(q, TextFormat("*ptr = %d;", v[3]));    *ptr = v[3];
        line(q, "i = *ptr;");                       i = *ptr;
    }

    int result = j;
    if (askI) result = i;
    check(q, askI ? "cout << i;" : "cout << j;");
    numberAnswer(q, askI ? "i" : "j", result);
    for (int k = 0; k < 4; k++) {
        mistake(q, TextFormat("%d", v[k]), "That isn't the last value written into it. Check where ptr points before each *ptr = ... line.");
    }
    hints(q, "Make a table with i, j and ptr, and update it after every line.",
             "*ptr = ... always writes to whatever ptr points at right then.",
             "Watch for the line that moves ptr to the other variable.");
    layout(q, TextFormat("start:  i = %d, j = %d\nGo down the code one line at a time.\nAfter each line write down i, j, and which one ptr points at.", v[0], v[1]));
    note(q, "A pointer can be moved: ptr = &i; then later ptr = &j;. Each *ptr = ... changes whichever variable ptr points at at that moment (slides 10-15).");
    finish(q, TASK_TRACE, TextFormat("Trace the code. What is %s at the end?", askI ? "i" : "j"));
}

void makeLevel5(MadeUpQuestion *q) {
    int v[2];
    differentNumbers(v, 2, 1, 50);
    int variant = randomInt(0, 2);

    if (variant == 0) {
        def(q, "void f(int j) {");
        def(q, TextFormat("    j = %d;", v[1]));
        def(q, "}");
        line(q, TextFormat("int i = %d;", v[0]));
        line(q, "f(i);");
        check(q, "cout << i;");
        numberAnswer(q, "i", v[0]);
        mistake(q, TextFormat("%d", v[1]), "f was given a copy of i (pass by value), so only the copy changed.");
        layout(q, TextFormat("f(i) gives f a copy: inside f, j = %d\nj = %d changes the copy only\nback in main, i is ...", v[0], v[1]));
        finish(q, TASK_TRACE, "f takes an int, not a pointer. What is i after the call?");
    } else if (variant == 1) {
        def(q, "void f(int *ptr) {");
        def(q, TextFormat("    *ptr = %d;", v[1]));
        def(q, "}");
        line(q, TextFormat("int i = %d;", v[0]));
        line(q, "f(&i);");
        check(q, "cout << i;");
        numberAnswer(q, "i", v[1]);
        mistake(q, TextFormat("%d", v[0]), "f was given the address of i, so *ptr = ... changed i itself.");
        layout(q, TextFormat("f(&i) gives f the address of i: ptr -> i\n*ptr = %d writes into i itself\nback in main, i is ...", v[1]));
        finish(q, TASK_TRACE, "f takes a pointer. What is i after the call?");
    } else {
        int calls = randomInt(2, 3);
        def(q, "void increment(int *numPtr) {");
        def(q, "    (*numPtr)++;");
        def(q, "}");
        line(q, TextFormat("int x = %d;", v[0]));
        int x = v[0];
        for (int c = 0; c < calls; c++) {
            line(q, "increment(&x);");
            x++;
        }
        check(q, "cout << x;");
        numberAnswer(q, "x", x);
        mistake(q, TextFormat("%d", v[0]), "increment changes x through numPtr, so x goes up every time it's called.");
        mistake(q, TextFormat("%d", v[0] + 1), "Count the calls: each one adds 1.");
        layout(q, TextFormat("x starts at %d\neach increment(&x) adds 1 to x through the pointer\nhow many calls are there?", v[0]));
        finish(q, TASK_TRACE, "What is x after these calls?");
    }
    hints(q, "Look at the parameter: is it an int or an int *?",
             "Pass by value gives the function a copy. Pass by pointer gives it the address.",
             "Only a pointer lets the function change the caller's variable.");
    note(q, "Passing a normal int gives the function a copy. Passing &x gives it x's address, so the function can change x (slide 9).");
}

void makeLevel6(MadeUpQuestion *q) {
    int v[6];
    differentNumbers(v, 6, -30, 60);
    int variant = randomInt(0, 2);
    bool askI = randomInt(0, 1) == 1;

    int i = v[0], j = v[1];
    int *ptr;
    int **pptr;
    line(q, TextFormat("int i = %d, j = %d;", v[0], v[1]));
    line(q, "int *ptr;");
    line(q, "int **pptr;");
    line(q, "ptr = &i;");                       ptr = &i;
    line(q, "pptr = &ptr;");                    pptr = &ptr;
    line(q, TextFormat("*ptr = %d;", v[2]));    *ptr = v[2];
    line(q, TextFormat("**pptr = %d;", v[3]));  **pptr = v[3];
    line(q, "ptr = &j;");                       ptr = &j;
    line(q, TextFormat("**pptr = %d;", v[4]));  **pptr = v[4];
    if (variant == 0) {
        line(q, "*pptr = &i;");                 *pptr = &i;
        line(q, TextFormat("*ptr = %d;", v[5]));    *ptr = v[5];
    } else if (variant == 1) {
        line(q, "*pptr = &i;");                 *pptr = &i;
        line(q, TextFormat("**pptr = %d;", v[5]));  **pptr = v[5];
    } else {
        line(q, "*pptr = &j;");                 *pptr = &j;
        line(q, TextFormat("*ptr = %d;", v[5]));    *ptr = v[5];
    }

    int result = j;
    if (askI) result = i;
    check(q, askI ? "cout << i;" : "cout << j;");
    numberAnswer(q, askI ? "i" : "j", result);
    for (int k = 0; k < 6; k++) {
        mistake(q, TextFormat("%d", v[k]), "That's not the last value written into it. Keep track of where ptr points, because pptr always goes through ptr.");
    }
    hints(q, "Keep a table for i, j, ptr and pptr.",
             "**pptr means: go to ptr, then to what ptr points at right now.",
             "*pptr = &...; moves ptr, because *pptr IS ptr.");
    layout(q, TextFormat("ptr = &i; pptr = &ptr;      ptr -> i\n*ptr = %d; **pptr = %d;   i = %d\nptr = &j;                   ptr -> j\n... carry on from here", v[2], v[3], v[3]));
    note(q, "int **pptr = &ptr; is a pointer to a pointer. *pptr is ptr itself, and **pptr is the int that ptr points at (slides 16-21).");
    finish(q, TASK_TRACE, TextFormat("Trace it like the exercise on slide 21. What is %s at the end?", askI ? "i" : "j"));
}

void makeLevel7(MadeUpQuestion *q) {
    int x = randomInt(1, 60);
    bool pointAtX = randomInt(0, 1) == 1;
    line(q, TextFormat("int x = %d;", x));
    line(q, "int *ptr = nullptr;");
    if (pointAtX) line(q, "ptr = &x;");
    line(q, "if (ptr == nullptr)");
    line(q, "    cout << 0;");
    line(q, "else");
    line(q, "    cout << *ptr;");
    int result = 0;
    if (pointAtX) result = x;
    answer(q, TextFormat("%d", result));
    if (pointAtX) mistake(q, "0", "ptr = &x; ran, so ptr isn't nullptr any more.");
    else mistake(q, TextFormat("%d", x), "ptr is still nullptr, so the first branch runs.");
    hints(q, "Is ptr still nullptr when the if runs?",
             "Look for a line that points ptr at something.",
             "If it's nullptr, the first branch prints. Otherwise *ptr is printed.");
    layout(q, pointAtX ? "ptr = nullptr, then ptr = &x;\nso when the if runs, ptr is ..." : "ptr = nullptr, and nothing changes it\nso when the if runs, ptr is ...");
    note(q, "nullptr means 'points at nothing' (slide 39). Always check if (ptr == nullptr) before using *ptr.");
    finish(q, TASK_TRACE, "What does this print?");
}

void makeLevel8(MadeUpQuestion *q) {
    int v[2];
    differentNumbers(v, 2, 10, 60);
    def(q, "void swap(int* x, int* y) {");
    def(q, "    int z = *x; *x = *y; *y = z;");
    def(q, "}");
    def(q, "void swap(int &x, int &y) {");
    def(q, "    int z = x; x = y; y = z;");
    def(q, "}");
    line(q, TextFormat("int a = %d; int b = %d;", v[0], v[1]));

    int a = v[0], b = v[1];
    int calls = randomInt(1, 3);
    for (int c = 0; c < calls; c++) {
        if (randomInt(0, 1) == 0) line(q, "swap(&a, &b);");
        else line(q, "swap(a, b);");
        int z = a; a = b; b = z;   // both versions swap the caller's variables
    }
    bool askA = randomInt(0, 1) == 1;
    check(q, askA ? "cout << a;" : "cout << b;");
    numberAnswer(q, askA ? "a" : "b", askA ? a : b);
    mistake(q, TextFormat("%d", askA ? b : a), "Both versions swap the caller's variables. Count how many swaps run.");
    hints(q, "Both swap functions really swap a and b.",
             "swap(&a, &b) uses the pointer version, swap(a, b) the reference version.",
             "An even number of swaps puts them back where they started.");
    layout(q, TextFormat("start: a = %d, b = %d\nevery swap line swaps them\nnumber of swap lines: count them", v[0], v[1]));
    note(q, "Pass by pointer: swap(&a, &b) and use *x inside. Pass by reference: swap(a, b) and use x inside. Both change the caller's variables (slide 40).");
    finish(q, TASK_TRACE, TextFormat("What is %s at the end?", askA ? "a" : "b"));
}

// ============================================================
// Chapter 2: Lab 2 swaps
// ============================================================

void makeLevel9(MadeUpQuestion *q) {
    int v[3];
    differentNumbers(v, 3, 1, 50);
    int x = v[0], y = v[1], z = v[2];
    const char *ask = "x";
    int result;
    if (randomInt(0, 1) == 0) {
        line(q, TextFormat("int x = %d, y = %d;", v[0], v[1]));
        line(q, "swap_ints(&x, &y);");
        swap_ints(&x, &y);
        if (randomInt(0, 1) == 0) {
            ask = "y";
            result = y;
        } else {
            result = x;
        }
    } else {
        line(q, TextFormat("int x = %d, y = %d, z = %d;", v[0], v[1], v[2]));
        line(q, "swap_ints(&x, &y);");
        line(q, "swap_ints(&y, &z);");
        swap_ints(&x, &y);
        swap_ints(&y, &z);
        int pick = randomInt(0, 2);
        if (pick == 0) { ask = "x"; result = x; }
        else if (pick == 1) { ask = "y"; result = y; }
        else { ask = "z"; result = z; }
    }
    check(q, TextFormat("cout << %s;", ask));
    numberAnswer(q, ask, result);
    for (int k = 0; k < 3; k++) {
        mistake(q, TextFormat("%d", v[k]), "Trace each swap_ints call: it swaps the two values its pointers point at.");
    }
    hints(q, "swap_ints(&x, &y) swaps the values of x and y.",
             "Do the calls in order, one at a time.",
             "Write the values down after each call.");
    layout(q, TextFormat("start:   x = %d, y = %d, z = %d\nafter each swap_ints line, swap the two named variables", v[0], v[1], v[2]));
    note(q, "Lab 2 Q1: swap_ints(int *a, int *b) swaps the values a and b point at, so swap_ints(&x, &y) swaps x and y.");
    finish(q, TASK_TRACE, TextFormat("What is %s at the end?", ask));
}

void makeLevel10(MadeUpQuestion *q) {
    char letters[3];
    int v[3];
    differentNumbers(v, 3, 0, 25);
    for (int k = 0; k < 3; k++) letters[k] = (char)('A' + v[k]);
    char a = letters[0], b = letters[1];
    line(q, TextFormat("char a = '%c', b = '%c';", letters[0], letters[1]));
    line(q, "swap_chars(&a, &b);");
    swap_chars(&a, &b);
    if (randomInt(0, 1) == 0) {
        line(q, "swap_chars(&a, &b);");
        swap_chars(&a, &b);
    }
    bool askA = randomInt(0, 1) == 1;
    char result = askA ? a : b;
    check(q, askA ? "cout << a;" : "cout << b;");
    answer(q, TextFormat("%c", result));
    answer(q, TextFormat("'%c'", result));
    char other = askA ? b : a;
    mistake(q, TextFormat("%c", other), "Count the swap_chars calls: each one swaps a and b.");
    hints(q, "swap_chars(&a, &b) swaps the two chars.",
             "Two swaps put them back the way they started.",
             "Count the calls.");
    layout(q, TextFormat("start: a = '%c', b = '%c'\neach swap_chars line swaps them", letters[0], letters[1]));
    note(q, "Lab 2 Q2: swap_chars(char *a, char *b) is swap_ints for chars: char temp = *a; *a = *b; *b = temp;");
    finish(q, TASK_TRACE, TextFormat("What char is %s at the end? (just type the letter)", askA ? "a" : "b"));
}

// ============================================================
// Chapter 3: Pointer arithmetic
// ============================================================

void makeLevel11(MadeUpQuestion *q) {
    int arr[5];
    differentNumbers(arr, 5, 1, 40);
    int start = randomInt(0, 2);
    bool minus = start > 0 && randomInt(0, 1) == 1;
    int step;
    if (minus) step = randomInt(1, start);
    else step = randomInt(0, 4 - start);

    line(q, TextFormat("int arr[] = {%d, %d, %d, %d, %d};", arr[0], arr[1], arr[2], arr[3], arr[4]));
    if (start == 0) line(q, "int* ptr = arr;");
    else line(q, TextFormat("int* ptr = arr + %d;", start));
    int *ptr = arr + start;
    int result;
    if (minus) {
        line(q, TextFormat("cout << *(ptr - %d);", step));
        result = *(ptr - step);
    } else {
        line(q, TextFormat("cout << *(ptr + %d);", step));
        result = *(ptr + step);
    }
    answer(q, TextFormat("%d", result));
    if (start != 0 && !minus) mistake(q, TextFormat("%d", arr[step]), "ptr doesn't start at the first element here.");
    for (int k = 0; k < 5; k++) {
        mistake(q, TextFormat("%d", arr[k]), "Count from where ptr starts. ptr + 0 is the element ptr points at.");
    }
    hints(q, "Where does ptr start? arr is element 0.",
             "ptr + n moves n elements forward, ptr - n moves back.",
             "Count the steps from where ptr starts.");
    layout(q, TextFormat("index:  0   1   2   3   4\nvalue:  %d   %d   %d   %d   %d\nptr starts at index %d", arr[0], arr[1], arr[2], arr[3], arr[4], start));
    note(q, "ptr + 1 is the next element and ptr - 1 the previous one (slide 23). arr + 2 is the address of element 2, so *(arr + 2) is arr[2].");
    finish(q, TASK_TRACE, "What does the last line print?");
}

void makeLevel12(MadeUpQuestion *q) {
    int v[4];
    differentNumbers(v, 4, 1, 50);
    int a[5] = { 0, 0, 0, 0, 0 };
    int start = randomInt(1, 3);
    int back = randomInt(1, start + 1);
    int afterBack = start + 1 - back;
    int forward = randomInt(1, 4 - afterBack);

    line(q, "int a[5] = {0, 0, 0, 0, 0};");
    line(q, "int* ptr;");
    line(q, TextFormat("ptr = &(a[%d]);", start));   int *ptr = &(a[start]);
    line(q, TextFormat("*ptr = %d;", v[0]));          *ptr = v[0];
    line(q, "ptr++;");                                ptr++;
    line(q, TextFormat("*ptr = %d;", v[1]));          *ptr = v[1];
    line(q, TextFormat("ptr = ptr - %d;", back));     ptr = ptr - back;
    line(q, TextFormat("*ptr = %d;", v[2]));          *ptr = v[2];
    line(q, TextFormat("ptr += %d;", forward));       ptr += forward;
    line(q, TextFormat("*ptr = %d;", v[3]));          *ptr = v[3];

    int ask = randomInt(0, 4);
    check(q, TextFormat("cout << a[%d];", ask));
    answer(q, TextFormat("%d", a[ask]));
    answer(q, TextFormat("a[%d]=%d", ask, a[ask]));
    for (int k = 0; k < 4; k++) {
        mistake(q, TextFormat("%d", v[k]), "That value went into a different element. Track which element ptr points at after every move.");
    }
    mistake(q, "0", "Something is written into this element. Track where ptr is after each move.");
    hints(q, "Write a table with a[0] to a[4], and where ptr points.",
             "ptr++ moves one element forward; ptr - n and ptr += n move by n.",
             "*ptr = ... writes into the element ptr points at right then.");
    layout(q, TextFormat("ptr = &(a[%d])     -> a[%d] = %d\nptr++              -> ptr at a[%d]\nptr = ptr - %d      -> ptr at a[?]\n... keep going", start, start, v[0], start + 1, back));
    note(q, "ptr++ moves to the next element, ptr = ptr - 3 moves back three, ptr += 2 moves forward two. *ptr = ... writes wherever ptr is (slides 24-32).");
    finish(q, TASK_TRACE, TextFormat("Trace it like slides 24-32. What is a[%d] at the end?", ask));
}

void makeLevel13(MadeUpQuestion *q) {
    const char *types[3] = { "int", "double", "Ball" };
    int sizes[3] = { 4, 8, 24 };
    int t = randomInt(0, 2);
    int base = randomInt(10, 50) * 100;
    int step = randomInt(1, 9);
    int result = base + step * sizes[t];

    line(q, TextFormat("%s arr[10];          // starts at address %d", types[t], base));
    line(q, TextFormat("%s *ptr = arr + %d;", types[t], step));
    line(q, "// what address does ptr hold?");
    answer(q, TextFormat("%d", result));
    mistake(q, TextFormat("%d", base + step), TextFormat("+ %d moves %d whole elements, not %d bytes.", step, step, step));
    mistake(q, TextFormat("%d", base + (step + 1) * sizes[t]), "That's one element too far.");
    hints(q, "Pointer arithmetic counts in elements, not bytes.",
             TextFormat("Each element here is %d bytes.", sizes[t]),
             "start address + steps * size of one element");
    layout(q, TextFormat("address = %d + %d * sizeof(%s)\nsizeof(%s) = %d here", base, step, types[t], types[t], sizes[t]));
    note(q, "+ and - move by the size of the type the pointer points at (slide 35): 1000 + sizeof(double) = 1004 on the slide.");
    finish(q, TASK_TRACE, TextFormat("On this computer a %s is %d bytes. What address does ptr hold?", types[t], sizes[t]));
}

void makeLevel14(MadeUpQuestion *q) {
    int arr[6];
    differentNumbers(arr, 6, 1, 30);
    int len = randomInt(5, 6);
    int start = randomInt(0, 1);
    int step = randomInt(1, 2);

    if (len == 5) line(q, TextFormat("int arr[] = {%d, %d, %d, %d, %d};", arr[0], arr[1], arr[2], arr[3], arr[4]));
    else line(q, TextFormat("int arr[] = {%d, %d, %d, %d, %d, %d};", arr[0], arr[1], arr[2], arr[3], arr[4], arr[5]));
    line(q, "int* ptr = arr;");
    line(q, TextFormat("int len = %d;", len));
    if (step == 1) line(q, TextFormat("for (int i = %d; i < len; i++) {", start));
    else line(q, TextFormat("for (int i = %d; i < len; i += 2) {", start));
    line(q, "    cout << *(ptr + i) << \" \";");
    line(q, "}");

    int printed[6];
    int count = 0;
    for (int i = start; i < len; i += step) {
        printed[count] = *(arr + i);
        count++;
    }
    listAnswer(q, printed, count);
    int all[6];
    for (int i = 0; i < len; i++) all[i] = arr[i];
    char allText[100];
    joinNumbers(all, len, allText, sizeof(allText));
    mistake(q, allText, "Not every element is printed. Check where i starts and how much it goes up by.");
    hints(q, "*(ptr + i) is element i.",
             "Which values does i take? Check its start and step.",
             "Print the element for each of those values of i.");
    layout(q, TextFormat("i takes the values %d, %d, ... while i < %d\nfor each one, print element i", start, start + step, len));
    note(q, "*(ptr + i) is element i, without [ ] (slide 36). Type the numbers in order with spaces between them.");
    finish(q, TASK_TRACE, "What does this print? (numbers with spaces between)");
}

void makeLevel15(MadeUpQuestion *q) {
    int arr[6];
    differentNumbers(arr, 6, 1, 20);
    if (randomInt(0, 1) == 0) {
        int size = randomInt(3, 6);
        char list[100];
        joinNumbers(arr, size, list, sizeof(list));
        char commas[120] = "";
        for (int i = 0; i < size; i++) {
            int length = (int)strlen(commas);
            snprintf(commas + length, sizeof(commas) - length, i == 0 ? "%d" : ", %d", arr[i]);
        }
        line(q, TextFormat("int arr[%d] = {%s};", size, commas));
        line(q, TextFormat("int sum = sum_array(arr, %d);", size));
        int result = sum_array(arr, size);
        check(q, "cout << sum;");
        numberAnswer(q, "sum", result);
        layout(q, TextFormat("%s\nadd them all up", list));
        finish(q, TASK_TRACE, "Using Lab 2's sum_array, what is sum?");
    } else {
        // Slide 38: add until reaching a 0
        int zeroAt = randomInt(2, 4);
        arr[zeroAt] = 0;
        line(q, TextFormat("int a[] = {%d, %d, %d, %d, %d, %d, 0};", arr[0], arr[1], arr[2], arr[3], arr[4], arr[5]));
        line(q, "int sum = 0;");
        line(q, "for (int *ptr = a; *ptr != 0; ptr++) {");
        line(q, "    sum += *ptr;");
        line(q, "}");
        int a[7] = { arr[0], arr[1], arr[2], arr[3], arr[4], arr[5], 0 };
        int sum = 0;
        for (int *ptr = a; *ptr != 0; ptr++) {
            sum += *ptr;
        }
        int total = arr[0] + arr[1] + arr[2] + arr[3] + arr[4] + arr[5];
        check(q, "cout << sum;");
        numberAnswer(q, "sum", sum);
        mistake(q, TextFormat("%d", total), "The loop stops at the FIRST 0 it reaches.");
        layout(q, "add each element in turn\nstop as soon as *ptr is 0 (don't add anything after it)");
        finish(q, TASK_TRACE, "This is slide 38's 'add until 0' loop. What is sum at the end?");
    }
    hints(q, "Add the elements one at a time.",
             "Check exactly which elements the loop visits.",
             "Then add those up.");
    note(q, "Lab 2 Q9: sum_array adds every element with *(arr + i). Slide 38 adds until it reaches a 0.");
}

void makeLevel16(MadeUpQuestion *q) {
    int arr[6];
    differentNumbers(arr, 6, 1, 60);
    int size = 6;
    line(q, TextFormat("int arr[6] = {%d, %d, %d, %d, %d, %d};", arr[0], arr[1], arr[2], arr[3], arr[4], arr[5]));
    int result;
    if (randomInt(0, 1) == 0) {
        line(q, "int max = find_max(arr, 6);");
        result = find_max(arr, size);
    } else {
        line(q, "int max = find_max(arr + 1, 5);   // starts at element 1");
        result = find_max(arr + 1, size - 1);
    }
    check(q, "cout << max;");
    numberAnswer(q, "max", result);
    // The biggest of the whole array is only a mistake if it's different (it was element 0)
    int all = find_max(arr, size);
    if (all != result) mistake(q, TextFormat("%d", all), "arr + 1 starts at element 1, so element 0 isn't looked at.");
    hints(q, "find_max returns the biggest value it looks at.",
             "Which elements does it look at? Check the pointer and the size.",
             "Find the biggest of those.");
    layout(q, "list the elements find_max is given\npick the biggest");
    note(q, "Lab 2 Q3: find_max(int *arr, int size) starts with *arr as the biggest so far and keeps the larger one. find_max(arr + 1, 5) starts at element 1.");
    finish(q, TASK_TRACE, "Using Lab 2's find_max, what is max?");
}

void makeLevel17(MadeUpQuestion *q) {
    int arr[5];
    differentNumbers(arr, 5, 1, 40);
    line(q, TextFormat("int arr[5] = {%d, %d, %d, %d, %d};", arr[0], arr[1], arr[2], arr[3], arr[4]));
    int original[5];
    for (int k = 0; k < 5; k++) original[k] = arr[k];
    if (randomInt(0, 1) == 0) {
        line(q, "reverse_array(arr, 5);");
        reverse_array(arr, 5);
    } else {
        line(q, "reverse_array(arr + 1, 3);   // only the middle three");
        reverse_array(arr + 1, 3);
    }
    int ask = randomInt(0, 4);
    line(q, TextFormat("cout << *(arr + %d);", ask));
    answer(q, TextFormat("%d", arr[ask]));
    mistake(q, TextFormat("%d", original[ask]), "That's what was there before reverse_array ran.");
    hints(q, "reverse_array swaps the ends and works inwards.",
             "Check which part of the array it was given.",
             "Then read element N of the result.");
    layout(q, TextFormat("before: %d %d %d %d %d\nreverse the part that reverse_array is given\nthen count to the element", original[0], original[1], original[2], original[3], original[4]));
    note(q, "Lab 2 Q4: reverse_array(int *arr, int size) reverses in place with two pointers. reverse_array(arr + 1, 3) only reverses elements 1 to 3.");
    finish(q, TASK_TRACE, "Using Lab 2's reverse_array, what does the last line print?");
}

// ============================================================
// Chapter 4: Lab 2 strings
// ============================================================

const char *words[10] = { "pointer", "memory", "address", "Hello", "array", "string", "OpenAI", "billiards", "cue", "linked" };

void makeLevel18(MadeUpQuestion *q) {
    char word[20];
    snprintf(word, sizeof(word), "%s", words[randomInt(0, 9)]);
    line(q, TextFormat("char str[] = \"%s\";", word));
    int result;
    if (randomInt(0, 1) == 0) {
        line(q, "int length = strlen_ptr(str);");
        result = strlen_ptr(word);
    } else {
        int skip = randomInt(1, 2);
        line(q, TextFormat("int length = strlen_ptr(str + %d);", skip));
        result = strlen_ptr(word + skip);
        mistake(q, TextFormat("%d", strlen_ptr(word)), TextFormat("str + %d starts %d chars in, so those chars aren't counted.", skip, skip));
    }
    check(q, "cout << length;");
    numberAnswer(q, "length", result);
    hints(q, "strlen_ptr counts chars until it reaches the '\\0'.",
             "Where does the pointer start?",
             "Count from there to the end.");
    layout(q, TextFormat("the chars: %s, then '\\0'\ncount from where the pointer starts, up to the '\\0'", word));
    note(q, "Lab 2 Q5: strlen_ptr walks a pointer along the string and counts until *str is '\\0'. str + 2 starts at the third char.");
    finish(q, TASK_TRACE, "Using Lab 2's strlen_ptr, what is length?");
}

void makeLevel19(MadeUpQuestion *q) {
    const char *vowelWords[8] = { "OpenAI", "Pointer", "AEIOU", "MEMORY", "Address", "linkedList", "Billiards", "Education" };
    char word[20];
    snprintf(word, sizeof(word), "%s", vowelWords[randomInt(0, 7)]);
    line(q, TextFormat("char str[] = \"%s\";", word));
    line(q, "int vowels = count_vowels(str);");
    int result = count_vowels(word);
    check(q, "cout << vowels;");
    numberAnswer(q, "vowels", result);
    int smallOnly = 0;
    for (char *p = word; *p != '\0'; p++) {
        if (*p == 'a' || *p == 'e' || *p == 'i' || *p == 'o' || *p == 'u') smallOnly++;
    }
    if (smallOnly != result) mistake(q, TextFormat("%d", smallOnly), "Capital vowels count too.");
    hints(q, "Go through the word one char at a time.",
             "a, e, i, o, u count, in capitals or small letters.",
             "Add up the vowels.");
    layout(q, TextFormat("%s\nmark each vowel (upper or lower case), then count them", word));
    note(q, "Lab 2 Q6: count_vowels checks each char with *str == 'a' || ... and counts capitals too. (The sheet's 'OpenAI' answer missed the I.)");
    finish(q, TASK_TRACE, "Using the full Lab 2 count_vowels (small and capital vowels), what is vowels?");
}

void makeLevel20(MadeUpQuestion *q) {
    char word[20];
    snprintf(word, sizeof(word), "%s", words[randomInt(0, 9)]);
    // Skip at most length - 1 chars, so at least one char is copied and dest
    // still ends with its '\0' after *dest is changed below
    int most = strlen_ptr(word) - 1;
    if (most > 3) most = 3;
    int skip = randomInt(0, most);
    line(q, TextFormat("char src[] = \"%s\";", word));
    line(q, "char dest[20];");
    if (skip == 0) line(q, "copy_string(dest, src);");
    else line(q, TextFormat("copy_string(dest, src + %d);", skip));
    char dest[20];
    copy_string(dest, word + skip);
    bool changeFirst = randomInt(0, 1) == 1;
    char letter = (char)('A' + randomInt(0, 25));
    if (changeFirst) {
        line(q, TextFormat("*dest = '%c';", letter));
        *dest = letter;
    }
    line(q, "cout << dest;");
    answer(q, dest);
    answer(q, TextFormat("\"%s\"", dest));
    if (skip != 0) mistake(q, word, TextFormat("src + %d skips the first %d chars before copying.", skip, skip));
    hints(q, "copy_string copies chars from src until the '\\0'.",
             "Where does the src pointer start?",
             "Then apply any change made after the copy.");
    layout(q, TextFormat("src: %s\ncopy from where the pointer starts, to the end\nthen apply any line after the copy", word));
    note(q, "Lab 2 Q7: copy_string(dest, src) copies one char at a time and ends dest with '\\0'. src + 2 starts the copy at the third char.");
    finish(q, TASK_TRACE, "What does the last line print? (type the text)");
}

void makeLevel21(MadeUpQuestion *q) {
    const char *starts[4] = { "Hi ", "Hello, ", "C", "Ball " };
    const char *ends[4] = { "there", "World!", "++", "8" };
    int s = randomInt(0, 3);
    int e = randomInt(0, 3);
    char dest[40];
    snprintf(dest, sizeof(dest), "%s", starts[s]);
    char src[20];
    snprintf(src, sizeof(src), "%s", ends[e]);
    line(q, TextFormat("char dest[40] = \"%s\";", starts[s]));
    line(q, TextFormat("char src[] = \"%s\";", ends[e]));
    line(q, "concat_strings(dest, src);");
    concat_strings(dest, src);
    if (randomInt(0, 1) == 0) {
        line(q, "concat_strings(dest, src);");
        concat_strings(dest, src);
    }
    line(q, "cout << dest;");
    answer(q, dest);
    answer(q, TextFormat("\"%s\"", dest));
    hints(q, "concat_strings adds src onto the end of dest.",
             "Each call adds another copy of src.",
             "Count the calls.");
    layout(q, TextFormat("dest starts as: %s\neach concat_strings call adds: %s", starts[s], ends[e]));
    note(q, "Lab 2 Q8: concat_strings moves dest to its '\\0', then copies src from there, so each call adds src onto the end.");
    finish(q, TASK_TRACE, "What does the last line print? (spaces don't matter)");
}

void makeLevel22(MadeUpQuestion *q) {
    const char *pairs[8][2] = { { "cat", "car" }, { "apple", "apply" }, { "pool", "pool" }, { "abc", "abd" },
                                { "ball", "bat" }, { "node", "nod" }, { "zoo", "zero" }, { "cue", "cue" } };
    int p = randomInt(0, 7);
    char str1[10], str2[10];
    snprintf(str1, sizeof(str1), "%s", pairs[p][0]);
    snprintf(str2, sizeof(str2), "%s", pairs[p][1]);
    line(q, TextFormat("char str1[] = \"%s\";", str1));
    line(q, TextFormat("char str2[] = \"%s\";", str2));
    line(q, "int cmp = compare_strings(str1, str2);");
    int result = compare_strings(str1, str2);
    check(q, "cout << cmp;");
    numberAnswer(q, "cmp", result);
    if (result != 0) mistake(q, TextFormat("%d", -result), "It's the first string's char minus the second's, not the other way round.");
    if (result != 0) mistake(q, "0", "0 means they're the same, but they differ somewhere.");
    hints(q, "Walk along both strings until the chars differ (or the first string ends).",
             "Return *str1 - *str2 at that point.",
             "chars are numbers: 'a' is 97, 'b' is 98, and so on.");
    layout(q, TextFormat("str1: %s\nstr2: %s\nfind the first place they differ (a '\\0' counts as 0)", str1, str2));
    note(q, "Lab 2 Q10: compare_strings returns the difference of the first chars that don't match: negative, 0 or positive.");
    finish(q, TASK_TRACE, "Using Lab 2's compare_strings, what is cmp?");
}

// ============================================================
// Chapter 5: Memory management
// ============================================================

void makeLevel23(MadeUpQuestion *q) {
    int value = randomInt(2, 60);
    if (randomInt(0, 1) == 0) {
        // Slides 4-8: a pointer to a variable that has gone out of scope
        line(q, "int main() {");
        line(q, "  int *p;");
        if (randomInt(0, 1) == 0) line(q, TextFormat("  int count = %d;", randomInt(1, 9)));
        line(q, "  if (true) {");
        line(q, TextFormat("    int x = %d;", value));
        line(q, "    p = &x;");
        int assignLine = countLinesIn(q->body);
        line(q, "  }");
        line(q, "  cout << *p << endl;");
        int bugLine = countLinesIn(q->body);
        line(q, "}");
        answer(q, TextFormat("%d", bugLine));
        mistake(q, TextFormat("%d", assignLine), "When p = &x; runs, x still exists, so that line is fine. The problem comes later.");
        mistake(q, TextFormat("%d", assignLine + 1), "x goes out of scope at that }, but nothing reads through p on that line.");
        layout(q, TextFormat("x only exists inside the if block.\nThe block ends on line %d.\nWhich later line reads through p?", assignLine + 1));
        finish(q, TASK_BUG, "Which line uses a dangling pointer? Type the line number.");
    } else {
        // Slides 9-11: returning the address of a local variable
        line(q, "int* getPtrToFive() {");
        line(q, TextFormat("  int x = %d;", value));
        line(q, "  return &x;");
        line(q, "}");
        line(q, "int main() {");
        line(q, "  int *p = getPtrToFive();");
        line(q, "  cout << *p << endl;");
        line(q, "}");
        answer(q, "3");
        mistake(q, "2", "Declaring x is fine. The problem is what the function does with x's address.");
        mistake(q, "6", "The pointer it gets is already dangling, but the mistake was made inside the function.");
        layout(q, "x is a local variable: it disappears when getPtrToFive returns.\nWhich line hands its address back to main?");
        finish(q, TASK_BUG, "Which line inside getPtrToFive causes a dangling pointer? Type the line number.");
    }
    hints(q, "A local variable disappears at the end of the block it was declared in.",
             "A pointer to it after that is a dangling pointer (slide 8).",
             "Find the line where the dead variable's address is used or handed back.");
    note(q, "When a variable goes out of scope its memory can be reused. A pointer that still points there is dangling (slide 8). "
            "Never return the address of a local variable (slide 9).");
}

void makeLevel24(MadeUpQuestion *q) {
    int v[3];
    differentNumbers(v, 3, 1, 60);
    bool curly = randomInt(0, 1) == 1;
    const char *first = curly ? TextFormat("int *x = new int{%d};", v[0]) : TextFormat("int *x = new int(%d);", v[0]);
    line(q, first);
    int result;
    if (randomInt(0, 1) == 0) {
        // Two pointers to the same int
        line(q, "int *y = x;");
        line(q, TextFormat("*y = %d;", v[1]));
        line(q, "cout << *x;");
        line(q, "delete x;");
        int *x = new int(v[0]);
        int *y = x;
        *y = v[1];
        result = *x;
        delete x;
        mistake(q, TextFormat("%d", v[0]), "y = x copies the address, so x and y point at the SAME int.");
        layout(q, "x -> one int on the heap\ny = x;   now y points at the same int\n*y = ... changes that int");
    } else {
        // Two different ints: copying the value doesn't link them
        line(q, TextFormat("int *y = new int(%d);", v[1]));
        line(q, "*x = *y;");
        line(q, TextFormat("*y = %d;", v[2]));
        line(q, "cout << *x;");
        line(q, "delete x;");
        line(q, "delete y;");
        int *x = new int(v[0]);
        int *y = new int(v[1]);
        *x = *y;
        *y = v[2];
        result = *x;
        delete x;
        delete y;
        mistake(q, TextFormat("%d", v[2]), "*x = *y copies the VALUE. x and y still point at two different ints.");
        layout(q, "x and y point at two different ints\n*x = *y copies y's value into x's int\nthen only y's int changes");
    }
    answer(q, TextFormat("%d", result));
    hints(q, "Draw a box for each int made with new, and an arrow for each pointer.",
             "Assigning pointers (y = x) shares one int. Assigning values (*x = *y) copies a number.",
             "Then follow the arrows to the int that x points at.");
    note(q, "new int(5) makes an int on the heap and gives you its address (slides 12-14). Two pointers can point at the same int.");
    finish(q, TASK_TRACE, "What does it print?");
}

void makeLevel25(MadeUpQuestion *q) {
    int count = randomInt(2, 5);
    int variant = randomInt(0, 2);
    line(q, "int main() {");
    line(q, "  int *p;");
    line(q, TextFormat("  for (int i = 0; i < %d; ++i) {", count));
    line(q, "    p = getPtrToFive();");
    line(q, "    cout << *p << endl;");
    if (variant == 2) line(q, "    delete p;");
    line(q, "  }");
    if (variant == 1) line(q, "  delete p;");
    line(q, "}");

    int leaked = count;
    if (variant == 1) leaked = count - 1;
    if (variant == 2) leaked = 0;
    answer(q, TextFormat("%d", leaked));
    if (variant == 0) mistake(q, "0", "Nothing here deletes the ints, so every one of them is leaked.");
    if (variant == 1) {
        mistake(q, TextFormat("%d", count), "The delete after the loop does free one of them: the last one.");
        mistake(q, "0", "One delete can only free one int (slide 25).");
    }
    if (variant == 2) mistake(q, TextFormat("%d", count), "delete p; runs every time round the loop, so each int is freed.");
    hints(q, "Each call to getPtrToFive() allocates one int with new.",
             "Count how many times delete runs.",
             "Leaked = allocated - deleted.");
    layout(q, TextFormat("new runs %d times (once each time round the loop)\ndelete runs ... times\nleaked = %d - deletes", count, count));
    note(q, "Memory from new that is never deleted is a memory leak (slide 23). In a loop, the delete has to be inside the loop (slide 26).");
    finish(q, TASK_TRACE, "How many of the ints that getPtrToFive() allocates are never deleted (leaked)?");
}

void makeLevel26(MadeUpQuestion *q) {
    int variant = randomInt(0, 2);
    bool extra = randomInt(0, 1) == 1;
    int bugLine;
    int otherLine;
    const char *otherWhy;

    line(q, "int main() {");
    if (extra) line(q, "  cout << \"start\" << endl;");
    if (variant == 0) {
        // Slide 34: using memory after deleting it
        line(q, "  int *x = getPtrToFive();");
        line(q, "  delete x;");
        otherLine = countLinesIn(q->body);
        line(q, "  cout << *x << endl;");
        bugLine = countLinesIn(q->body);
        otherWhy = "Deleting is right. Look at what happens after it.";
    } else if (variant == 1) {
        // Slide 36: deleting the same memory twice
        line(q, "  int *x = getPtrToFive();");
        line(q, "  cout << *x << endl;");
        line(q, "  delete x;");
        otherLine = countLinesIn(q->body);
        line(q, "  delete x;");
        bugLine = countLinesIn(q->body);
        otherWhy = "The first delete is fine. It's the second one that frees memory that's already gone.";
    } else {
        // Slide 38: deleting memory that didn't come from new
        line(q, TextFormat("  int x = %d;", randomInt(2, 50)));
        line(q, "  int *xPtr = &x;");
        otherLine = countLinesIn(q->body);
        line(q, "  cout << *xPtr << endl;");
        line(q, "  delete xPtr;");
        bugLine = countLinesIn(q->body);
        otherWhy = "Pointing at x is fine. Which line frees memory that never came from new?";
    }
    line(q, "}");
    answer(q, TextFormat("%d", bugLine));
    mistake(q, TextFormat("%d", otherLine), otherWhy);
    hints(q, "Slides 34-39: don't use memory after deleting it, don't delete it twice, only delete memory from new.",
             "Follow the pointer from top to bottom.",
             "Find the first line that breaks one of those three rules.");
    layout(q, "Rule 1: no reading after delete\nRule 2: no second delete\nRule 3: only delete memory from new\nWhich line breaks a rule?");
    note(q, "Three rules (slides 34-39): don't use memory after deletion, don't delete memory twice, and only delete memory that was allocated by new.");
    finish(q, TASK_BUG, "Which line is wrong? Type the line number.");
}

void makeLevel27(MadeUpQuestion *q) {
    int count = randomInt(3, 6);
    int times = randomInt(2, 9);
    int ask = randomInt(0, count - 1);
    line(q, TextFormat("int numItems = %d;", count));
    line(q, "int *arr = new int[numItems];");
    line(q, "for (int i = 0; i < numItems; ++i) {");
    line(q, TextFormat("  arr[i] = i * %d;", times));
    line(q, "}");
    line(q, TextFormat("cout << arr[%d] << endl;", ask));
    line(q, "delete[] arr;");

    int *arr = new int[count];
    for (int i = 0; i < count; ++i) {
        arr[i] = i * times;
    }
    int result = arr[ask];
    delete[] arr;
    answer(q, TextFormat("%d", result));
    mistake(q, TextFormat("%d", (ask + 1) * times), "Arrays count from 0: arr[0] is the first item.");
    hints(q, "new int[numItems] makes an array of numItems ints.",
             "Each item gets i times a number.",
             "Work out which i the printed item has.");
    layout(q, TextFormat("arr[0] = 0 * %d\narr[1] = 1 * %d\n...\narr[%d] = ...", times, times, ask));
    note(q, "new int[numItems] makes an array whose size is only known when the program runs (slide 41). Free it with delete[] (slide 45).");
    finish(q, TASK_TRACE, "What does it print?");
}

void makeLevel28(MadeUpQuestion *q) {
    int rows = randomInt(2, 3);
    int cols = randomInt(2, 4);
    int r = randomInt(0, rows - 1);
    int c = randomInt(0, cols - 1);
    line(q, TextFormat("int rows = %d, cols = %d;", rows, cols));
    line(q, "int** arr2D = new int*[rows];");
    line(q, "for (int row = 0; row < rows; row++) {");
    line(q, "    arr2D[row] = new int[cols];");
    line(q, "}");
    line(q, "int counter = 1;");
    line(q, "for (int row = 0; row < rows; row++) {");
    line(q, "    for (int col = 0; col < cols; col++) {");
    line(q, "        arr2D[row][col] = counter++;");
    line(q, "    }");
    line(q, "}");
    line(q, TextFormat("cout << arr2D[%d][%d] << endl;", r, c));
    line(q, "for (int row = 0; row < rows; row++) {");
    line(q, "    delete[] arr2D[row];");
    line(q, "}");
    line(q, "delete[] arr2D;");

    // The same steps for real
    int **arr2D = new int*[rows];
    for (int row = 0; row < rows; row++) {
        arr2D[row] = new int[cols];
    }
    int counter = 1;
    for (int row = 0; row < rows; row++) {
        for (int col = 0; col < cols; col++) {
            arr2D[row][col] = counter++;
        }
    }
    int result = arr2D[r][c];
    for (int row = 0; row < rows; row++) {
        delete[] arr2D[row];
    }
    delete[] arr2D;

    answer(q, TextFormat("%d", result));
    if (r != c && c < rows && r < cols) mistake(q, TextFormat("%d", c * cols + r + 1), "The first number picks the row and the second the column.");
    mistake(q, TextFormat("%d", c * rows + r + 1), "counter goes along each row first, then moves down to the next row.");
    hints(q, "counter fills row 0 first, left to right, then row 1, and so on.",
             TextFormat("Each row has %d items.", cols),
             "Count to the element: rows before it, then columns.");
    layout(q, TextFormat("row 0: 1, 2, ... %d\nrow 1: %d, ...\nfind row %d, column %d", cols, cols + 1, r, c));
    note(q, "A 2D array of any size is an array of row pointers (int**), and each row is its own new int[cols] (slides 46-48).");
    finish(q, TASK_TRACE, "This is the code from slide 46. What does it print?");
}

void makeLevel29(MadeUpQuestion *q) {
    int size = randomInt(3, 6);
    int times = randomInt(2, 9);
    int ask = randomInt(0, size - 1);
    line(q, TextFormat("int size = %d;", size));
    line(q, "int *arr = (int*)malloc(size * sizeof(int));");
    line(q, "if (arr == NULL) {");
    line(q, "    printf(\"Memory allocation failed!\\n\");");
    line(q, "    return 1;");
    line(q, "}");
    line(q, "for (int i = 0; i < size; i++) {");
    line(q, TextFormat("    arr[i] = i * %d;", times));
    line(q, "}");
    line(q, TextFormat("printf(\"%%d\", arr[%d]);", ask));
    line(q, "free(arr);");
    line(q, "arr = NULL;");

    int *arr = (int *)malloc(size * sizeof(int));
    int result = 0;
    if (arr != NULL) {
        for (int i = 0; i < size; i++) {
            arr[i] = i * times;
        }
        result = arr[ask];
        free(arr);
    }
    answer(q, TextFormat("%d", result));
    mistake(q, TextFormat("%d", (ask + 1) * times), "Arrays count from 0: arr[0] is the first item.");
    hints(q, "malloc gives room for size ints, used like an array.",
             "Each item gets i times a number.",
             "Work out which i the printed item has.");
    layout(q, TextFormat("arr[0] = 0 * %d\narr[1] = 1 * %d\n...\narr[%d] = ...", times, times, ask));
    note(q, "C uses malloc and free instead of new and delete. Check for NULL after malloc, and set the pointer to NULL after free (slide 51).");
    finish(q, TASK_TRACE, "This is the code from slide 50. What does it print?");
}

int Add(int num1, int num2) {
    return num1 + num2;
}

int Subtract(int num1, int num2) {
    return num1 - num2;
}

void makeLevel30(MadeUpQuestion *q) {
    int v[2];
    differentNumbers(v, 2, 5, 60);
    def(q, "int Add(int num1, int num2)");
    def(q, "{");
    def(q, "    return num1 + num2;");
    def(q, "}");
    def(q, "int Subtract(int num1, int num2)");
    def(q, "{");
    def(q, "    return num1 - num2;");
    def(q, "}");

    // A real function pointer, pointed at the same function as the question
    int (*functionPtr)(int, int);
    line(q, "int (*functionPtr)(int, int);");
    bool useAdd = randomInt(0, 1) == 1;
    if (useAdd) {
        line(q, "functionPtr = Add;");
        functionPtr = Add;
    } else {
        line(q, "functionPtr = Subtract;");
        functionPtr = Subtract;
    }
    if (randomInt(0, 1) == 0) {
        line(q, useAdd ? "functionPtr = Subtract;" : "functionPtr = Add;");
        if (useAdd) functionPtr = Subtract;
        else functionPtr = Add;
    }
    line(q, TextFormat("int result = functionPtr(%d, %d);", v[0], v[1]));
    line(q, "cout << result;");
    int result = functionPtr(v[0], v[1]);
    answer(q, TextFormat("%d", result));
    mistake(q, TextFormat("%d", v[0] + v[1]), "Check which function functionPtr points at when it's called.");
    mistake(q, TextFormat("%d", v[0] - v[1]), "Check which function functionPtr points at when it's called.");
    hints(q, "functionPtr = Add; makes the pointer point at Add.",
             "If it's assigned twice, the last assignment counts.",
             "functionPtr(a, b) calls whichever function it points at.");
    layout(q, "functionPtr points at: (the last function assigned)\nthat function is called with the two numbers");
    note(q, "A function pointer stores a function's address: int (*functionPtr)(int, int); functionPtr = Add; then functionPtr(20, 10) calls Add (slides 55-56).");
    finish(q, TASK_TRACE, "What does it print?");
}

void makeLevel31(MadeUpQuestion *q) {
    int v[2];
    differentNumbers(v, 2, 1, 60);
    bool local = randomInt(0, 1) == 1;
    def(q, TextFormat("int glVar = %d;", v[0]));
    def(q, "int *getPtrToFive() {");
    if (local) def(q, "    int glVar;");
    def(q, "    int *x = new int;");
    def(q, "    *x = 5;");
    def(q, TextFormat("    glVar = %d;", v[1]));
    def(q, "    return x;");
    def(q, "}");
    line(q, "int *p = getPtrToFive();");
    line(q, "cout << glVar;");
    line(q, "delete p;");
    int result = v[1];
    if (local) result = v[0];
    answer(q, TextFormat("%d", result));
    if (local) mistake(q, TextFormat("%d", v[1]), "The local glVar hides the global one inside the function, so only the local copy changed.");
    else mistake(q, TextFormat("%d", v[0]), "There's no local glVar, so glVar = ... changes the global one.");
    hints(q, "Is there a variable called glVar declared inside the function?",
             "A local variable with the same name hides the global inside that function.",
             "main prints the GLOBAL glVar.");
    layout(q, local ? "inside getPtrToFive: int glVar; is a NEW local variable\nglVar = ... changes the local one\nthe global is still ..." :
                      "inside getPtrToFive there's no local glVar\nso glVar = ... changes the global\nthe global is now ...");
    note(q, "A global variable is visible in every function, unless a function declares its own local variable with the same name (slides 52-54).");
    finish(q, TASK_TRACE, "What does it print?");
}

// ============================================================
// Chapter 6: Pointers and classes
// ============================================================

void pointClass(MadeUpQuestion *q) {
    def(q, "class Point {");
    def(q, "public:");
    def(q, "    double x, y;");
    def(q, "    Point() { x = 0.0; y = 0.0; }");
    def(q, "};");
}

void makeLevel32(MadeUpQuestion *q) {
    int v[4];
    differentNumbers(v, 4, 1, 20);
    int variant = randomInt(0, 2);
    pointClass(q);
    if (variant == 0) def(q, "void offsetPoint(Point p, double x, double y) {");
    if (variant == 1) def(q, "void offsetPoint(Point &p, double x, double y) {");
    if (variant == 2) def(q, "void offsetPoint(Point *p, double x, double y) {");
    def(q, variant == 2 ? "    p->x += x;" : "    p.x += x;");
    def(q, variant == 2 ? "    p->y += y;" : "    p.y += y;");
    def(q, "}");
    line(q, "Point p;");
    line(q, TextFormat("p.x = %d.0;", v[0]));
    line(q, TextFormat("p.y = %d.0;", v[1]));
    line(q, TextFormat(variant == 2 ? "offsetPoint(&p, %d.0, %d.0);" : "offsetPoint(p, %d.0, %d.0);", v[2], v[3]));
    bool askX = randomInt(0, 1) == 1;
    line(q, askX ? "cout << p.x;" : "cout << p.y;");

    int start = askX ? v[0] : v[1];
    int added = askX ? v[2] : v[3];
    int result = start + added;
    if (variant == 0) result = start;
    answer(q, TextFormat("%d", result));
    answer(q, TextFormat("%d.0", result));
    if (variant == 0) mistake(q, TextFormat("%d", start + added), "Passing by value gives offsetPoint a copy, so the original p doesn't change (slide 28).");
    else mistake(q, TextFormat("%d", start), "With & or *, offsetPoint changes the original p (slides 29 and 39).");
    hints(q, "Look at offsetPoint's parameter: Point, Point & or Point *?",
             "A plain Point is a copy. & and * reach the original.",
             "Only the original p is printed.");
    layout(q, TextFormat("parameter type: %s\nso the change %s the original p", variant == 0 ? "Point (a copy)" : (variant == 1 ? "Point & (a reference)" : "Point * (a pointer)"),
                         variant == 0 ? "does NOT reach" : "DOES reach"));
    note(q, "Passing an object by value passes a copy (slide 28). Pass by reference (Point &p) or pointer (Point *p, using ->) to change the original.");
    finish(q, TASK_TRACE, "What does it print?");
}

void makeLevel33(MadeUpQuestion *q) {
    int v[3];
    differentNumbers(v, 3, 1, 40);
    pointClass(q);
    line(q, "Point* pPtr;");
    line(q, "Point p;");
    line(q, "pPtr = &p;");
    line(q, TextFormat("pPtr->x = %d;", v[0]));
    line(q, TextFormat("p.y = %d;", v[1]));
    double x = v[0], y = v[1];
    if (randomInt(0, 1) == 0) {
        line(q, TextFormat("pPtr->y = %d;", v[2]));
        y = v[2];
    }
    int pick = randomInt(0, 3);
    const char *asks[4] = { "cout << p.x;", "cout << p.y;", "cout << pPtr->x;", "cout << pPtr->y;" };
    line(q, asks[pick]);
    int result = (int)(pick % 2 == 0 ? x : y);
    answer(q, TextFormat("%d", result));
    answer(q, TextFormat("%d.0", result));
    mistake(q, "0", "pPtr points at p, so a change through either name changes the same Point.");
    hints(q, "How many Point objects are there?",
             "pPtr = &p; so pPtr->x and p.x are the same variable.",
             "Find the last line that set the one being printed.");
    layout(q, "pPtr -> p   (one Point)\npPtr->x is p.x, pPtr->y is p.y\nthe last value written wins");
    note(q, "Use -> to reach a member through a pointer: pPtr->x is the same as p.x when pPtr = &p (slide 57).");
    finish(q, TASK_TRACE, "What does it print?");
}

void makeLevel34(MadeUpQuestion *q) {
    int v[3];
    differentNumbers(v, 3, 1, 40);
    pointClass(q);
    line(q, "Point* p = new Point();");
    line(q, "Point* pPtr;");
    line(q, "pPtr = p;");
    line(q, TextFormat("pPtr->x = %d;", v[0]));
    line(q, TextFormat("p->y = %d;", v[1]));
    double x = v[0], y = v[1];
    if (randomInt(0, 1) == 0) {
        line(q, TextFormat("p->x = %d;", v[2]));
        x = v[2];
    }
    bool askX = randomInt(0, 1) == 1;
    line(q, askX ? "cout << pPtr->x;" : "cout << pPtr->y;");
    line(q, "delete p;");
    int result = (int)(askX ? x : y);
    answer(q, TextFormat("%d", result));
    answer(q, TextFormat("%d.0", result));
    mistake(q, "0", "p and pPtr point at the SAME Point, so changes through either one show up in both.");
    hints(q, "new Point() makes ONE Point.",
             "pPtr = p; copies the address, not the object.",
             "Find the last line that set the member being printed.");
    layout(q, "p -> the Point made by new\npPtr = p;   pPtr -> the same Point\nthe last value written to that member wins");
    note(q, "p = new Point(); creates an object on the heap. Two pointers can point at it, and a change through one is seen through the other (slides 58-60).");
    finish(q, TASK_TRACE, "What does it print?");
}

void makeLevel35(MadeUpQuestion *q) {
    int v[4];
    differentNumbers(v, 4, 1, 30);
    def(q, "class Point {");
    def(q, "public:");
    def(q, "    double x, y;");
    def(q, "    Point() {");
    def(q, TextFormat("        this->x = %d.0; this->y = %d.0;", v[0], v[1]));
    def(q, "    }");
    def(q, "    Point(double nx, double ny) {");
    def(q, "        this->x = nx; this->y = ny;");
    def(q, "    }");
    def(q, "};");
    line(q, "Point p;");
    line(q, TextFormat("Point q(%d.0, %d.0);", v[2], v[3]));
    int pick = randomInt(0, 3);
    const char *asks[4] = { "cout << p.x;", "cout << p.y;", "cout << q.x;", "cout << q.y;" };
    line(q, asks[pick]);
    int result = v[pick];
    answer(q, TextFormat("%d", result));
    answer(q, TextFormat("%d.0", result));
    for (int k = 0; k < 4; k++) {
        mistake(q, TextFormat("%d", v[k]), "Check which constructor each object uses: Point p; uses the one with no parameters.");
    }
    hints(q, "this points at the object being made.",
             "Point p; calls the constructor with no parameters. q(.., ..) calls the other one.",
             "this->x is that object's own x.");
    layout(q, "Point p;        -> the default constructor sets p's x and y\nPoint q(a, b);  -> the second constructor sets q's x and y");
    note(q, "Inside a method, this points at the object it was called on, so this->x is that object's own x (slide 61).");
    finish(q, TASK_TRACE, "What does it print?");
}

void makeLevel36(MadeUpQuestion *q) {
    const char *namesList[6] = { "foo", "bar", "sam", "ann", "joe", "max" };
    char name[8];
    snprintf(name, sizeof(name), "%s", namesList[randomInt(0, 5)]);
    int position = randomInt(0, 2);
    char letter = (char)('a' + randomInt(0, 25));
    if (letter == name[position]) letter = (letter == 'z') ? 'a' : (char)(letter + 1);

    def(q, "class MITStudent {");
    def(q, "public:");
    def(q, "    int studentID;");
    def(q, "    char *name;");
    def(q, "};");
    line(q, "MITStudent student1;");
    line(q, TextFormat("student1.studentID = %d;", randomInt(10, 99)));
    line(q, TextFormat("char n[] = \"%s\";", name));
    line(q, "student1.name = n;");
    line(q, "MITStudent student2 = student1;");
    line(q, TextFormat("student2.name[%d] = '%c';", position, letter));
    line(q, "cout << student1.name;");

    char original[8];
    snprintf(original, sizeof(original), "%s", name);
    name[position] = letter;   // student1.name and student2.name both point at n
    answer(q, name);
    answer(q, TextFormat("\"%s\"", name));
    mistake(q, original, "The default copy copied the pointer, so both students share ONE name array.");
    hints(q, "name is a char *, so copying the object copies only the address.",
             "student1.name and student2.name point at the same array n.",
             "So changing student2.name changes what student1.name shows too.");
    layout(q, TextFormat("n = \"%s\"\nstudent1.name -> n, and student2.name -> n (the same array)\nstudent2.name[%d] = '%c' changes n", original, position, letter));
    note(q, "The default copy copies every field. For a char * field that means only the address: both objects share one array - a shallow copy (slide 71).");
    finish(q, TASK_TRACE, "What does it print?");
}

void makeLevel37(MadeUpQuestion *q) {
    int count = randomInt(3, 8);
    int times = randomInt(2, 9);
    int ask = randomInt(0, count - 1);
    def(q, "class Point {");
    def(q, "public:");
    def(q, "    int x;");
    def(q, "    Point(int val) : x(val) {}");
    def(q, "};");
    line(q, TextFormat("Point* pointPointers[%d];", count));
    line(q, TextFormat("for (int i = 0; i < %d; i++) {", count));
    line(q, TextFormat("    pointPointers[i] = new Point(i * %d);", times));
    line(q, "}");
    line(q, TextFormat("cout << pointPointers[%d]->x;", ask));
    line(q, TextFormat("for (int i = 0; i < %d; i++) {", count));
    line(q, "    delete pointPointers[i];");
    line(q, "}");
    int result = ask * times;
    answer(q, TextFormat("%d", result));
    mistake(q, TextFormat("%d", (ask + 1) * times), "Arrays count from 0: pointPointers[0] holds Point(0).");
    hints(q, "Each element points at its own Point, made with new.",
             "Point i is made with x = i times a number.",
             "-> reaches x through the pointer.");
    layout(q, TextFormat("pointPointers[0] -> Point(0 * %d)\npointPointers[1] -> Point(1 * %d)\n...\npointPointers[%d] -> ...", times, times, ask));
    note(q, "An array of pointers to objects: each element needs its own new, and each one needs its own delete (slide 77).");
    finish(q, TASK_TRACE, "What does it print?");
}

void makeLevel38(MadeUpQuestion *q) {
    def(q, "class TwentyThousandPoints {");
    def(q, "public:");
    def(q, "    Point* points;");
    def(q, "    TwentyThousandPoints() {");
    def(q, "        points = new Point[20000];");
    def(q, "    }");
    def(q, "    ~TwentyThousandPoints() {");
    def(q, "        delete[] points;");
    def(q, "    }");
    def(q, "};");
    int result;
    if (randomInt(0, 1) == 0) {
        int count = randomInt(2, 6);
        line(q, TextFormat("for (int i = 0; i < %d; i++) {", count));
        line(q, "    TwentyThousandPoints t;");
        line(q, "}");
        result = count;
        mistake(q, "1", "There's a new t each time round the loop, and each one goes out of scope at the }.");
    } else {
        int count = randomInt(2, 3);
        line(q, "{");
        line(q, "    TwentyThousandPoints a;");
        line(q, "    TwentyThousandPoints b;");
        if (count == 3) line(q, "    TwentyThousandPoints c;");
        line(q, "}");
        result = count;
        mistake(q, "1", "Every object in the block goes out of scope at the }, and each one runs its destructor.");
    }
    answer(q, TextFormat("%d", result));
    mistake(q, "0", "A destructor runs automatically when an object goes out of scope.");
    hints(q, "A destructor runs when an object goes out of scope (slide 79).",
             "Count the objects that are created, then reach their }.",
             "One object = one destructor call.");
    layout(q, "count every TwentyThousandPoints object that's created\n(a loop makes a new one each time round)\neach one runs ~TwentyThousandPoints() once");
    note(q, "A destructor (~ClassName) runs automatically when an object goes out of scope - the place to delete what the constructor made with new (slide 79).");
    finish(q, TASK_TRACE, "How many times does ~TwentyThousandPoints() run?");
}

// ============================================================
// Chapter 7: Linked lists (the real functions from lab2.cpp)
// ============================================================

// Frees every node of a singly linked list (the same idea as clear())
void freeList(Node *headPtr) {
    while (headPtr != nullptr) {
        Node *temp = headPtr;
        headPtr = headPtr->nextPtr;
        delete temp;
    }
}

// Reads the list's values into values[], returns how many there were
int listValues(Node *headPtr, int values[], int most) {
    int count = 0;
    for (Node *curNodePtr = headPtr; curNodePtr != nullptr && count < most; curNodePtr = curNodePtr->nextPtr) {
        values[count] = curNodePtr->data;
        count++;
    }
    return count;
}

void makeLevel39(MadeUpQuestion *q) {
    int v[3];
    differentNumbers(v, 3, 1, 60);
    bool three = randomInt(0, 1) == 1;

    Node *first = new Node();
    Node *second = new Node();
    Node *third = nullptr;
    line(q, "Node* first = new Node();");
    line(q, TextFormat("first->data = %d;", v[0]));         first->data = v[0];
    line(q, "Node* second = new Node();");
    line(q, TextFormat("second->data = %d;", v[1]));        second->data = v[1];
    line(q, "first->nextPtr = second;");                    first->nextPtr = second;
    if (three) {
        third = new Node();
        line(q, "Node* third = new Node();");
        line(q, TextFormat("third->data = %d;", v[2]));     third->data = v[2];
        line(q, "second->nextPtr = third;");                second->nextPtr = third;
        line(q, "third->nextPtr = nullptr;");               third->nextPtr = nullptr;
        line(q, "cout << first->nextPtr->nextPtr->data;");
    } else {
        line(q, "second->nextPtr = nullptr;");              second->nextPtr = nullptr;
        line(q, "cout << first->nextPtr->data;");
    }
    int result = three ? first->nextPtr->nextPtr->data : first->nextPtr->data;
    line(q, "delete first;");
    line(q, "delete second;");
    if (three) line(q, "delete third;");
    delete first;
    delete second;
    delete third;   // deleting nullptr does nothing

    answer(q, TextFormat("%d", result));
    mistake(q, TextFormat("%d", v[0]), "first->nextPtr is the node AFTER first.");
    hints(q, "Draw each node as a box with an arrow to the next one.",
             "first->nextPtr is the second node, and ->nextPtr again moves one more.",
             "->data reads the number in the node you end up at.");
    layout(q, "first -> second -> ...\nfollow nextPtr once for each ->nextPtr in the line");
    note(q, "Each node holds data and a pointer to the next node. Follow the links with ->nextPtr; nullptr marks the end.");
    finish(q, TASK_TRACE, "What does it print?");
}

void makeLevel40(MadeUpQuestion *q) {
    int count = randomInt(3, 5);
    int v[5];
    differentNumbers(v, 5, 1, 60);
    Node *headPtr = nullptr;
    line(q, "Node* headPtr = nullptr;");
    for (int k = 0; k < count; k++) {
        line(q, TextFormat("InsertAtEnd(&headPtr, %d);", v[k]));
        InsertAtEnd(&headPtr, v[k]);
    }
    line(q, "PrintList(headPtr);");
    int values[5];
    int n = listValues(headPtr, values, 5);
    freeList(headPtr);
    listAnswer(q, values, n);
    int reversed[5];
    for (int k = 0; k < n; k++) reversed[k] = values[n - 1 - k];
    char reversedText[100];
    joinNumbers(reversed, n, reversedText, sizeof(reversedText));
    mistake(q, reversedText, "InsertAtEnd adds each new node at the END, so the list keeps the order they were added.");
    hints(q, "Each InsertAtEnd puts the new node after the last one.",
             "PrintList walks from headPtr to the end.",
             "Type the numbers in the order PrintList visits them, with spaces.");
    layout(q, "after each InsertAtEnd, write the list out:\n[first] -> [second] -> ...\nPrintList prints them left to right");
    note(q, "InsertAtEnd(&headPtr, value) adds a node at the end; PrintList follows nextPtr from the head and prints each data.");
    finish(q, TASK_TRACE, "What does PrintList print? (numbers with spaces between)");
}

void makeLevel41(MadeUpQuestion *q) {
    int count = randomInt(3, 5);
    int v[5];
    differentNumbers(v, 5, 1, 60);
    Node *headPtr = nullptr;
    line(q, "Node* headPtr = nullptr;");
    for (int k = 0; k < count; k++) {
        line(q, TextFormat("InsertAtEnd(&headPtr, %d);", v[k]));
        InsertAtEnd(&headPtr, v[k]);
    }
    int steps = randomInt(1, count - 1);
    char path[100] = "headPtr";
    Node *node = headPtr;
    for (int s = 0; s < steps; s++) {
        int length = (int)strlen(path);
        snprintf(path + length, sizeof(path) - length, "->nextPtr");
        node = node->nextPtr;
    }
    line(q, TextFormat("cout << %s->data;", path));
    int result = node->data;
    freeList(headPtr);
    answer(q, TextFormat("%d", result));
    mistake(q, TextFormat("%d", v[steps - 1]), "Count the ->nextPtr steps: each one moves one node along from the head.");
    hints(q, "headPtr points at the first node added.",
             "Each ->nextPtr moves one node along.",
             "Count the steps, then read ->data.");
    layout(q, "list: first -> second -> third -> ...\nstart at headPtr (the first one)\nmove once per ->nextPtr");
    note(q, "InsertAtEnd takes Node** so it can change headPtr when the list is empty. Each new node goes on the end.");
    finish(q, TASK_TRACE, "What does it print?");
}

void makeLevel42(MadeUpQuestion *q) {
    int v[4];
    differentNumbers(v, 4, 1, 60);
    Node *headPtr = nullptr;
    line(q, "Node* headPtr = nullptr;");
    for (int k = 0; k < 4; k++) {
        line(q, TextFormat("InsertAtEnd(&headPtr, %d);", v[k]));
        InsertAtEnd(&headPtr, v[k]);
    }
    int removeValue = v[randomInt(0, 3)];
    line(q, TextFormat("DeleteWithValue(&headPtr, %d);", removeValue));
    DeleteWithValue(&headPtr, removeValue);
    line(q, "PrintList(headPtr);");
    int values[4];
    int n = listValues(headPtr, values, 4);
    freeList(headPtr);
    listAnswer(q, values, n);
    char allText[100];
    joinNumbers(v, 4, allText, sizeof(allText));
    mistake(q, allText, "DeleteWithValue unlinks the node with that value and deletes it.");
    hints(q, "Write out the list after the four inserts.",
             "DeleteWithValue removes the node holding that value.",
             "The nodes either side of it get linked together.");
    layout(q, TextFormat("after the inserts: %d %d %d %d\nremove the node holding %d\nprint what's left, in order", v[0], v[1], v[2], v[3], removeValue));
    note(q, "DeleteWithValue finds the node (keeping track of prev), sets prev->nextPtr = curNodePtr->nextPtr, then deletes it. Deleting the head changes *ptrToHeadPtr.");
    finish(q, TASK_TRACE, "What does PrintList print? (numbers with spaces between)");
}

void makeLevel43(MadeUpQuestion *q) {
    int count = randomInt(2, 6);
    Node *headPtr = nullptr;
    line(q, "Node* headPtr = nullptr;");
    for (int k = 0; k < count; k++) {
        int value = (k + 1) * 10;
        line(q, TextFormat("InsertAtEnd(&headPtr, %d);", value));
        InsertAtEnd(&headPtr, value);
    }
    line(q, "// clear the list, like clear() in the lecture");
    line(q, "Node* current = headPtr;");
    line(q, "int deletes = 0;");
    line(q, "while (current != nullptr) {");
    line(q, "    Node* temp = current;");
    line(q, "    current = current->nextPtr;");
    line(q, "    delete temp;");
    line(q, "    deletes++;");
    line(q, "}");
    line(q, "headPtr = nullptr;");
    line(q, "cout << deletes;");

    Node *current = headPtr;
    int deletes = 0;
    while (current != nullptr) {
        Node *temp = current;
        current = current->nextPtr;
        delete temp;
        deletes++;
    }
    answer(q, TextFormat("%d", deletes));
    mistake(q, TextFormat("%d", deletes - 1), "The loop keeps going until current is nullptr, so the last node is deleted too.");
    hints(q, "How many nodes are in the list?",
             "The loop deletes one node each time round.",
             "It stops when current reaches nullptr, after the last node.");
    layout(q, TextFormat("the list has one node per InsertAtEnd: count them\neach time round: save, move on, delete, count"));
    note(q, "clear() saves the node in temp, moves current on FIRST, then deletes temp - so every node is freed (the class's destructor calls it).");
    finish(q, TASK_TRACE, "What does it print?");
}

void makeLevel44(MadeUpQuestion *q) {
    int count = randomInt(3, 5);
    int v[5];
    differentNumbers(v, 5, 1, 60);
    DoublyLinkedList list;
    line(q, "DoublyLinkedList list;");
    for (int k = 0; k < count; k++) {
        line(q, TextFormat("list.InsertAtEnd(%d);", v[k]));
        list.InsertAtEnd(v[k]);
    }
    line(q, "DoublyLinkedNode *curNodePtr = list.head;");
    line(q, "while (curNodePtr->nextPointer != nullptr) {");
    line(q, "    curNodePtr = curNodePtr->nextPointer;");
    line(q, "}");
    line(q, "cout << curNodePtr->prevPointer->data;");
    DoublyLinkedNode *curNodePtr = list.head;
    while (curNodePtr->nextPointer != nullptr) {
        curNodePtr = curNodePtr->nextPointer;
    }
    int result = curNodePtr->prevPointer->data;
    answer(q, TextFormat("%d", result));
    mistake(q, TextFormat("%d", v[count - 1]), "The loop stops at the LAST node, then prevPointer goes back one.");
    hints(q, "The while loop walks to the last node.",
             "prevPointer points at the node before.",
             "So it prints the second-to-last value.");
    layout(q, "walk forward until nextPointer is nullptr (the last node)\nthen step back once with prevPointer");
    note(q, "In a doubly linked list each node has nextPointer AND prevPointer, so you can step backwards (Lab 2 Part 2).");
    finish(q, TASK_TRACE, "What does it print?");
}

void makeLevel45(MadeUpQuestion *q) {
    int count = randomInt(3, 4);
    int v[4];
    differentNumbers(v, 4, 1, 60);
    DoublyLinkedList list;
    line(q, "DoublyLinkedList list;");
    for (int k = 0; k < count; k++) {
        line(q, TextFormat("list.InsertAtBeginning(%d);", v[k]));
        list.InsertAtBeginning(v[k]);
    }
    line(q, "DoublyLinkedNode *curNodePtr = list.head;");
    line(q, "while (curNodePtr != nullptr) {");
    line(q, "    cout << curNodePtr->data << \" \";");
    line(q, "    curNodePtr = curNodePtr->nextPointer;");
    line(q, "}");
    int values[4];
    int n = 0;
    for (DoublyLinkedNode *node = list.head; node != nullptr; node = node->nextPointer) {
        values[n] = node->data;
        n++;
    }
    listAnswer(q, values, n);
    char addedText[100];
    joinNumbers(v, count, addedText, sizeof(addedText));
    mistake(q, addedText, "Each InsertAtBeginning puts the new node at the FRONT, so the last one added comes first.");
    hints(q, "InsertAtBeginning puts each new node in front of the head.",
             "So the newest node ends up first.",
             "Write the list after each call.");
    layout(q, "after each InsertAtBeginning, write the list:\n[newest] -> ... -> [oldest]\nthen print from the head");
    note(q, "InsertAtBeginning: newNode->nextPointer = head, the old head points back at newNode, then head = newNode.");
    finish(q, TASK_TRACE, "What does it print? (numbers with spaces between)");
}

void makeLevel46(MadeUpQuestion *q) {
    int v[4];
    differentNumbers(v, 4, 1, 60);
    int position = randomInt(0, 3);
    DoublyLinkedList list;
    line(q, "DoublyLinkedList list;");
    for (int k = 0; k < 3; k++) {
        line(q, TextFormat("list.InsertAtEnd(%d);", v[k]));
        list.InsertAtEnd(v[k]);
    }
    line(q, TextFormat("list.InsertAtPosition(%d, %d);", v[3], position));
    list.InsertAtPosition(v[3], position);
    line(q, "DoublyLinkedNode *curNodePtr = list.head;");
    line(q, "while (curNodePtr != nullptr) {");
    line(q, "    cout << curNodePtr->data << \" \";");
    line(q, "    curNodePtr = curNodePtr->nextPointer;");
    line(q, "}");
    int values[4];
    int n = 0;
    for (DoublyLinkedNode *node = list.head; node != nullptr; node = node->nextPointer) {
        values[n] = node->data;
        n++;
    }
    listAnswer(q, values, n);
    hints(q, "The list is the three InsertAtEnd values, in order.",
             "Positions count from 0: position 0 is the very front.",
             "The new node ends up AT that position, and the rest move along.");
    layout(q, TextFormat("before: %d %d %d\npositions: 0  1  2  (3 is the end)\nput %d at position %d", v[0], v[1], v[2], v[3], position));
    note(q, "InsertAtPosition walks to the node before the position, then links the new node in between: four links change (Lab 2 Part 2).");
    finish(q, TASK_TRACE, "In this game's lab2.cpp, positions count from 0 like array indexes. What does it print? (numbers with spaces between)");
}

// ============================================================
// Chapter 8: Lab 1 review (the real functions from lab1.cpp)
// ============================================================

// Writes an array as "{1, 2, 3}"
void arrayText(int arr[], int size, char *out, int outSize) {
    snprintf(out, outSize, "{");
    for (int i = 0; i < size; i++) {
        int length = (int)strlen(out);
        snprintf(out + length, outSize - length, i == 0 ? "%d" : ", %d", arr[i]);
    }
    int length = (int)strlen(out);
    snprintf(out + length, outSize - length, "}");
}

void makeLevel47(MadeUpQuestion *q) {
    int size = randomInt(6, 8);
    int arr[8];
    for (int i = 0; i < size; i++) arr[i] = randomInt(1, 4);
    int value = randomInt(1, 4);
    char text[100];
    arrayText(arr, size, text, sizeof(text));
    line(q, TextFormat("int arr[%d] = %s;", size, text));
    line(q, TextFormat("int count = frequencyCount(%d, arr, %d);", size, value));
    int result = frequencyCount(size, arr, value);
    check(q, "cout << count;");
    numberAnswer(q, "count", result);
    hints(q, "frequencyCount counts how many elements equal value.",
             "Go through the array one element at a time.",
             "Add 1 each time an element matches.");
    layout(q, TextFormat("%s\ncount the %ds", text, value));
    note(q, "Lab 1 Q13: loop i from 0 to size - 1 and add 1 to count whenever arr[i] == value.");
    finish(q, TASK_TRACE, "Using Lab 1's frequencyCount, what is count?");
}

void makeLevel48(MadeUpQuestion *q) {
    int size = randomInt(5, 7);
    int arr[7];
    for (int i = 0; i < size; i++) arr[i] = randomInt(1, 4);
    char text[100];
    arrayText(arr, size, text, sizeof(text));
    line(q, TextFormat("int arr[%d] = %s;", size, text));
    line(q, TextFormat("int dupes = countDuplicates(%d, arr);", size));
    int result = countDuplicates(size, arr);
    check(q, "cout << dupes;");
    numberAnswer(q, "dupes", result);
    hints(q, "This version counts every matching PAIR: arr[i] == arr[j] with j after i.",
             "Three 2s make three pairs: (1st,2nd), (1st,3rd), (2nd,3rd).",
             "Count the pairs for each value, then add them up.");
    layout(q, TextFormat("%s\nfor each value: how many times does it appear (n)?\npairs for that value = n * (n - 1) / 2", text));
    note(q, "Lab 1 Q14 (the version in this game): two loops with j starting at i + 1 count every pair of equal elements.");
    finish(q, TASK_TRACE, "Using this game's Lab 1 countDuplicates (it counts matching pairs), what is dupes?");
}

void makeLevel49(MadeUpQuestion *q) {
    int size = randomInt(5, 6);
    int arr[6];
    differentNumbers(arr, size, 1, 50);
    char text[100];
    arrayText(arr, size, text, sizeof(text));
    int ask = randomInt(0, size - 1);
    line(q, TextFormat("int arr[%d] = %s;", size, text));
    line(q, TextFormat("reverse(%d, arr);", size));
    line(q, TextFormat("cout << arr[%d];", ask));
    int before = arr[ask];
    reverse(size, arr);
    answer(q, TextFormat("%d", arr[ask]));
    mistake(q, TextFormat("%d", before), "That's what was there before reverse ran.");
    hints(q, "reverse swaps the first and last, then works inwards.",
             "Element i swaps with element size - 1 - i.",
             "Which element ends up at the position being printed?");
    layout(q, TextFormat("before: %s\nwrite it backwards, then count to the position", text));
    note(q, "Lab 1 Q15: swap arr[i] with arr[size - 1 - i], for i up to size / 2.");
    finish(q, TASK_TRACE, "Using Lab 1's reverse, what does it print?");
}

void makeLevel50(MadeUpQuestion *q) {
    int size = randomInt(4, 6);
    int arr[6];
    differentNumbers(arr, size, 1, 50);
    char text[100];
    arrayText(arr, size, text, sizeof(text));
    int times = randomInt(1, 2);
    int ask = randomInt(0, size - 1);
    line(q, TextFormat("int arr[%d] = %s;", size, text));
    for (int t = 0; t < times; t++) {
        line(q, TextFormat("rotateLeft(%d, arr);", size));
        rotateLeft(size, arr);
    }
    line(q, TextFormat("cout << arr[%d];", ask));
    answer(q, TextFormat("%d", arr[ask]));
    hints(q, "rotateLeft moves every element one place left, and the first goes to the end.",
             "Do it once for each call.",
             "Then count to the position being printed.");
    layout(q, TextFormat("before: %s\nafter one rotateLeft: the first element moves to the end", text));
    note(q, "Lab 1 Q16: save arr[0], shift everything left with arr[i] = arr[i + 1], then put the saved one at the end.");
    finish(q, TASK_TRACE, "Using Lab 1's rotateLeft, what does it print?");
}

void makeLevel51(MadeUpQuestion *q) {
    int size = randomInt(4, 5);
    int movies[5];
    differentNumbers(movies, size, 6, 15);
    for (int i = 0; i < size; i++) movies[i] = movies[i] * 10;   // 60 to 150 minutes
    int flight;
    if (randomInt(0, 1) == 0) {
        int a = randomInt(0, size - 1);
        int b = (a + 1 + randomInt(0, size - 2)) % size;
        flight = movies[a] + movies[b];
    } else {
        flight = randomInt(12, 30) * 10;
    }
    char text[100];
    arrayText(movies, size, text, sizeof(text));
    line(q, TextFormat("int movies[] = %s;", text));
    line(q, TextFormat("bool ok = twoMovies(%d, movies, %d);", flight, size));
    bool result = twoMovies(flight, movies, size);
    check(q, "cout << (ok ? \"true\" : \"false\");");
    answer(q, result ? "true" : "false");
    answer(q, result ? "True" : "False");
    mistake(q, result ? "false" : "true", result ? "Check every pair of two DIFFERENT movies: one of them adds up exactly." : "No pair of two different movies adds up exactly (watching the same movie twice isn't allowed).");
    hints(q, "Try every pair of two different movies.",
             TextFormat("Does any pair add up to exactly %d?", flight),
             "The same movie twice doesn't count.");
    layout(q, TextFormat("%s\ncheck each pair: first + second, first + third, ...\nlooking for exactly %d", text, flight));
    note(q, "Lab 1 Q17: two loops, with j starting at i + 1 so a movie is never paired with itself.");
    finish(q, TASK_TRACE, "Using Lab 1's twoMovies, is ok true or false?");
}

void makeLevel52(MadeUpQuestion *q) {
    const char *sentences[8] = { "hi there", "C and C++ pointers", "pool  table", " lab two ", "one", "Ball 8 wins", "a b c d", "linked-list nodes" };
    char text[40];
    snprintf(text, sizeof(text), "%s", sentences[randomInt(0, 7)]);
    int size = (int)strlen(text);
    line(q, TextFormat("char characters[] = \"%s\";", text));
    line(q, TextFormat("int words = wordCounter(%d, characters);", size));
    int result = wordCounter(size, text);
    check(q, "cout << words;");
    numberAnswer(q, "words", result);
    hints(q, "A word is one or more letters (A to Z or a to z) in a row.",
             "Anything else (spaces, digits, + or -) ends a word.",
             "Count where each run of letters starts.");
    layout(q, TextFormat("\"%s\"\nmark the start of each run of letters, then count them", text));
    note(q, "Lab 1 Q18: count a word when you reach a letter and you weren't already inside a word.");
    finish(q, TASK_TRACE, "Using Lab 1's wordCounter, what is words?");
}

// The 4x6 array from the Lab 1 main
int lab1Array[4][6] = { { 0, 0, 3, 1, 3, 4 },
                        { 0, 0, 2, 3, 4, 3 },
                        { 0, 0, 1, 3, 3, 2 },
                        { 0, 0, 1, 1, 1, 1 } };

void lab1ArrayLines(MadeUpQuestion *q) {
    line(q, "int array[4][6] = { {0,0,3,1,3,4},");
    line(q, "                    {0,0,2,3,4,3},");
    line(q, "                    {0,0,1,3,3,2},");
    line(q, "                    {0,0,1,1,1,1} };");
}

void makeLevel53(MadeUpQuestion *q) {
    int row = randomInt(0, 3);
    int col = randomInt(2, 5);
    lab1ArrayLines(q);
    line(q, TextFormat("cout << array[%d][%d];", row, col));
    int result = lab1Array[row][col];
    answer(q, TextFormat("%d", result));
    if (col < 4 && lab1Array[col][row] != result) mistake(q, TextFormat("%d", lab1Array[col][row]), "The first number picks the row, the second picks the column.");
    hints(q, "array[row][col]: the first number is the row.",
             "Rows and columns both count from 0.",
             "Go down to the row, then along to the column.");
    layout(q, TextFormat("row %d is the line that starts with {0,0,...}\ncount along that row to column %d (starting at 0)", row, col));
    note(q, "A 2D array is indexed array[row][col]. PrintArray (Lab 1 Q19) loops over the rows, and inside that over the columns.");
    finish(q, TASK_TRACE, "This is the array from the Lab 1 main. What does it print?");
}

void makeLevel54(MadeUpQuestion *q) {
    int row, col;
    if (randomInt(0, 1) == 0) {
        row = 3;                  // the bottom row has a line of 1s
        col = randomInt(2, 3);
    } else {
        row = randomInt(0, 3);
        col = randomInt(0, 3);
    }
    lab1ArrayLines(q);
    line(q, TextFormat("int row = %d, col = %d;", row, col));
    line(q, "int v = array[row][col];");
    line(q, "if (v > 0 && col + 2 < 6 && array[row][col + 1] == v && array[row][col + 2] == v)");
    line(q, "    cout << \"yes\";");
    line(q, "else");
    line(q, "    cout << \"no\";");
    int v = lab1Array[row][col];
    bool found = v > 0 && col + 2 < 6 && lab1Array[row][col + 1] == v && lab1Array[row][col + 2] == v;
    answer(q, found ? "yes" : "no");
    mistake(q, found ? "no" : "yes", found ? "All three items in that row match, and they're not 0." : "Check all three: v must be more than 0, and the next two in the row must equal v.");
    hints(q, "Find v = array[row][col] first.",
             "Then compare the next two items along the same row.",
             "0 never counts as a cluster.");
    layout(q, TextFormat("row %d, columns %d, %d and %d\nare all three the same, and more than 0?", row, col, col + 1, col + 2));
    note(q, "Lab 1 Q20 checks for a HORIZONTAL line of 3 identical items first: array[row][col + 1] and array[row][col + 2] must both equal v.");
    finish(q, TASK_TRACE, "Q20's horizontal check: what does it print?");
}

// ============================================================
// Picking the generator for a level
// ============================================================

bool makeQuestion(int level, MadeUpQuestion *q) {
    begin(q);
    switch (level) {
        case 1: makeLevel1(q); break;
        case 2: makeLevel2(q); break;
        case 3: makeLevel3(q); break;
        case 4: makeLevel4(q); break;
        case 5: makeLevel5(q); break;
        case 6: makeLevel6(q); break;
        case 7: makeLevel7(q); break;
        case 8: makeLevel8(q); break;
        case 9: makeLevel9(q); break;
        case 10: makeLevel10(q); break;
        case 11: makeLevel11(q); break;
        case 12: makeLevel12(q); break;
        case 13: makeLevel13(q); break;
        case 14: makeLevel14(q); break;
        case 15: makeLevel15(q); break;
        case 16: makeLevel16(q); break;
        case 17: makeLevel17(q); break;
        case 18: makeLevel18(q); break;
        case 19: makeLevel19(q); break;
        case 20: makeLevel20(q); break;
        case 21: makeLevel21(q); break;
        case 22: makeLevel22(q); break;
        case 23: makeLevel23(q); break;
        case 24: makeLevel24(q); break;
        case 25: makeLevel25(q); break;
        case 26: makeLevel26(q); break;
        case 27: makeLevel27(q); break;
        case 28: makeLevel28(q); break;
        case 29: makeLevel29(q); break;
        case 30: makeLevel30(q); break;
        case 31: makeLevel31(q); break;
        case 32: makeLevel32(q); break;
        case 33: makeLevel33(q); break;
        case 34: makeLevel34(q); break;
        case 35: makeLevel35(q); break;
        case 36: makeLevel36(q); break;
        case 37: makeLevel37(q); break;
        case 38: makeLevel38(q); break;
        case 39: makeLevel39(q); break;
        case 40: makeLevel40(q); break;
        case 41: makeLevel41(q); break;
        case 42: makeLevel42(q); break;
        case 43: makeLevel43(q); break;
        case 44: makeLevel44(q); break;
        case 45: makeLevel45(q); break;
        case 46: makeLevel46(q); break;
        case 47: makeLevel47(q); break;
        case 48: makeLevel48(q); break;
        case 49: makeLevel49(q); break;
        case 50: makeLevel50(q); break;
        case 51: makeLevel51(q); break;
        case 52: makeLevel52(q); break;
        case 53: makeLevel53(q); break;
        case 54: makeLevel54(q); break;
        default: return false;
    }
    return true;
}
