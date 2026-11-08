#include "questions.h"
#include "lab2.h"
#include <cstdio>

// levels[1] to levels[54]. levels[0] is not used, so level n is levels[n].
Level levels[LEVEL_COUNT + 1];

// The call line has its own answers: before the pointer-steps level, and after it
Task callBeforeStep;
Task callAfterStep;

const char *chapterNames[CHAPTER_COUNT + 1] = {
    "",
    "Pointer basics",
    "Lab 2: swaps",
    "Pointer arithmetic",
    "Lab 2: strings",
    "Memory management",
    "Pointers and classes",
    "Linked lists",
    "Lab 1 review",
};

Level *getLevel(int number) {
    return &levels[number];
}

Task *getCallTask(bool afterStep) {
    if (afterStep) return &callAfterStep;
    return &callBeforeStep;
}

const char *chapterName(int chapter) {
    return chapterNames[chapter];
}

int lastLevelOfChapter(int chapter) {
    int last = 0;
    for (int n = 1; n <= LEVEL_COUNT; n++) {
        if (levels[n].chapter == chapter) last = n;
    }
    return last;
}

// ============================================================
// Helpers for writing lessons
// ============================================================

Level *startLevel(int number, int chapter, const char *title, const char *shortName, const char *source) {
    Level *level = &levels[number];
    level->chapter = chapter;
    level->title = title;
    level->shortName = shortName;
    level->source = source;
    level->noIndexing = false;
    return level;
}

Task *startTask(Level *level, int version, TaskKind kind, const char *task, const char *code) {
    Task *t = &level->tasks[version];
    t->kind = kind;
    t->task = task;
    t->code = code;
    return t;
}

void addAnswer(Task *task, const char *answer) {
    for (int i = 0; i < 8; i++) {
        if (task->answers[i] == nullptr) {
            task->answers[i] = answer;
            return;
        }
    }
}

void addMistake(Task *task, const char *answer, const char *why) {
    for (int i = 0; i < 4; i++) {
        if (task->mistakes[i].answer == nullptr) {
            task->mistakes[i].answer = answer;
            task->mistakes[i].why = why;
            return;
        }
    }
}

void setHints(Task *task, const char *hint1, const char *hint2, const char *hint3) {
    task->hints[0] = hint1;
    task->hints[1] = hint2;
    task->hints[2] = hint3;
}

void addTile(Task *task, const char *line) {
    for (int i = 0; i < 6; i++) {
        if (task->tiles[i] == nullptr) {
            task->tiles[i] = line;
            return;
        }
    }
}

void setupLevels() {
    setupPointerLessons();
    setupMemoryLessons();
    setupListLessons();
}

// ============================================================
// Checking answers
// ============================================================
// 1. Spaces, tabs and new lines are removed from the answer, so
//    "* b = temp ;" and "*b=temp;" count as the same thing.
// 2. It's compared with each accepted answer (spaces removed too),
//    using Lab 2's compare_strings. A # in an accepted answer
//    matches any whole number.
// 3. No match? If it's a common mistake, explain what went wrong.
//    Otherwise give a general nudge. The answer is never shown:
//    the player asks for hints one at a time instead.

const int MAX_ANSWER = 500;

// Copies text into result, leaving out spaces, tabs and new lines
void removeSpaces(const char *text, char *result) {
    int length = 0;
    while (*text != '\0' && length < MAX_ANSWER - 2) {
        if (*text != ' ' && *text != '\t' && *text != '\n') {
            *result = *text;
            result++;
            length++;
        }
        text++;
    }
    *result = '\0';
}

bool contains(const char *text, char c) {
    while (*text != '\0') {
        if (*text == c) return true;
        text++;
    }
    return false;
}

// Does the answer use array indexing like arr[i]?
// Empty brackets are fine, because delete[] isn't indexing.
bool usesIndexing(const char *text) {
    while (*text != '\0') {
        if (*text == '[' && *(text + 1) != ']') return true;
        text++;
    }
    return false;
}

// Does the typed answer (spaces already removed) match this answer?
// A '#' in the answer matches a whole number, which is stored in *number.
bool matchesText(char *typed, const char *answer, int *number) {
    char clean[MAX_ANSWER];
    removeSpaces(answer, clean);

    // No '#': a plain comparison, using Lab 2's compare_strings
    if (!contains(clean, '#')) {
        return compare_strings(typed, clean) == 0;
    }

    // With a '#': walk along both strings together
    char *a = clean;
    while (*a != '\0') {
        if (*a == '#') {
            if (*typed < '0' || *typed > '9') return false;
            int value = 0;
            while (*typed >= '0' && *typed <= '9') {
                if (value < 10000) value = value * 10 + (*typed - '0');
                typed++;
            }
            *number = value;
            a++;
        } else {
            if (*typed != *a) return false;
            typed++;
            a++;
        }
    }
    return *typed == '\0';   // both must end at the same time
}

bool matchesAnAnswer(Task *task, char *typed, int *number) {
    for (int i = 0; i < 8 && task->answers[i] != nullptr; i++) {
        if (matchesText(typed, task->answers[i], number)) return true;
    }
    return false;
}

// Would the answer be right with one more character on the end (like ; or {)?
bool nearlyRight(Task *task, char *typed, char missing) {
    char longer[MAX_ANSWER + 2];
    copy_string(longer, typed);
    char extra[2] = { missing, '\0' };
    concat_strings(longer, extra);
    int unused;
    return matchesAnAnswer(task, longer, &unused);
}

CheckResult checkAnswer(Task *task, const char *typed, bool lockOn) {
    CheckResult result;
    result.correct = false;
    result.feedback = "";
    result.number = -1;

    char clean[MAX_ANSWER];
    removeSpaces(typed, clean);

    if (clean[0] == '\0') {
        if (task->kind == TASK_ORDER) result.feedback = "Click the lines in the order they should run.";
        else result.feedback = "Type your answer in the box first.";
        return result;
    }
    bool codeTask = task->kind == TASK_TYPE || task->kind == TASK_ORDER;
    if (lockOn && codeTask && usesIndexing(clean)) {
        result.feedback = "Lab 2 rule: no [ ] here. Use pointer arithmetic instead.";
        return result;
    }

    // 1. One of the accepted answers?
    if (matchesAnAnswer(task, clean, &result.number)) {
        result.correct = true;
        result.feedback = "Correct!";
        return result;
    }

    // 2. A common mistake? Explain what went wrong.
    for (int i = 0; i < 4 && task->mistakes[i].answer != nullptr; i++) {
        int unused;
        if (matchesText(clean, task->mistakes[i].answer, &unused)) {
            result.feedback = task->mistakes[i].why;
            return result;
        }
    }

    // 3. Only missing a ; or a { at the end?
    if (task->kind == TASK_TYPE) {
        if (nearlyRight(task, clean, ';')) {
            result.feedback = "Nearly! You're missing a ; at the end.";
            return result;
        }
        if (nearlyRight(task, clean, '{')) {
            result.feedback = "Nearly! End the line with { (the closing } is already there).";
            return result;
        }
    }

    // 4. A general nudge for each kind of task
    if (task->kind == TASK_TRACE) {
        result.feedback = "Not quite. Go line by line and write down every variable's value after each line, like the data tables in the lecture.";
    } else if (task->kind == TASK_BUG) {
        result.feedback = "Not that one. Read each line and ask: what does each pointer point at right now, and does that memory still exist?";
    } else if (task->kind == TASK_ORDER) {
        result.feedback = "Not quite. For each line, ask what has to happen before it can run.";
    } else {
        result.feedback = "Not quite. Check your line against the lesson, or click Hint for a clue.";
    }
    return result;
}

CheckResult checkCallLine(const char *typed, bool afterStep) {
    char clean[MAX_ANSWER];
    removeSpaces(typed, clean);

    CheckResult result;
    result.correct = false;
    result.number = -1;

    if (clean[0] == '\0') {
        result.feedback = "Now call your ball in the second box: point target at the ball you want.";
        return result;
    }
    if (!afterStep) return checkAnswer(&callBeforeStep, typed, false);

    if (usesIndexing(clean)) {
        result.feedback = "[ ] is locked now (Lab 2 rule). Point target at your ball with pointer arithmetic.";
        return result;
    }
    return checkAnswer(&callAfterStep, typed, false);
}

// ============================================================
// Saving progress: levels done on the first line, then the stars for each level
// ============================================================

void loadProgress(int *levelsDone, int stars[]) {
    *levelsDone = 0;
    for (int n = 0; n <= LEVEL_COUNT; n++) stars[n] = 0;

    FILE *file = fopen("progress.txt", "r");
    if (file == nullptr) return;

    if (fscanf(file, "%d", levelsDone) != 1) *levelsDone = 0;
    for (int n = 1; n <= LEVEL_COUNT; n++) {
        if (fscanf(file, "%d", &stars[n]) != 1) stars[n] = 0;
    }
    fclose(file);

    if (*levelsDone < 0) *levelsDone = 0;
    if (*levelsDone > LEVEL_COUNT) *levelsDone = LEVEL_COUNT;
}

void saveProgress(int levelsDone, int stars[]) {
    FILE *file = fopen("progress.txt", "w");
    if (file == nullptr) return;

    fprintf(file, "%d\n", levelsDone);
    for (int n = 1; n <= LEVEL_COUNT; n++) {
        fprintf(file, "%d ", stars[n]);
    }
    fprintf(file, "\n");
    fclose(file);
}
