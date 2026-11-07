#include "questions.h"
#include "lab2.h"
#include <cstdio>

// levels[1] to levels[29]. levels[0] is not used, so level n is levels[n].
Level levels[LEVEL_COUNT + 1];

// The call line has its own accepted answers: one set before [ ] is locked, one after
Task callBeforeLock;
Task callAfterLock;

const char *chapterNames[CHAPTER_COUNT + 1] = {
    "",
    "Pointer basics",
    "Swaps",
    "Arrays (Lab 1)",
    "Pointer arithmetic",
    "Strings with pointers",
    "Memory and linked lists",
};

Level *getLevel(int number) {
    return &levels[number];
}

Task *getCallTask(bool afterLock) {
    if (afterLock) return &callAfterLock;
    return &callBeforeLock;
}

const char *chapterName(int chapter) {
    return chapterNames[chapter];
}

// ============================================================
// Checking answers
// ============================================================
// 1. Spaces, tabs and new lines are removed from the answer, so
//    "* b = temp ;" and "*b=temp;" count as the same thing.
// 2. It's compared with each accepted answer (spaces removed too).
//    A # in an accepted answer matches any whole number.
// 3. No match? Check the common mistakes, then a missing ; or {,
//    and if nothing fits, show the task's general hint.

const int MAX_ANSWER = 400;

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
            // Read the digits the player typed here
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
    for (int i = 0; i < 6 && task->answers[i] != nullptr; i++) {
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
    result.hint = "";
    result.number = -1;

    char clean[MAX_ANSWER];
    removeSpaces(typed, clean);

    if (clean[0] == '\0') {
        result.hint = "Type your answer in the box first, then click Run.";
        return result;
    }
    if (lockOn && usesIndexing(clean)) {
        result.hint = "Lab 2 rule: no [ ] from here on. Use pointer arithmetic instead, like *(arr + i) or arr + i.";
        return result;
    }

    // 1. One of the accepted answers?
    if (matchesAnAnswer(task, clean, &result.number)) {
        result.correct = true;
        return result;
    }

    // 2. A common mistake with its own hint?
    for (int i = 0; i < 4 && task->mistakes[i].answer != nullptr; i++) {
        int unused;
        if (matchesText(clean, task->mistakes[i].answer, &unused)) {
            result.hint = task->mistakes[i].hint;
            return result;
        }
    }

    // 3. Only missing a ; or a { at the end?
    if (nearlyRight(task, clean, ';')) {
        result.hint = "Nearly! You're missing a ; at the end.";
        return result;
    }
    if (nearlyRight(task, clean, '{')) {
        result.hint = "Nearly! End the line with { (the closing } is already there).";
        return result;
    }

    // 4. Otherwise, the general hint for this task
    result.hint = task->hint;
    return result;
}

CheckResult checkCallLine(const char *typed, bool lockOn) {
    char clean[MAX_ANSWER];
    removeSpaces(typed, clean);

    // Nothing typed in the call box yet
    if (clean[0] == '\0') {
        CheckResult result;
        result.correct = false;
        result.number = -1;
        if (lockOn) result.hint = "Now call your ball in the second box, like Ball *target = balls + 3;";
        else result.hint = "Now call your ball in the second box, like Ball *target = &balls[3];";
        return result;
    }

    if (!lockOn) return checkAnswer(&callBeforeLock, typed, false);

    if (usesIndexing(clean)) {
        CheckResult result;
        result.correct = false;
        result.number = -1;
        result.hint = "[ ] is locked now (Lab 2 rule). Call your ball with pointer arithmetic: Ball *target = balls + 3;";
        return result;
    }
    return checkAnswer(&callAfterLock, typed, false);
}

// ============================================================
// Saving progress: one number in progress.txt
// ============================================================

int loadProgress() {
    int levelsDone = 0;
    FILE *file = fopen("progress.txt", "r");
    if (file != nullptr) {
        if (fscanf(file, "%d", &levelsDone) != 1) levelsDone = 0;
        fclose(file);
    }
    if (levelsDone < 0) levelsDone = 0;
    if (levelsDone > LEVEL_COUNT) levelsDone = LEVEL_COUNT;
    return levelsDone;
}

void saveProgress(int levelsDone) {
    FILE *file = fopen("progress.txt", "w");
    if (file != nullptr) {
        fprintf(file, "%d\n", levelsDone);
        fclose(file);
    }
}
