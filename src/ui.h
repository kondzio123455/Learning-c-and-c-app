#ifndef UI_H
#define UI_H

#include "raylib.h"
#include "pool.h"

// ---------- Colours used everywhere ----------
const Color COLOUR_BG = { 24, 26, 32, 255 };
const Color COLOUR_PANEL = { 34, 37, 46, 255 };
const Color COLOUR_CODE_BG = { 18, 20, 26, 255 };
const Color COLOUR_TEXT = { 225, 228, 235, 255 };
const Color COLOUR_MUTED = { 140, 146, 160, 255 };
const Color COLOUR_ACCENT = { 250, 205, 70, 255 };    // yellow
const Color COLOUR_GOOD = { 110, 210, 120, 255 };     // green
const Color COLOUR_BAD = { 255, 135, 120, 255 };      // soft red
const Color COLOUR_BUTTON = { 55, 60, 75, 255 };
const Color COLOUR_BUTTON_HOVER = { 78, 85, 105, 255 };

// ---------- Text (the font is monospace: every character is the same width) ----------
void loadFonts();
void unloadFonts();
void drawText(const char *text, float x, float y, float size, Color colour);
void drawBold(const char *text, float x, float y, float size, Color colour);
float textWidth(const char *text, float size);
float charWidth(float size);
float lineHeight(float size);
float drawWrapped(const char *text, float x, float y, float width, float size, Color colour);

// ---------- Buttons ----------
bool button(Rectangle area, const char *label);   // draws it, returns true when clicked

// ---------- Pieces of the game screen ----------
void drawPlayerBox(Player *player, Rectangle area, bool isTurn, bool tableOpen, int left, bool showLeft);
void drawMemoryRow(Game *game, float y);
void drawTargetPointer(Game *game, float x, float y, float memoryRowY);
void drawTray(Player *player, float x, float y, bool newestFirst);
void drawShotLog(DoublyLinkedNode *newest, Rectangle area, bool canScroll);
void drawBallTooltip(Game *game);
void drawDataTableHeader(float x, float y);
void drawDataRow(float x, float y, const char *name, const char *type, const char *value);
void drawPowerBar(float x, float y, float width, float power, bool fromCode, bool showPointer);

#endif
