#include "guide.h"
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <cmath>

// ============================================================
// Guided help: clues that lead to the answer, one step at a time
// ============================================================

// The answer to lead to. A # (any ball number) is shown as N.
void guideAnswer(Task *task, char *out, int size) {
    int length = 0;
    for (const char *p = task->answers[0]; *p != '\0' && length < size - 1; p++) {
        out[length] = (*p == '#') ? 'N' : *p;
        length++;
    }
    out[length] = '\0';
}

// Puts each statement on its own line (not inside brackets, like a for loop)
void splitStatements(const char *text, char *out, int size) {
    int length = 0;
    int brackets = 0;
    for (const char *p = text; *p != '\0' && length < size - 2; p++) {
        if (*p == '(') brackets++;
        if (*p == ')') brackets--;
        out[length] = *p;
        length++;
        if (*p == ';' && brackets == 0 && *(p + 1) == ' ') {
            out[length] = '\n';
            length++;
            p++;
        }
    }
    out[length] = '\0';
}

int countStatements(const char *answer) {
    char lines[600];
    splitStatements(answer, lines, sizeof(lines));
    int count = 1;
    for (const char *p = lines; *p != '\0'; p++) {
        if (*p == '\n') count++;
    }
    return count;
}

// Is the text a whole number (maybe with a minus or a decimal point)?
bool isNumber(const char *text) {
    const char *p = text;
    if (*p == '-') p++;
    if (*p < '0' || *p > '9') return false;
    for (; *p != '\0'; p++) {
        if ((*p < '0' || *p > '9') && *p != '.') return false;
    }
    return true;
}

int countCodeLines(const char *code) {
    int lines = 1;
    for (const char *p = code; *p != '\0'; p++) {
        if (*p == '\n') lines++;
    }
    return lines;
}

bool isPieceChar(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '_';
}

int guideClueCount(Task *task) {
    if (task->kind == TASK_ORDER) return countStatements(task->answers[0]);
    return 4;
}

bool guideClueIsCode(Task *task, int step) {
    (void)step;
    return task->kind == TASK_TYPE || task->kind == TASK_ORDER;
}

// TYPE: the first part of the answer, with ___ for the rest.
// Step 1 shows about a quarter of it, step 4 the whole thing.
void typeClue(Task *task, int step, char *out, int size) {
    char answer[600];
    guideAnswer(task, answer, sizeof(answer));

    // Count the pieces: each word or number is one piece, and so is each symbol
    int pieces = 0;
    for (const char *p = answer; *p != '\0'; ) {
        if (*p == ' ') { p++; continue; }
        if (isPieceChar(*p)) { while (isPieceChar(*p)) p++; }
        else p++;
        pieces++;
    }
    int show = pieces * step / 4;          // a quarter more each time...
    if (show >= pieces) show = pieces - 1; // ...but only the last clue has all of it
    if (show < 1) show = 1;
    if (step >= 4) show = pieces;

    // Copy that many pieces (with their spaces), then ___
    char part[600];
    int length = 0;
    int shown = 0;
    const char *p = answer;
    while (*p != '\0' && shown < show && length < (int)sizeof(part) - 1) {
        if (*p == ' ') {
            part[length++] = *p++;
            continue;
        }
        if (isPieceChar(*p)) {
            while (isPieceChar(*p) && length < (int)sizeof(part) - 1) part[length++] = *p++;
        } else {
            part[length++] = *p++;
        }
        shown++;
    }
    part[length] = '\0';
    if (shown < pieces) snprintf(part + length, sizeof(part) - length, " ___");
    splitStatements(part, out, size);
}

// TRACE: what kind of answer, roughly where, how it starts, then the answer
void traceClue(Task *task, int step, char *out, int size) {
    const char *answer = task->answers[0];
    if (isNumber(answer)) {
        double value = atof(answer);
        if (step == 1) {
            snprintf(out, size, "The answer is a number%s. Go down the code one line at a time and write every variable's value after each line.",
                     value < 0 ? ", and it's below 0" : ", and it's 0 or more");
        } else if (step == 2) {
            int low = (int)floor(value / 10.0) * 10;   // the ten it's in: 23 -> 20 to 29, -2 -> -10 to -1
            snprintf(out, size, "It's between %d and %d.", low, low + 9);
        } else if (step == 3) {
            int first = 1;
            if (answer[0] == '-') first = 2;           // keep the minus with the first digit
            snprintf(out, size, "It's %d character%s long and starts with %.*s.", (int)strlen(answer),
                     strlen(answer) == 1 ? "" : "s", first, answer);
        } else {
            snprintf(out, size, "The answer is %s. Type it in to finish, then trace it through once more so it sticks.", answer);
        }
        return;
    }
    // A word or some text
    int length = (int)strlen(answer);
    if (step == 1) {
        snprintf(out, size, "The answer is a word or some text, %d character%s long.", length, length == 1 ? "" : "s");
    } else if (step == 2) {
        snprintf(out, size, "It starts with '%c'.", answer[0]);
    } else if (step == 3) {
        char half[100];
        int show = (length + 1) / 2;
        for (int i = 0; i < length && i < 99; i++) half[i] = (i < show) ? answer[i] : '_';
        half[length < 99 ? length : 99] = '\0';
        snprintf(out, size, "Half of it: %s", half);
    } else {
        snprintf(out, size, "The answer is %s. Type it in to finish.", answer);
    }
}

// BUG: half the code, then a few lines, then two, then the line
void bugClue(Task *task, const char *code, int step, char *out, int size) {
    int bad = atoi(task->answers[0]);
    int lines = countCodeLines(code);
    if (step == 1) {
        int middle = lines / 2;
        if (bad <= middle) snprintf(out, size, "The wrong line is in the top half: lines 1 to %d.", middle);
        else snprintf(out, size, "The wrong line is in the bottom half: lines %d to %d.", middle + 1, lines);
    } else if (step == 2) {
        int first = bad - 1;
        if (first < 1) first = 1;
        if (first + 2 > lines) first = lines - 2;
        if (first < 1) first = 1;
        snprintf(out, size, "It's one of lines %d, %d or %d.", first, first + 1, first + 2);
    } else if (step == 3) {
        int other = (bad % 2 == 0) ? bad - 1 : bad + 1;
        if (other > lines) other = bad - 1;
        if (other < 1) other = bad + 1;
        int a = other < bad ? other : bad;
        int b = other < bad ? bad : other;
        snprintf(out, size, "It's line %d or line %d. Ask: does the memory this line uses still exist?", a, b);
    } else {
        snprintf(out, size, "It's line %d. Type it in to finish, then read that line again and work out why.", bad);
    }
}

// ORDER: the right lines, one more each time
void orderClue(Task *task, int step, char *out, int size) {
    char lines[600];
    splitStatements(task->answers[0], lines, sizeof(lines));
    int length = 0;
    int line = 1;
    for (const char *p = lines; *p != '\0' && length < size - 6; p++) {
        if (*p == '\n') {
            if (line == step) break;
            line++;
        }
        out[length++] = *p;
    }
    out[length] = '\0';
    if (step < countStatements(task->answers[0])) snprintf(out + length, size - length, "\n...");
}

void guideClue(Task *task, const char *code, int step, char *out, int size) {
    if (task->kind == TASK_TYPE) typeClue(task, step, out, size);
    else if (task->kind == TASK_TRACE) traceClue(task, step, out, size);
    else if (task->kind == TASK_BUG) bugClue(task, code, step, out, size);
    else orderClue(task, step, out, size);
}

// ============================================================
// How close is a wrong answer?
// ============================================================

// Spaces don't count, like in the checker
void withoutSpaces(const char *text, char *out, int size) {
    int length = 0;
    for (; *text != '\0' && length < size - 1; text++) {
        if (*text != ' ' && *text != '\t' && *text != '\n') out[length++] = *text;
    }
    out[length] = '\0';
}

// How many characters of typed match the start of answer (a # matches any number)
int matchingStart(const char *typed, const char *answer) {
    int t = 0;
    int a = 0;
    while (typed[t] != '\0' && answer[a] != '\0') {
        if (answer[a] == '#') {
            if (typed[t] < '0' || typed[t] > '9') break;
            while (typed[t] >= '0' && typed[t] <= '9') t++;
            a++;
        } else if (typed[t] == answer[a]) {
            t++;
            a++;
        } else {
            break;
        }
    }
    return t;
}

void guideProgress(Task *task, const char *typed, char *out, int size) {
    char clean[600];
    withoutSpaces(typed, clean, sizeof(clean));
    out[0] = '\0';
    if (clean[0] == '\0') return;

    // Numbers in trace and bug questions: too high or too low
    const char *answer = task->answers[0];
    if ((task->kind == TASK_TRACE || task->kind == TASK_BUG) && isNumber(answer) && isNumber(clean)) {
        double mine = atof(clean);
        double right = atof(answer);
        if (task->kind == TASK_BUG) {
            snprintf(out, size, "Guide: the wrong line is further %s.", mine < right ? "down" : "up");
        } else {
            snprintf(out, size, "Guide: your answer is too %s.", mine < right ? "low" : "high");
        }
        return;
    }

    // Otherwise: how much of it is right, using whichever accepted answer it's closest to
    int best = 0;
    int bestLength = 0;
    for (int i = 0; i < 8 && task->answers[i] != nullptr; i++) {
        char right[600];
        withoutSpaces(task->answers[i], right, sizeof(right));
        int matched = matchingStart(clean, right);
        if (matched > best) {
            best = matched;
            bestLength = (int)strlen(right);
        }
    }
    int typedLength = (int)strlen(clean);
    if (best == 0) {
        snprintf(out, size, "Guide: the very first part is already off. Compare the start of your answer with the clue.");
    } else if (best == typedLength && best < bestLength) {
        snprintf(out, size, "Guide: everything you've typed is right so far - it just isn't finished.");
    } else if (best < typedLength) {
        // Show the right part as they typed it (with their spaces)
        char shown[300];
        int length = 0;
        int counted = 0;
        for (const char *p = typed; *p != '\0' && counted < best && length < (int)sizeof(shown) - 1; p++) {
            shown[length++] = (*p == '\n') ? ' ' : *p;
            if (*p != ' ' && *p != '\t' && *p != '\n') counted++;
        }
        shown[length] = '\0';
        if (best >= bestLength) snprintf(out, size, "Guide: you have the whole answer, plus something extra after: %s", shown);
        else snprintf(out, size, "Guide: right up to  %s  - it goes wrong just after that.", shown);
    }
}
