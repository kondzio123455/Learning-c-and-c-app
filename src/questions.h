#ifndef QUESTIONS_H
#define QUESTIONS_H

const int LEVEL_COUNT = 29;
const int CHAPTER_COUNT = 6;
const int CALL_LEVEL = 3;     // "Call your ball": after this you call a ball before every shot
const int LOCK_LEVEL = 14;    // pointer arithmetic: [ ] is locked from here on (Lab 2 rule)

// A common wrong answer, with a hint that explains what went wrong
struct Mistake {
    const char *answer;
    const char *hint;
};

// One version of a level's task
struct Task {
    const char *task;          // what to type, in plain English
    const char *code;          // the code around the gap; ____ marks where your code goes
    const char *answers[6];    // accepted answers (spaces don't matter, # means any number)
    Mistake mistakes[4];       // common wrong answers, each with its own hint
    const char *hint;          // shown when nothing else matches
    int power;                 // Level 2 only: the shot power this code sets
};

struct Level {
    int chapter;
    const char *title;
    const char *shortName;     // for the progress tree
    const char *source;        // where this comes from in the notes and labs
    const char *explanation;
    const char *example;
    const char *inGame;        // how the game uses it
    Task tasks[2];             // two versions, so repeats aren't identical
};

// The result of checking an answer
struct CheckResult {
    bool correct;
    const char *hint;          // what to show when it's wrong
    int number;                // the number typed where an answer has a #
};

void setupLevels();                      // in levels.cpp: all the lesson text
Level *getLevel(int number);
Task *getCallTask(bool afterLock);       // the call line's answers
const char *chapterName(int chapter);
CheckResult checkAnswer(Task *task, const char *typed, bool lockOn);
CheckResult checkCallLine(const char *typed, bool lockOn);
int loadProgress();
void saveProgress(int levelsDone);

#endif
