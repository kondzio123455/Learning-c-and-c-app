#ifndef UI_H
#define UI_H

#include "raylib.h"
#include "pool.h"
#include "theme.h"   // the colours, fonts and shapes

// ---------- Text (the font is monospace: every character is the same width) ----------
void loadFonts();
void unloadFonts();
void drawText(const char *text, float x, float y, float size, Color colour);
void drawBold(const char *text, float x, float y, float size, Color colour);
float textWidth(const char *text, float size);
float charWidth(float size);
float lineHeight(float size);
float drawWrapped(const char *text, float x, float y, float width, float size, Color colour);
float wrappedHeight(const char *text, float width, float size);
void drawTitle(const char *text, float x, float y, float size, Color colour);   // Inter, for titles
float titleWidth(const char *text, float size);
void drawGlowTitle(const char *text, float x, float y, float size, Color colour, float glow);   // with a soft glow behind it

// ---------- Stars (3 = solved without hints) ----------
void drawStar(float x, float y, float size, Color colour);
void drawStars(float x, float y, int count, float size);

// ---------- The view: the 1280 x 720 game, scaled to fit the window ----------
void setView(float scale, float offsetX, float offsetY);
float getViewScale();
void beginClip(float x, float y, float width, float height);   // like BeginScissorMode, in game coordinates

// ---------- Panels and backgrounds ----------
void drawBackground();                          // the room behind every screen
void drawBar(float x, float y, float width, float height);   // a bar along the top of a screen
void drawPanel(Rectangle area);                 // a rounded panel with a soft shadow
void drawPopupBox(Rectangle area);              // a panel with a brass edge, for windows over the screen
void drawPanelCover(Rectangle area, float alpha);   // covers a panel (for fading in what's on it)
void drawCodeBackground(Rectangle area);        // the dark rounded box behind a piece of code
void drawSoftShadow(Rectangle area, float radius, float size, float alpha);
float roundnessFor(Rectangle area, float radius);   // a corner radius in pixels, as raylib's "roundness"

// ---------- Buttons ----------
void blockButtons(bool blocked);   // true while a window is open over the screen
bool buttonsAreBlocked();
bool button(Rectangle area, const char *label);   // draws it, returns true when clicked

// ---------- Pieces of the game screen ----------
void drawPlayerBox(Player *player, Rectangle area, bool isTurn, bool tableOpen, int left, bool showLeft);
float memoryBoxX(int n);
void drawMemoryRow(Game *game, float y);
void drawTargetPointer(Game *game, float x, float y, float memoryRowY);
void drawTray(Player *player, float x, float y, bool newestFirst);
void drawShotLog(DoublyLinkedNode *newest, Rectangle area, bool canScroll);
void drawBallTooltip(Game *game);
void drawDataTableHeader(float x, float y);
void drawDataRow(float x, float y, const char *name, const char *type, const char *value);
void drawPowerBar(float x, float y, float width, float power, bool fromCode, bool showPointer);

#endif
