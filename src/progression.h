#ifndef PROGRESSION_H
#define PROGRESSION_H

#include "raylib.h"
#include "questions.h"
#include "pool.h"

struct App;

// ============================================================
// Progression: XP and ranks, the combo meter, the unlock card
// (like the unlocks in The Farmer Was Replaced) and the skill tree.
// ============================================================

// ---------- XP and ranks (XP is saved with each player's progress) ----------
int rankOf(int xp, int *into, int *needed);    // the rank for this much XP, how far into it, and how much it needs
const char *rankName(int rank);
void giveXp(Profile *profile, int amount);      // adds XP, saves it, and pops up "+50 XP" by the XP bar
void drawXpBar(Profile *profile, float x, float y, float width);   // fills smoothly; flashes on a rank up

// ---------- The combo meter (on the table while a combo is going) ----------
void drawComboMeter(Game *game, float x, float y);

// ---------- The unlock card: the screen dims, a card flips in, the text types out ----------
void showUnlockCard(int badge, const char *heading, const char *title, const char *text);   // badge: the number on its ball
bool unlockCardOpen();
void updateUnlockCard(float dt);
void drawUnlockCard();

// ---------- The skill tree (the Progress screen) ----------
void drawSkillTree(App *app, float top);

#endif
