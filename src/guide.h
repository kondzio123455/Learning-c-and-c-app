#ifndef GUIDE_H
#define GUIDE_H

#include "questions.h"

// Guided help for practice: after GUIDE_AFTER wrong tries, "Guide me" gives
// clues one at a time that lead to the answer. Every clue is worked out from
// the question's own accepted answer, so nothing new is added.
const int GUIDE_AFTER = 10;

int guideClueCount(Task *task);                                        // how many clues there are
void guideClue(Task *task, const char *code, int step, char *out, int size);   // clue number step (from 1)
bool guideClueIsCode(Task *task, int step);                            // show it as code?
void guideProgress(Task *task, const char *typed, char *out, int size);        // how close a wrong answer is

#endif
