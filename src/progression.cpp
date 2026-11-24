#include "progression.h"
#include "app.h"
#include "ui.h"
#include "fx.h"
#include "sound.h"
#include "anim.h"
#include "rlgl.h"
#include <cmath>
#include <cstdio>
#include <cstring>

// ============================================================
// XP and ranks
// ============================================================
// Each rank needs a bit more XP than the one before: 200, then 300, 400...

int xpForRank(int rank) {
    return 200 + 100 * (rank - 1);
}

int rankOf(int xp, int *into, int *needed) {
    int rank = 1;
    while (xp >= xpForRank(rank)) {
        xp -= xpForRank(rank);
        rank++;
    }
    if (into != nullptr) *into = xp;
    if (needed != nullptr) *needed = xpForRank(rank);
    return rank;
}

const char *rankName(int rank) {
    const char *names[] = { "Null Pointer", "Rookie", "Chalk Dust", "Bank Shot", "Pointer Pro",
                            "Hustler", "Shark", "Memory Master", "Pool Legend" };
    if (rank > 9) rank = 9;
    return names[rank - 1];
}

// Where the XP bar was last drawn, so "+50 XP" can pop up right next to it
float xpBarX = SCREEN_WIDTH / 2.0f;
float xpBarY = 60;
float xpBarWidth = 0;
double xpBarDrawnAt = -10;   // when it was last drawn (screens like practice don't have one)

void giveXp(Profile *profile, int amount) {
    if (profile == nullptr || amount <= 0) return;
    profile->xp += amount;
    saveProfile(profile);
    bool barShowing = GetTime() - xpBarDrawnAt < 0.2;
    float x = barShowing ? xpBarX + xpBarWidth - 40 : SCREEN_WIDTH / 2.0f;
    float y = barShowing ? xpBarY + 40 : 100;
    spawnPopup(TextFormat("+%d XP", amount), x, y, COLOUR_ACCENT, 24);
}

// The bar fills smoothly towards the real XP. When it passes the end of a rank,
// the rank goes up: the bar flashes, "RANK UP!" pops up, confetti and a fanfare.
Profile *barProfile = nullptr;
float shownXp = 0;
float barFlash = 0;

void drawXpBar(Profile *profile, float x, float y, float width) {
    xpBarX = x;
    xpBarY = y;
    xpBarWidth = width;
    xpBarDrawnAt = GetTime();
    float dt = GetFrameTime();

    if (profile != barProfile) {   // a different player: show their XP straight away
        barProfile = profile;
        shownXp = (float)profile->xp;
    }
    int rankBefore = rankOf((int)shownXp, nullptr, nullptr);
    if (!unlockCardOpen()) {       // (it waits while an unlock card is showing)
        shownXp = smoothTowards(shownXp, (float)profile->xp, 3, dt);
        if (fabsf(shownXp - profile->xp) < 0.5f) shownXp = (float)profile->xp;
    }
    int into, needed;
    int rank = rankOf((int)shownXp, &into, &needed);
    if (rank > rankBefore) {
        barFlash = 1;
        spawnPopup(TextFormat("RANK UP!  %s", rankName(rank)), SCREEN_WIDTH / 2.0f, 250, COLOUR_ACCENT, 50);
        spawnConfetti(x + width / 2, y + 30, 70);
        playSound(SOUND_LEVEL_UP, 0.7f);
    }
    barFlash = fmaxf(0, barFlash - 1.5f * dt);

    // "RANK 3  Chalk Dust" and "140 / 400 XP"
    drawTitle(TextFormat("RANK %d", rank), x, y, 15, COLOUR_ACCENT);
    drawText(rankName(rank), x + titleWidth(TextFormat("RANK %d", rank), 15) + 8, y + 1, 13, COLOUR_TEXT);
    const char *numbers = TextFormat("%d / %d XP", into, needed);
    drawText(numbers, x + width - textWidth(numbers, 12), y + 2, 12, COLOUR_MUTED);

    // The bar, with a shine that slides along the filled part
    Rectangle back = { x, y + 22, width, 10 };
    DrawRectangleRounded(back, 1, 8, COLOUR_CODE_BG);
    float fill = width * into / needed;
    if (fill > 3) {
        beginClip(x, y + 22, fill, 10);
        DrawRectangleGradientH((int)x, (int)(y + 22), (int)width, 10, Color{ 246, 196, 84, 255 }, Color{ 255, 140, 70, 255 });
        float shine = fmodf((float)GetTime() * 120, width + 80) - 40;
        DrawRectangleGradientH((int)(x + shine - 20), (int)(y + 22), 20, 10, Fade(WHITE, 0), Fade(WHITE, 0.35f));
        DrawRectangleGradientH((int)(x + shine), (int)(y + 22), 20, 10, Fade(WHITE, 0.35f), Fade(WHITE, 0));
        EndScissorMode();
        DrawCircleV(Vector2{ x + fill, y + 27 }, 7, Fade(COLOUR_ACCENT, 0.25f));   // a glowing tip
    }
    if (barFlash > 0) DrawRectangleRounded(back, 1, 8, Fade(WHITE, barFlash * 0.8f));
    DrawRectangleRoundedLinesEx(back, 1, 8, 1, Fade(COLOUR_ACCENT, 0.3f + 0.7f * barFlash));
}

// ============================================================
// The combo meter: shows while you're potting your own balls in a row
// ============================================================

float comboAlpha = 0;
float comboPop = 0;
int lastCombo = 0;

void drawComboMeter(Game *game, float x, float y) {
    float dt = GetFrameTime();
    int combo = comboCount(game);
    if (combo > lastCombo) comboPop = 1;   // it went up: pop
    lastCombo = combo;
    comboPop = fmaxf(0, comboPop - 3 * dt);
    comboAlpha = smoothTowards(comboAlpha, combo > 0 ? 1.0f : 0.0f, 8, dt);
    if (comboAlpha < 0.01f) return;
    int shown = combo > 0 ? combo : 1;

    Rectangle box = { x, y, 168, 62 };
    drawSoftShadow(box, 10, 8, 0.4f * comboAlpha);
    DrawRectangleRounded(box, roundnessFor(box, 10), 8, Fade(Color{ 20, 18, 16, 255 }, 0.85f * comboAlpha));
    DrawRectangleRoundedLinesEx(box, roundnessFor(box, 10), 8, 1.5f, Fade(COLOUR_ACCENT, (0.4f + 0.6f * comboPop) * comboAlpha));

    drawTitle("COMBO", x + 12, y + 8, 13, Fade(COLOUR_MUTED, comboAlpha));
    float size = 32 * (1 + 0.35f * comboPop);
    drawGlowTitle(TextFormat("x%d", shown), x + 12, y + 22 - (size - 32) / 2, size, Fade(COLOUR_ACCENT, comboAlpha), comboAlpha);

    // Seven pips (seven balls in a group), hotter colours as the combo grows
    for (int i = 0; i < 7; i++) {
        Rectangle pip = { x + 78 + i * 12.0f, y + 34, 8, 16 };
        bool lit = i < shown;
        Color hot = ColorLerp(Color{ 246, 196, 84, 255 }, Color{ 255, 90, 60, 255 }, i / 6.0f);
        DrawRectangleRounded(pip, 0.5f, 4, Fade(lit ? hot : Color{ 60, 58, 64, 255 }, comboAlpha));
        if (lit) DrawRectangleRounded(Rectangle{ pip.x - 2, pip.y - 2, pip.width + 4, pip.height + 4 }, 0.5f, 4, Fade(hot, 0.15f * comboAlpha));
    }
}

// ============================================================
// The unlock card
// ============================================================
// Like unlocking something in The Farmer Was Replaced: the screen dims, a card
// flips over to show what you unlocked, and its description types itself out.

struct UnlockCard {
    bool open;
    int badge;        // the number on the glowing ball at the top (0 = a star, for an upgrade)
    bool closing;
    float age;        // seconds since it appeared
    float closeAge;   // seconds since it started closing
    int typed;        // how many letters of the text are showing
    char heading[80];
    char title[80];
    char text[200];
};
UnlockCard card = {};

const float FLIP_TIME = 0.7f;      // the card turning over
const float LETTERS_PER_SECOND = 45;

void showUnlockCard(int badge, const char *heading, const char *title, const char *text) {
    card.open = true;
    card.badge = badge;
    card.closing = false;
    card.age = 0;
    card.typed = 0;
    snprintf(card.heading, sizeof(card.heading), "%s", heading);
    snprintf(card.title, sizeof(card.title), "%s", title);
    snprintf(card.text, sizeof(card.text), "%s", text);
    playSound(SOUND_UNLOCK, 0.7f);
}

bool unlockCardOpen() {
    return card.open;
}

void updateUnlockCard(float dt) {
    if (!card.open) return;
    card.age += dt;
    int length = (int)strlen(card.text);

    // Type the text out, a letter at a time, with a tiny tick every other letter
    int shouldShow = (int)((card.age - FLIP_TIME) * LETTERS_PER_SECOND);
    if (shouldShow > length) shouldShow = length;
    while (card.typed < shouldShow) {
        card.typed++;
        if (card.typed % 2 == 0) playSound(SOUND_TYPE, 0.25f);
    }
    bool typedAll = card.typed >= length;

    if (card.closing) {
        card.closeAge += dt;
        if (card.closeAge > 0.25f) card.open = false;   // (it stays "open" while fading, so the click doesn't go through)
        return;
    }
    // A click or a key skips the typing, then closes the card. It also closes by itself after a while.
    bool pressed = IsMouseButtonPressed(MOUSE_BUTTON_LEFT) || IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_ESCAPE);
    if (pressed && card.age > 0.3f) {
        if (!typedAll) {
            card.typed = length;
            card.age = FLIP_TIME + length / LETTERS_PER_SECOND;
        } else {
            card.closing = true;
            card.closeAge = 0;
            playSound(SOUND_BUTTON, 0.5f);
        }
    }
    if (typedAll && card.age > FLIP_TIME + length / LETTERS_PER_SECOND + 6) {
        card.closing = true;
        card.closeAge = 0;
    }
}

// The back of the card: dark, with pointer symbols on it
void drawCardBack(Rectangle area) {
    DrawRectangleRounded(area, roundnessFor(area, 18), 10, Color{ 36, 30, 22, 255 });
    DrawRectangleRoundedLinesEx(area, roundnessFor(area, 18), 10, 3, COLOUR_ACCENT);
    const char *symbols = "&*&*";
    for (int row = 0; row < 4; row++) {
        for (int column = 0; column < 6; column++) {
            char symbol[2] = { symbols[(row + column) % 4], '\0' };
            drawTitle(symbol, area.x + 40 + column * (area.width - 80) / 5 - 8, area.y + 40 + row * (area.height - 80) / 3 - 16,
                      32, Fade(COLOUR_ACCENT, 0.25f));
        }
    }
}

// The front: what was unlocked, and the text typing itself out
void drawCardFront(Rectangle area) {
    DrawRectangleRounded(area, roundnessFor(area, 18), 10, COLOUR_PANEL);
    DrawRectangleRoundedLinesEx(area, roundnessFor(area, 18), 10, 3, COLOUR_ACCENT);
    float x = area.x + 36;
    float width = area.width - 72;
    float middle = area.x + area.width / 2;
    drawTitle(card.heading, middle - titleWidth(card.heading, 15) / 2, area.y + 22, 15, COLOUR_ACCENT);

    // A glowing pool ball with the new level's number on it (a star for an upgrade)
    Vector2 ball = { middle, area.y + 84 };
    float pulse = 0.5f + 0.5f * sinf(card.age * 3);
    DrawCircleV(ball, 42 + pulse * 3, Fade(COLOUR_ACCENT, 0.1f));
    DrawCircleV(ball, 34, COLOUR_ACCENT);
    DrawCircleV(Vector2{ ball.x - 10, ball.y - 12 }, 9, Fade(WHITE, 0.35f));   // the shine
    DrawCircleV(ball, 17, Color{ 245, 243, 232, 255 });
    if (card.badge > 0) {
        const char *number = TextFormat("%d", card.badge);
        drawTitle(number, ball.x - titleWidth(number, 18) / 2, ball.y - 11, 18, Color{ 30, 30, 30, 255 });
    } else {
        drawStar(ball.x, ball.y, 10, COLOUR_ACCENT);
    }

    drawTitle("UNLOCKED", middle - titleWidth("UNLOCKED", 13) / 2, area.y + 136, 13, COLOUR_MUTED);
    float titleSize = 32;
    while (titleWidth(card.title, titleSize) > width && titleSize > 18) titleSize -= 2;
    drawGlowTitle(card.title, middle - titleWidth(card.title, titleSize) / 2, area.y + 152, titleSize, COLOUR_TEXT, 0.8f);

    // The text so far: it types itself out
    char shown[200];
    snprintf(shown, sizeof(shown), "%.*s", card.typed, card.text);
    drawWrapped(shown, x, area.y + 206, width, 15, COLOUR_TEXT);
    bool typedAll = card.typed >= (int)strlen(card.text);
    if (typedAll) {
        float blink = 0.5f + 0.5f * sinf(card.age * 4);
        const char *hint = "click to continue";
        drawText(hint, area.x + (area.width - textWidth(hint, 13)) / 2, area.y + area.height - 34, 13, Fade(COLOUR_MUTED, 0.4f + 0.6f * blink));
    }
}

void drawUnlockCard() {
    if (!card.open) return;
    float appear = clamp01(card.age / 0.25f);
    float fade = card.closing ? 1 - clamp01(card.closeAge / 0.25f) : 1;

    // The screen dims, and soft rays of light turn slowly behind the card
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Fade(BLACK, 0.7f * appear * fade));
    Vector2 centre = { SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f };
    for (int i = 0; i < 12; i++) {
        float angle = card.age * 0.3f + i * 2 * PI / 12;
        Vector2 a = { centre.x + cosf(angle - 0.08f) * 700, centre.y + sinf(angle - 0.08f) * 700 };
        Vector2 b = { centre.x + cosf(angle + 0.08f) * 700, centre.y + sinf(angle + 0.08f) * 700 };
        DrawTriangle(centre, b, a, Fade(COLOUR_ACCENT, 0.05f * appear * fade));
        DrawTriangle(centre, a, b, Fade(COLOUR_ACCENT, 0.05f * appear * fade));
    }

    // The flip: the card turns from its back (180 degrees) to its front (0), overshooting a little.
    // Squashing its width by cos(angle) makes it look like it's turning round.
    float t = clamp01(card.age / FLIP_TIME);
    float angle = (1 - easeOutBack(t)) * PI;
    float squash = cosf(angle);                 // -1 = the back facing us, 1 = the front
    float grow = 0.75f + 0.25f * easeOutCubic(t);
    if (card.closing) grow *= 1 - 0.2f * clamp01(card.closeAge / 0.25f);

    Rectangle area = { -290, -165, 580, 330 };   // drawn round (0, 0), then moved and squashed
    rlPushMatrix();
    rlTranslatef(centre.x, centre.y, 0);
    rlScalef(fmaxf(0.02f, fabsf(squash)) * grow, grow, 1);
    drawSoftShadow(area, 18, 24, 0.6f * fade);
    if (squash < 0) drawCardBack(area);
    else drawCardFront(area);
    if (fade < 1) DrawRectangleRounded(area, roundnessFor(area, 18), 10, Fade(BLACK, 1 - fade));
    rlPopMatrix();
}

// ============================================================
// The skill tree (the Progress screen)
// ============================================================
// Each level is a node; a line joins it to the next one. Finished paths glow green
// with light flowing along them, the level you're on pulses, and the three upgrades
// sit at the end of the chapters that unlock them.

const float NODE_RADIUS = 18;

Vector2 nodeCentre(int column, float rowY) {
    return Vector2{ 282.0f + column * 112, rowY + 22 };
}

// A glowing line between two nodes (lit = finished)
void drawTreeLine(Vector2 a, Vector2 b, bool lit, float seed) {
    if (!lit) {
        DrawLineEx(a, b, 2, Color{ 52, 52, 62, 255 });
        return;
    }
    DrawLineEx(a, b, 7, Fade(COLOUR_GOOD, 0.12f));
    DrawLineEx(a, b, 2.5f, COLOUR_GOOD);
    float t = fmodf((float)GetTime() * 0.7f + seed, 1.0f);   // a spark of light flowing along it
    Vector2 spark = { a.x + (b.x - a.x) * t, a.y + (b.y - a.y) * t };
    DrawCircleV(spark, 5, Fade(WHITE, 0.25f));
    DrawCircleV(spark, 2.5f, WHITE);
}

// The upgrade at the end of a chapter: a diamond with a little picture of what it does
void drawUpgradeNode(Vector2 centre, int upgrade, bool unlocked) {
    float r = 22;
    Color colour = unlocked ? (upgrade == 2 ? COLOUR_ACCENT : COLOUR_GOOD) : Color{ 70, 70, 80, 255 };
    if (unlocked) DrawPoly(centre, 4, r + 8, 0, Fade(colour, 0.15f + 0.08f * sinf((float)GetTime() * 3)));
    DrawPoly(centre, 4, r, 0, unlocked ? Color{ 34, 44, 36, 255 } : Color{ 34, 34, 40, 255 });
    DrawPolyLinesEx(centre, 4, r, 0, 2, colour);
    if (upgrade == 0) {   // ghost ball: a see-through ball
        DrawCircleLinesV(centre, 7, colour);
        DrawCircleV(centre, 6, Fade(colour, 0.3f));
    } else if (upgrade == 1) {   // ball path: a ball and its line
        DrawCircleV(Vector2{ centre.x - 5, centre.y + 5 }, 4, colour);
        DrawLineEx(Vector2{ centre.x - 2, centre.y + 2 }, Vector2{ centre.x + 8, centre.y - 8 }, 2, colour);
    } else {   // golden cue
        DrawLineEx(Vector2{ centre.x - 9, centre.y + 9 }, Vector2{ centre.x + 9, centre.y - 9 }, 3, colour);
    }
    const char *names[3] = { "Ghost ball", "Ball path", "Golden cue" };
    drawText(names[upgrade], centre.x - textWidth(names[upgrade], 11) / 2, centre.y + 26, 11, colour);
}

void drawSkillTree(App *app, float top) {
    Profile *learner = app->learner;
    Vector2 mouse = GetMousePosition();
    int hovered = -1;
    Vector2 hoveredAt = { 0, 0 };

    // Work out where every node goes (one row per chapter, like before)
    Vector2 at[LEVEL_COUNT + 1];
    float rowY = top;
    int column = 0;
    int rowFirst[16];
    float rowYs[16];
    int rows = 0;
    for (int n = 1; n <= LEVEL_COUNT; n++) {
        bool newChapter = (n == 1) || getLevel(n - 1)->chapter != getLevel(n)->chapter;
        if (newChapter) {
            if (n > 1) rowY += 64;
            column = 0;
            rowFirst[rows] = n;
            rowYs[rows] = rowY;
            rows++;
        } else if (column == 9) {
            rowY += 64;
            column = 0;
        }
        at[n] = nodeCentre(column, rowY);
        column++;
    }

    // The spine down the left joins the chapters; each chapter's first level hangs off it
    float spineX = 228;
    for (int r = 0; r < rows; r++) {
        int first = rowFirst[r];
        bool lit = first <= learner->levelsDone + 1 && learner->levelsDone >= first - 1;
        if (r > 0) drawTreeLine(Vector2{ spineX, rowYs[r - 1] + 22 }, Vector2{ spineX, rowYs[r] + 22 }, learner->levelsDone >= first - 1, r * 0.17f);
        drawTreeLine(Vector2{ spineX, rowYs[r] + 22 }, Vector2{ at[first].x - NODE_RADIUS, at[first].y }, lit, r * 0.29f);
        DrawCircleV(Vector2{ spineX, rowYs[r] + 22 }, 4, lit ? COLOUR_GOOD : Color{ 70, 70, 80, 255 });
        Level *level = getLevel(first);
        drawBold(TextFormat("%d. %s", level->chapter, chapterName(level->chapter)), 30, rowYs[r] + 14, 13,
                 learner->levelsDone >= first - 1 ? COLOUR_TEXT : COLOUR_MUTED);
    }

    // Lines between levels in the same row
    for (int n = 1; n < LEVEL_COUNT; n++) {
        if (at[n + 1].y != at[n].y) continue;   // the next one starts a new row
        Vector2 a = { at[n].x + NODE_RADIUS, at[n].y };
        Vector2 b = { at[n + 1].x - NODE_RADIUS, at[n + 1].y };
        drawTreeLine(a, b, n + 1 <= learner->levelsDone, n * 0.13f);
    }

    // The upgrades, after the last level of the chapter that unlocks them
    int needs[3] = { AIM_GUIDE_LEVEL, BALL_PATH_LEVEL, GOLD_CUE_LEVEL };
    for (int u = 0; u < 3; u++) {
        Vector2 last = at[needs[u]];
        Vector2 centre = { last.x + 112, last.y };
        bool unlocked = learner->levelsDone >= needs[u];
        drawTreeLine(Vector2{ last.x + NODE_RADIUS, last.y }, Vector2{ centre.x - 22, centre.y }, unlocked, u * 0.31f);
        drawUpgradeNode(centre, u, unlocked);
    }

    // The nodes
    for (int n = 1; n <= LEVEL_COUNT; n++) {
        Vector2 c = at[n];
        bool done = n <= learner->levelsDone;
        bool current = n == currentLevel(app) && !done;
        bool canOpen = reached(app, n);
        bool hover = canOpen && CheckCollisionPointCircle(mouse, c, NODE_RADIUS + 4);
        float r = NODE_RADIUS + (hover ? 3 : 0);

        Color fill = Color{ 32, 32, 38, 255 };
        Color ring = Color{ 64, 64, 76, 255 };
        Color number = Color{ 96, 96, 110, 255 };
        if (done) {
            fill = Color{ 28, 62, 40, 255 };
            ring = COLOUR_GOOD;
            number = COLOUR_TEXT;
            DrawCircleV(c, r + 6, Fade(COLOUR_GOOD, 0.12f));
        } else if (current) {
            float pulse = 0.5f + 0.5f * sinf((float)GetTime() * 4);
            fill = Color{ 70, 60, 22, 255 };
            ring = COLOUR_ACCENT;
            number = COLOUR_TEXT;
            DrawCircleV(c, r + 6 + pulse * 6, Fade(COLOUR_ACCENT, 0.12f + 0.12f * pulse));
        }
        DrawCircleV(c, r, fill);
        DrawRing(c, r - 2, r, 0, 360, 36, ring);
        const char *label = TextFormat("%d", n);
        drawBold(label, c.x - textWidth(label, 14) / 2, c.y - 8, 14, number);

        // Stars: three dots under the node, gold for each one earned
        for (int s = 0; s < 3; s++) {
            float angle = (120 - s * 30) * DEG2RAD;
            Vector2 dot = { c.x + cosf(angle) * (r + 1), c.y + sinf(angle) * (r + 1) };
            bool earned = learner->stars[n] > s;
            DrawCircleV(dot, 2.5f, earned ? COLOUR_ACCENT : Color{ 50, 50, 58, 255 });
        }
        const char *name = getLevel(n)->shortName;
        drawText(name, c.x - textWidth(name, 11) / 2, c.y + r + 5, 11, done || current ? COLOUR_MUTED : Color{ 80, 80, 92, 255 });

        if (hover) {
            hovered = n;
            hoveredAt = c;
        }
        // Click a node you've reached to read it
        if (hover && IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && !buttonsAreBlocked()) {
            app->codexLevel = n;
            app->screen = SCREEN_CODEX;
        }
    }

    // The node under the mouse: its full title
    if (hovered > 0) {
        Level *level = getLevel(hovered);
        const char *text = TextFormat("Level %d: %s", hovered, level->title);
        float width = textWidth(text, 14) + 24;
        Rectangle tip = { hoveredAt.x - width / 2, hoveredAt.y - 58, width, 30 };
        if (tip.x < 10) tip.x = 10;
        if (tip.x + tip.width > SCREEN_WIDTH - 10) tip.x = SCREEN_WIDTH - 10 - tip.width;
        drawSoftShadow(tip, 8, 8, 0.5f);
        DrawRectangleRounded(tip, roundnessFor(tip, 8), 8, Color{ 20, 20, 24, 245 });
        DrawRectangleRoundedLinesEx(tip, roundnessFor(tip, 8), 8, 1, Fade(COLOUR_ACCENT, 0.6f));
        drawText(text, tip.x + 12, tip.y + 7, 14, COLOUR_TEXT);
    }
}
