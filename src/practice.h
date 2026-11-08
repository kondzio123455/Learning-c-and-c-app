#ifndef PRACTICE_H
#define PRACTICE_H

#include "questions.h"

// A made-up question in the lecturer's style: the same code as the slides and labs,
// with new numbers, names or words each time. The answer is worked out by running
// the same code (or the real lab function), so it's always right.
struct MadeUpQuestion {
    Task task;               // points at the text below, so the game can use it like any task
    char taskText[300];
    char defs[900];          // functions and classes, shown first
    char body[1200];         // the statements
    char code[2200];         // what the player sees: defs, then body
    char check[200];         // a line that prints the answer (only used by the tests)
    char answers[4][200];
    char mistakeAnswers[4][200];
    char mistakeWhy[4][260];
    char hints[3][260];
    char layout[500];
    char note[420];
};

// Makes a new question for this level. Returns false if the level has no made-up questions.
bool makeQuestion(int level, MadeUpQuestion *q);

#endif
