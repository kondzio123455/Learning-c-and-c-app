#ifndef LEVEL_H
#define LEVEL_H

#include "raylib.h"

// The code panel sits on the right of the screen
const int PANEL_X = 770;

// One level: some code with a blank, and a few answers to pick from.
// The line with the blank contains %s, which gets replaced by the answer.
struct Level {
    const char *title;
    const char *task;
    const char *code[12];
    int lineCount;
    int blankLine;          // which line of code has the blank
    const char *choices[3];
    int choiceCount;
    int correctChoice;      // index into choices
};

// Level 1
Level getLevel1();
const char *runLevel1Choice(int choice, int rack[]);

// Code panel
int getClickedChoice(const Level &level);   // returns the choice clicked, or -1
void drawCodePanel(const Level &level, int pickedChoice, const char *message);

#endif
