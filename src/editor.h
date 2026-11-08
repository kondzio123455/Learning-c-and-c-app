#ifndef EDITOR_H
#define EDITOR_H

#include "raylib.h"

const int EDITOR_MAX_TEXT = 400;
const float CODE_SIZE = 16;          // font size of code

// A box the player types code into.
// To keep it simple, the cursor is always at the end of the text.
struct CodeBox {
    char text[EDITOR_MAX_TEXT];
    int length;
    Rectangle area;              // where it is on the screen
    int maxLines;                // 1 for the call line, more for answers
    bool focused;                // true when key presses go to this box
    const char *placeholder;     // grey text shown while the box is empty
};

void setupCodeBox(CodeBox *box, int maxLines, const char *placeholder);
void clearCodeBox(CodeBox *box);
void updateCodeBox(CodeBox *box);                      // reads the keyboard if focused
void drawCodeBox(CodeBox *box, int highlightLine);     // highlightLine = -1 for none
float drawCode(const char *code, float x, float y, float size, float width);   // syntax-coloured code, returns height
float drawNumberedCode(const char *code, float x, float y, float size);         // the same, with line numbers
int countLines(const char *text);

#endif
