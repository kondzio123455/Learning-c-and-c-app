#include "editor.h"
#include "ui.h"
#include <cstring>

// ============================================================
// Typing
// ============================================================

void setupCodeBox(CodeBox *box, int maxLines, const char *placeholder) {
    box->maxLines = maxLines;
    box->placeholder = placeholder;
    box->focused = false;
    box->area = Rectangle{ 0, 0, 0, 0 };
    clearCodeBox(box);
}

void clearCodeBox(CodeBox *box) {
    box->text[0] = '\0';
    box->length = 0;
}

int countLines(const char *text) {
    int lines = 1;
    while (*text != '\0') {
        if (*text == '\n') lines++;
        text++;
    }
    return lines;
}

void addChar(CodeBox *box, char c) {
    if (box->length < EDITOR_MAX_TEXT - 1) {
        box->text[box->length] = c;
        box->length++;
        box->text[box->length] = '\0';
    }
}

void updateCodeBox(CodeBox *box) {
    if (!box->focused) return;

    // Typed characters (GetCharPressed handles Shift for us)
    int key = GetCharPressed();
    while (key > 0) {
        if (key >= 32 && key <= 126) addChar(box, (char)key);
        key = GetCharPressed();
    }

    // Enter starts a new line (if the box has room for one)
    if (IsKeyPressed(KEY_ENTER) || IsKeyPressedRepeat(KEY_ENTER)) {
        if (countLines(box->text) < box->maxLines) addChar(box, '\n');
    }

    // Backspace deletes the last character
    if (IsKeyPressed(KEY_BACKSPACE) || IsKeyPressedRepeat(KEY_BACKSPACE)) {
        if (box->length > 0) {
            box->length--;
            box->text[box->length] = '\0';
        }
    }
}

// ============================================================
// Syntax colouring
// ============================================================

const Color CODE_PLAIN = { 212, 212, 212, 255 };
const Color CODE_KEYWORD = { 86, 156, 214, 255 };    // blue
const Color CODE_TYPE = { 78, 201, 176, 255 };       // teal
const Color CODE_NUMBER = { 181, 206, 168, 255 };    // light green
const Color CODE_STRING = { 206, 145, 120, 255 };    // orange
const Color CODE_COMMENT = { 106, 153, 85, 255 };    // green

const char *keywords[] = { "int", "char", "bool", "float", "double", "void", "return",
                           "if", "else", "while", "for", "new", "delete", "nullptr", "NULL",
                           "true", "false", "struct", "class", "public", "const", "using",
                           "namespace", "sizeof" };
const int KEYWORD_COUNT = 24;

const char *typeNames[] = { "Ball", "Node", "DoublyLinkedNode", "DoublyLinkedList", "cout", "endl" };
const int TYPE_COUNT = 6;

bool isWordChar(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '_';
}

// Is 'word' in the list? (strcmp returns 0 when two strings are the same)
bool inList(const char *word, const char *list[], int count) {
    for (int i = 0; i < count; i++) {
        if (strcmp(word, list[i]) == 0) return true;
    }
    return false;
}

// Draws one line of code (up to '\n' or the end), colouring each piece.
// Every character is the same width, so piece number n starts at x + column * charWidth.
void drawCodeLine(const char *line, float x, float y, float size) {
    float w = charWidth(size);
    int column = 0;
    const char *p = line;

    while (*p != '\0' && *p != '\n') {
        int length = 1;
        Color colour = CODE_PLAIN;

        if (p[0] == '/' && p[1] == '/') {
            // A comment: the rest of the line
            while (p[length] != '\0' && p[length] != '\n') length++;
            colour = CODE_COMMENT;
        } else if (*p == '\'' || *p == '"') {
            // A char or string: up to the matching quote
            while (p[length] != '\0' && p[length] != '\n' && p[length] != *p) length++;
            if (p[length] == *p) length++;
            colour = CODE_STRING;
        } else if (*p >= '0' && *p <= '9') {
            while (p[length] >= '0' && p[length] <= '9') length++;
            colour = CODE_NUMBER;
        } else if (isWordChar(*p)) {
            while (isWordChar(p[length])) length++;
            char word[40];
            int copyLength = length;
            if (copyLength > 39) copyLength = 39;
            for (int i = 0; i < copyLength; i++) word[i] = p[i];
            word[copyLength] = '\0';

            if (word[0] == '_') colour = COLOUR_ACCENT;   // the ____ gap in a task
            else if (inList(word, keywords, KEYWORD_COUNT)) colour = CODE_KEYWORD;
            else if (inList(word, typeNames, TYPE_COUNT)) colour = CODE_TYPE;
        }

        // Draw this piece
        char piece[200];
        int pieceLength = length;
        if (pieceLength > 199) pieceLength = 199;
        for (int i = 0; i < pieceLength; i++) piece[i] = p[i];
        piece[pieceLength] = '\0';
        drawText(piece, x + column * w, y, size, colour);

        column += length;
        p += length;
    }
}

// Draws several lines of code. A line with ____ in it gets a yellow bar behind it, 'width' wide.
float drawCode(const char *code, float x, float y, float size, float width) {
    float lineY = y;
    const char *line = code;

    while (true) {
        const char *nextLine = strchr(line, '\n');   // where this line ends (nullptr on the last line)
        const char *gap = strstr(line, "____");       // where the gap is, if there is one

        // Mark this line if the gap is on it
        if (gap != nullptr && (nextLine == nullptr || gap < nextLine)) {
            DrawRectangle((int)x - 4, (int)lineY - 2, (int)width + 8, (int)lineHeight(size), Color{ 70, 62, 20, 255 });
        }

        drawCodeLine(line, x, lineY, size);
        lineY += lineHeight(size);

        if (nextLine == nullptr) break;
        line = nextLine + 1;
    }
    return lineY - y;
}

// ============================================================
// Drawing the box
// ============================================================

void drawCodeBox(CodeBox *box, int highlightLine) {
    Rectangle area = box->area;
    DrawRectangleRec(area, COLOUR_CODE_BG);
    Color border = Color{ 70, 75, 90, 255 };
    if (box->focused) border = COLOUR_ACCENT;
    DrawRectangleLinesEx(area, 2, border);

    float x = area.x + 10;
    float y = area.y + 8;
    float size = CODE_SIZE;

    // While the code runs, the current line gets a bar behind it
    if (highlightLine >= 0) {
        DrawRectangle((int)area.x + 2, (int)(y + highlightLine * lineHeight(size)) - 2,
                      (int)area.width - 4, (int)lineHeight(size), Color{ 40, 90, 50, 255 });
    }

    if (box->length == 0 && !box->focused) {
        drawText(box->placeholder, x, y, size, COLOUR_MUTED);
    } else {
        drawCode(box->text, x, y, size, area.width - 20);
    }

    // Blinking cursor at the end of the text
    if (box->focused && ((int)(GetTime() * 2) % 2 == 0)) {
        int lines = countLines(box->text);
        const char *lastLine = strrchr(box->text, '\n');
        if (lastLine == nullptr) lastLine = box->text;
        else lastLine++;
        float cursorX = x + strlen(lastLine) * charWidth(size);
        float cursorY = y + (lines - 1) * lineHeight(size);
        DrawRectangle((int)cursorX, (int)cursorY, 2, (int)size + 2, COLOUR_ACCENT);
    }
}
