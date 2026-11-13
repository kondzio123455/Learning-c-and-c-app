#include "diagram.h"
#include "editor.h"
#include "ui.h"
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <cmath>

// ============================================================
// Splitting a line into tokens: words, numbers and symbols
// ============================================================

const int MAX_TOKENS = 64;

struct Token {
    char text[40];
};

bool isLetter(char c) { return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_'; }
bool isDigit(char c) { return c >= '0' && c <= '9'; }

int splitTokens(const char *line, Token tokens[]) {
    int count = 0;
    const char *p = line;
    while (*p != '\0' && count < MAX_TOKENS) {
        if (*p == ' ' || *p == '\t') {
            p++;
            continue;
        }
        if (*p == '/' && *(p + 1) == '/') break;   // a comment: the rest of the line
        if (*p == '/' && *(p + 1) == '*') {        // a /* comment */
            p += 2;
            while (*p != '\0' && !(*p == '*' && *(p + 1) == '/')) p++;
            if (*p != '\0') p += 2;
            continue;
        }

        int length = 0;
        if (isLetter(*p)) {
            while ((isLetter(p[length]) || isDigit(p[length])) && length < 39) length++;
        } else if (isDigit(*p)) {
            while ((isDigit(p[length]) || p[length] == '.') && length < 39) length++;
        } else if (*p == '"') {
            length = 1;
            while (p[length] != '\0' && p[length] != '"' && length < 38) length++;
            if (p[length] == '"') length++;
        } else {
            // Two-character symbols, or just one
            const char *pairs[] = { "++", "--", "+=", "-=", "<<", "==", "!=", "->" };
            length = 1;
            for (int k = 0; k < 8; k++) {
                if (p[0] == pairs[k][0] && p[1] == pairs[k][1]) length = 2;
            }
        }
        snprintf(tokens[count].text, length + 1, "%s", p);
        count++;
        p += length;
    }
    return count;
}

// ============================================================
// Running the code
// ============================================================

// A value while working something out: a number, or a pointer to a row
struct Value {
    bool ok;
    bool isPointer;
    int target;         // the row (-1 = nullptr)
    double number;
    bool isFloat;
};

// A name that has been declared: one row, or (for an array) its first row
struct Symbol {
    char name[20];
    int row;
    bool isArray;
    int size;
};

struct Machine {
    Cell cells[DIAGRAM_ROWS];
    int cellCount;
    Symbol symbols[DIAGRAM_ROWS];
    int symbolCount;
    char output[80];
    bool ok;
    Token *t;           // the tokens of the line being run
    int count;
    int at;             // the next token
};

bool next(Machine *m, const char *text) {
    if (m->at < m->count && strcmp(m->t[m->at].text, text) == 0) {
        m->at++;
        return true;
    }
    return false;
}

const char *peek(Machine *m) {
    if (m->at < m->count) return m->t[m->at].text;
    return "";
}

Value fail(Machine *m) {
    m->ok = false;
    Value v = { false, false, -1, 0, false };
    return v;
}

Symbol *findSymbol(Machine *m, const char *name) {
    for (int i = 0; i < m->symbolCount; i++) {
        if (strcmp(m->symbols[i].name, name) == 0) return &m->symbols[i];
    }
    return nullptr;
}

// The value stored in a row
Value readRow(Machine *m, int row) {
    if (row < 0 || row >= m->cellCount) return fail(m);
    Cell *cell = &m->cells[row];
    Value v = { true, cell->stars > 0, cell->target, cell->number, cell->isFloat };
    return v;
}

// Pointer + n: moves along an array (and must stay inside it)
Value movePointer(Machine *m, Value pointer, int steps) {
    if (pointer.target < 0) return fail(m);
    Cell *cell = &m->cells[pointer.target];
    if (steps == 0) return pointer;
    if (cell->arrayStart < 0) return fail(m);
    int newRow = pointer.target + steps;
    if (newRow < cell->arrayStart || newRow >= cell->arrayStart + cell->arraySize) return fail(m);
    pointer.target = newRow;
    return pointer;
}

Value expression(Machine *m);
int lvalue(Machine *m);

// The row that a name (or name[index]) means
int nameRow(Machine *m) {
    if (m->at >= m->count || !isLetter(peek(m)[0])) return -1;
    Symbol *symbol = findSymbol(m, peek(m));
    if (symbol == nullptr) return -1;
    m->at++;
    if (next(m, "[")) {
        Value index = expression(m);
        if (!next(m, "]") || index.isPointer) return -1;
        if (!symbol->isArray) {
            // ptr[i] means *(ptr + i)
            Value v = readRow(m, symbol->row);
            if (!v.isPointer) return -1;
            v = movePointer(m, v, (int)index.number);
            return v.target;
        }
        int i = (int)index.number;
        if (i < 0 || i >= symbol->size) return -1;
        return symbol->row + i;
    }
    if (symbol->isArray) return -2;   // the array's name on its own (used as a pointer)
    return symbol->row;
}

Value term(Machine *m) {
    if (next(m, "-")) {
        Value v = term(m);
        if (v.isPointer) return fail(m);
        v.number = -v.number;
        return v;
    }
    if (next(m, "*")) {
        Value pointer = term(m);
        if (!pointer.ok || !pointer.isPointer || pointer.target < 0) return fail(m);
        return readRow(m, pointer.target);
    }
    if (next(m, "&")) {
        int row = lvalue(m);
        if (row < 0) return fail(m);
        Value v = { true, true, row, 0, false };
        return v;
    }
    if (next(m, "(")) {
        Value v = expression(m);
        if (!next(m, ")")) return fail(m);
        return v;
    }
    if (next(m, "nullptr") || next(m, "NULL")) {
        Value v = { true, true, -1, 0, false };
        return v;
    }
    const char *word = peek(m);
    if (isDigit(word[0])) {
        Value v = { true, false, -1, atof(word), strchr(word, '.') != nullptr };
        m->at++;
        return v;
    }
    Symbol *symbol = findSymbol(m, word);
    if (symbol == nullptr) return fail(m);
    int start = m->at;
    int row = nameRow(m);
    if (row == -2) {
        Value v = { true, true, symbol->row, 0, false };   // an array's name is the address of element 0
        return v;
    }
    if (row < 0) {
        m->at = start;
        return fail(m);
    }
    return readRow(m, row);
}

Value expression(Machine *m) {
    Value v = term(m);
    while (m->ok && (strcmp(peek(m), "+") == 0 || strcmp(peek(m), "-") == 0)) {
        bool minus = next(m, "-");
        if (!minus) next(m, "+");
        Value right = term(m);
        if (!m->ok) return fail(m);
        if (v.isPointer && !right.isPointer) {
            v = movePointer(m, v, minus ? -(int)right.number : (int)right.number);
        } else if (!v.isPointer && !right.isPointer) {
            v.number = minus ? v.number - right.number : v.number + right.number;
            v.isFloat = v.isFloat || right.isFloat;
        } else {
            return fail(m);
        }
    }
    return v;
}

// The row a left-hand side means: name, name[i], *pointer, **pointer, *(pointer + n)
int lvalue(Machine *m) {
    if (next(m, "*")) {
        Value pointer = term(m);
        if (!pointer.ok || !pointer.isPointer || pointer.target < 0) return -1;
        return pointer.target;
    }
    if (next(m, "(")) {
        int row = lvalue(m);
        if (!next(m, ")")) return -1;
        return row;
    }
    return nameRow(m);
}

// Puts a value into a row (a pointer into a pointer row, a number into a number row)
void store(Machine *m, int row, Value v) {
    if (row < 0 || !v.ok) {
        m->ok = false;
        return;
    }
    Cell *cell = &m->cells[row];
    if ((cell->stars > 0) != v.isPointer) {
        m->ok = false;
        return;
    }
    if (v.isPointer) cell->target = v.target;
    else cell->number = cell->isFloat ? v.number : (double)(long)v.number;
    cell->isSet = true;
}

// int i = 5, j = 10;   int* ptr;   int **pptr = &ptr;   float a[4];   int arr[] = {4, 6, 7};
void declaration(Machine *m) {
    bool isFloat = strcmp(peek(m), "float") == 0 || strcmp(peek(m), "double") == 0;
    const char *typeName = peek(m);
    m->at++;
    do {
        int stars = 0;
        while (next(m, "*")) stars++;
        if (!isLetter(peek(m)[0]) || findSymbol(m, peek(m)) != nullptr) {
            m->ok = false;
            return;
        }
        char name[20];
        snprintf(name, sizeof(name), "%s", peek(m));
        m->at++;

        if (next(m, "[")) {
            // An array: its size, and maybe a list of starting values
            if (stars > 0) {
                m->ok = false;
                return;
            }
            int size = 0;
            if (isDigit(peek(m)[0])) {
                size = atoi(peek(m));
                m->at++;
            }
            if (!next(m, "]")) {
                m->ok = false;
                return;
            }
            double values[DIAGRAM_ROWS];
            int valueCount = 0;
            if (next(m, "=")) {
                if (!next(m, "{")) {
                    m->ok = false;
                    return;
                }
                while (m->ok && !next(m, "}")) {
                    Value v = expression(m);
                    if (!m->ok || v.isPointer || valueCount >= DIAGRAM_ROWS) {
                        m->ok = false;
                        return;
                    }
                    values[valueCount] = v.number;
                    valueCount++;
                    next(m, ",");
                }
            }
            if (size == 0) size = valueCount;
            if (size <= 0 || m->cellCount + size > DIAGRAM_ROWS || m->symbolCount >= DIAGRAM_ROWS) {
                m->ok = false;
                return;
            }
            Symbol *symbol = &m->symbols[m->symbolCount];
            m->symbolCount++;
            snprintf(symbol->name, sizeof(symbol->name), "%s", name);
            symbol->row = m->cellCount;
            symbol->isArray = true;
            symbol->size = size;
            for (int i = 0; i < size; i++) {
                Cell *cell = &m->cells[m->cellCount];
                m->cellCount++;
                snprintf(cell->name, sizeof(cell->name), "%s[%d]", name, i);
                snprintf(cell->type, sizeof(cell->type), "%s", typeName);
                cell->stars = 0;
                cell->isFloat = isFloat;
                cell->isSet = i < valueCount;
                cell->number = i < valueCount ? values[i] : 0;
                cell->target = -1;
                cell->arrayStart = symbol->row;
                cell->arraySize = size;
            }
        } else {
            // One variable (a number, a pointer or a pointer to a pointer)
            if (m->cellCount >= DIAGRAM_ROWS || m->symbolCount >= DIAGRAM_ROWS || stars > 2) {
                m->ok = false;
                return;
            }
            int row = m->cellCount;
            Cell *cell = &m->cells[row];
            m->cellCount++;
            snprintf(cell->name, sizeof(cell->name), "%s", name);
            const char *starText[3] = { "", " *", " **" };
            snprintf(cell->type, sizeof(cell->type), "%s%s", typeName, starText[stars]);
            cell->stars = stars;
            cell->isFloat = isFloat;
            cell->isSet = false;
            cell->number = 0;
            cell->target = -1;
            cell->arrayStart = -1;
            cell->arraySize = 0;
            Symbol *symbol = &m->symbols[m->symbolCount];
            m->symbolCount++;
            snprintf(symbol->name, sizeof(symbol->name), "%s", name);
            symbol->row = row;
            symbol->isArray = false;
            symbol->size = 1;
            if (next(m, "=")) store(m, row, expression(m));
        }
    } while (m->ok && next(m, ","));
    if (!next(m, ";")) m->ok = false;
}

void cellValueText(Cell rows[], Cell *cell, char *out, int size) {
    if (!cell->isSet) {
        snprintf(out, size, "?");
    } else if (cell->stars > 0) {
        if (cell->target < 0) snprintf(out, size, "nullptr");
        else snprintf(out, size, "address of %s", rows[cell->target].name);
    } else if (cell->isFloat) {
        snprintf(out, size, "%g", cell->number);
        if (strchr(out, '.') == nullptr) {
            int length = (int)strlen(out);
            snprintf(out + length, size - length, ".0");
        }
    } else {
        snprintf(out, size, "%d", (int)cell->number);
    }
}

// Adds something to what has been printed
void print(Machine *m, const char *text) {
    int length = (int)strlen(m->output);
    snprintf(m->output + length, sizeof(m->output) - length, "%s", text);
}

void printValue(Machine *m, Value v) {
    char text[40];
    if (v.isPointer) snprintf(text, sizeof(text), "(an address)");
    else if (v.isFloat) snprintf(text, sizeof(text), "%g", v.number);
    else snprintf(text, sizeof(text), "%d", (int)v.number);
    print(m, text);
}

// cout << a << " "; and printf("*ptr = %d\n", *ptr);
void output(Machine *m) {
    if (next(m, "cout")) {
        while (m->ok && next(m, "<<")) {
            const char *word = peek(m);
            if (word[0] == '"') {
                char text[40];
                snprintf(text, sizeof(text), "%s", word + 1);
                text[strlen(text) - 1] = '\0';
                print(m, text);
                m->at++;
            } else if (next(m, "endl")) {
                print(m, " ");
            } else {
                printValue(m, expression(m));
            }
        }
    } else if (next(m, "printf")) {
        if (!next(m, "(") || peek(m)[0] != '"') {
            m->ok = false;
            return;
        }
        char format[40];
        snprintf(format, sizeof(format), "%s", peek(m) + 1);
        format[strlen(format) - 1] = '\0';
        m->at++;
        for (char *f = format; *f != '\0'; f++) {
            if (*f == '%' && *(f + 1) != '\0') {
                f++;
                if (!next(m, ",")) {
                    m->ok = false;
                    return;
                }
                Value v = expression(m);
                if (*f == 'p') print(m, "(an address)");
                else printValue(m, v);
            } else if (*f == '\\' && *(f + 1) == 'n') {
                f++;
                print(m, "  ");
            } else {
                char one[2] = { *f, '\0' };
                print(m, one);
            }
        }
        if (!next(m, ")")) m->ok = false;
    }
    if (!next(m, ";")) m->ok = false;
}

// Runs one line. Returns true if it was a statement (a step in the diagram).
bool runLine(Machine *m, const char *line) {
    Token tokens[MAX_TOKENS];
    m->t = tokens;
    m->count = splitTokens(line, tokens);
    m->at = 0;
    if (m->count == 0) return false;                      // blank or just a comment
    if (m->count == 1 && (strcmp(tokens[0].text, "{") == 0 || strcmp(tokens[0].text, "}") == 0)) return false;

    const char *first = tokens[0].text;
    if (strcmp(first, "int") == 0 || strcmp(first, "float") == 0 || strcmp(first, "double") == 0) {
        declaration(m);
    } else if (strcmp(first, "cout") == 0 || strcmp(first, "printf") == 0) {
        output(m);
    } else {
        // x = ...;  x += n;  x -= n;  x++;  x--;
        int row = lvalue(m);
        if (row < 0) {
            m->ok = false;
            return true;
        }
        if (next(m, "=")) {
            store(m, row, expression(m));
        } else if (next(m, "+=") || next(m, "-=")) {
            bool minus = strcmp(tokens[m->at - 1].text, "-=") == 0;
            Value amount = expression(m);
            Value v = readRow(m, row);
            if (!m->ok || amount.isPointer) {
                m->ok = false;
                return true;
            }
            if (v.isPointer) v = movePointer(m, v, minus ? -(int)amount.number : (int)amount.number);
            else v.number = minus ? v.number - amount.number : v.number + amount.number;
            store(m, row, v);
        } else if (next(m, "++") || next(m, "--")) {
            bool minus = strcmp(tokens[m->at - 1].text, "--") == 0;
            Value v = readRow(m, row);
            if (v.isPointer) v = movePointer(m, v, minus ? -1 : 1);
            else v.number += minus ? -1 : 1;
            store(m, row, v);
        } else {
            m->ok = false;
        }
        if (m->ok && !next(m, ";")) m->ok = false;
    }
    if (m->at != m->count) m->ok = false;                 // something left over that wasn't understood
    return true;
}

bool buildDiagram(const char *code, Diagram *diagram) {
    static Machine m;   // static: it's quite big
    memset(&m, 0, sizeof(m));
    m.ok = true;
    diagram->ok = false;
    diagram->steps = 0;
    diagram->rowCount[0] = 0;
    diagram->output[0][0] = '\0';
    diagram->codeLine[0] = -1;

    const char *line = code;
    int lineNumber = 0;
    while (*line != '\0') {
        const char *end = strchr(line, '\n');
        int length = end ? (int)(end - line) : (int)strlen(line);
        char text[200];
        snprintf(text, sizeof(text), "%.*s", length, line);

        if (runLine(&m, text)) {
            if (!m.ok || diagram->steps >= DIAGRAM_STEPS) return false;
            diagram->steps++;
            int s = diagram->steps;
            diagram->codeLine[s] = lineNumber;
            diagram->rowCount[s] = m.cellCount;
            for (int r = 0; r < m.cellCount; r++) diagram->rows[s][r] = m.cells[r];
            snprintf(diagram->output[s], sizeof(diagram->output[s]), "%s", m.output);
        }
        if (end == nullptr) break;
        line = end + 1;
        lineNumber++;
    }
    diagram->ok = diagram->steps > 0;
    return diagram->ok;
}

// ============================================================
// Drawing
// ============================================================

const float ROW_HEIGHT = 22;

float diagramHeight(Diagram *diagram, const char *code) {
    float codeHeight = countLines(code) * lineHeight(14) + 14;
    int rows = diagram->rowCount[diagram->steps];
    return codeHeight + 16 + ROW_HEIGHT * (rows + 1) + 34;
}

void drawDiagram(Diagram *diagram, const char *code, int step, float x, float y, float width) {
    if (step < 0) step = 0;
    if (step > diagram->steps) step = diagram->steps;

    // The code, with the line that just ran lit up
    float codeHeight = countLines(code) * lineHeight(14) + 14;
    DrawRectangle((int)x, (int)y, (int)width, (int)codeHeight, COLOUR_CODE_BG);
    if (step > 0) {
        float lineY = y + 7 + diagram->codeLine[step] * lineHeight(14);
        DrawRectangle((int)x + 2, (int)lineY - 2, (int)width - 4, (int)lineHeight(14), Color{ 40, 90, 50, 255 });
    }
    drawCode(code, x + 10, y + 7, 14, width - 20);
    y += codeHeight + 16;

    // The data table, like the slides: Name, Type, Value
    float tableWidth = 330;
    if (tableWidth > width - 90) tableWidth = width - 90;
    drawText("Name", x + 10, y + 3, 14, COLOUR_MUTED);
    drawText("Type", x + 100, y + 3, 14, COLOUR_MUTED);
    drawText("Value", x + 180, y + 3, 14, COLOUR_MUTED);
    DrawLine((int)x, (int)(y + ROW_HEIGHT - 2), (int)(x + tableWidth), (int)(y + ROW_HEIGHT - 2), Color{ 70, 75, 90, 255 });
    float firstRowY = y + ROW_HEIGHT;

    Cell *rows = diagram->rows[step];
    Cell *before = diagram->rows[step - 1 > 0 ? step - 1 : 0];
    int beforeCount = step > 0 ? diagram->rowCount[step - 1] : 0;
    for (int r = 0; r < diagram->rowCount[step]; r++) {
        float rowY = firstRowY + r * ROW_HEIGHT;
        // Rows that this line changed are lit up
        bool changed = r >= beforeCount || rows[r].isSet != before[r].isSet ||
                       rows[r].number != before[r].number || rows[r].target != before[r].target;
        if (changed && step > 0) DrawRectangle((int)x, (int)rowY, (int)tableWidth, (int)ROW_HEIGHT - 2, Color{ 70, 62, 20, 255 });
        char value[40];
        cellValueText(rows, &rows[r], value, sizeof(value));
        drawText(rows[r].name, x + 10, rowY + 3, 14, COLOUR_TEXT);
        drawText(rows[r].type, x + 100, rowY + 3, 14, Color{ 78, 201, 176, 255 });
        drawText(value, x + 180, rowY + 3, 14, rows[r].stars > 0 ? COLOUR_ACCENT : COLOUR_TEXT);
    }

    // Arrows from each pointer to what it points at, each one a bit further out
    Color arrowColours[3] = { COLOUR_ACCENT, { 90, 170, 255, 255 }, { 255, 120, 160, 255 } };
    int arrowNumber = 0;
    for (int r = 0; r < diagram->rowCount[step]; r++) {
        if (rows[r].stars == 0 || !rows[r].isSet || rows[r].target < 0) continue;
        Color colour = arrowColours[arrowNumber % 3];
        float outX = x + tableWidth + 14 + arrowNumber * 14;
        float fromY = firstRowY + r * ROW_HEIGHT + ROW_HEIGHT / 2 - 1;
        float toY = firstRowY + rows[r].target * ROW_HEIGHT + ROW_HEIGHT / 2 - 1;
        float startX = x + tableWidth + 2;
        DrawLineEx(Vector2{ startX, fromY }, Vector2{ outX, fromY }, 2, colour);
        DrawLineEx(Vector2{ outX, fromY }, Vector2{ outX, toY }, 2, colour);
        DrawLineEx(Vector2{ outX, toY }, Vector2{ startX, toY }, 2, colour);
        DrawLineEx(Vector2{ startX, toY }, Vector2{ startX + 7, toY - 5 }, 2, colour);   // the arrow head
        DrawLineEx(Vector2{ startX, toY }, Vector2{ startX + 7, toY + 5 }, 2, colour);
        arrowNumber++;
    }

    // Anything printed so far
    float outputY = firstRowY + diagram->rowCount[step] * ROW_HEIGHT + 8;
    if (diagram->output[step][0] != '\0') {
        drawText(TextFormat("Prints: %s", diagram->output[step]), x + 10, outputY, 14, COLOUR_GOOD);
    }
}
