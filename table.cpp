#include "table.h"
#include <cmath>

// Colours for balls 1 to 7. Balls 9 to 15 use the same colours as stripes.
// Index 0 is not used (the cue ball is always white).
Color ballColours[8] = { WHITE, GOLD, BLUE, RED, DARKPURPLE, ORANGE, DARKGREEN, MAROON };

Color FELT_COLOUR = { 30, 120, 75, 255 };
Color RAIL_COLOUR = { 100, 60, 30, 255 };

// The 6 pockets: 3 along the top cushion and 3 along the bottom
float pocketX[6] = { FELT_LEFT, FELT_LEFT + FELT_WIDTH / 2, FELT_LEFT + FELT_WIDTH,
                     FELT_LEFT, FELT_LEFT + FELT_WIDTH / 2, FELT_LEFT + FELT_WIDTH };
float pocketY[6] = { FELT_TOP, FELT_TOP, FELT_TOP,
                     FELT_TOP + FELT_HEIGHT, FELT_TOP + FELT_HEIGHT, FELT_TOP + FELT_HEIGHT };

// ============================================================
// Drawing
// ============================================================

void drawTable() {
    // Wooden rail, then the felt on top of it
    DrawRectangle(FELT_LEFT - RAIL, FELT_TOP - RAIL, FELT_WIDTH + RAIL * 2, FELT_HEIGHT + RAIL * 2, RAIL_COLOUR);
    DrawRectangle(FELT_LEFT, FELT_TOP, FELT_WIDTH, FELT_HEIGHT, FELT_COLOUR);

    // Faint line where the cue ball starts (the "baulk line")
    DrawLine((int)CUE_START_X, FELT_TOP, (int)CUE_START_X, FELT_TOP + FELT_HEIGHT, Fade(WHITE, 0.2f));

    // Pockets
    for (int p = 0; p < 6; p++) {
        Vector2 centre = { pocketX[p], pocketY[p] };
        DrawCircleV(centre, POCKET_RADIUS, BLACK);
    }
}

// Draws one ball. Used for the rack preview, the table and the potted row.
void drawBall(int number, float x, float y) {
    Vector2 centre = { x, y };
    float r = BALL_RADIUS;

    if (number == 0) {
        // Cue ball: plain white
        DrawCircleV(centre, r, RAYWHITE);
    } else if (number == 8) {
        // 8-ball: black
        DrawCircleV(centre, r, BLACK);
    } else if (number < 8) {
        // Solids: balls 1 to 7
        DrawCircleV(centre, r, ballColours[number]);
    } else {
        // Stripes: balls 9 to 15 are white with a coloured band
        DrawCircleV(centre, r, RAYWHITE);
        DrawRectangle((int)(x - r * 0.87f), (int)(y - r * 0.5f), (int)(r * 1.74f), (int)r, ballColours[number - 8]);
    }

    // Number on a small white circle (the cue ball has no number)
    if (number != 0) {
        DrawCircleV(centre, r * 0.6f, RAYWHITE);
        const char *text = TextFormat("%d", number);
        int textWidth = MeasureText(text, 10);
        DrawText(text, (int)x - textWidth / 2, (int)y - 5, 10, BLACK);
    }

    // Dark outline so balls stand out on the felt
    DrawCircleLines((int)x, (int)y, r, Fade(BLACK, 0.6f));
}

// Draws the rack straight from the int array, before any Ball structs exist.
// If the array holds the same number twice, you will see that ball twice.
void drawRackPreview(int rack[]) {
    for (int slot = 0; slot < RACK_SIZE; slot++) {
        float x, y;
        getSlotPosition(slot, &x, &y);
        drawBall(rack[slot], x, y);
    }
    drawBall(0, CUE_START_X, CUE_START_Y);
}

// Draws a label above or below the rack, with a line to one slot.
// Draw this BEFORE the balls, so the line goes under them.
void drawSlotLabel(int slot, const char *label, bool above) {
    float x, y;
    getSlotPosition(slot, &x, &y);

    float labelY;
    if (above) {
        labelY = RACK_Y - 80;
    } else {
        labelY = RACK_Y + 65;
    }

    Vector2 from = { x, y };
    Vector2 to = { x, labelY + 8 };
    DrawLineEx(from, to, 1.5f, YELLOW);

    int textWidth = MeasureText(label, 16);
    DrawText(label, (int)x - textWidth / 2, (int)labelY, 16, YELLOW);
}

// Draws a yellow ring around one slot. Draw this AFTER the balls so it shows on top.
void drawSlotRing(int slot) {
    float x, y;
    getSlotPosition(slot, &x, &y);
    DrawCircleLines((int)x, (int)y, BALL_RADIUS + 3, YELLOW);
    DrawCircleLines((int)x, (int)y, BALL_RADIUS + 4, YELLOW);
}

// Draws the rack array as a row of boxes, like the memory diagrams in the
// Pointers lecture: each box is one int, with its slot number underneath.
void drawRackArray(int rack[], int highlightA, int highlightB) {
    int x = FELT_LEFT - RAIL;
    int y = FELT_TOP + FELT_HEIGHT + RAIL + 90;
    int boxSize = 40;

    DrawText("int rack[15]", x, y - 28, 20, RAYWHITE);

    for (int slot = 0; slot < RACK_SIZE; slot++) {
        int boxX = x + slot * (boxSize + 2);

        Color boxColour = DARKGRAY;
        if (slot == highlightA || slot == highlightB) {
            boxColour = Color{ 120, 110, 30, 255 };   // dark yellow for the slots the code uses
        }

        DrawRectangle(boxX, y, boxSize, boxSize, boxColour);
        DrawRectangleLines(boxX, y, boxSize, boxSize, GRAY);

        // The ball number stored in this slot
        const char *value = TextFormat("%d", rack[slot]);
        int valueWidth = MeasureText(value, 20);
        DrawText(value, boxX + boxSize / 2 - valueWidth / 2, y + 10, 20, RAYWHITE);

        // The slot number underneath
        const char *index = TextFormat("%d", slot);
        int indexWidth = MeasureText(index, 10);
        DrawText(index, boxX + boxSize / 2 - indexWidth / 2, y + boxSize + 4, 10, GRAY);
    }
}

void drawBalls(Ball balls[]) {
    for (int i = 0; i < BALL_COUNT; i++) {
        if (!balls[i].pocketed) {
            drawBall(balls[i].number, balls[i].x, balls[i].y);
        }
    }
}

// Aim line from the cue ball towards the mouse, plus the power bar
void drawAim(Ball *cue, float power) {
    Vector2 mouse = GetMousePosition();
    float dx = mouse.x - cue->x;
    float dy = mouse.y - cue->y;
    float length = sqrtf(dx * dx + dy * dy);

    if (length > 0) {
        Vector2 start = { cue->x, cue->y };
        Vector2 end = { cue->x + dx / length * 200, cue->y + dy / length * 200 };
        DrawLineEx(start, end, 2, Fade(WHITE, 0.6f));
    }

    // Power bar under the table
    int barX = FELT_LEFT + FELT_WIDTH - 200;
    int barY = FELT_TOP + FELT_HEIGHT + RAIL + 25;
    DrawText("Power", barX - 70, barY - 2, 20, RAYWHITE);
    DrawRectangle(barX, barY, (int)(200 * power / MAX_POWER), 18, ORANGE);
    DrawRectangleLines(barX, barY, 200, 18, RAYWHITE);
}

// Row of potted balls under the table
void drawPotted(int potted[], int pottedCount) {
    int y = FELT_TOP + FELT_HEIGHT + RAIL + 25;
    DrawText("Potted:", FELT_LEFT - RAIL, y - 2, 20, RAYWHITE);

    for (int i = 0; i < pottedCount; i++) {
        drawBall(potted[i], FELT_LEFT + 70 + i * 26.0f, y + 9.0f);
    }
}

// ============================================================
// Rack
// ============================================================

// Works out where rack slot number 'slot' sits on the table.
// The triangle has 5 rows: 1 ball, then 2, 3, 4 and 5 (like the Lab 1 triangle).
// x and y are passed by pointer so this function can fill them in.
void getSlotPosition(int slot, float *x, float *y) {
    float gap = BALL_RADIUS * 2 + 1;   // ball width plus a tiny space
    int s = 0;

    for (int row = 0; row < 5; row++) {
        for (int i = 0; i <= row; i++) {
            if (s == slot) {
                *x = RACK_X + row * gap * 0.87f;          // 0.87 packs the rows together like a real rack
                *y = RACK_Y + (i - row / 2.0f) * gap;     // centre each row on the middle of the table
                return;
            }
            s++;
        }
    }
}

// Turns the int rack array into real balls on the table
void placeRack(Ball balls[], int rack[]) {
    // Every ball starts still and on the table
    for (int i = 0; i < BALL_COUNT; i++) {
        balls[i].number = i;
        balls[i].vx = 0;
        balls[i].vy = 0;
        balls[i].pocketed = false;
    }

    // Cue ball on the left
    balls[0].x = CUE_START_X;
    balls[0].y = CUE_START_Y;

    // Each slot in the rack array says which ball sits there
    for (int slot = 0; slot < RACK_SIZE; slot++) {
        Ball *ball = &balls[rack[slot]];
        getSlotPosition(slot, &ball->x, &ball->y);
    }
}

// ============================================================
// Physics
// ============================================================

void shootCueBall(Ball *cue, float targetX, float targetY, float power) {
    float dx = targetX - cue->x;
    float dy = targetY - cue->y;
    float length = sqrtf(dx * dx + dy * dy);

    if (length > 0) {
        // Divide by length to get a direction of size 1, then scale by power
        cue->vx = dx / length * power;
        cue->vy = dy / length * power;
    }
}

void bounceOffCushions(Ball *ball) {
    float left = FELT_LEFT + BALL_RADIUS;
    float right = FELT_LEFT + FELT_WIDTH - BALL_RADIUS;
    float top = FELT_TOP + BALL_RADIUS;
    float bottom = FELT_TOP + FELT_HEIGHT - BALL_RADIUS;

    if (ball->x < left)   { ball->x = left;   ball->vx = -ball->vx * CUSHION_BOUNCE; }
    if (ball->x > right)  { ball->x = right;  ball->vx = -ball->vx * CUSHION_BOUNCE; }
    if (ball->y < top)    { ball->y = top;    ball->vy = -ball->vy * CUSHION_BOUNCE; }
    if (ball->y > bottom) { ball->y = bottom; ball->vy = -ball->vy * CUSHION_BOUNCE; }
}

void checkPockets(Ball *ball, int potted[], int *pottedCount) {
    for (int p = 0; p < 6; p++) {
        float dx = ball->x - pocketX[p];
        float dy = ball->y - pocketY[p];

        if (sqrtf(dx * dx + dy * dy) < POCKET_RADIUS) {
            ball->pocketed = true;
            ball->vx = 0;
            ball->vy = 0;

            // Remember which ball went down (the cue ball doesn't count)
            if (ball->number != 0) {
                potted[*pottedCount] = ball->number;
                (*pottedCount)++;   // same idea as increment(int *numPtr) in the pointers lecture
            }
            return;
        }
    }
}

void handleCollisions(Ball balls[]) {
    for (int i = 0; i < BALL_COUNT; i++) {
        for (int j = i + 1; j < BALL_COUNT; j++) {
            Ball *a = &balls[i];
            Ball *b = &balls[j];

            if (!a->pocketed && !b->pocketed) {
                float dx = b->x - a->x;
                float dy = b->y - a->y;
                float distance = sqrtf(dx * dx + dy * dy);

                if (distance < BALL_RADIUS * 2 && distance > 0) {
                    // Direction from a to b, with a length of 1
                    float nx = dx / distance;
                    float ny = dy / distance;

                    // Push the balls apart so they no longer overlap
                    float overlap = BALL_RADIUS * 2 - distance;
                    a->x -= nx * overlap / 2;
                    a->y -= ny * overlap / 2;
                    b->x += nx * overlap / 2;
                    b->y += ny * overlap / 2;

                    // How fast a is moving towards b along the line between them
                    float closingSpeed = (a->vx - b->vx) * nx + (a->vy - b->vy) * ny;

                    // Balls of equal weight hand that speed over to each other
                    if (closingSpeed > 0) {
                        a->vx -= closingSpeed * nx;
                        a->vy -= closingSpeed * ny;
                        b->vx += closingSpeed * nx;
                        b->vy += closingSpeed * ny;
                    }
                }
            }
        }
    }
}

void updatePhysics(Ball balls[], float dt, int potted[], int *pottedCount) {
    float stepTime = dt / PHYSICS_STEPS;

    for (int step = 0; step < PHYSICS_STEPS; step++) {
        for (int i = 0; i < BALL_COUNT; i++) {
            Ball *ball = &balls[i];
            if (!ball->pocketed) {
                ball->x += ball->vx * stepTime;
                ball->y += ball->vy * stepTime;
                bounceOffCushions(ball);
                checkPockets(ball, potted, pottedCount);
            }
        }
        handleCollisions(balls);
    }

    // Friction slows every ball down a little each frame
    for (int i = 0; i < BALL_COUNT; i++) {
        Ball *ball = &balls[i];
        ball->vx *= FRICTION;
        ball->vy *= FRICTION;

        float speed = sqrtf(ball->vx * ball->vx + ball->vy * ball->vy);
        if (speed < STOP_SPEED) {
            ball->vx = 0;
            ball->vy = 0;
        }
    }
}

bool ballsMoving(Ball balls[]) {
    for (int i = 0; i < BALL_COUNT; i++) {
        if (!balls[i].pocketed && (balls[i].vx != 0 || balls[i].vy != 0)) {
            return true;
        }
    }
    return false;
}
