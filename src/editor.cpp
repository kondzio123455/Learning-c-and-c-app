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
    box->textX = 0;
    box->textY = 0;
    clearCodeBox(box);
}

void clearCodeBox(CodeBox *box) {
    box->text[0] = '\0';
    box->length = 0;
    box->cursor = 0;
}

int countLines(const char *text) {
    int lines = 1;
    while (*text != '\0') {
        if (*text == '\n') lines++;
        text++;
    }
    return lines;
}

// Puts a character in at the cursor, moving the rest of the text along one place
void insertChar(CodeBox *box, char c) {
    if (box->length >= EDITOR_MAX_TEXT - 1) return;
    for (int i = box->length; i > box->cursor; i--) {
        box->text[i] = box->text[i - 1];
    }
    box->text[box->cursor] = c;
    box->length++;
    box->cursor++;
    box->text[box->length] = '\0';
}

// Removes the character at position 'at', moving the rest of the text back one place
void removeChar(CodeBox *box, int at) {
    if (at < 0 || at >= box->length) return;
    for (int i = at; i < box->length; i++) {
        box->text[i] = box->text[i + 1];   // this also moves the '\0' back
    }
    box->length--;
}

// Where the line that contains 'position' starts
int lineStart(CodeBox *box, int position) {
    while (position > 0 && box->text[position - 1] != '\n') position--;
    return position;
}

// Where the line that contains 'position' ends (at its '\n', or the end of the text)
int lineEnd(CodeBox *box, int position) {
    while (position < box->length && box->text[position] != '\n') position++;
    return position;
}

// Up and Down keep the same column if the other line is long enough
void moveUpOrDown(CodeBox *box, bool up) {
    int start = lineStart(box, box->cursor);
    int column = box->cursor - start;
    int newStart;
    if (up) {
        if (start == 0) return;                  // already on the first line
        newStart = lineStart(box, start - 1);
    } else {
        int end = lineEnd(box, box->cursor);
        if (end >= box->length) return;          // already on the last line
        newStart = end + 1;
    }
    box->cursor = newStart + column;
    int newEnd = lineEnd(box, newStart);
    if (box->cursor > newEnd) box->cursor = newEnd;   // a shorter line: go to its end
}

// Every character is the same width, so the click's column is just x / charWidth
void placeCursorAt(CodeBox *box, Vector2 mouse) {
    int line = (int)((mouse.y - box->textY) / lineHeight(CODE_SIZE));
    int column = (int)((mouse.x - box->textX) / charWidth(CODE_SIZE) + 0.5f);
    if (line < 0) line = 0;
    if (column < 0) column = 0;

    // Walk down to the start of that line, then along it
    int position = 0;
    for (int l = 0; l < line && position < box->length; l++) {
        position = lineEnd(box, position);
        if (position < box->length) position++;   // step past the '\n'
    }
    int end = lineEnd(box, position);
    box->cursor = position + column;
    if (box->cursor > end) box->cursor = end;
}

// True when a key is pressed, and again and again while it's held down
bool keyPressed(int key) {
    return IsKeyPressed(key) || IsKeyPressedRepeat(key);
}

void updateCodeBox(CodeBox *box) {
    if (!box->focused) return;
    if (box->cursor > box->length) box->cursor = box->length;   // in case the text was changed

    // Typed characters (GetCharPressed handles Shift for us)
    int key = GetCharPressed();
    while (key > 0) {
        if (key >= 32 && key <= 126) insertChar(box, (char)key);
        key = GetCharPressed();
    }

    // Enter starts a new line (if the box has room for one)
    if (keyPressed(KEY_ENTER) && countLines(box->text) < box->maxLines) insertChar(box, '\n');

    // Backspace deletes the character before the cursor, Delete the one after it
    if (keyPressed(KEY_BACKSPACE) && box->cursor > 0) {
        box->cursor--;
        removeChar(box, box->cursor);
    }
    if (keyPressed(KEY_DELETE)) removeChar(box, box->cursor);

    // Arrow keys move the cursor. Cmd (or Ctrl) + Left/Right, Home and End
    // jump to the start or end of the line.
    bool jump = IsKeyDown(KEY_LEFT_SUPER) || IsKeyDown(KEY_RIGHT_SUPER) ||
                IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL);
    if (keyPressed(KEY_LEFT)) {
        if (jump) box->cursor = lineStart(box, box->cursor);
        else if (box->cursor > 0) box->cursor--;
    }
    if (keyPressed(KEY_RIGHT)) {
        if (jump) box->cursor = lineEnd(box, box->cursor);
        else if (box->cursor < box->length) box->cursor++;
    }
    if (keyPressed(KEY_UP)) moveUpOrDown(box, true);
    if (keyPressed(KEY_DOWN)) moveUpOrDown(box, false);
    if (keyPressed(KEY_HOME)) box->cursor = lineStart(box, box->cursor);
    if (keyPressed(KEY_END)) box->cursor = lineEnd(box, box->cursor);

    // Clicking inside the box puts the cursor where you clicked
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(GetMousePosition(), box->area)) {
        placeCursorAt(box, GetMousePosition());
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
            // A // comment: the rest of the line
            while (p[length] != '\0' && p[length] != '\n') length++;
            colour = CODE_COMMENT;
        } else if (p[0] == '/' && p[1] == '*') {
            // A /* comment */: up to the closing */ (or the end of the line)
            length = 2;
            while (p[length] != '\0' && p[length] != '\n' && !(p[length] == '*' && p[length + 1] == '/')) length++;
            if (p[length] == '*') length += 2;
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

// Draws code with a line number in front of each line (for FIND THE BUG tasks)
float drawNumberedCode(const char *code, float x, float y, float size) {
    float lineY = y;
    const char *line = code;
    int number = 1;

    while (true) {
        drawText(TextFormat("%2d", number), x, lineY, size, COLOUR_MUTED);
        drawCodeLine(line, x + charWidth(size) * 4, lineY, size);
        lineY += lineHeight(size);
        number++;

        const char *nextLine = strchr(line, '\n');
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

    if (box->length == 0) {
        drawText(box->placeholder, x, y, size, COLOUR_MUTED);
    } else {
        drawCode(box->text, x, y, size, area.width - 20);
    }

    box->textX = x;   // remembered for placing the cursor with the mouse
    box->textY = y;

    // Blinking cursor: count the lines before it, and the characters since its line started
    if (box->focused && ((int)(GetTime() * 2) % 2 == 0)) {
        int cursor = box->cursor;
        if (cursor > box->length) cursor = box->length;
        int line = 0;
        int column = 0;
        for (int i = 0; i < cursor; i++) {
            if (box->text[i] == '\n') {
                line++;
                column = 0;
            } else {
                column++;
            }
        }
        float cursorX = x + column * charWidth(size);
        float cursorY = y + line * lineHeight(size);
        DrawRectangle((int)cursorX, (int)cursorY, 2, (int)size + 2, COLOUR_ACCENT);
    }
}
