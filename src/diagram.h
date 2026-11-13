#ifndef DIAGRAM_H
#define DIAGRAM_H

// Pointer diagrams: runs a short piece of trace code one line at a time and
// keeps the data table after each line (Name, Type, Value), like the tables
// on slides 10-32. Arrows go from each pointer to what it points at.
//
// It understands what those slides use: int and float variables, arrays,
// pointers and pointers to pointers, &, *, ++, + and -, cout and printf.
// If the code uses anything else, there's simply no diagram for it.

const int DIAGRAM_ROWS = 14;      // the most rows a table can have
const int DIAGRAM_STEPS = 24;     // the most lines of code

struct Cell {
    char name[32];      // i, ptr, a[2] ...
    char type[10];      // int, float, int *, int **
    int stars;          // 0 = a number, 1 = a pointer, 2 = a pointer to a pointer
    bool isFloat;       // float numbers show as 3.14 or 6.0
    bool isSet;         // false = never given a value, shown as ?
    double number;      // the value, for numbers
    int target;         // for pointers: the row it points at (-1 = nowhere)
    int arrayStart;     // for array elements: the row of element 0 (-1 = not in an array)
    int arraySize;
};

struct Diagram {
    bool ok;                                      // false: this code can't be shown as a diagram
    int steps;                                    // how many lines of code ran
    int codeLine[DIAGRAM_STEPS + 1];              // which line of the code each step was (from 0)
    int rowCount[DIAGRAM_STEPS + 1];
    Cell rows[DIAGRAM_STEPS + 1][DIAGRAM_ROWS];   // the table after each step (step 0 = before the first line)
    char output[DIAGRAM_STEPS + 1][80];           // what has been printed so far
};

bool buildDiagram(const char *code, Diagram *diagram);
float diagramHeight(Diagram *diagram, const char *code);
void drawDiagram(Diagram *diagram, const char *code, int step, float x, float y, float width);
void cellValueText(Cell rows[], Cell *cell, char *out, int size);   // "5", "6.0", "address of i", "?"

#endif
