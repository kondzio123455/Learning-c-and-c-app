#ifndef THEME_H
#define THEME_H

#include "raylib.h"

// ============================================================
// The look of the game, all in one place: a dark pool hall with
// warm blacks, brass yellow and felt green. Change a colour here
// and it changes everywhere.
// ============================================================

// ---------- Colours used everywhere ----------
const Color COLOUR_BG = { 16, 16, 19, 255 };           // the room
const Color COLOUR_PANEL = { 31, 31, 37, 255 };        // panels and boxes
const Color COLOUR_PANEL_TOP = { 40, 40, 47, 255 };    // the lighter top edge of a panel
const Color COLOUR_CODE_BG = { 14, 15, 19, 255 };      // the code box
const Color COLOUR_TEXT = { 234, 230, 222, 255 };
const Color COLOUR_MUTED = { 152, 148, 142, 255 };
const Color COLOUR_ACCENT = { 246, 196, 84, 255 };     // brass yellow
const Color COLOUR_GOOD = { 112, 214, 132, 255 };      // green
const Color COLOUR_BAD = { 255, 128, 112, 255 };       // soft red
const Color COLOUR_BUTTON = { 50, 49, 58, 255 };
const Color COLOUR_BUTTON_HOVER = { 74, 72, 84, 255 };
const Color COLOUR_SHADOW = { 0, 0, 0, 255 };

// ---------- Shapes ----------
const float PANEL_RADIUS = 12;     // how round the corners of a panel are, in pixels
const float BUTTON_RADIUS = 10;
const float SHADOW_SIZE = 14;      // how far a panel's soft shadow spreads

// ---------- Fonts (in assets/) ----------
// JetBrains Mono is used for normal text and code: every character is the same
// width, which the code box and some lessons rely on. Inter is used for titles
// and buttons. Both are free (SIL Open Font License).
#define FONT_REGULAR "assets/JetBrainsMono-Regular.ttf"
#define FONT_BOLD "assets/JetBrainsMono-Bold.ttf"
#define FONT_TITLE "assets/Inter-ExtraBold.ttf"
#define FONT_BUTTON "assets/Inter-SemiBold.ttf"

// ---------- Timings (seconds) ----------
const float SCREEN_FADE_TIME = 0.4f;   // fading in a new screen

#endif
