#ifndef APP_H
#define APP_H

#include "pool.h"
#include "editor.h"
#include "questions.h"
#include "practice.h"

// Set this to false to unlock the next concept after every correct answer,
// instead of after potting one of your balls (like spending crops in The Farmer Was Replaced).
const bool UNLOCK_BY_POTTING = true;

// Which screen is showing
enum Screen { SCREEN_MENU, SCREEN_PLAY, SCREEN_CODEX, SCREEN_PROGRESS, SCREEN_PRACTICE };

// What the game screen is doing right now
enum Phase {
    PHASE_WELCOME,     // "welcome back": offers a Rewind before carrying on
    PHASE_LESSON,      // reading about a concept
    PHASE_CODE,        // answering the question for this shot
    PHASE_RUNNING,     // watching the code run, line by line
    PHASE_AIM,         // aiming with the mouse
    PHASE_STRIKE,      // the cue stick hitting the ball
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

    // ---------- Players (each with their own saved progress) ----------
    Profile profiles[MAX_PROFILES];   // everyone who has played on this computer
    int profileCount;
    Profile *playerProfile[2];  // who Player 1 and Player 2 are (both can point at the same profile)
    Profile *learner;           // whose progress the questions use right now: the shooter, in a game

    // ---------- The current question ----------
    int questionLevel;          // the level being asked
    int version;                // which version of its task: 0 or 1
    bool reviewMode;            // every level is done: questions come from earlier levels
    bool callNeeded;            // does the player have to call a ball this shot?
    bool practising;            // Practice screen: questions without the pool table
    bool solved;                // Practice screen: this question has been answered
    int hintsShown;             // how many hints the player has asked for (0 to 4)
    float taskScroll;           // how far a long question is scrolled (mouse wheel)
    float viewTop, viewBottom;  // the visible part of the question area (for clicks)
    int chosen[6];              // ORDER tasks: the tiles clicked so far, in order
    int chosenCount;
    MadeUpQuestion madeUp;      // a new question in the lecturer's style (practice.cpp)
    bool usingMadeUp;           // true when the current question is the made-up one
    bool showSlide;             // the Slide window is open

    CodeBox answerBox;          // where the player types the answer
    CodeBox callBox;            // the call line: Ball *target = ...
    char feedback[200];         // what's wrong, or "Correct!"
    bool feedbackGood;

    // ---------- Shooting ----------
    float runTimer;             // how long the code has been "running"
    float power;                // shot power, 0 to 100
    bool charging;              // the mouse button is held down
    float strikeTimer;          // the cue stick moving forward
    float aimX, aimY;           // where the shot is aimed

    // ---------- Animation ----------
    float phaseTimer;           // time since the phase changed (panels fade in)
    float wrongTimer;           // > 0 after a wrong answer: the answer box shakes
    float bannerTimer;          // > 0 while an unlock banner is showing
    char bannerTitle[80];
    char bannerText[120];

    // ---------- Welcome back, and Rewind ----------
    Phase afterWelcome;         // where to go after the welcome panel
    bool showRewind;            // the Rewind list of levels is open
    bool rewinding;             // going over an earlier level in the middle of a game
    bool shotSaved;             // the shot's question is saved, to come back to
    int savedLevel, savedVersion;
    bool savedMadeUp, savedCallNeeded;
    Phase savedPhase;

    // ---------- Choosing players (main menu) ----------
    int pickingSlot;            // -1 = not choosing, 0 = Player 1, 1 = Player 2
    CodeBox nameBox;            // where a new player types their name
    int confirmDelete;          // which player's Delete was clicked once (-1 = none)

    // ---------- Other screens ----------
    int codexLevel;             // the level shown in the codex
    bool confirmReset;          // reset progress needs a second click
    bool confirmUnlockAll;      // so does unlocking everything
    DoublyLinkedNode *logView;  // the newest shot-log entry on screen (nullptr = the latest)
};

// Progress helpers (play.cpp)
Task *currentTask(App *app);   // the question being asked right now
int currentLevel(App *app);
bool reached(App *app, int level);

// The game screen and the practice screen (play.cpp)
void startNewGame(App *app);
void prepareNextShot(App *app);
void playScreen(App *app);
void startPractice(App *app, int level);
void practiceScreen(App *app);
float drawLessonContent(Level *level, float x, float y, float width);
void drawBanner(App *app);
void startRewind(App *app, int level);
void endRewind(App *app);
void saveShot(App *app);       // remembers the shot's question
void restoreShot(App *app);    // goes back to it
void drawSlideOverlay(App *app);   // the Slide window: what the slides say

#endif
