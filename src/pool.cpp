#include "pool.h"
#include "lab1.h"
#include "lab2.h"
#include "ui.h"
#include <cmath>
#include <cstdio>

// Colours of balls 1 to 7. Balls 9 to 15 use the same colours as stripes.
Color ballColours[8] = {
    { 245, 245, 240, 255 },   // 0 cue ball (white)
    { 245, 195, 25, 255 },    // 1 yellow
    { 35, 85, 200, 255 },     // 2 blue
    { 215, 40, 40, 255 },     // 3 red
    { 105, 45, 150, 255 },    // 4 purple
    { 240, 125, 25, 255 },    // 5 orange
    { 25, 135, 70, 255 },     // 6 green
    { 135, 30, 40, 255 },     // 7 maroon
};

// The rack, front to back: 1 at the front, 8 in the middle,
// one solid (6) and one stripe (15) in the back corners.
int startRack[RACK_SIZE] = { 1, 9, 2, 10, 8, 3, 11, 4, 12, 5, 6, 13, 7, 14, 15 };

// The 6 pockets: 3 along the top cushion and 3 along the bottom
float pocketX[6] = { FELT_LEFT, FELT_LEFT + FELT_WIDTH / 2, FELT_LEFT + FELT_WIDTH,
                     FELT_LEFT, FELT_LEFT + FELT_WIDTH / 2, FELT_LEFT + FELT_WIDTH };
float pocketY[6] = { FELT_TOP, FELT_TOP, FELT_TOP,
                     FELT_TOP + FELT_HEIGHT, FELT_TOP + FELT_HEIGHT, FELT_TOP + FELT_HEIGHT };

// ============================================================
// Small helpers
// ============================================================

// Level 4: the function the game calls when you pot one of your balls
void addPoint(int *score) {
    (*score)++;
}

// Ball n lives at BASE_ADDRESS + n * sizeof(Ball), just like real pointer arithmetic
int addressOf(int ballNumber) {
    return BASE_ADDRESS + ballNumber * (int)sizeof(Ball);
}

char groupOf(int ballNumber) {
    if (ballNumber == 0) return 'C';
    if (ballNumber == 8) return '8';
    if (ballNumber < 8) return 'S';
    return 'T';
}

const char *groupName(char group) {
    if (group == 'S') return "solids";
    return "stripes";
}

// Counts how many balls of a group are still on the table.
// Each ball on the table goes into an array as 1 (solid) or 2 (stripe),
// then Lab 1's frequencyCount does the counting.
int ballsLeft(Game *game, char group) {
    int types[RACK_SIZE];
    int count = 0;
    for (int n = 1; n < BALL_COUNT; n++) {
        if (!game->balls[n].pocketed && n != 8) {
            if (n < 8) types[count] = 1;
            else types[count] = 2;
            count++;
        }
    }

    int wanted = 1;
    if (group == 'T') wanted = 2;
    return frequencyCount(count, types, wanted);
}

// The highest-numbered ball still on the table, using Lab 2's find_max
int highestBallLeft(Game *game) {
    int numbers[RACK_SIZE];
    int count = 0;
    for (int n = 1; n < BALL_COUNT; n++) {
        if (!game->balls[n].pocketed) {
            numbers[count] = n;
            count++;
        }
    }
    if (count == 0) return 0;
    return find_max(numbers, count);
}

// Works out where rack slot number 'slot' sits on the table.
// The triangle has 5 rows: 1 ball, then 2, 3, 4 and 5.
// x and y are passed by pointer so this function can fill them in.
void getSlotPosition(int slot, float *x, float *y) {
    float gap = BALL_RADIUS * 2 + 1;   // ball width plus a tiny space
    int s = 0;
    for (int row = 0; row < 5; row++) {
        for (int i = 0; i <= row; i++) {
            if (s == slot) {
                *x = RACK_X + row * gap * 0.87f;        // 0.87 packs the rows together
                *y = RACK_Y + (i - row / 2.0f) * gap;   // centre each row
                return;
            }
            s++;
        }
    }
}

// Is (x, y) clear of every ball on the table (apart from ball 'ignore')?
bool spotIsFree(Game *game, float x, float y, int ignore) {
    for (int n = 0; n < BALL_COUNT; n++) {
        Ball *ball = &game->balls[n];
        if (n != ignore && !ball->pocketed) {
            float dx = ball->x - x;
            float dy = ball->y - y;
            if (sqrtf(dx * dx + dy * dy) < BALL_RADIUS * 2 + 1) return false;
        }
    }
    return true;
}

// Puts a ball back on the table at (x, y), moving it along if that spot is taken
void respotBall(Game *game, int number, float x, float y, bool moveSideways) {
    Ball *ball = &game->balls[number];
    ball->pocketed = false;
    ball->vx = 0;
    ball->vy = 0;

    float offsets[7] = { 0, 25, -25, 50, -50, 75, -75 };
    for (int i = 0; i < 7; i++) {
        float tryX = x;
        float tryY = y;
        if (moveSideways) tryX = x + offsets[i];
        else tryY = y + offsets[i];

        if (spotIsFree(game, tryX, tryY, number)) {
            ball->x = tryX;
            ball->y = tryY;
            return;
        }
    }
    ball->x = x;   // every spot was taken: use the first one anyway
    ball->y = y;
}

// Adds the status line to the end of the shot log (Lab 2 Part 2)
void addToLog(Game *game) {
    game->shotLog.InsertAtEnd(game->shotNumber);

    // Walk to the node we just added (the last one) and copy the text into it
    DoublyLinkedNode *last = game->shotLog.head;
    while (last->nextPointer != nullptr) {
        last = last->nextPointer;
    }
    copy_string(last->text, game->status);
}

// ============================================================
// Setting up a game
// ============================================================

void newGame(Game *game) {
    // Every ball still and on the table
    for (int n = 0; n < BALL_COUNT; n++) {
        game->balls[n].number = n;
        game->balls[n].vx = 0;
        game->balls[n].vy = 0;
        game->balls[n].pocketed = false;
    }
    game->balls[0].x = CUE_START_X;
    game->balls[0].y = CUE_START_Y;

    // Each slot in the rack array says which ball sits there
    for (int slot = 0; slot < RACK_SIZE; slot++) {
        Ball *ball = &game->balls[startRack[slot]];
        getSlotPosition(slot, &ball->x, &ball->y);
    }

    // Players. Groups start as S and T, but the table is open until the first pot.
    char name1[] = "Player 1";
    char name2[] = "Player 2";
    copy_string(game->players[0].name, name1);
    copy_string(game->players[1].name, name2);
    for (int p = 0; p < 2; p++) {
        game->players[p].score = 0;
        game->players[p].trayCount = 0;
    }
    game->players[0].group = 'S';
    game->players[1].group = 'T';

    game->currentPlayer = 0;
    game->waitingPlayer = 1;
    game->tableOpen = true;
    game->isBreak = true;
    game->calledBall = -1;
    game->lastPotted = nullptr;
    game->pottedThisShotCount = 0;
    game->ownLeftBeforeShot = 7;
    game->shotNumber = 0;
    game->gameOver = false;
    game->winner = -1;
    game->winMessage[0] = '\0';
    game->reason[0] = '\0';

    // Start a fresh shot log (clear() deletes the old nodes)
    game->shotLog.clear();
    int duplicates = countDuplicates(RACK_SIZE, startRack);   // Lab 1 Q14
    snprintf(game->status, sizeof(game->status),
             "Rack checked: %d duplicate balls. Player 1 breaks.", duplicates);
    addToLog(game);
}

// Can the current player call this ball? If not, 'why' gets a friendly reason.
bool callIsAllowed(Game *game, int ballNumber, char *why, int whySize) {
    Player *shooter = &game->players[game->currentPlayer];
    char group = shooter->group;

    if (ballNumber == 0) {
        snprintf(why, whySize, "Ball 0 is the cue ball. Call the ball you want to pot.");
        return false;
    }
    if (ballNumber < 0 || ballNumber > 15) {
        snprintf(why, whySize, "There is no ball %d. The balls go from 1 to 15.", ballNumber);
        return false;
    }
    if (game->balls[ballNumber].pocketed) {
        snprintf(why, whySize, "Ball %d is already potted. Pick a ball that's still on the table.", ballNumber);
        return false;
    }

    if (ballNumber == 8) {
        if (game->tableOpen) {
            snprintf(why, whySize, "You can't call the 8 yet. First pot all of your group.");
            return false;
        }
        if (ballsLeft(game, group) > 0) {
            snprintf(why, whySize, "You can only call the 8 once all your %s are down.", groupName(group));
            return false;
        }
        return true;
    }

    // While the table is open you can call any ball apart from the 8
    if (game->tableOpen) return true;

    if (ballsLeft(game, group) == 0) {
        snprintf(why, whySize, "All your %s are down. Call the 8-ball!", groupName(group));
        return false;
    }
    if (groupOf(ballNumber) != group) {
        const char *range = "1 to 7";
        if (group == 'T') range = "9 to 15";
        snprintf(why, whySize, "Ball %d is one of the %s, but you're on %s. Pick a ball from %s.",
                 ballNumber, groupName(groupOf(ballNumber)), groupName(group), range);
        return false;
    }
    return true;
}

// ============================================================
// Shooting and physics
// ============================================================

// power goes from 0 to 100
void startShot(Game *game, float aimX, float aimY, float power) {
    Ball *cue = &game->balls[0];
    float dx = aimX - cue->x;
    float dy = aimY - cue->y;
    float length = sqrtf(dx * dx + dy * dy);

    if (length > 0) {
        // Divide by length to get a direction of size 1, then scale by the speed
        float speed = MAX_SPEED * power / 100;
        cue->vx = dx / length * speed;
        cue->vy = dy / length * speed;
    }

    // Remember how things were before the shot, for the rules afterwards
    game->pottedThisShotCount = 0;
    Player *shooter = &game->players[game->currentPlayer];
    if (game->tableOpen) game->ownLeftBeforeShot = 7;
    else game->ownLeftBeforeShot = ballsLeft(game, shooter->group);
    game->shotNumber++;
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

void checkPockets(Game *game, Ball *ball) {
    for (int p = 0; p < 6; p++) {
        float dx = ball->x - pocketX[p];
        float dy = ball->y - pocketY[p];

        if (sqrtf(dx * dx + dy * dy) < POCKET_RADIUS) {
            ball->pocketed = true;
            ball->vx = 0;
            ball->vy = 0;

            // Remember which object balls went down this shot
            if (ball->number != 0) {
                game->pottedThisShot[game->pottedThisShotCount] = ball->number;
                game->pottedThisShotCount++;
            }
            return;
        }
    }
}

void handleCollisions(Game *game) {
    for (int i = 0; i < BALL_COUNT; i++) {
        for (int j = i + 1; j < BALL_COUNT; j++) {
            Ball *a = &game->balls[i];
            Ball *b = &game->balls[j];
            if (a->pocketed || b->pocketed) continue;

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

void updateBalls(Game *game, float dt) {
    float stepTime = dt / PHYSICS_STEPS;

    for (int step = 0; step < PHYSICS_STEPS; step++) {
        for (int n = 0; n < BALL_COUNT; n++) {
            Ball *ball = &game->balls[n];
            if (!ball->pocketed) {
                ball->x += ball->vx * stepTime;
                ball->y += ball->vy * stepTime;
                bounceOffCushions(ball);
                checkPockets(game, ball);
            }
        }
        handleCollisions(game);
    }

    // Friction slows every ball down a little each frame
    for (int n = 0; n < BALL_COUNT; n++) {
        Ball *ball = &game->balls[n];
        ball->vx *= FRICTION;
        ball->vy *= FRICTION;
        float speed = sqrtf(ball->vx * ball->vx + ball->vy * ball->vy);
        if (speed < STOP_SPEED) {
            ball->vx = 0;
            ball->vy = 0;
        }
    }
}

bool ballsMoving(Game *game) {
    for (int n = 0; n < BALL_COUNT; n++) {
        Ball *ball = &game->balls[n];
        if (!ball->pocketed && (ball->vx != 0 || ball->vy != 0)) return true;
    }
    return false;
}

// ============================================================
// The 8-ball rules, applied once the balls stop
// ============================================================

// Ends the game. The win message is built with Lab 2's copy_string and concat_strings.
void endGame(Game *game, int winner, const char *reason) {
    game->gameOver = true;
    game->winner = winner;

    char wins[] = " wins!";
    copy_string(game->winMessage, game->players[winner].name);
    concat_strings(game->winMessage, wins);

    snprintf(game->reason, sizeof(game->reason), "%s", reason);
    snprintf(game->status, sizeof(game->status), "%s %s", game->winMessage, reason);
    addToLog(game);
}

bool wasPottedThisShot(Game *game, int number) {
    for (int i = 0; i < game->pottedThisShotCount; i++) {
        if (game->pottedThisShot[i] == number) return true;
    }
    return false;
}

// Returns true if the shooter legally potted one of their own balls
// (the game uses that to unlock the next concept).
bool endOfShot(Game *game) {
    Player *shooter = &game->players[game->currentPlayer];
    Player *other = &game->players[game->waitingPlayer];
    bool scratch = game->balls[0].pocketed;
    char reason[100];

    // ---------- The 8-ball ----------
    if (wasPottedThisShot(game, 8)) {
        if (game->isBreak) {
            // Friendly rule: the 8 on the break just goes back on its spot
            respotBall(game, 8, RACK_X, RACK_Y, true);
        } else {
            bool groupCleared = !game->tableOpen && game->ownLeftBeforeShot == 0;
            if (groupCleared && !scratch) {
                snprintf(reason, sizeof(reason), "%s potted the 8-ball.", shooter->name);
                endGame(game, game->currentPlayer, reason);
            } else if (scratch) {
                snprintf(reason, sizeof(reason), "%s potted the cue ball on the 8.", shooter->name);
                endGame(game, game->waitingPlayer, reason);
            } else {
                snprintf(reason, sizeof(reason), "%s potted the 8 before clearing their group.", shooter->name);
                endGame(game, game->waitingPlayer, reason);
            }
            return false;
        }
    }

    // ---------- Decide the groups on the first pot ----------
    if (game->tableOpen && !scratch) {
        int first = -1;
        for (int i = 0; i < game->pottedThisShotCount && first == -1; i++) {
            if (game->pottedThisShot[i] != 8) first = game->pottedThisShot[i];
        }
        if (first != -1) {
            // Groups start as 'S' and 'T'. If the shooter has the wrong one, swap them (Lab 2 Q2).
            if (shooter->group != groupOf(first)) {
                swap_chars(&shooter->group, &other->group);
            }
            game->tableOpen = false;
        }
    }

    // ---------- Give each potted ball to its owner ----------
    bool pottedOwn = false;
    for (int i = 0; i < game->pottedThisShotCount; i++) {
        int number = game->pottedThisShot[i];
        if (number == 8) continue;

        game->lastPotted = &game->balls[number];   // Level 5: no longer nullptr

        if (!game->tableOpen) {
            Player *owner = other;
            if (groupOf(number) == shooter->group) {
                owner = shooter;
                pottedOwn = true;
            }
            addPoint(&owner->score);   // Level 4
            owner->tray[owner->trayCount] = number;
            owner->trayCount++;
        }
    }

    // ---------- Shoot again, or pass the turn? ----------
    bool keepTurn;
    if (scratch) {
        keepTurn = false;
        snprintf(game->status, sizeof(game->status), "Shot %d: Foul! %s potted the cue ball. %s's turn.",
                 game->shotNumber, shooter->name, other->name);
    } else if (game->isBreak) {
        keepTurn = game->pottedThisShotCount > 0;
        snprintf(game->status, sizeof(game->status), "Shot %d: %s broke and potted %d ball(s).",
                 game->shotNumber, shooter->name, game->pottedThisShotCount);
    } else if (game->calledBall != -1) {
        // A ball was called: you only shoot again if that ball went down
        keepTurn = wasPottedThisShot(game, game->calledBall);
        if (keepTurn) {
            snprintf(game->status, sizeof(game->status), "Shot %d: %s potted the %d they called. Shoot again!",
                     game->shotNumber, shooter->name, game->calledBall);
        } else {
            snprintf(game->status, sizeof(game->status), "Shot %d: %s missed the %d they called.",
                     game->shotNumber, shooter->name, game->calledBall);
        }
    } else {
        keepTurn = pottedOwn;
        if (keepTurn) {
            snprintf(game->status, sizeof(game->status), "Shot %d: %s potted one of theirs. Shoot again!",
                     game->shotNumber, shooter->name);
        } else {
            snprintf(game->status, sizeof(game->status), "Shot %d: %s didn't pot one of theirs.",
                     game->shotNumber, shooter->name);
        }
    }
    addToLog(game);

    if (!keepTurn) {
        swap_ints(&game->currentPlayer, &game->waitingPlayer);   // Lab 2 Q1
    }

    // After a scratch the cue ball goes back on the start line
    if (scratch) {
        respotBall(game, 0, CUE_START_X, CUE_START_Y, false);
    }

    game->isBreak = false;
    game->calledBall = -1;
    return pottedOwn && !scratch;
}

// ============================================================
// Drawing
// ============================================================

void drawTable() {
    Color wood = { 110, 62, 30, 255 };
    Color woodDark = { 70, 38, 18, 255 };
    Color cushion = { 18, 95, 55, 255 };
    Color felt = { 28, 125, 72, 255 };
    Color feltLight = { 38, 140, 82, 255 };

    // Wooden rail with a darker edge
    Rectangle outer = { (float)(FELT_LEFT - RAIL), (float)(FELT_TOP - RAIL),
                        (float)(FELT_WIDTH + RAIL * 2), (float)(FELT_HEIGHT + RAIL * 2) };
    DrawRectangleRounded(outer, 0.08f, 8, woodDark);
    Rectangle inner = { outer.x + 3, outer.y + 3, outer.width - 6, outer.height - 6 };
    DrawRectangleRounded(inner, 0.08f, 8, wood);

    // Cushions: a dark green band just inside the wood
    DrawRectangle(FELT_LEFT - 8, FELT_TOP - 8, FELT_WIDTH + 16, FELT_HEIGHT + 16, cushion);

    // Felt, slightly lighter in the middle
    DrawRectangleGradientV(FELT_LEFT, FELT_TOP, FELT_WIDTH, FELT_HEIGHT / 2, felt, feltLight);
    DrawRectangleGradientV(FELT_LEFT, FELT_TOP + FELT_HEIGHT / 2, FELT_WIDTH, FELT_HEIGHT / 2, feltLight, felt);

    // Faint start line for the cue ball
    DrawLine((int)CUE_START_X, FELT_TOP, (int)CUE_START_X, FELT_TOP + FELT_HEIGHT, Fade(WHITE, 0.15f));

    // Diamonds (markers) on the rails
    for (int i = 1; i < 8; i++) {
        if (i == 4) continue;   // the middle pocket is there
        float x = FELT_LEFT + FELT_WIDTH * i / 8.0f;
        DrawCircleV(Vector2{ x, (float)(FELT_TOP - RAIL / 2 - 4) }, 2.5f, Fade(WHITE, 0.7f));
        DrawCircleV(Vector2{ x, (float)(FELT_TOP + FELT_HEIGHT + RAIL / 2 + 4) }, 2.5f, Fade(WHITE, 0.7f));
    }
    for (int i = 1; i < 4; i++) {
        float y = FELT_TOP + FELT_HEIGHT * i / 4.0f;
        DrawCircleV(Vector2{ (float)(FELT_LEFT - RAIL / 2 - 4), y }, 2.5f, Fade(WHITE, 0.7f));
        DrawCircleV(Vector2{ (float)(FELT_LEFT + FELT_WIDTH + RAIL / 2 + 4), y }, 2.5f, Fade(WHITE, 0.7f));
    }

    // Pockets: a few circles from grey to black give a simple shading
    for (int p = 0; p < 6; p++) {
        Vector2 centre = { pocketX[p], pocketY[p] };
        DrawCircleV(centre, POCKET_RADIUS + 3, Color{ 45, 30, 20, 255 });
        DrawCircleV(centre, POCKET_RADIUS, Color{ 40, 40, 40, 255 });
        DrawCircleV(centre, POCKET_RADIUS * 0.8f, Color{ 22, 22, 22, 255 });
        DrawCircleV(centre, POCKET_RADIUS * 0.55f, BLACK);
    }
}

// Draws one ball with a shadow, stripe, number and a small shine
void drawBall(int number, float x, float y, float radius) {
    Vector2 centre = { x, y };
    float r = radius;

    // Shadow, a little down and to the right
    DrawCircleV(Vector2{ x + r * 0.15f, y + r * 0.25f }, r, Fade(BLACK, 0.35f));

    if (number == 0) {
        DrawCircleV(centre, r, ballColours[0]);
    } else if (number == 8) {
        DrawCircleV(centre, r, Color{ 20, 20, 20, 255 });
        DrawRing(centre, r - 1.5f, r, 0, 360, 32, Fade(WHITE, 0.3f));   // faint rim so it shows on dark backgrounds
    } else if (number < 8) {
        DrawCircleV(centre, r, ballColours[number]);
    } else {
        // Stripe: a white ball with a coloured band across the middle.
        // Each row of the band is as wide as the circle is at that height.
        DrawCircleV(centre, r, ballColours[0]);
        int band = (int)(r * 0.55f);
        for (int dy = -band; dy <= band; dy++) {
            float halfWidth = sqrtf(r * r - dy * dy);
            DrawRectangle((int)(x - halfWidth), (int)y + dy, (int)(halfWidth * 2), 1, ballColours[number - 8]);
        }
    }

    // Number on a small white circle (the cue ball has no number)
    if (number != 0) {
        DrawCircleV(centre, r * 0.52f, ballColours[0]);
        const char *text = TextFormat("%d", number);
        float size = r * 0.85f;
        float width = textWidth(text, size);
        drawBold(text, x - width / 2, y - size / 2, size, BLACK);
    }

    // Shine in the top-left
    DrawCircleV(Vector2{ x - r * 0.35f, y - r * 0.4f }, r * 0.22f, Fade(WHITE, 0.55f));
}

void drawBalls(Game *game) {
    for (int n = 0; n < BALL_COUNT; n++) {
        Ball *ball = &game->balls[n];
        if (!ball->pocketed) drawBall(ball->number, ball->x, ball->y, BALL_RADIUS);
    }
}

// A pulsing yellow ring around the ball that target points at
void drawTargetRing(Game *game) {
    if (game->calledBall < 1) return;
    Ball *target = &game->balls[game->calledBall];
    if (target->pocketed) return;

    float pulse = 2 + 2 * sinf((float)GetTime() * 6);
    Vector2 centre = { target->x, target->y };
    DrawRing(centre, BALL_RADIUS + 3 + pulse, BALL_RADIUS + 5 + pulse, 0, 360, 32, YELLOW);
}

// Aim dots from the cue ball towards the mouse, and the cue stick behind the ball
void drawCueStick(Game *game, float power) {
    Ball *cue = &game->balls[0];
    Vector2 mouse = GetMousePosition();
    float dx = mouse.x - cue->x;
    float dy = mouse.y - cue->y;
    float length = sqrtf(dx * dx + dy * dy);
    if (length == 0) return;
    dx = dx / length;   // direction with a length of 1
    dy = dy / length;

    // Dotted aim line
    for (int i = 1; i <= 22; i++) {
        float d = BALL_RADIUS + i * 12;
        DrawCircleV(Vector2{ cue->x + dx * d, cue->y + dy * d }, 1.6f, Fade(WHITE, 0.7f));
    }

    // The stick is pulled back further the more power there is
    float pullBack = BALL_RADIUS + 6 + power * 0.6f;
    Vector2 tip = { cue->x - dx * pullBack, cue->y - dy * pullBack };
    Vector2 end = { tip.x - dx * 300, tip.y - dy * 300 };
    Vector2 tipEnd = { tip.x - dx * 8, tip.y - dy * 8 };
    DrawLineEx(end, tip, 7, Color{ 205, 160, 100, 255 });
    DrawLineEx(tipEnd, tip, 7, Color{ 90, 140, 200, 255 });   // chalked tip
}
