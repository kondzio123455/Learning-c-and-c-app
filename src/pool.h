#ifndef POOL_H
#define POOL_H

#include "raylib.h"
#include "lab2.h"   // the shot log is a DoublyLinkedList from Lab 2

// ---------- Screen layout ----------
const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;
const int PANEL_X = 750;             // the code panel on the right starts here

// The green felt the balls roll on
const int FELT_LEFT = 60;
const int FELT_TOP = 100;
const int FELT_WIDTH = 640;
const int FELT_HEIGHT = 320;
const int RAIL = 30;                 // wooden rail around the felt

const float CUE_START_X = FELT_LEFT + FELT_WIDTH / 4;     // where the cue ball starts
const float CUE_START_Y = FELT_TOP + FELT_HEIGHT / 2;
const float RACK_X = FELT_LEFT + FELT_WIDTH * 0.7f;       // front ball of the rack
const float RACK_Y = FELT_TOP + FELT_HEIGHT / 2;

// ---------- Balls and physics ----------
const int BALL_COUNT = 16;           // cue ball (0) and balls 1 to 15
const int RACK_SIZE = 15;
const float BALL_RADIUS = 11;
const float POCKET_RADIUS = 20;
const float MAX_SPEED = 1600;        // speed of a 100% power shot, in pixels per second
const float FRICTION = 0.988f;       // each frame, speed is multiplied by this
const float STOP_SPEED = 14;         // slower than this and a ball stops
const float CUSHION_BOUNCE = 0.75f;  // a cushion keeps 75% of the speed
const int PHYSICS_STEPS = 8;         // small steps per frame so balls don't pass through each other

// Pretend address of balls[0], like the 1000, 1001... in the Pointers lecture.
// Ball n is at BASE_ADDRESS + n * sizeof(Ball).
const int BASE_ADDRESS = 1000;

// sizeof(Ball) is 24 bytes: an int (4) + 4 floats (16) + a bool (1), rounded up to 24.
// Level 14 uses that number, so keep it in mind if you add fields.
struct Ball {
    int number;      // 0 is the cue ball, 1 to 15 are the object balls
    float x, y;      // position on the screen
    float vx, vy;    // velocity in pixels per second
    bool pocketed;   // true once it has gone down a pocket
};

struct Player {
    char name[20];
    char group;      // 'S' = solids (1-7), 'T' = stripes (9-15)
    int score;       // how many of your own balls you have potted
    int tray[8];     // the numbers of your potted balls, in order
    int trayCount;
};

struct Game {
    Ball balls[BALL_COUNT];
    Player players[2];
    int currentPlayer;        // the player shooting (0 or 1)
    int waitingPlayer;        // the other one -- swap_ints switches these two
    bool tableOpen;           // true until the groups are decided
    bool isBreak;             // true for the first shot of the game
    int calledBall;           // the ball target points at this shot, or -1
    int potsSinceCall[2];     // each player's own balls potted since they last called one
    Ball *lastPotted;         // the last object ball potted, or nullptr
    int pottedThisShot[BALL_COUNT];   // object balls that went down this shot
    int pottedThisShotCount;
    int ownLeftBeforeShot;    // how many of the shooter's group were left before the shot
    int shotNumber;
    bool gameOver;
    int winner;               // index of the winning player
    char winMessage[60];      // built with copy_string and concat_strings
    char reason[100];         // why the game ended
    char status[100];         // what happened on the last shot
    DoublyLinkedList shotLog; // one node per shot (Lab 2 Part 2)

    // Animation only (kept out of Ball so a Ball stays 24 bytes)
    float rollAngle[BALL_COUNT];          // how far each ball has rolled, so its number turns
    float rollDirX[BALL_COUNT], rollDirY[BALL_COUNT];
    float sinkTimer[BALL_COUNT];          // > 0 while a ball is dropping into a pocket
    float sinkFromX[BALL_COUNT], sinkFromY[BALL_COUNT];
    float sinkToX[BALL_COUNT], sinkToY[BALL_COUNT];
};

// ---------- Setting up and rules ----------
void newGame(Game *game);
void addPoint(int *score);                           // Level 4
int addressOf(int ballNumber);
char groupOf(int ballNumber);                        // 'S', 'T', '8', or 'C' for the cue ball
const char *groupName(char group);                   // "solids" or "stripes"
int ballsLeft(Game *game, char group);
int highestBallLeft(Game *game);
bool callIsAllowed(Game *game, int ballNumber, char *why, int whySize);

// ---------- Shooting and physics ----------
void startShot(Game *game, float aimX, float aimY, float power);
void updateBalls(Game *game, float dt);
bool ballsMoving(Game *game);
bool endOfShot(Game *game);                          // applies the 8-ball rules

// ---------- Drawing ----------
void setupTableTextures();           // call once after the window opens
void unloadTableTextures();
void drawTable();
void drawBall(int number, float x, float y, float radius);
void drawBallRolling(int number, float x, float y, float r, float rollAngle, float dirX, float dirY, float alpha);
void drawBalls(Game *game);
void drawTargetRing(Game *game);
void drawCueStick(Game *game, float aimX, float aimY, float power, float strike, bool golden);
void drawAimGuide(Game *game, float aimX, float aimY, bool showGhost, bool showPath);

#endif
