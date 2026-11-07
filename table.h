#ifndef TABLE_H
#define TABLE_H

#include "raylib.h"

// ---------- Screen layout ----------
const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;

// The green felt (the area the balls can roll on)
const int FELT_LEFT = 60;
const int FELT_TOP = 200;
const int FELT_WIDTH = 640;
const int FELT_HEIGHT = 320;
const int RAIL = 30;                 // width of the wooden cushion around the felt

// Where the cue ball starts (a quarter of the way along the table)
const float CUE_START_X = FELT_LEFT + FELT_WIDTH / 4;
const float CUE_START_Y = FELT_TOP + FELT_HEIGHT / 2;

// Where the front ball of the rack goes (the foot spot)
const float RACK_X = FELT_LEFT + FELT_WIDTH * 0.7f;
const float RACK_Y = FELT_TOP + FELT_HEIGHT / 2;

// ---------- Balls and physics ----------
const int BALL_COUNT = 16;           // cue ball (0) + balls 1 to 15
const int RACK_SIZE = 15;            // balls in the rack

const float BALL_RADIUS = 11;
const float POCKET_RADIUS = 20;

const float MAX_POWER = 1100;        // fastest shot, in pixels per second
const float FRICTION = 0.985f;       // each frame, speed is multiplied by this
const float STOP_SPEED = 5;          // below this speed a ball stops
const float CUSHION_BOUNCE = 0.75f;  // a cushion keeps 75% of the speed
const int PHYSICS_STEPS = 8;         // small steps per frame so balls don't pass through each other

struct Ball {
    int number;      // 0 is the cue ball, 1 to 15 are the object balls
    float x, y;      // position on the screen
    float vx, vy;    // velocity in pixels per second
    bool pocketed;   // true once the ball has gone down a pocket
};

// ---------- Drawing ----------
void drawTable();
void drawBall(int number, float x, float y);
void drawRackPreview(int rack[]);
void drawSlotLabel(int slot, const char *label, bool above);
void drawSlotRing(int slot);
void drawRackArray(int rack[], int highlightA, int highlightB);
void drawBalls(Ball balls[]);
void drawAim(Ball *cue, float power);
void drawPotted(int potted[], int pottedCount);

// ---------- Rack ----------
void getSlotPosition(int slot, float *x, float *y);
void placeRack(Ball balls[], int rack[]);

// ---------- Physics ----------
void shootCueBall(Ball *cue, float targetX, float targetY, float power);
void updatePhysics(Ball balls[], float dt, int potted[], int *pottedCount);
bool ballsMoving(Ball balls[]);

#endif
