#include "editor.h"
#include "ui.h"
#include "anim.h"
#include <cstring>
#include <cmath>

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
    box->size = CODE_SIZE;
    box->codeMode = false;
    box->firstLine = 0;
    box->caretPlaced = false;
    box->lastChange = 0;
    box->runBarY = -1;
    box->error = false;
    clearCodeBox(box);
}

void clearCodeBox(CodeBox *box) {
    box->text[0] = '\0';
    box->length = 0;
    box->cursor = 0;
    box->firstLine = 0;
    box->error = false;
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
    int line = (int)((mouse.y - box->textY) / lineHeight(box->size));
    int column = (int)((mouse.x - box->textX) / charWidth(box->size) + 0.5f);
    if (box->codeMode) line += box->firstLine;
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
    int lengthBefore = box->length;
    int cursorBefore = box->cursor;

    // Typed characters (GetCharPressed handles Shift for us)
    int key = GetCharPressed();
    while (key > 0) {
        // codeMode: a } typed on a line of only spaces moves back one indent first
        if (key == '}' && box->codeMode) {
            int start = lineStart(box, box->cursor);
            bool onlySpaces = box->cursor - start >= 4;
            for (int i = start; i < box->cursor; i++) {
                if (box->text[i] != ' ') onlySpaces = false;
            }
            if (onlySpaces) {
                for (int i = 0; i < 4; i++) {
                    box->cursor--;
                    removeChar(box, box->cursor);
                }
            }
        }
        if (key >= 32 && key <= 126) insertChar(box, (char)key);
        key = GetCharPressed();
    }

    // Enter starts a new line (if the box has room for one)
    bool control = IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL) ||
                   IsKeyDown(KEY_LEFT_SUPER) || IsKeyDown(KEY_RIGHT_SUPER);
    if (keyPressed(KEY_ENTER) && !control && countLines(box->text) < box->maxLines) {
        if (box->codeMode) {
            // Auto-indent: the new line starts with the same spaces as this one,
            // and 4 more after a {
            int start = lineStart(box, box->cursor);
            int spaces = 0;
            while (start + spaces < box->cursor && box->text[start + spaces] == ' ') spaces++;
            if (box->cursor > 0 && box->text[box->cursor - 1] == '{') spaces += 4;
            insertChar(box, '\n');
            for (int i = 0; i < spaces; i++) insertChar(box, ' ');
        } else {
            insertChar(box, '\n');
        }
    }

    if (box->codeMode) {
        // Tab puts in 4 spaces
        if (keyPressed(KEY_TAB)) {
            for (int i = 0; i < 4; i++) insertChar(box, ' ');
        }
        // Ctrl/Cmd + V pastes
        if (control && IsKeyPressed(KEY_V)) {
            const char *pasted = GetClipboardText();
            for (const char *p = pasted; p != nullptr && *p != '\0'; p++) {
                if (*p == '\t') {
                    for (int i = 0; i < 4; i++) insertChar(box, ' ');
                } else if (*p == '\n' || (*p >= 32 && *p <= 126)) {
                    insertChar(box, *p);
                }
            }
        }
        // The mouse wheel scrolls
        if (CheckCollisionPointRec(GetMousePosition(), box->area)) {
            box->firstLine -= (int)(GetMouseWheelMove() * 3);
            if (box->firstLine < 0) box->firstLine = 0;
        }
    }

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

    // Typing clears the red underline; any change keeps the cursor solid for a moment
    if (box->length != lengthBefore) box->error = false;
    if (box->length != lengthBefore || box->cursor != cursorBefore) box->lastChange = GetTime();
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

const char *typeNames[] = { "Ball", "Node", "DoublyLinkedNode", "DoublyLinkedList", "cout", "endl", "printf" };
const int TYPE_COUNT = 7;

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
// Drawing the box: it looks like a code editor (IDE)
// ============================================================

const Color GUTTER = { 10, 11, 14, 255 };             // the strip with the line numbers
const Color CURRENT_LINE = { 255, 255, 255, 10 };     // a faint band on the line you're typing on
const Color RUN_GREEN = { 110, 214, 132, 255 };

// The glowing bar for the line that's running: a soft green glow with a bright edge on the left
void drawRunningBar(Rectangle line) {
    for (int i = 3; i >= 1; i--) {
        Rectangle glow = { line.x, line.y - i * 2.0f, line.width, line.height + i * 4.0f };
        DrawRectangleRec(glow, Fade(RUN_GREEN, 0.05f));
    }
    DrawRectangleRec(line, Fade(RUN_GREEN, 0.18f));
    DrawRectangleRec(Rectangle{ line.x, line.y, 3, line.height }, RUN_GREEN);
    // A small play arrow
    float mid = line.y + line.height / 2;
    DrawTriangle(Vector2{ line.x + 7, mid - 5 }, Vector2{ line.x + 7, mid + 5 }, Vector2{ line.x + 14, mid }, RUN_GREEN);
}

// A red wavy line (like a spelling mistake) under 'columns' characters of a line
void drawSquiggle(float x, float y, int columns, float size) {
    float width = columns * charWidth(size);
    if (width < 8) width = 8;
    Vector2 last = { x, y };
    for (float d = 2; d <= width; d += 2) {
        Vector2 point = { x + d, y + sinf(d * 0.8f) * 1.5f };
        DrawLineEx(last, point, 1.3f, COLOUR_BAD);
        last = point;
    }
}

void drawCodeBox(CodeBox *box, int highlightLine) {
    Rectangle area = box->area;
    float size = box->size;
    float lh = lineHeight(size);
    float dt = GetFrameTime();
    bool numbered = box->maxLines > 1;   // line numbers for code; not for one-line boxes
    float gutterWidth = numbered ? charWidth(size) * 3 + 14 : 0;

    // The box: rounded, with a glow round it while you're typing in it
    float roundness = roundnessFor(area, 6);
    if (box->focused) {
        for (int i = 3; i >= 1; i--) {
            Rectangle glow = { area.x - i * 2.0f, area.y - i * 2.0f, area.width + i * 4.0f, area.height + i * 4.0f };
            DrawRectangleRounded(glow, roundnessFor(glow, 6 + i * 2.0f), 8, Fade(COLOUR_ACCENT, 0.05f));
        }
    }
    DrawRectangleRounded(area, roundness, 8, COLOUR_CODE_BG);
    if (numbered) DrawRectangleRec(Rectangle{ area.x + 2, area.y + 2, gutterWidth - 4, area.height - 4 }, GUTTER);
    Color border = box->focused ? COLOUR_ACCENT : Color{ 58, 60, 72, 255 };
    if (box->error) border = COLOUR_BAD;
    DrawRectangleRoundedLinesEx(area, roundness, 8, box->focused ? 1.5f : 1.0f, border);

    float x = area.x + gutterWidth + 10;
    float y = area.y + 8;

    // codeMode: scroll so the cursor's line is showing, and draw only inside the box
    int visibleLines = (int)((area.height - 16) / lh);
    int cursorLine = 0;
    int cursorColumn = 0;
    int cursor = box->cursor < box->length ? box->cursor : box->length;
    for (int i = 0; i < cursor; i++) {
        if (box->text[i] == '\n') {
            cursorLine++;
            cursorColumn = 0;
        } else {
            cursorColumn++;
        }
    }
    if (box->codeMode) {
        static int lastCursor = -1;
        if (box->cursor != lastCursor) {   // only follow the cursor when it moves (so the wheel still works)
            if (cursorLine < box->firstLine) box->firstLine = cursorLine;
            if (cursorLine >= box->firstLine + visibleLines) box->firstLine = cursorLine - visibleLines + 1;
            lastCursor = box->cursor;
        }
        int maxFirst = countLines(box->text) - visibleLines;
        if (maxFirst < 0) maxFirst = 0;
        if (box->firstLine > maxFirst) box->firstLine = maxFirst;
        beginClip(area.x + 2, area.y + 2, area.width - 4, area.height - 4);
        y -= box->firstLine * lh;
    }

    // The line you're typing on gets a faint band
    if (box->focused && highlightLine < 0) {
        DrawRectangleRec(Rectangle{ area.x + gutterWidth, y + cursorLine * lh - 2, area.width - gutterWidth - 2, lh }, CURRENT_LINE);
    }

    // While the code runs, a glowing bar glides from line to line
    if (highlightLine >= 0) {
        float target = y + highlightLine * lh - 2;
        if (box->runBarY < 0) box->runBarY = target;   // the first line: start there
        box->runBarY = smoothTowards(box->runBarY, target, 18, dt);
        drawRunningBar(Rectangle{ area.x + 2, box->runBarY, area.width - 4, lh });
    } else {
        box->runBarY = -1;
    }

    // Line numbers (the one you're on is brighter)
    if (numbered) {
        int lines = countLines(box->text);
        if (lines < 1) lines = 1;
        for (int i = 0; i < lines; i++) {
            Color colour = (box->focused && i == cursorLine) ? COLOUR_TEXT : Color{ 88, 90, 104, 255 };
            if (i == highlightLine) colour = RUN_GREEN;
            const char *number = TextFormat("%d", i + 1);
            drawText(number, area.x + gutterWidth - 8 - textWidth(number, size), y + i * lh, size, colour);
        }
    }

    if (box->length == 0) {
        drawText(box->placeholder, x, y, size, COLOUR_MUTED);
    } else {
        drawCode(box->text, x, y, size, area.width - gutterWidth - 20);
    }

    // After a wrong answer: a red wavy line under each line of code
    if (box->error && box->length > 0) {
        int line = 0;
        int column = 0;
        int indent = -1;   // where the line's code starts (after its spaces)
        for (int i = 0; i <= box->length; i++) {
            if (i == box->length || box->text[i] == '\n') {
                if (indent >= 0) drawSquiggle(x + indent * charWidth(size), y + line * lh + size + 2, column - indent, size);
                line++;
                column = 0;
                indent = -1;
            } else {
                if (indent < 0 && box->text[i] != ' ') indent = column;
                column++;
            }
        }
    }

    box->textX = x;   // remembered for placing the cursor with the mouse
    box->textY = y;

    // The cursor glides to where it should be, and pulses gently (solid while you're typing)
    if (box->focused) {
        float cursorX = x + cursorColumn * charWidth(size);
        float cursorY = y + cursorLine * lh;
        if (!box->caretPlaced) {
            box->caretX = cursorX;
            box->caretY = cursorY;
            box->caretPlaced = true;
        }
        box->caretX = smoothTowards(box->caretX, cursorX, 35, dt);
        box->caretY = smoothTowards(box->caretY, cursorY, 35, dt);
        float sinceTyping = (float)(GetTime() - box->lastChange);
        float alpha = sinceTyping < 0.6f ? 1.0f : 0.55f + 0.45f * cosf((sinceTyping - 0.6f) * 5);
        DrawRectangleRec(Rectangle{ box->caretX - 2, box->caretY - 1, 6, size + 4 }, Fade(COLOUR_ACCENT, 0.12f * alpha));
        DrawRectangleRec(Rectangle{ box->caretX, box->caretY, 2, size + 2 }, Fade(COLOUR_ACCENT, alpha));
    } else {
        box->caretPlaced = false;
    }

    if (box->codeMode) {
        EndScissorMode();
        // A scroll bar when there's more code than fits
        int lines = countLines(box->text);
        if (lines > visibleLines) {
            float barHeight = (area.height - 8) * visibleLines / lines;
            float barY = area.y + 4 + (area.height - 8 - barHeight) * box->firstLine / (lines - visibleLines);
            DrawRectangleRounded(Rectangle{ area.x + area.width - 7, barY, 4, barHeight }, 1, 4, Color{ 90, 95, 110, 255 });
        }
    }
}
