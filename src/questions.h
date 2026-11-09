#ifndef QUESTIONS_H
#define QUESTIONS_H

// ---------- The levels ----------
const int LEVEL_COUNT = 54;
const int CHAPTER_COUNT = 8;

const int CALL_LEVEL = 3;        // "Call your ball": after this you call a ball before every shot
const int STEP_LEVEL = 13;       // pointer steps: from here the call line uses balls + N

// Upgrades you unlock by finishing chapters (like The Farmer Was Replaced)
const int AIM_GUIDE_LEVEL = 8;   // finish Chapter 1: aim guide (ghost ball)
const int BALL_PATH_LEVEL = 17;  // finish Chapter 3: see where the object ball will go
const int GOLD_CUE_LEVEL = 46;   // finish Chapter 7: golden cue

// The four kinds of task
enum TaskKind {
    TASK_TYPE,     // type the missing code
    TASK_TRACE,    // work out what a variable holds (like the data tables in the lecture)
    TASK_BUG,      // find the line that's wrong (type its number)
    TASK_ORDER     // click the lines in the right order
};

// A common wrong answer, with an explanation of what went wrong (not the fix)
struct Mistake {
    const char *answer;
    const char *why;
};

// One version of a level's task
struct Task {
    TaskKind kind;
    const char *task;          // what to do, in plain English
    const char *code;          // the code; ____ marks the gap (TYPE and ORDER)
    const char *answers[8];    // accepted answers (spaces don't matter, # means any number)
    Mistake mistakes[4];
    const char *hints[3];      // shown one at a time, only when the player asks
    const char *layout;        // the last hint: the shape of the answer, or a partly done trace
    const char *tiles[6];      // ORDER only: the lines to choose from (some are wrong)
    const char *note;          // made-up practice questions: a learner note shown above the task
};

struct Level {
    int chapter;
    const char *title;
    const char *shortName;     // for the progress tree
    const char *source;        // where this comes from in the notes and labs
    const char *explanation;
    const char *example;
    const char *inGame;        // how the game uses it
    const char *slideTitle;    // which slides the Slide button shows
    const char *slideText;     // what those slides say (so you don't have to open the PDF)
    bool noIndexing;           // Lab 2 rule: [ ] is not allowed in answers
    Task tasks[2];             // two versions, so repeats aren't identical
};

// The result of checking an answer
struct CheckResult {
    bool correct;
    const char *feedback;      // why it's wrong (never the answer itself)
    int number;                // the number typed where an answer has a #
};

// ---------- Writing lessons (used by the lesson files) ----------
Level *startLevel(int number, int chapter, const char *title, const char *shortName, const char *source);
Task *startTask(Level *level, int version, TaskKind kind, const char *task, const char *code);
void addAnswer(Task *task, const char *answer);
void addMistake(Task *task, const char *answer, const char *why);
void setHints(Task *task, const char *hint1, const char *hint2, const char *hint3);
void addTile(Task *task, const char *line);
void makeLayout(const char *answer, char *out, int size);   // turns an answer into its shape

void setupLevels();
void setupPointerLessons();      // lessons_pointers.cpp: Chapters 1 to 4
void setupMemoryLessons();       // lessons_memory.cpp:   Chapters 5 and 6
void setupListLessons();         // lessons_lists.cpp:    Chapters 7 and 8

Level *getLevel(int number);
Task *getCallTask(bool afterStep);
const char *chapterName(int chapter);
int lastLevelOfChapter(int chapter);

// ---------- Checking ----------
CheckResult checkAnswer(Task *task, const char *typed, bool lockOn);
CheckResult checkCallLine(const char *typed, bool afterStep);

// ---------- Players and their saved progress ----------
// Everyone who plays on this computer gets their own profile and save file
// (progress_Name.txt), like the save slots in The Farmer Was Replaced.
const int MAX_PROFILES = 8;
const int NAME_LENGTH = 13;      // names can be up to 12 letters

struct Profile {
    char name[NAME_LENGTH];
    int levelsDone;
    int stars[LEVEL_COUNT + 1];  // best stars for each level: 3 = no hints used
    // These only last while the game is open:
    bool lessonShown;            // has the current level's lesson been shown?
    int askCount;                // how many times the current level has been asked
    bool welcomed;               // has the "welcome back" message been shown?
};

void setupProfile(Profile *profile, const char *name);   // a new player, at Level 1
void saveProfile(Profile *profile);                      // writes progress_Name.txt
void deleteProfileFile(Profile *profile);
int loadProfiles(Profile profiles[], int *slot1, int *slot2);            // returns how many players
void saveProfileList(Profile profiles[], int count, int slot1, int slot2);

#endif
