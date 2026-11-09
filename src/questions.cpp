#include "questions.h"
#include "lab2.h"
#include <cstdio>
#include <cstring>

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

// Words that stay in a layout (everything else that's a name or number becomes ___)
const char *layoutKeywords[] = { "int", "char", "bool", "float", "double", "void", "return", "if", "while",
                                 "for", "new", "delete", "nullptr", "NULL", "true", "false", "this", "Ball",
                                 "Node", "Point", "DoublyLinkedNode", "sizeof", "struct", "cout", "endl" };
const int LAYOUT_KEYWORD_COUNT = 25;

bool isNameChar(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '_';
}

// Turns an answer into its "shape" for the last hint:
//   ptr = &i;                          ->  ___ = &___;
//   int temp = *a; *a = *b; *b = temp; ->  int ___ = *___;  *___ = *___;  *___ = ___;
// Names and numbers become ___, but keywords, symbols and function names stay.
void makeLayout(const char *answer, char *out, int size) {
    int length = 0;
    int brackets = 0;   // inside ( ) a ; doesn't start a new line (like in a for loop)
    const char *p = answer;

    while (*p != '\0' && length < size - 6) {
        if (isNameChar(*p)) {
            // Read the whole word or number
            char word[60];
            int w = 0;
            while (isNameChar(*p) && w < 59) {
                word[w] = *p;
                w++;
                p++;
            }
            word[w] = '\0';

            // Keep it if it's a keyword, or a function name (followed by a bracket)
            const char *after = p;
            while (*after == ' ') after++;
            bool keep = *after == '(';
            for (int k = 0; k < LAYOUT_KEYWORD_COUNT; k++) {
                if (strcmp(word, layoutKeywords[k]) == 0) keep = true;   // strcmp gives 0 when they match
            }

            const char *piece = "___";
            if (keep) piece = word;
            while (*piece != '\0' && length < size - 6) {
                out[length] = *piece;
                length++;
                piece++;
            }
        } else if (*p == '\'') {
            // A char like 'a' or '\0' stays as it is
            do {
                out[length] = *p;
                length++;
                p++;
            } while (*p != '\0' && *p != '\'' && length < size - 6);
            if (*p == '\'') {
                out[length] = *p;
                length++;
                p++;
            }
        } else {
            if (*p == '(') brackets++;
            if (*p == ')') brackets--;
            out[length] = *p;
            length++;
            // Each statement goes on its own line
            if (*p == ';' && brackets == 0 && *(p + 1) == ' ') {
                out[length] = '\n';
                length++;
                p++;
            }
            p++;
        }
    }
    out[length] = '\0';
}

// Storage for the layouts made from the answers (two tasks per level)
char autoLayouts[LEVEL_COUNT + 1][2][300];

void setupLevels() {
    setupPointerLessons();
    setupMemoryLessons();
    setupListLessons();

    // Typed tasks without a hand-written layout get one made from their answer
    for (int n = 1; n <= LEVEL_COUNT; n++) {
        for (int v = 0; v < 2; v++) {
            Task *task = &levels[n].tasks[v];
            if (task->layout == nullptr && task->answers[0] != nullptr) {
                makeLayout(task->answers[0], autoLayouts[n][v], 300);
                task->layout = autoLayouts[n][v];
            }
        }
    }
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
// Players and their saved progress
// ============================================================
// profiles.txt lists the players (the first line says who Player 1 and
// Player 2 were last time), and each player's progress is in its own file.

// progress_Name.txt (anything that isn't a letter or digit becomes _)
void profileFileName(Profile *profile, char *out, int size) {
    char safe[NAME_LENGTH];
    int i = 0;
    for (const char *p = profile->name; *p != '\0' && i < NAME_LENGTH - 1; p++) {
        bool letterOrDigit = (*p >= 'a' && *p <= 'z') || (*p >= 'A' && *p <= 'Z') || (*p >= '0' && *p <= '9');
        if (letterOrDigit) safe[i] = *p;
        else safe[i] = '_';
        i++;
    }
    safe[i] = '\0';
    snprintf(out, size, "progress_%s.txt", safe);
}

void setupProfile(Profile *profile, const char *name) {
    snprintf(profile->name, NAME_LENGTH, "%s", name);
    profile->levelsDone = 0;
    for (int n = 0; n <= LEVEL_COUNT; n++) profile->stars[n] = 0;
    profile->lessonShown = false;
    profile->askCount = 0;
    profile->welcomed = false;
}

// Reads levels done (first line) and the stars for each level
void readProgressFile(const char *fileName, Profile *profile) {
    FILE *file = fopen(fileName, "r");
    if (file == nullptr) return;
    if (fscanf(file, "%d", &profile->levelsDone) != 1) profile->levelsDone = 0;
    for (int n = 1; n <= LEVEL_COUNT; n++) {
        if (fscanf(file, "%d", &profile->stars[n]) != 1) profile->stars[n] = 0;
    }
    fclose(file);
    if (profile->levelsDone < 0) profile->levelsDone = 0;
    if (profile->levelsDone > LEVEL_COUNT) profile->levelsDone = LEVEL_COUNT;
}

void saveProfile(Profile *profile) {
    char fileName[40];
    profileFileName(profile, fileName, sizeof(fileName));
    FILE *file = fopen(fileName, "w");
    if (file == nullptr) return;
    fprintf(file, "%d\n", profile->levelsDone);
    for (int n = 1; n <= LEVEL_COUNT; n++) {
        fprintf(file, "%d ", profile->stars[n]);
    }
    fprintf(file, "\n");
    fclose(file);
}

void deleteProfileFile(Profile *profile) {
    char fileName[40];
    profileFileName(profile, fileName, sizeof(fileName));
    remove(fileName);
}

void saveProfileList(Profile profiles[], int count, int slot1, int slot2) {
    FILE *file = fopen("profiles.txt", "w");
    if (file == nullptr) return;
    fprintf(file, "%d %d\n", slot1, slot2);
    for (int i = 0; i < count; i++) {
        fprintf(file, "%s\n", profiles[i].name);
    }
    fclose(file);
}

int loadProfiles(Profile profiles[], int *slot1, int *slot2) {
    *slot1 = 0;
    *slot2 = 0;
    int count = 0;

    FILE *file = fopen("profiles.txt", "r");
    if (file != nullptr) {
        if (fscanf(file, "%d %d\n", slot1, slot2) != 2) {
            *slot1 = 0;
            *slot2 = 0;
        }
        char line[64];
        while (count < MAX_PROFILES && fgets(line, sizeof(line), file) != nullptr) {
            // Remove the new line at the end
            char *end = line;
            while (*end != '\0' && *end != '\n' && *end != '\r') end++;
            *end = '\0';
            if (line[0] == '\0') continue;

            setupProfile(&profiles[count], line);
            char fileName[40];
            profileFileName(&profiles[count], fileName, sizeof(fileName));
            readProgressFile(fileName, &profiles[count]);
            count++;
        }
        fclose(file);
    } else {
        // An older version kept one progress.txt for everyone: keep it as a player called "Old save"
        FILE *old = fopen("progress.txt", "r");
        if (old != nullptr) {
            fclose(old);
            setupProfile(&profiles[0], "Old save");
            readProgressFile("progress.txt", &profiles[0]);
            saveProfile(&profiles[0]);
            count = 1;
            saveProfileList(profiles, count, 0, 0);
        }
    }

    if (*slot1 < 0 || *slot1 >= count) *slot1 = 0;
    if (*slot2 < 0 || *slot2 >= count) *slot2 = *slot1;
    return count;
}
