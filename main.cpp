#include "raylib.h"
#include "table.h"
#include "level.h"

// Which screen the game is on
enum Screen { SCREEN_LEVEL, SCREEN_PLAY };

// The rack is a plain int array of ball numbers, in rack order.
// Slot 0 is the front ball and slot 4 is the middle of the triangle.
// For Level 1 the 8-ball starts in the wrong place (slot 0).
int startRack[RACK_SIZE] = { 8, 2, 3, 9, 1, 10, 11, 4, 5, 12, 6, 13, 7, 14, 15 };

void resetRack(int rack[]) {
    for (int i = 0; i < RACK_SIZE; i++) {
        rack[i] = startRack[i];
    }
}

int main() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Pointer Pool");
    SetTargetFPS(60);   // the friction is tuned for 60 frames per second

    Level level = getLevel1();
    Screen screen = SCREEN_LEVEL;

    int rack[RACK_SIZE];
    resetRack(rack);

    Ball balls[BALL_COUNT];
    int potted[RACK_SIZE];
    int pottedCount = 0;

    int pickedChoice = -1;   // -1 means no answer picked yet
    const char *message = "Click an answer to fill in the blank.";
    float power = 0;

    while (!WindowShouldClose()) {
        // ==================== Update ====================
        if (screen == SCREEN_LEVEL) {
            int clicked = getClickedChoice(level);
            if (clicked != -1) {
                pickedChoice = clicked;
                resetRack(rack);   // every try starts from the original rack
                message = runLevel1Choice(clicked, rack);
            }

            // Only allow the break once the rack is correct
            if (pickedChoice == level.correctChoice && IsKeyPressed(KEY_SPACE)) {
                placeRack(balls, rack);
                pottedCount = 0;
                screen = SCREEN_PLAY;
                message = "Aim with the mouse.\n"
                          "Hold the left button to charge,\n"
                          "release to shoot.\n\n"
                          "Press R to go back to the level.";
            }
        } else {
            updatePhysics(balls, GetFrameTime(), potted, &pottedCount);

            // You can only shoot once everything has stopped
            if (!ballsMoving(balls)) {
                Ball *cue = &balls[0];

                // Put the cue ball back if it went down a pocket
                if (cue->pocketed) {
                    cue->x = CUE_START_X;
                    cue->y = CUE_START_Y;
                    cue->pocketed = false;
                }

                Vector2 mouse = GetMousePosition();
                if (mouse.x < PANEL_X && IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
                    power += MAX_POWER * GetFrameTime();   // takes 1 second to fill
                    if (power > MAX_POWER) {
                        power = MAX_POWER;
                    }
                }

                if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && power > 0) {
                    shootCueBall(cue, mouse.x, mouse.y, power);
                    power = 0;
                }
            }

            // R goes back to the level with the original rack
            if (IsKeyPressed(KEY_R)) {
                screen = SCREEN_LEVEL;
                pickedChoice = -1;
                resetRack(rack);
                pottedCount = 0;
                power = 0;
                message = "Click an answer to fill in the blank.";
            }
        }

        // ==================== Draw ====================
        BeginDrawing();
        ClearBackground(Color{ 28, 28, 34, 255 });

        DrawText("Pointer Pool", FELT_LEFT - RAIL, 30, 40, RAYWHITE);
        DrawText("Learn C/C++ pointers one shot at a time", FELT_LEFT - RAIL, 80, 20, GRAY);

        drawTable();

        if (screen == SCREEN_LEVEL) {
            drawSlotLabel(0, "rack", true);        // labels first so their lines go under the balls
            drawSlotLabel(4, "rack + 4", false);
            drawRackPreview(rack);
            drawSlotRing(0);                       // rings last so they show on top
            drawSlotRing(4);
        } else {
            drawBalls(balls);
            if (!ballsMoving(balls)) {
                drawAim(&balls[0], power);
            }
        }

        drawPotted(potted, pottedCount);
        drawRackArray(rack, 0, 4);
        drawCodePanel(level, pickedChoice, message);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
