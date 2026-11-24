#include "ui.h"
#include "render3d.h"
#include "anim.h"
#include "sound.h"
#include "lab2.h"
#include <cstdio>
#include <cmath>

// ============================================================
// Fonts and text
// ============================================================

Font regularFont;
Font boldFont;
Font titleFont;    // Inter ExtraBold, for titles
Font buttonFont;   // Inter SemiBold, for button labels

// Loaded large, with mipmaps, so it stays sharp when drawn small
Font loadSharpFont(const char *file) {
    Font font = LoadFontEx(file, 96, nullptr, 0);
    GenTextureMipmaps(&font.texture);
    SetTextureFilter(font.texture, TEXTURE_FILTER_TRILINEAR);
    return font;
}

void loadFonts() {
    regularFont = loadSharpFont(FONT_REGULAR);
    boldFont = loadSharpFont(FONT_BOLD);
    titleFont = loadSharpFont(FONT_TITLE);
    buttonFont = loadSharpFont(FONT_BUTTON);
}

void unloadFonts() {
    UnloadFont(regularFont);
    UnloadFont(boldFont);
    UnloadFont(titleFont);
    UnloadFont(buttonFont);
}

void drawText(const char *text, float x, float y, float size, Color colour) {
    DrawTextEx(regularFont, text, Vector2{ x, y }, size, 0, colour);
}

void drawBold(const char *text, float x, float y, float size, Color colour) {
    DrawTextEx(boldFont, text, Vector2{ x, y }, size, 0, colour);
}

void drawTitle(const char *text, float x, float y, float size, Color colour) {
    DrawTextEx(titleFont, text, Vector2{ x, y }, size, 0, colour);
}

float titleWidth(const char *text, float size) {
    return MeasureTextEx(titleFont, text, size, 0).x;
}

// A title with a soft glow: faint copies of it drawn in a ring round it, then the title on top
void drawGlowTitle(const char *text, float x, float y, float size, Color colour, float glow) {
    for (int ring = 1; ring <= 2; ring++) {
        float r = ring * size * 0.05f;
        for (int k = 0; k < 8; k++) {
            float angle = k * PI / 4;
            drawTitle(text, x + cosf(angle) * r, y + sinf(angle) * r, size, Fade(colour, 0.07f * glow));
        }
    }
    drawTitle(text, x, y, size, colour);
}

float textWidth(const char *text, float size) {
    return MeasureTextEx(regularFont, text, size, 0).x;
}

float charWidth(float size) {
    return textWidth("M", size);
}

float lineHeight(float size) {
    return size * 1.4f;
}

// Draws text that wraps onto a new line when it gets too wide, and returns the height used.
// The font is monospace, so a line can hold exactly width / charWidth characters.
// A '\n' in the text starts a new line.
// Wraps text into lines that fit 'width'. Draws it if draw is true; either way returns its height.
float wrapText(const char *text, float x, float y, float width, float size, Color colour, bool draw) {
    int maxChars = (int)(width / charWidth(size));
    char line[300];
    int lineLength = 0;
    float lineY = y;
    const char *p = text;

    while (*p != '\0') {
        // Measure the next word
        int wordLength = 0;
        while (p[wordLength] != '\0' && p[wordLength] != ' ' && p[wordLength] != '\n') {
            wordLength++;
        }

        // Start a new line if the word doesn't fit on this one
        if (lineLength > 0 && lineLength + 1 + wordLength > maxChars) {
            line[lineLength] = '\0';
            if (draw) drawText(line, x, lineY, size, colour);
            lineY += lineHeight(size);
            lineLength = 0;
        }

        // Add the word (with a space in front if it isn't the first on the line)
        if (lineLength > 0) {
            line[lineLength] = ' ';
            lineLength++;
        }
        for (int i = 0; i < wordLength && lineLength < 298; i++) {
            line[lineLength] = p[i];
            lineLength++;
        }
        p += wordLength;

        // A '\n' finishes the line
        if (*p == '\n') {
            line[lineLength] = '\0';
            if (draw) drawText(line, x, lineY, size, colour);
            lineY += lineHeight(size);
            lineLength = 0;
            p++;
        } else if (*p == ' ') {
            p++;
        }
    }

    if (lineLength > 0) {
        line[lineLength] = '\0';
        if (draw) drawText(line, x, lineY, size, colour);
        lineY += lineHeight(size);
    }
    return lineY - y;
}

float drawWrapped(const char *text, float x, float y, float width, float size, Color colour) {
    return wrapText(text, x, y, width, size, colour, true);
}

// How tall some wrapped text will be, without drawing it
float wrappedHeight(const char *text, float width, float size) {
    return wrapText(text, 0, 0, width, size, WHITE, false);
}

// ============================================================
// The view
// ============================================================
// Everything is laid out for a 1280 x 720 screen. main() scales that to fit the
// window, so these remember by how much (clipping works in window pixels).

float viewScale = 1;
float viewOffsetX = 0;
float viewOffsetY = 0;

float getViewScale() {
    return viewScale;
}

void setView(float scale, float offsetX, float offsetY) {
    viewScale = scale;
    viewOffsetX = offsetX;
    viewOffsetY = offsetY;
}

void beginClip(float x, float y, float width, float height) {
    BeginScissorMode((int)(viewOffsetX + x * viewScale), (int)(viewOffsetY + y * viewScale),
                     (int)(width * viewScale + 1), (int)(height * viewScale + 1));
}

// ============================================================
// Stars
// ============================================================

// One star: 10 points (outer, inner, outer...) filled with triangles from the centre
void drawStar(float x, float y, float size, Color colour) {
    Vector2 points[10];
    for (int k = 0; k < 10; k++) {
        float angle = -1.5708f + k * 0.6283f;   // start at the top, 36 degrees apart
        float radius = size;
        if (k % 2 == 1) radius = size * 0.45f;
        points[k] = Vector2{ x + cosf(angle) * radius, y + sinf(angle) * radius };
    }
    for (int k = 0; k < 10; k++) {
        DrawTriangle(Vector2{ x, y }, points[(k + 1) % 10], points[k], colour);
    }
}

// Three stars in a row; the first 'count' are gold
void drawStars(float x, float y, int count, float size) {
    for (int s = 0; s < 3; s++) {
        Color colour = Color{ 70, 74, 88, 255 };
        if (s < count) colour = COLOUR_ACCENT;
        drawStar(x + s * size * 2.3f, y, size, colour);
    }
}

// ============================================================
// Buttons
// ============================================================

// While a window (the Slide, Rewind or player list) is open, the buttons
// underneath it are drawn but don't react to the mouse.
bool buttonsBlocked = false;

void blockButtons(bool blocked) {
    buttonsBlocked = blocked;
}

bool buttonsAreBlocked() {
    return buttonsBlocked;
}

// raylib's roundness is a fraction of the shorter side; this turns a radius in pixels into that
float roundnessFor(Rectangle area, float radius) {
    float shortest = fminf(area.width, area.height);
    if (shortest <= 0) return 0;
    return fminf(1.0f, radius * 2 / shortest);
}

// Grows a rectangle by 'by' pixels on every side
Rectangle grow(Rectangle area, float by) {
    return Rectangle{ area.x - by, area.y - by, area.width + 2 * by, area.height + 2 * by };
}

// A soft shadow: a few see-through rounded boxes, each a little bigger, shifted down
void drawSoftShadow(Rectangle area, float radius, float size, float alpha) {
    const int LAYERS = 6;
    for (int i = LAYERS; i >= 1; i--) {
        float spread = size * i / LAYERS;
        Rectangle layer = grow(area, spread);
        layer.y += size * 0.35f;
        DrawRectangleRounded(layer, roundnessFor(layer, radius + spread), 8, Fade(COLOUR_SHADOW, alpha / LAYERS));
    }
}

// The room behind every screen: dark, with the warm light of a lamp at the top
void drawBackground() {
    ClearBackground(COLOUR_BG);
    // The lamp's glow: see-through circles, each smaller one adding a little more light
    for (int i = 24; i >= 1; i--) {
        DrawCircleV(Vector2{ SCREEN_WIDTH / 2.0f, -260 }, 900.0f * i / 24, Fade(Color{ 70, 56, 38, 255 }, 0.016f));
    }
    DrawRectangleGradientV(0, SCREEN_HEIGHT - 220, SCREEN_WIDTH, 220, Fade(BLACK, 0), Fade(BLACK, 0.3f));
}

// The bar along the top of a screen, with a thin brass line under it
void drawBar(float x, float y, float width, float height) {
    DrawRectangleGradientV((int)x, (int)y, (int)width, (int)height, COLOUR_PANEL_TOP, COLOUR_PANEL);
    DrawRectangleGradientV((int)x, (int)(y + height), (int)width, 10, Fade(BLACK, 0.35f), Fade(BLACK, 0));
    DrawRectangle((int)x, (int)(y + height - 1), (int)width, 1, Fade(COLOUR_ACCENT, 0.2f));
}

void drawPanel(Rectangle area) {
    drawSoftShadow(area, PANEL_RADIUS, SHADOW_SIZE, 0.55f);
    float roundness = roundnessFor(area, PANEL_RADIUS);
    DrawRectangleRounded(area, roundness, 10, COLOUR_PANEL);
    DrawRectangleRoundedLinesEx(area, roundness, 10, 1, Fade(WHITE, 0.06f));   // a faint edge catches the light
}

void drawPopupBox(Rectangle area) {
    float roundness = roundnessFor(area, PANEL_RADIUS);
    for (int i = 3; i >= 1; i--) {
        DrawRectangleRounded(grow(area, i * 4.0f), roundnessFor(grow(area, i * 4.0f), PANEL_RADIUS + i * 4), 10, Fade(COLOUR_ACCENT, 0.035f));
    }
    drawPanel(area);
    DrawRectangleRoundedLinesEx(area, roundness, 10, 2, Fade(COLOUR_ACCENT, 0.85f));
}

void drawCodeBackground(Rectangle area) {
    DrawRectangleRounded(area, roundnessFor(area, 6), 8, COLOUR_CODE_BG);
}

void drawPanelCover(Rectangle area, float alpha) {
    DrawRectangleRounded(area, roundnessFor(area, PANEL_RADIUS), 10, Fade(COLOUR_PANEL, alpha));
}

// Each button remembers how lit up it is (0 to 1), so it can glow and grow smoothly.
// Buttons are drawn fresh every frame, so a button is found again by where it is.
struct ButtonGlow {
    float x, y;
    float amount;
    bool hovered;   // was the mouse over it last frame? (to tick once when it arrives)
};
const int MAX_BUTTON_GLOWS = 64;
ButtonGlow buttonGlows[MAX_BUTTON_GLOWS];
int buttonGlowCount = 0;

ButtonGlow *glowFor(Rectangle area) {
    for (int i = 0; i < buttonGlowCount; i++) {
        if (fabsf(buttonGlows[i].x - area.x) < 0.5f && fabsf(buttonGlows[i].y - area.y) < 0.5f) return &buttonGlows[i];
    }
    int slot = buttonGlowCount;
    if (buttonGlowCount < MAX_BUTTON_GLOWS) buttonGlowCount++;
    else slot = GetRandomValue(0, MAX_BUTTON_GLOWS - 1);   // full: reuse one
    buttonGlows[slot] = ButtonGlow{ area.x, area.y, 0, false };
    return &buttonGlows[slot];
}

bool button(Rectangle area, const char *label) {
    bool hover = !buttonsBlocked && CheckCollisionPointRec(GetMousePosition(), area);

    // Glide towards lit up (hovered) or not
    ButtonGlow *glow = glowFor(area);
    if (hover && !glow->hovered) playSound(SOUND_HOVER, 0.35f);   // a soft tick as the mouse arrives
    glow->hovered = hover;
    glow->amount = smoothTowards(glow->amount, hover ? 1.0f : 0.0f, 14, GetFrameTime());
    float g = glow->amount;

    // A hovered button grows a little and glows brass
    Rectangle box = grow(area, g * 3);
    float roundness = roundnessFor(box, BUTTON_RADIUS);
    if (g > 0.01f) {
        for (int i = 3; i >= 1; i--) {
            Rectangle halo = grow(box, i * 3.0f);
            DrawRectangleRounded(halo, roundnessFor(halo, BUTTON_RADIUS + i * 3), 8, Fade(COLOUR_ACCENT, 0.06f * g));
        }
    }
    drawSoftShadow(box, BUTTON_RADIUS, 6, 0.4f);
    Color colour = ColorLerp(COLOUR_BUTTON, COLOUR_BUTTON_HOVER, g);
    DrawRectangleRounded(box, roundness, 8, colour);
    DrawRectangleRoundedLinesEx(box, roundness, 8, 1, Fade(COLOUR_ACCENT, 0.12f + 0.5f * g));

    float size = 18;
    float width = MeasureTextEx(buttonFont, label, size, 0).x;
    Color text = ColorLerp(COLOUR_TEXT, Color{ 255, 236, 190, 255 }, g);
    DrawTextEx(buttonFont, label, Vector2{ box.x + (box.width - width) / 2, box.y + (box.height - size) / 2 - 1 }, size, 0, text);

    bool clicked = hover && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
    if (clicked) playSound(SOUND_BUTTON, 0.5f);
    return clicked;
}

// ============================================================
// Pieces of the game screen
// ============================================================

// One player's box in the top bar: name, group and how many they have left
void drawPlayerBox(Player *player, Rectangle area, bool isTurn, bool tableOpen, int left, bool showLeft) {
    Color background = COLOUR_PANEL;
    if (isTurn) background = Color{ 60, 55, 30, 255 };
    DrawRectangleRec(area, background);
    if (isTurn) DrawRectangleLinesEx(area, 2, COLOUR_ACCENT);

    // Centre the name. Every character is the same width, so the name's width
    // is just strlen_ptr(name) * charWidth -- Lab 2 Q5 in action.
    float size = 18;
    float nameWidth = strlen_ptr(player->name) * charWidth(size);
    drawBold(player->name, area.x + (area.width - nameWidth) / 2, area.y + 5, size, COLOUR_TEXT);

    char info[60];
    if (tableOpen) {
        snprintf(info, sizeof(info), "table open");
    } else if (showLeft) {
        snprintf(info, sizeof(info), "%s - %d left", groupName(player->group), left);
    } else {
        snprintf(info, sizeof(info), "%s - %d potted", groupName(player->group), player->score);
    }
    float infoWidth = textWidth(info, 14);
    drawText(info, area.x + (area.width - infoWidth) / 2, area.y + 27, 14, COLOUR_MUTED);
}

// Where box n of the memory row goes
float memoryBoxX(int n) {
    return FELT_LEFT - RAIL + n * 44.0f;
}

// The balls array drawn like the memory diagrams in the Pointers lecture:
// one box per Ball, with its address underneath.
void drawMemoryRow(Game *game, float y) {
    drawText("Ball balls[16];   each Ball is 24 bytes, so each address goes up by 24",
             FELT_LEFT - RAIL, y - 22, 14, COLOUR_MUTED);

    for (int n = 0; n < BALL_COUNT; n++) {
        float x = memoryBoxX(n);
        Rectangle box = { x, y, 40, 34 };

        Color background = COLOUR_PANEL;
        if (n == game->calledBall) background = Color{ 85, 75, 25, 255 };
        DrawRectangleRec(box, background);
        DrawRectangleLinesEx(box, 1, Color{ 70, 75, 90, 255 });

        if (game->balls[n].pocketed) {
            drawText("--", x + 10, y + 8, 16, COLOUR_MUTED);
        } else {
            drawBall(n, x + 20, y + 17, 10);
        }

        const char *address = TextFormat("%d", addressOf(n));
        float width = textWidth(address, 12);
        drawText(address, x + 20 - width / 2, y + 37, 12, COLOUR_MUTED);
    }
}

// The 'target' pointer box, with an arrow up to the ball it points at
void drawTargetPointer(Game *game, float x, float y, float memoryRowY) {
    Rectangle box = { x, y, 250, 30 };
    DrawRectangleRec(box, COLOUR_CODE_BG);
    DrawRectangleLinesEx(box, 1, COLOUR_ACCENT);

    if (game->calledBall < 1) {
        drawText("Ball *target = nullptr", x + 8, y + 7, 15, COLOUR_MUTED);
        return;
    }

    const char *text = TextFormat("Ball *target = %d", addressOf(game->calledBall));
    drawText(text, x + 8, y + 7, 15, COLOUR_ACCENT);

    // Arrow from the box up to the memory box of the called ball
    Vector2 from = { x + 230, y };
    Vector2 to = { memoryBoxX(game->calledBall) + 20, memoryRowY + 50 };
    DrawLineEx(from, to, 2, COLOUR_ACCENT);
    DrawLineEx(to, Vector2{ to.x - 5, to.y + 8 }, 2, COLOUR_ACCENT);
    DrawLineEx(to, Vector2{ to.x + 5, to.y + 8 }, 2, COLOUR_ACCENT);
}

// A player's potted balls. Newest first uses Lab 2's reverse_array on a copy.
void drawTray(Player *player, float x, float y, bool newestFirst) {
    drawText(TextFormat("%s:", player->name), x, y, 15, COLOUR_MUTED);

    int balls[8];
    for (int i = 0; i < player->trayCount; i++) {
        balls[i] = player->tray[i];
    }
    if (newestFirst) reverse_array(balls, player->trayCount);

    for (int i = 0; i < player->trayCount; i++) {
        drawBall(balls[i], x + 100 + i * 24.0f, y + 9, 10);
    }
}

// The last few entries of the shot log. Starts at 'newest' and follows prevPointer back.
void drawShotLog(DoublyLinkedNode *newest, Rectangle area, bool canScroll) {
    DrawRectangleRec(area, COLOUR_CODE_BG);
    const char *title = "Shot log (linked list)";
    if (canScroll) title = "Shot log - Up/Down to scroll (prevPointer)";
    drawText(title, area.x + 8, area.y + 5, 13, COLOUR_MUTED);

    // Each line is cut short (with "..") if it's wider than the box
    int maxChars = (int)((area.width - 16) / charWidth(12));
    if (maxChars > 98) maxChars = 98;

    DoublyLinkedNode *node = newest;
    float y = area.y + 24;
    while (node != nullptr && y < area.y + area.height - 16) {
        char line[100];
        copy_string(line, node->text);
        if (strlen_ptr(line) > maxChars) {
            line[maxChars - 2] = '.';
            line[maxChars - 1] = '.';
            line[maxChars] = '\0';
        }
        drawText(line, area.x + 8, y, 12, COLOUR_TEXT);
        y += 17;
        node = node->prevPointer;
    }
}

// Hovering over a ball shows which ball it is and its address
void drawBallTooltip(Game *game) {
    // The 3D table works out which ball is under the mouse
    int n = ballUnderMouse(game);
    if (n < 0) return;
    Vector2 mouse = GetMousePosition();
    const char *text = TextFormat("balls[%d] at address %d", n, addressOf(n));
    if (n == 0) text = TextFormat("balls[0] (cue ball) at address %d", addressOf(0));
    float width = textWidth(text, 14) + 16;
    Rectangle box = { mouse.x - width / 2, mouse.y - 40, width, 24 };
    DrawRectangleRounded(box, 0.3f, 6, Fade(BLACK, 0.85f));
    drawText(text, box.x + 8, box.y + 4, 14, COLOUR_TEXT);
}

// A table of variables, like the Name / Type / Value tables in the Pointers lecture
void drawDataTableHeader(float x, float y) {
    drawBold("Name", x, y, 15, COLOUR_MUTED);
    drawBold("Type", x + 140, y, 15, COLOUR_MUTED);
    drawBold("Value", x + 260, y, 15, COLOUR_MUTED);
    DrawLine((int)x, (int)y + 20, (int)x + 470, (int)y + 20, Color{ 70, 75, 90, 255 });
}

void drawDataRow(float x, float y, const char *name, const char *type, const char *value) {
    drawText(name, x, y, 15, COLOUR_TEXT);
    drawText(type, x + 140, y, 15, Color{ 78, 201, 176, 255 });
    drawText(value, x + 260, y, 15, COLOUR_ACCENT);
}

// The power bar (power goes from 0 to 100). It fills smoothly, and its end
// glows while the value is changing (when *powerPtr writes a new value).
float barShown = 0;
float barLast = 0;
float barGlow = 0;

void drawPowerBar(float x, float y, float width, float power, bool fromCode, bool showPointer) {
    float dt = GetFrameTime();
    if (fabsf(power - barLast) > 0.01f) barGlow = 1;
    barLast = power;
    barGlow = fmaxf(0, barGlow - 3 * dt);
    barShown = smoothTowards(barShown, power, 14, dt);

    const char *label = "Power";
    if (fromCode) label = "Power (set by your code)";
    drawText(label, x, y, 15, COLOUR_MUTED);

    Rectangle back = { x, y + 22, width, 22 };
    DrawRectangleRounded(back, 0.5f, 8, COLOUR_CODE_BG);
    float fillWidth = width * barShown / 100;
    if (fillWidth > 2) {
        beginClip(x, y + 22, fillWidth, 22);
        DrawRectangleGradientH((int)x, (int)(y + 22), (int)width, 22, Color{ 90, 200, 90, 255 }, Color{ 240, 110, 60, 255 });
        EndScissorMode();
        // The end of the bar glows while it fills
        Vector2 end = { x + fillWidth, y + 33 };
        DrawCircleV(end, 14, Fade(Color{ 255, 220, 150, 255 }, 0.18f * barGlow));
        DrawRectangleRec(Rectangle{ end.x - 2, y + 22, 3, 22 }, Fade(WHITE, 0.4f + 0.6f * barGlow));
    }
    DrawRectangleRoundedLinesEx(back, 0.5f, 8, 1, Color{ 90, 95, 110, 255 });
    drawText(TextFormat("%d", (int)roundf(barShown)), x + width + 10, y + 24, 16, COLOUR_TEXT);

    // Level 1: the game reads the power through a pointer
    if (showPointer) {
        drawText("int *powerPtr = &power;", x, y + 50, 14, COLOUR_MUTED);
    }
}
