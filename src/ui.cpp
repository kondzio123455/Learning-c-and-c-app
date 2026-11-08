#include "ui.h"
#include "lab2.h"
#include <cstdio>
#include <cmath>

// ============================================================
// Fonts and text
// ============================================================

Font regularFont;
Font boldFont;

void loadFonts() {
    // Loaded large, with mipmaps, so they stay sharp when drawn small
    regularFont = LoadFontEx("assets/JetBrainsMono-Regular.ttf", 48, nullptr, 0);
    boldFont = LoadFontEx("assets/JetBrainsMono-Bold.ttf", 48, nullptr, 0);
    GenTextureMipmaps(&regularFont.texture);
    GenTextureMipmaps(&boldFont.texture);
    SetTextureFilter(regularFont.texture, TEXTURE_FILTER_TRILINEAR);
    SetTextureFilter(boldFont.texture, TEXTURE_FILTER_TRILINEAR);
}

void unloadFonts() {
    UnloadFont(regularFont);
    UnloadFont(boldFont);
}

void drawText(const char *text, float x, float y, float size, Color colour) {
    DrawTextEx(regularFont, text, Vector2{ x, y }, size, 0, colour);
}

void drawBold(const char *text, float x, float y, float size, Color colour) {
    DrawTextEx(boldFont, text, Vector2{ x, y }, size, 0, colour);
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

bool button(Rectangle area, const char *label) {
    bool hover = CheckCollisionPointRec(GetMousePosition(), area);
    Color colour = COLOUR_BUTTON;
    if (hover) colour = COLOUR_BUTTON_HOVER;

    DrawRectangleRounded(area, 0.25f, 8, colour);
    float size = 18;
    float width = textWidth(label, size);
    drawBold(label, area.x + (area.width - width) / 2, area.y + (area.height - size) / 2, size, COLOUR_TEXT);

    return hover && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
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
    Vector2 mouse = GetMousePosition();
    for (int n = 0; n < BALL_COUNT; n++) {
        Ball *ball = &game->balls[n];
        if (ball->pocketed) continue;

        float dx = mouse.x - ball->x;
        float dy = mouse.y - ball->y;
        if (dx * dx + dy * dy < BALL_RADIUS * BALL_RADIUS) {
            const char *text = TextFormat("balls[%d] at address %d", n, addressOf(n));
            if (n == 0) text = TextFormat("balls[0] (cue ball) at address %d", addressOf(0));
            float width = textWidth(text, 14) + 16;
            Rectangle box = { ball->x - width / 2, ball->y - 42, width, 24 };
            DrawRectangleRounded(box, 0.3f, 6, Fade(BLACK, 0.85f));
            drawText(text, box.x + 8, box.y + 4, 14, COLOUR_TEXT);
            return;
        }
    }
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

// The power bar (power goes from 0 to 100)
void drawPowerBar(float x, float y, float width, float power, bool fromCode, bool showPointer) {
    const char *label = "Power";
    if (fromCode) label = "Power (set by your code)";
    drawText(label, x, y, 15, COLOUR_MUTED);

    Rectangle back = { x, y + 22, width, 22 };
    DrawRectangleRec(back, COLOUR_CODE_BG);
    Rectangle fill = { x, y + 22, width * power / 100, 22 };
    DrawRectangleGradientH((int)fill.x, (int)fill.y, (int)fill.width, (int)fill.height,
                           Color{ 90, 200, 90, 255 }, Color{ 240, 110, 60, 255 });
    DrawRectangleLinesEx(back, 1, Color{ 90, 95, 110, 255 });
    drawText(TextFormat("%d", (int)power), x + width + 10, y + 24, 16, COLOUR_TEXT);

    // Level 1: the game reads the power through a pointer
    if (showPointer) {
        drawText("int *powerPtr = &power;", x, y + 50, 14, COLOUR_MUTED);
    }
}
