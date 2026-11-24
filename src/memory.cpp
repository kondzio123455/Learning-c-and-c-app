#include "memory.h"
#include "app.h"
#include "ui.h"
#include "anim.h"
#include "editor.h"
#include <cmath>

// Made-up but consistent addresses, like the lecture's diagrams
// (power is an int: 4 bytes; the pointers are 8 bytes each)
const int POWER_ADDRESS = 2000;
const int POWER_PTR_ADDRESS = 2008;
const int TARGET_ADDRESS = 2016;

const Color TYPE_COLOUR = { 78, 201, 176, 255 };   // teal, like types in the code

// What's on screen counts up or down to the real value instead of jumping
float shownPower = 0;
float shownTarget = 0;
float lastPower = -1;
float writePulse = 0;    // 1 just after *powerPtr wrote a new value, fading back to 0
float packet = 0;        // 0 to 1: where the bright dot is on its way along the arrow

// A point on a curve from a to b that bends towards 'control' (a quadratic Bezier curve)
Vector2 curvePoint(Vector2 a, Vector2 control, Vector2 b, float t) {
    float u = 1 - t;
    return Vector2{ u * u * a.x + 2 * u * t * control.x + t * t * b.x,
                    u * u * a.y + 2 * u * t * control.y + t * t * b.y };
}

// A pointer: a curved arrow from 'from' to 'to'. Dots flow along it all the time
// (it points somewhere), and while something is written through it, it glows and
// a bright dot carries the value to the box it points at.
void drawPointerArrow(Vector2 from, Vector2 to, float bend, float pulse, Color colour) {
    Vector2 control = { (from.x + to.x) / 2, (from.y + to.y) / 2 + bend };
    const int STEPS = 24;
    Vector2 last = from;
    for (int i = 1; i <= STEPS; i++) {
        Vector2 point = curvePoint(from, control, to, (float)i / STEPS);
        if (pulse > 0.01f) DrawLineEx(last, point, 7, Fade(colour, 0.18f * pulse));   // the glow
        DrawLineEx(last, point, 2 + pulse * 1.5f, Fade(colour, 0.75f + 0.25f * pulse));
        last = point;
    }
    // The arrowhead, pointing along the end of the curve
    Vector2 before = curvePoint(from, control, to, 0.9f);
    float dx = to.x - before.x;
    float dy = to.y - before.y;
    float length = sqrtf(dx * dx + dy * dy);
    if (length > 0) {
        dx /= length;
        dy /= length;
        float size = 8 + pulse * 3;
        Vector2 left = { to.x - dx * size - dy * size * 0.6f, to.y - dy * size + dx * size * 0.6f };
        Vector2 right = { to.x - dx * size + dy * size * 0.6f, to.y - dy * size - dx * size * 0.6f };
        DrawTriangle(to, left, right, colour);
        DrawTriangle(to, right, left, colour);   // (whichever way round it is, one of these shows)
    }
    // Dots flowing along it
    for (int k = 0; k < 3; k++) {
        float t = fmodf((float)GetTime() * 0.5f + k / 3.0f, 1.0f);
        DrawCircleV(curvePoint(from, control, to, t), 2, Fade(WHITE, 0.5f));
    }
    // The value travelling along it
    if (pulse > 0.01f) {
        Vector2 dot = curvePoint(from, control, to, packet);
        DrawCircleV(dot, 7, Fade(colour, 0.25f * pulse));
        DrawCircleV(dot, 3.5f, Fade(WHITE, pulse));
    }
}

// One variable: "name : type" above, the value inside, the address in the corner
void drawVariable(Rectangle box, const char *name, const char *type, const char *value, int address, float glow) {
    if (glow > 0.01f) {
        for (int i = 3; i >= 1; i--) {
            Rectangle halo = { box.x - i * 3.0f, box.y - i * 3.0f, box.width + i * 6.0f, box.height + i * 6.0f };
            DrawRectangleRounded(halo, roundnessFor(halo, 8 + i * 3.0f), 8, Fade(COLOUR_ACCENT, 0.07f * glow));
        }
    }
    float roundness = roundnessFor(box, 8);
    DrawRectangleRounded(box, roundness, 8, COLOUR_CODE_BG);
    DrawRectangleRoundedLinesEx(box, roundness, 8, 1, Fade(COLOUR_ACCENT, 0.25f + 0.6f * glow));

    drawText(name, box.x, box.y - 20, 14, COLOUR_TEXT);
    drawText(type, box.x + textWidth(name, 14) + 8, box.y - 20, 13, TYPE_COLOUR);
    drawBold(value, box.x + 10, box.y + 8, 19, COLOUR_ACCENT);
    const char *where = TextFormat("@%d", address);
    drawText(where, box.x + box.width - textWidth(where, 11) - 6, box.y + box.height - 15, 11, COLOUR_MUTED);
}

float drawMemoryPanel(App *app, float x, float y, float width) {
    Game *game = &app->game;
    float dt = GetFrameTime();
    float top = y;

    // Charging the shot writes the power through the pointer: *powerPtr = value
    if (lastPower >= 0 && fabsf(app->power - lastPower) > 0.01f) writePulse = 1;
    lastPower = app->power;
    writePulse = fmaxf(0, writePulse - 2.5f * dt);
    packet = fmodf(packet + 2.2f * dt, 1.0f);
    shownPower = smoothTowards(shownPower, app->power, 12, dt);
    float targetValue = game->calledBall > 0 ? (float)addressOf(game->calledBall) : 0;
    shownTarget = smoothTowards(shownTarget, targetValue, 5, dt);
    if (fabsf(shownTarget - targetValue) < 0.5f) shownTarget = targetValue;

    drawBold("MEMORY", x, y, 13, COLOUR_MUTED);
    drawText("(each box is a variable; arrows are pointers)", x + 66, y + 1, 12, Color{ 100, 100, 110, 255 });
    y += 42;

    bool showTarget = reached(app, CALL_LEVEL);
    float gap = 34;
    float boxWidth = (width - 2 * gap) / 3;
    float boxHeight = 46;
    Rectangle powerBox = { x, y, boxWidth, boxHeight };
    Rectangle pointerBox = { x + boxWidth + gap, y, boxWidth, boxHeight };
    Rectangle targetBox = { x + 2 * (boxWidth + gap), y, boxWidth, boxHeight };

    drawVariable(powerBox, "power", "int", TextFormat("%d", (int)roundf(shownPower)), POWER_ADDRESS, writePulse);
    drawVariable(pointerBox, "powerPtr", "int *", TextFormat("%d", POWER_ADDRESS), POWER_PTR_ADDRESS, 0);

    // powerPtr holds power's address, so its arrow points at power's box
    Vector2 from = { pointerBox.x + 24, pointerBox.y + pointerBox.height };
    Vector2 to = { powerBox.x + powerBox.width - 24, powerBox.y + powerBox.height + 2 };
    drawPointerArrow(from, to, 30, writePulse, COLOUR_ACCENT);

    if (showTarget) {
        const char *value = "nullptr";
        if (shownTarget > 0.5f) value = TextFormat("%d", (int)roundf(shownTarget));
        drawVariable(targetBox, "target", "Ball *", value, TARGET_ADDRESS, game->calledBall > 0 ? 0.3f : 0);
        if (game->calledBall > 0) {
            // *target is the ball it points at (its box is in the row under the table)
            Vector2 start = { targetBox.x + targetBox.width / 2, targetBox.y + targetBox.height };
            Vector2 end = { targetBox.x + targetBox.width / 2 - 6, targetBox.y + targetBox.height + 26 };
            drawPointerArrow(start, end, 0, 0, TYPE_COLOUR);
            drawText(TextFormat("balls[%d]", game->calledBall), end.x + 10, end.y - 9, 14, TYPE_COLOUR);
        }
    }
    y += boxHeight + 36;

    // The line that's doing it, lit up while it writes
    const char *line = TextFormat("*powerPtr = %d;", (int)roundf(app->power));
    Rectangle band = { x - 4, y - 3, textWidth(line, 15) + 16, lineHeight(15) + 2 };
    if (writePulse > 0.01f) DrawRectangleRounded(band, 0.3f, 6, Fade(COLOUR_ACCENT, 0.14f * writePulse));
    drawCode(line, x + 4, y, 15, width);
    drawText("// changes power through the pointer", x + band.width + 6, y + 1, 13, Color{ 106, 153, 85, 255 });
    y += 26;

    return y - top;
}
