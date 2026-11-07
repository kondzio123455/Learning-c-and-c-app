#ifndef APP_H
#define APP_H

#include "pool.h"
#include "editor.h"
#include "questions.h"

// Set this to false to unlock the next concept after every correct answer,
// instead of after potting one of your balls (like spending crops in The Farmer Was Replaced).
const bool UNLOCK_BY_POTTING = true;

// Which screen is showing
enum Screen { SCREEN_MENU, SCREEN_PLAY, SCREEN_CODEX, SCREEN_PROGRESS };

// What the game screen is doing right now
enum Phase {
    PHASE_LESSON,      // reading about a concept
    PHASE_CODE,        // typing the code for this shot
    PHASE_RUNNING,     // watching the code run, line by line
    PHASE_AIM,         // aiming and shooting with the mouse
    PHASE_ROLLING,     // the balls are moving
    PHASE_GAME_OVER    // someone has won
};

// Everything the app needs to remember, in one struct.
// Functions get a pointer to it (App *app) so they can change it.
struct App {
    Screen screen;
    Phase phase;
    bool quit;

    Game game;
    bool gameInProgress;        // a game has started and isn't over yet

    int levelsDone;             // how many levels are finished (saved in progress.txt)
    bool lessonShown;           // has the current level's lesson been shown yet?
    int askCount;               // how many times the current level has been asked
    int questionLevel;          // the level being asked for this shot
    int version;                // which version of its task: 0 or 1
    bool reviewMode;            // every level is done: questions come from earlier levels
    bool callNeeded;            // does the player have to call a ball this shot?

    CodeBox answerBox;          // where the player types the answer
    CodeBox callBox;            // the call line: Ball *target = ...
    char feedback[200];         // hint or "Correct!" message
    bool feedbackGood;

    float runTimer;             // how long the code has been "running"
    float power;                // shot power, 0 to 100
    bool powerFromCode;         // Level 2: the code set the power
    bool charging;              // the mouse button is held down

    int codexLevel;             // the level shown in the codex
    bool confirmReset;          // reset progress needs a second click
    DoublyLinkedNode *logView;  // the newest shot-log entry on screen (nullptr = the latest)
};

// Progress helpers (play.cpp)
int currentLevel(App *app);
bool reached(App *app, int level);

// The game screen (play.cpp)
void startNewGame(App *app);
void prepareNextShot(App *app);
void playScreen(App *app);
float drawLessonContent(Level *level, float x, float y, float width);

#endif
