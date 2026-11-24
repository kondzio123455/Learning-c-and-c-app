#ifndef FX_H
#define FX_H

#include "raylib.h"
#include "pool.h"

// ============================================================
// Game feel: particles, popups, screen shake and hit-stop.
// pool.cpp only says WHAT happened on the table (two balls hit, a
// ball went down...). This file decides how that looks and sounds.
// ============================================================

// ---------- Things that happen on the table ----------
void fxStrike(Game *game, float power);                 // the cue hits the cue ball (call after startShot)
void fxBallsHit(float x, float y, float speed);         // two balls hit at table point (x, y)
void fxCushionHit(float speed);                         // a ball bounced off a cushion
void fxBallPotted(Game *game, int number, int pocket);  // a ball dropped into a pocket
void fxWin();                                           // fireworks!

// ---------- Particles ----------
void spawnConfetti(float x, float y, int count);        // on the screen (2D), e.g. for a right answer
void spawnSparkle(float x, float y, Color colour);      // a few tiny glowing sparks (2D)
void updateParticles(float dt);
void drawParticles();          // the 2D ones: confetti and fireworks, over everything
void drawParticles3D();        // the 3D ones: chalk dust, sparks, pot bursts (inside the 3D scene)

// ---------- Popups: text that pops in, rises and fades ("NICE SHOT!", "+1") ----------
void spawnPopup(const char *text, float x, float y, Color colour, float size);
void spawnFloatingText(const char *text, float x, float y, Color colour);   // a normal-sized popup
void updateFloatingTexts(float dt);
void drawFloatingTexts();

// ---------- Combo: your own balls potted in a row, in one turn ----------
int comboCount(Game *game);    // 0 when there's no combo going

// ---------- Screen shake and hit-stop ----------
void shakeScreen(float amount);   // bigger = more shake (it adds up, then dies away)
void updateShake(float dt);
Vector2 getShake();               // -1 to 1 each way: how far to nudge the camera right now
void hitStop(float seconds);      // freeze the balls for a moment, so a big hit feels heavy
void updateHitStop(float dt);
bool hitStopping();

#endif
