#ifndef EDITOR_H
#define EDITOR_H

#include "raylib.h"

const int EDITOR_MAX_TEXT = 400;
const float CODE_SIZE = 16;          // font size of code

// A box the player types code into
struct CodeBox {
    char text[EDITOR_MAX_TEXT];
    int length;
    int cursor;                  // where the next character goes (0 = before the first one)
    float textX, textY;          // where the text starts on screen (for clicking)
    Rectangle area;              // where it is on the screen
    int maxLines;                // 1 for the call line, more for answers
    bool focused;                // true when key presses go to this box
    const char *placeholder;     // grey text shown while the box is empty
};

void setupCodeBox(CodeBox *box, int maxLines, const char *placeholder);
void clearCodeBox(CodeBox *box);
void updateCodeBox(CodeBox *box);                      // reads the keyboard if focused
void placeCursorAt(CodeBox *box, Vector2 mouse);       // puts the cursor where you clicked
void drawCodeBox(CodeBox *box, int highlightLine);     // highlightLine = -1 for none
float drawCode(const char *code, float x, float y, float size, float width);   // syntax-coloured code, returns height
float drawNumberedCode(const char *code, float x, float y, float size);         // the same, with line numbers
int countLines(const char *text);

#endif
