#include "raylib.h"
#include "app.h"
#include "ui.h"

// The whole app's state. Every screen function gets a pointer to it.
App app;

void setupApp(App *app) {
    app->screen = SCREEN_MENU;
    app->phase = PHASE_CODE;
    app->quit = false;
    app->gameInProgress = false;

    app->levelsDone = loadProgress();
    app->lessonShown = false;
    app->askCount = 0;
    app->questionLevel = 1;
    app->version = 0;
    app->reviewMode = false;
    app->callNeeded = false;

    setupCodeBox(&app->answerBox, 4, "Click here and type your code...");
    setupCodeBox(&app->callBox, 1, "Ball *target = &balls[3];");
    app->feedback[0] = '\0';
    app->feedbackGood = false;

    app->runTimer = 0;
    app->power = 0;
    app->powerFromCode = false;
    app->charging = false;

    app->codexLevel = 1;
    app->confirmReset = false;
    app->logView = nullptr;
}

// ============================================================
// Main menu
// ============================================================

void menuScreen(App *app) {
    ClearBackground(COLOUR_BG);

    drawBold("POINTER POOL", 90, 120, 64, COLOUR_ACCENT);
    drawText("Learn C and C++ pointers, one shot at a time.", 94, 200, 20, COLOUR_TEXT);
    drawText("Based on the Pointers lectures and Labs 1 and 2.", 94, 230, 16, COLOUR_MUTED);

    // A little rack of balls for decoration
    for (int row = 0; row < 5; row++) {
        for (int i = 0; i <= row; i++) {
            int number = row * (row + 1) / 2 + i + 1;
            drawBall(number, 880 + row * 40.0f, 230 + (i - row / 2.0f) * 46, 20);
        }
    }
    drawBall(0, 760, 230, 20);

    // Buttons
    const char *playLabel = "Play";
    if (app->gameInProgress) playLabel = "Continue";
    float x = 94;
    float y = 310;
    if (button(Rectangle{ x, y, 260, 48 }, playLabel)) {
        if (!app->gameInProgress) startNewGame(app);
        app->screen = SCREEN_PLAY;
    }
    if (app->gameInProgress && button(Rectangle{ x + 280, y, 200, 48 }, "New game")) {
        startNewGame(app);
        app->screen = SCREEN_PLAY;
    }
    if (button(Rectangle{ x, y + 64, 260, 48 }, "Lessons (Codex)")) app->screen = SCREEN_CODEX;
    if (button(Rectangle{ x, y + 128, 260, 48 }, "Progress")) {
        app->confirmReset = false;
        app->screen = SCREEN_PROGRESS;
    }
    if (button(Rectangle{ x, y + 192, 260, 48 }, "Quit")) app->quit = true;

    drawText(TextFormat("%d of %d concepts done", app->levelsDone, LEVEL_COUNT), 94, 590, 18, COLOUR_TEXT);
    drawText("Two players, one computer. Write the code, then take the shot.", 94, 620, 16, COLOUR_MUTED);
}

// ============================================================
// Codex: re-read every concept you've reached
// ============================================================

void codexScreen(App *app) {
    ClearBackground(COLOUR_BG);
    if (IsKeyPressed(KEY_ESCAPE)) app->screen = SCREEN_MENU;

    drawBold("Codex", 30, 20, 34, COLOUR_ACCENT);
    drawText("Every concept you've reached. Click one to read it again.", 30, 62, 16, COLOUR_MUTED);

    // The list on the left
    for (int n = 1; n <= LEVEL_COUNT; n++) {
        Level *level = getLevel(n);
        float y = 96 + (n - 1) * 20.0f;
        Rectangle row = { 30, y, 340, 20 };

        // A thin line between chapters
        if (n > 1 && getLevel(n - 1)->chapter != level->chapter) {
            DrawLine(30, (int)y, 370, (int)y, Color{ 60, 65, 80, 255 });
        }

        if (!reached(app, n)) {
            drawText(TextFormat("%2d  locked", n), 36, y + 2, 15, Color{ 80, 85, 100, 255 });
            continue;
        }

        bool hover = CheckCollisionPointRec(GetMousePosition(), row);
        if (n == app->codexLevel) DrawRectangleRec(row, Color{ 70, 62, 20, 255 });
        else if (hover) DrawRectangleRec(row, COLOUR_PANEL);
        drawText(TextFormat("%2d  %s", n, level->title), 36, y + 2, 15, COLOUR_TEXT);
        if (hover && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) app->codexLevel = n;
    }

    // The chosen concept on the right
    if (!reached(app, app->codexLevel)) app->codexLevel = 1;
    Level *level = getLevel(app->codexLevel);
    float x = 420;
    Rectangle page = { x - 20, 90, SCREEN_WIDTH - x, SCREEN_HEIGHT - 160.0f };
    DrawRectangleRec(page, COLOUR_PANEL);
    drawText(TextFormat("Level %d - Chapter %d: %s", app->codexLevel, level->chapter, chapterName(level->chapter)),
             x, 104, 14, COLOUR_MUTED);
    drawBold(level->title, x, 124, 28, COLOUR_TEXT);
    float width = SCREEN_WIDTH - x - 40;
    float y = drawLessonContent(level, x, 168, width) + 18;

    // A practice task for revising (the first version of this level's task)
    Task *task = &level->tasks[0];
    drawText("Practice", x, y, 14, COLOUR_ACCENT);
    y += 20;
    y += drawWrapped(task->task, x, y, width, 15, COLOUR_TEXT) + 6;
    float codeHeight = countLines(task->code) * lineHeight(14) + 14;
    if (y + codeHeight < page.y + page.height - 10) {
        DrawRectangle((int)x, (int)y, (int)width, (int)codeHeight, COLOUR_CODE_BG);
        drawCode(task->code, x + 10, y + 7, 14, width - 20);
    }

    if (button(Rectangle{ SCREEN_WIDTH - 170.0f, SCREEN_HEIGHT - 56.0f, 150, 40 }, "Back")) {
        app->screen = SCREEN_MENU;
    }
}

// ============================================================
// Progress: the unlock tree
// ============================================================

void progressScreen(App *app) {
    ClearBackground(COLOUR_BG);
    if (IsKeyPressed(KEY_ESCAPE)) app->screen = SCREEN_MENU;

    drawBold("Progress", 30, 20, 34, COLOUR_ACCENT);
    drawText(TextFormat("%d of %d concepts done. Pot one of your balls to unlock the next one.", app->levelsDone, LEVEL_COUNT),
             30, 62, 16, COLOUR_MUTED);

    // Progress bar
    Rectangle bar = { 30, 90, SCREEN_WIDTH - 60.0f, 12 };
    DrawRectangleRec(bar, COLOUR_PANEL);
    DrawRectangle(30, 90, (int)(bar.width * app->levelsDone / LEVEL_COUNT), 12, COLOUR_GOOD);

    // One row per chapter, with a box for each level
    float rowY = 130;
    int column = 0;
    for (int n = 1; n <= LEVEL_COUNT; n++) {
        Level *level = getLevel(n);
        bool newChapter = (n == 1) || getLevel(n - 1)->chapter != level->chapter;
        if (newChapter) {
            if (n > 1) rowY += 88;
            column = 0;
            drawBold(TextFormat("Chapter %d", level->chapter), 30, rowY + 4, 16, COLOUR_TEXT);
            drawWrapped(chapterName(level->chapter), 30, rowY + 26, 190, 14, COLOUR_MUTED);
        }

        Rectangle node = { 240.0f + column * 140, rowY, 120, 52 };

        // A line joining this box to the one before it in the chapter
        if (column > 0) {
            DrawLineEx(Vector2{ node.x - 20, node.y + 26 }, Vector2{ node.x, node.y + 26 }, 2, Color{ 70, 75, 90, 255 });
        }

        Color background = Color{ 40, 43, 52, 255 };
        Color border = Color{ 70, 75, 90, 255 };
        const char *state = "locked";
        Color textColour = Color{ 90, 95, 110, 255 };
        if (n <= app->levelsDone) {
            background = Color{ 30, 60, 40, 255 };
            border = COLOUR_GOOD;
            state = "done";
            textColour = COLOUR_TEXT;
        } else if (n == currentLevel(app)) {
            background = Color{ 70, 62, 20, 255 };
            border = COLOUR_ACCENT;
            state = "current";
            textColour = COLOUR_TEXT;
        }

        DrawRectangleRec(node, background);
        DrawRectangleLinesEx(node, 2, border);
        drawBold(TextFormat("%d", n), node.x + 8, node.y + 6, 15, textColour);
        drawText(state, node.x + node.width - textWidth(state, 12) - 8, node.y + 8, 12, border);
        drawText(level->shortName, node.x + 8, node.y + 28, 15, textColour);

        // Click a box you've reached to read it in the codex
        bool hover = CheckCollisionPointRec(GetMousePosition(), node);
        if (hover && reached(app, n) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            app->codexLevel = n;
            app->screen = SCREEN_CODEX;
        }
        column++;
    }

    // Buttons
    if (button(Rectangle{ SCREEN_WIDTH - 170.0f, SCREEN_HEIGHT - 56.0f, 150, 40 }, "Back")) {
        app->screen = SCREEN_MENU;
    }
    const char *resetLabel = "Reset progress";
    if (app->confirmReset) resetLabel = "Click again to reset";
    if (button(Rectangle{ 30, SCREEN_HEIGHT - 56.0f, 240, 40 }, resetLabel)) {
        if (app->confirmReset) {
            app->levelsDone = 0;
            saveProgress(0);
            app->lessonShown = false;
            app->askCount = 0;
            app->gameInProgress = false;   // start again with a fresh game
            app->confirmReset = false;
        } else {
            app->confirmReset = true;
        }
    }
}

// ============================================================
// main
// ============================================================

int main() {
    SetConfigFlags(FLAG_MSAA_4X_HINT);   // smoother circles
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Pointer Pool - learn C and C++ pointers");
    SetExitKey(KEY_NULL);                // Esc goes back to the menu instead of closing the window
    SetTargetFPS(60);                    // the friction is tuned for 60 frames per second

    loadFonts();
    setupLevels();
    setupApp(&app);

    while (!WindowShouldClose() && !app.quit) {
        // Demo key: F5 unlocks every level (handy for showing any chapter)
        if (IsKeyPressed(KEY_F5)) {
            app.levelsDone = LEVEL_COUNT;
            saveProgress(LEVEL_COUNT);
        }

        BeginDrawing();
        if (app.screen == SCREEN_MENU) menuScreen(&app);
        else if (app.screen == SCREEN_PLAY) playScreen(&app);
        else if (app.screen == SCREEN_CODEX) codexScreen(&app);
        else if (app.screen == SCREEN_PROGRESS) progressScreen(&app);
        EndDrawing();
    }

    unloadFonts();
    CloseWindow();
    return 0;
}
