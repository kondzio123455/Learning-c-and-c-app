#ifndef RENDER3D_H
#define RENDER3D_H

#include "raylib.h"
#include "pool.h"

// ============================================================
// The 3D table. The game, its rules and its physics are still 2D
// (each ball has an x and y on the table). This file only DRAWS
// them in 3D: a table point (x, y) becomes the 3D point (x, height, z).
// ============================================================

struct App;

// Where the 3D view goes on the screen (the left side, where the 2D table was)
const Rectangle TABLE_VIEW = { 0, 56, (float)PANEL_X, 414 };

void setupRender3D();                    // after the window opens
void unloadRender3D();

void renderTable3D(App *app, float dt);  // draws the 3D scene into its own image (before BeginDrawing)
void drawTableView();                    // puts that image on the screen

// The main menu's background: the table seen from an orbiting camera, with balls rolling by themselves
void renderMenu3D(float dt);             // before BeginDrawing, like renderTable3D
void drawMenuView();                     // fills the whole screen with it

void toggleTopDownView();                // the T key
bool isTopDownView();

// The mouse and the 3D table
bool mouseOverTableView();
bool mouseOnTable(Vector2 *point);       // where the mouse points on the felt, in table coordinates
int ballUnderMouse(Game *game);          // the ball under the mouse, or -1
Vector2 tableToScreen(float x, float y, float height);   // a table point (height in table pixels) on the screen
Vector3 tableToWorld(float x, float y, float height);    // the same point in the 3D world

// Slow motion: the balls are drawn part of the way between where they were before the
// last physics step and where they are now (amount 0 to 1), so they move smoothly
void setBallBlend(const Vector2 *before, float amount);
void stopBallBlend();
// The camera swings round a point on the table (the 8 going down), with black bars like a film
void setCinematicCamera(bool on, float x, float y);

#endif
