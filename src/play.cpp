#include "app.h"
#include "ui.h"
#include <cstdio>
#include <cstring>

// ============================================================
// Progress helpers
// ============================================================

// The level you're on now (the next one to finish)
int currentLevel(App *app) {
    if (app->levelsDone >= LEVEL_COUNT) return LEVEL_COUNT;
    return app->levelsDone + 1;
}

// Have you got to this level yet? (finished it, or it's the current one)
bool reached(App *app, int level) {
    return level <= app->levelsDone + 1;
}

void completeLevel(App *app) {
    if (app->levelsDone >= LEVEL_COUNT) return;
    app->levelsDone++;
    saveProgress(app->levelsDone);
    app->lessonShown = false;   // the next level's lesson will show
    app->askCount = 0;
}

void setFeedback(App *app, const char *text, bool good) {
    snprintf(app->feedback, sizeof(app->feedback), "%s", text);
    app->feedbackGood = good;
}

// ============================================================
// The flow of a shot:
//   lesson (first time only) -> code -> run -> aim -> roll -> rules -> next shot
// ============================================================

void startNewGame(App *app) {
    newGame(&app->game);
    app->gameInProgress = true;
    app->logView = nullptr;
    prepareNextShot(app);
}

// Picks the question for the next shot and resets the code boxes
void prepareNextShot(App *app) {
    Game *game = &app->game;
    app->reviewMode = app->levelsDone >= LEVEL_COUNT;

    if (app->reviewMode) {
        // Every level is done, so practise a random one. (Not the call level:
        // you call a ball before every shot anyway.)
        app->questionLevel = GetRandomValue(1, LEVEL_COUNT);
        if (app->questionLevel == CALL_LEVEL) app->questionLevel = CALL_LEVEL + 1;
        app->version = GetRandomValue(0, 1);
    } else {
        app->questionLevel = currentLevel(app);
        // You don't call a ball on the break, so practise the level before instead
        if (game->isBreak && app->questionLevel == CALL_LEVEL) app->questionLevel = CALL_LEVEL - 1;
        app->version = app->askCount % 2;   // alternate between the two versions
        app->askCount++;
    }

    // After Level 3, you call a ball before every shot (apart from the break)
    app->callNeeded = !game->isBreak && app->levelsDone >= CALL_LEVEL && app->questionLevel != CALL_LEVEL;
    if (reached(app, LOCK_LEVEL)) app->callBox.placeholder = "Ball *target = balls + 3;";
    else app->callBox.placeholder = "Ball *target = &balls[3];";

    clearCodeBox(&app->answerBox);   // the call box keeps its text, so you only change the number
    app->answerBox.focused = true;
    app->callBox.focused = false;
    setFeedback(app, "", false);
    app->power = 0;
    app->powerFromCode = false;
    app->charging = false;
    game->calledBall = -1;

    // The first time a level comes up, show its lesson
    bool newLevel = !app->reviewMode && app->questionLevel == currentLevel(app) && !app->lessonShown;
    if (newLevel) app->phase = PHASE_LESSON;
    else app->phase = PHASE_CODE;
}

// The Run button: check the answer and the call line
void runCode(App *app) {
    Game *game = &app->game;
    Task *task = &getLevel(app->questionLevel)->tasks[app->version];
    bool lockOn = app->questionLevel >= LOCK_LEVEL;

    // 1. The answer to this level's task
    CheckResult result = checkAnswer(task, app->answerBox.text, lockOn);
    if (!result.correct) {
        setFeedback(app, result.hint, false);
        return;
    }

    // At Level 3 the answer itself calls a ball
    int called = -1;
    if (app->questionLevel == CALL_LEVEL) called = result.number;

    // 2. The call line
    if (app->callNeeded) {
        CheckResult callResult = checkCallLine(app->callBox.text, reached(app, LOCK_LEVEL));
        if (!callResult.correct) {
            setFeedback(app, callResult.hint, false);
            app->callBox.focused = true;
            app->answerBox.focused = false;
            return;
        }
        called = callResult.number;
    }

    // 3. Is the player allowed to call that ball?
    if (called != -1) {
        char why[150];
        if (!callIsAllowed(game, called, why, sizeof(why))) {
            setFeedback(app, why, false);
            return;
        }
        game->calledBall = called;
    }

    // Level 2: the code sets this shot's power
    if (task->power > 0) {
        app->power = (float)task->power;
        app->powerFromCode = true;
    }

    if (!UNLOCK_BY_POTTING && !app->reviewMode && app->questionLevel == currentLevel(app)) {
        completeLevel(app);
    }

    setFeedback(app, "Correct! Watch your code run...", true);
    app->runTimer = 0;
    app->phase = PHASE_RUNNING;
    app->answerBox.focused = false;
    app->callBox.focused = false;
}

// Called when the balls stop: apply the rules, then get the next shot ready
void finishShot(App *app) {
    int askedLevel = app->questionLevel;
    bool pottedOwn = endOfShot(&app->game);
    app->logView = nullptr;   // show the newest log entry

    if (app->game.gameOver) {
        app->phase = PHASE_GAME_OVER;
        app->gameInProgress = false;
        return;
    }

    // Like spending crops in The Farmer Was Replaced: potting one of your
    // balls after answering the current level unlocks the next concept
    if (UNLOCK_BY_POTTING && pottedOwn && !app->reviewMode && askedLevel == currentLevel(app)) {
        completeLevel(app);
    }
    prepareNextShot(app);
}

// ============================================================
// Updating each phase
// ============================================================

const float SECONDS_PER_LINE = 0.4f;   // how long each line lights up when the code runs

int linesToRun(App *app) {
    int lines = countLines(app->answerBox.text);
    if (app->callNeeded) lines++;
    return lines;
}

void updateCodePhase(App *app) {
    // Ctrl+Enter (or Cmd+Enter on a Mac) runs the code
    bool modifier = IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL) ||
                    IsKeyDown(KEY_LEFT_SUPER) || IsKeyDown(KEY_RIGHT_SUPER);
    if (modifier && IsKeyPressed(KEY_ENTER)) {
        runCode(app);
        return;
    }

    // Tab switches between the two boxes
    if (IsKeyPressed(KEY_TAB) && app->callNeeded) {
        app->answerBox.focused = !app->answerBox.focused;
        app->callBox.focused = !app->answerBox.focused;
    }

    updateCodeBox(&app->answerBox);
    updateCodeBox(&app->callBox);
}

void updateAimPhase(App *app, float dt) {
    Game *game = &app->game;
    Vector2 mouse = GetMousePosition();
    bool onTable = mouse.x < PANEL_X - 10 && mouse.y > 60;

    if (app->powerFromCode) {
        // Level 2: the power was set by the code, so one click shoots
        if (onTable && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            startShot(game, mouse.x, mouse.y, app->power);
            app->phase = PHASE_ROLLING;
        }
        return;
    }

    // Hold the button to charge the power, release to shoot
    if (onTable && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) app->charging = true;
    if (app->charging && IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
        app->power += 100 * dt;   // full power after 1 second
        if (app->power > 100) app->power = 100;
    }
    if (app->charging && IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
        app->charging = false;
        if (app->power > 2) {
            startShot(game, mouse.x, mouse.y, app->power);
            app->phase = PHASE_ROLLING;
        }
    }
}

// Up and Down move through the shot log by following prevPointer and nextPointer (Level 27)
void updateShotLog(App *app) {
    if (!reached(app, 27) || app->phase == PHASE_CODE) return;

    // Find the newest node
    DoublyLinkedNode *newest = app->game.shotLog.head;
    if (newest == nullptr) return;
    while (newest->nextPointer != nullptr) newest = newest->nextPointer;

    DoublyLinkedNode *view = app->logView;
    if (view == nullptr) view = newest;

    if (IsKeyPressed(KEY_UP) && view->prevPointer != nullptr) view = view->prevPointer;
    if (IsKeyPressed(KEY_DOWN) && view->nextPointer != nullptr) view = view->nextPointer;

    if (view == newest) app->logView = nullptr;   // back at the newest: follow new shots again
    else app->logView = view;
}

// ============================================================
// Drawing: the top bar and the area under the table
// ============================================================

void drawTopBar(App *app) {
    Game *game = &app->game;
    DrawRectangle(0, 0, SCREEN_WIDTH, 56, COLOUR_PANEL);

    drawBold("POINTER POOL", 20, 8, 22, COLOUR_ACCENT);
    const char *levelText;
    if (app->reviewMode) levelText = TextFormat("Review: Level %d", app->questionLevel);
    else levelText = TextFormat("Level %d of %d: %s", currentLevel(app), LEVEL_COUNT, getLevel(currentLevel(app))->title);
    drawText(levelText, 20, 34, 14, COLOUR_MUTED);

    bool showLeft = reached(app, 8);   // Level 8: frequencyCount counts your balls left
    for (int p = 0; p < 2; p++) {
        Rectangle box = { 380.0f + p * 200, 5, 190, 46 };
        int left = ballsLeft(game, game->players[p].group);
        drawPlayerBox(&game->players[p], box, game->currentPlayer == p, game->tableOpen, left, showLeft);
    }

    Rectangle menuButton = { SCREEN_WIDTH - 110.0f, 11, 90, 34 };
    if (button(menuButton, "Menu")) app->screen = SCREEN_MENU;
}

void drawUnderTable(App *app) {
    Game *game = &app->game;
    float memoryY = 478;

    // Level 3: the balls array in memory, and the target pointer
    if (reached(app, CALL_LEVEL)) {
        drawMemoryRow(game, memoryY);
        drawTargetPointer(game, FELT_LEFT - RAIL, 550, memoryY);
    } else {
        drawText("Level 3 unlocks: the balls array in memory", FELT_LEFT - RAIL, 470, 15, Color{ 80, 85, 100, 255 });
    }

    // Level 5: lastPotted (nullptr until something is potted)
    float infoX = 300;
    if (reached(app, 5)) {
        drawText("lastPotted:", infoX, 556, 15, COLOUR_MUTED);
        if (game->lastPotted == nullptr) drawText("nullptr", infoX + 110, 556, 15, COLOUR_ACCENT);
        else drawBall(game->lastPotted->number, infoX + 122, 565, 10);
    }

    // Level 16: find_max shows the highest ball left
    if (reached(app, 16)) {
        drawText(TextFormat("find_max: %d", highestBallLeft(game)), infoX + 160, 556, 15, COLOUR_MUTED);
    }

    // The players' potted balls (newest first once Level 11 is reached)
    bool newestFirst = reached(app, 11);
    drawTray(&game->players[0], FELT_LEFT - RAIL, 600, newestFirst);
    drawTray(&game->players[1], FELT_LEFT - RAIL, 630, newestFirst);

    // Level 25: the shot log (a linked list)
    Rectangle logArea = { 400, 595, 330, 118 };
    if (reached(app, 25)) {
        DoublyLinkedNode *newest = app->logView;
        if (newest == nullptr) {
            newest = game->shotLog.head;
            while (newest != nullptr && newest->nextPointer != nullptr) newest = newest->nextPointer;
        }
        drawShotLog(newest, logArea, reached(app, 27));
    } else {
        drawText("Level 25 unlocks: the shot log", logArea.x + 8, logArea.y + 8, 15, Color{ 80, 85, 100, 255 });
    }
}

// ============================================================
// Drawing: the panel on the right
// ============================================================

// Explanation, example and "in the game" for one level. Also used by the codex.
// Returns the y position just below what it drew.
float drawLessonContent(Level *level, float x, float y, float width) {
    y += drawWrapped(level->source, x, y, width, 13, COLOUR_MUTED) + 10;
    y += drawWrapped(level->explanation, x, y, width, 16, COLOUR_TEXT) + 12;

    drawText("Example", x, y, 14, COLOUR_MUTED);
    y += 20;
    float codeHeight = countLines(level->example) * lineHeight(15) + 16;
    DrawRectangle((int)x, (int)y, (int)width, (int)codeHeight, COLOUR_CODE_BG);
    drawCode(level->example, x + 10, y + 8, 15, width - 20);
    y += codeHeight + 14;

    drawText("In the game", x, y, 14, COLOUR_ACCENT);
    y += 20;
    y += drawWrapped(level->inGame, x, y, width, 15, COLOUR_TEXT);
    return y;
}

void drawLessonPanel(App *app) {
    Level *level = getLevel(app->questionLevel);
    float x = PANEL_X + 20;
    float width = SCREEN_WIDTH - x - 20;
    float y = 70;

    // It's only "new" the first time the current level comes up
    bool isNew = !app->lessonShown && !app->reviewMode && app->questionLevel == currentLevel(app);
    if (isNew) drawBold("NEW CONCEPT UNLOCKED", x, y, 15, COLOUR_ACCENT);
    else drawBold("LESSON", x, y, 15, COLOUR_ACCENT);
    drawText(TextFormat("Level %d - Chapter %d: %s", app->questionLevel, level->chapter, chapterName(level->chapter)),
             x, y + 22, 14, COLOUR_MUTED);
    drawBold(level->title, x, y + 44, 26, COLOUR_TEXT);

    drawLessonContent(level, x, y + 84, width);

    const char *label = "Back to the task";
    if (isNew) label = "Try it";
    Rectangle tryButton = { SCREEN_WIDTH - 200.0f, SCREEN_HEIGHT - 56.0f, 180, 40 };
    if (button(tryButton, label)) {
        app->lessonShown = true;
        app->phase = PHASE_CODE;
    }
}

void drawCodePanel(App *app) {
    Level *level = getLevel(app->questionLevel);
    Task *task = &level->tasks[app->version];
    float x = PANEL_X + 20;
    float width = SCREEN_WIDTH - x - 20;
    float y = 66;
    bool running = app->phase == PHASE_RUNNING;

    // Heading
    const char *heading = TextFormat("Level %d - %s", app->questionLevel, chapterName(level->chapter));
    if (app->reviewMode) heading = TextFormat("Review - Level %d - %s", app->questionLevel, chapterName(level->chapter));
    drawText(heading, x, y, 14, COLOUR_MUTED);
    drawBold(level->title, x, y + 18, 22, COLOUR_TEXT);
    Rectangle lessonButton = { SCREEN_WIDTH - 120.0f, y, 100, 30 };
    if (!running && button(lessonButton, "Lesson")) {
        app->phase = PHASE_LESSON;
        return;
    }
    y += 52;

    // The task, then the code around the gap
    y += drawWrapped(task->task, x, y, width, 15, COLOUR_TEXT) + 6;
    float codeHeight = countLines(task->code) * lineHeight(14) + 14;
    DrawRectangle((int)x, (int)y, (int)width, (int)codeHeight, COLOUR_CODE_BG);
    drawCode(task->code, x + 10, y + 7, 14, width - 20);
    y += codeHeight + 10;

    // Which line is running? The answer's lines first, then the call line.
    int step = (int)(app->runTimer / SECONDS_PER_LINE);
    int answerLines = countLines(app->answerBox.text);
    int answerHighlight = -1;
    int callHighlight = -1;
    if (running) {
        if (step < answerLines) answerHighlight = step;
        else if (app->callNeeded && step == answerLines) callHighlight = 0;
    }

    // The answer box (it grows if you type more lines)
    drawText("Your code (replaces the ____):", x, y, 14, COLOUR_MUTED);
    y += 20;
    int boxLines = answerLines;
    if (boxLines < 3) boxLines = 3;
    app->answerBox.area = Rectangle{ x, y, width, boxLines * lineHeight(CODE_SIZE) + 16 };
    drawCodeBox(&app->answerBox, answerHighlight);
    y += app->answerBox.area.height + 10;

    // The call line
    if (app->callNeeded) {
        drawText("Call your ball (Tab to switch boxes):", x, y, 14, COLOUR_ACCENT);
        y += 20;
        app->callBox.area = Rectangle{ x, y, width, lineHeight(CODE_SIZE) + 16 };
        drawCodeBox(&app->callBox, callHighlight);
        y += app->callBox.area.height + 10;
    }

    // Click a box to type in it
    if (!running && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        Vector2 mouse = GetMousePosition();
        if (CheckCollisionPointRec(mouse, app->answerBox.area)) {
            app->answerBox.focused = true;
            app->callBox.focused = false;
        } else if (app->callNeeded && CheckCollisionPointRec(mouse, app->callBox.area)) {
            app->callBox.focused = true;
            app->answerBox.focused = false;
        }
    }

    // Feedback: a hint (red) or "Correct!" (green)
    if (app->feedback[0] != '\0') {
        Color colour = COLOUR_BAD;
        if (app->feedbackGood) colour = COLOUR_GOOD;
        drawWrapped(app->feedback, x, y, width, 15, colour);
    }

    // Buttons
    if (!running) {
        Rectangle clearButton = { x, SCREEN_HEIGHT - 52.0f, 110, 38 };
        if (button(clearButton, "Clear")) {
            clearCodeBox(&app->answerBox);
            app->answerBox.focused = true;
            app->callBox.focused = false;
        }
        drawText("Ctrl/Cmd + Enter also runs", x + 125, SCREEN_HEIGHT - 42.0f, 13, COLOUR_MUTED);
        Rectangle runButton = { SCREEN_WIDTH - 150.0f, SCREEN_HEIGHT - 52.0f, 130, 38 };
        if (button(runButton, "Run >")) runCode(app);
    }
}

// The panel while aiming and while the balls roll: your code and its variables
void drawShotPanel(App *app) {
    Game *game = &app->game;
    float x = PANEL_X + 20;
    float width = SCREEN_WIDTH - x - 20;
    float y = 70;

    if (app->phase == PHASE_AIM) drawBold("Shot unlocked!", x, y, 24, COLOUR_GOOD);
    else if (app->phase == PHASE_GAME_OVER) drawBold("Game over", x, y, 24, COLOUR_ACCENT);
    else drawBold("Balls rolling...", x, y, 24, COLOUR_TEXT);
    if (app->phase != PHASE_GAME_OVER) {
        drawText(TextFormat("%s to shoot", game->players[game->currentPlayer].name), x, y + 32, 15, COLOUR_MUTED);
    }
    y += 64;

    // The code that ran
    drawText("Your code:", x, y, 14, COLOUR_MUTED);
    y += 20;
    int lines = countLines(app->answerBox.text);
    if (app->callNeeded) lines++;
    float codeHeight = lines * lineHeight(15) + 16;
    DrawRectangle((int)x, (int)y, (int)width, (int)codeHeight, COLOUR_CODE_BG);
    float codeY = y + 8;
    codeY += drawCode(app->answerBox.text, x + 10, codeY, 15, width - 20);
    if (app->callNeeded) drawCode(app->callBox.text, x + 10, codeY, 15, width - 20);
    y += codeHeight + 18;

    // Its variables, like the data tables in the Pointers lecture.
    // power changes live while you charge the shot: powerPtr points at it.
    drawDataTableHeader(x, y);
    y += 28;
    drawDataRow(x, y, "power", "int", TextFormat("%d", (int)app->power));
    y += 24;
    drawDataRow(x, y, "powerPtr", "int *", "&power");
    y += 24;
    if (reached(app, CALL_LEVEL)) {
        if (game->calledBall > 0) {
            drawDataRow(x, y, "target", "Ball *", TextFormat("%d", addressOf(game->calledBall)));
            y += 24;
            drawDataRow(x, y, "*target", "Ball", TextFormat("ball %d", game->calledBall));
        } else {
            drawDataRow(x, y, "target", "Ball *", "nullptr (no call)");
        }
        y += 24;
    }
    y += 16;

    // Power bar and instructions
    if (app->phase == PHASE_AIM) {
        drawPowerBar(x, y, width - 60, app->power, app->powerFromCode, true);
        y += 80;
        const char *help = "Aim with the mouse. Hold the left button to charge the power, then let go to shoot.";
        if (app->powerFromCode) help = "Aim with the mouse, then click once: your code already set the power.";
        drawWrapped(help, x, y, width, 15, COLOUR_TEXT);
    }

    // What happened last
    drawWrapped(game->status, x, SCREEN_HEIGHT - 70.0f, width, 14, COLOUR_MUTED);
}

void drawGameOver(App *app) {
    Game *game = &app->game;
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Fade(BLACK, 0.65f));

    Rectangle box = { SCREEN_WIDTH / 2 - 320.0f, 170, 640, 340 };
    DrawRectangleRec(box, COLOUR_PANEL);
    DrawRectangleLinesEx(box, 2, COLOUR_ACCENT);

    float x = box.x + 40;
    float y = box.y + 30;
    drawBold(game->winMessage, x, y, 36, COLOUR_ACCENT);
    y += 56;
    y += drawWrapped(game->reason, x, y, box.width - 80, 17, COLOUR_TEXT) + 16;

    // Points: Lab 2's sum_array adds up the numbers of each player's potted balls
    drawText("Points (sum_array of the ball numbers each player potted):", x, y, 14, COLOUR_MUTED);
    y += 24;
    for (int p = 0; p < 2; p++) {
        Player *player = &game->players[p];
        int points = sum_array(player->tray, player->trayCount);
        drawText(TextFormat("%s: %d points from %d balls", player->name, points, player->trayCount), x, y, 17, COLOUR_TEXT);
        y += 26;
    }

    Rectangle again = { box.x + 40, box.y + box.height - 66, 200, 42 };
    Rectangle menu = { box.x + box.width - 240, box.y + box.height - 66, 200, 42 };
    if (button(again, "Play again")) startNewGame(app);
    if (button(menu, "Main menu")) app->screen = SCREEN_MENU;
}

// ============================================================
// The game screen: update, then draw
// ============================================================

void playScreen(App *app) {
    Game *game = &app->game;
    float dt = GetFrameTime();

    // ---------- Update ----------
    if (IsKeyPressed(KEY_ESCAPE)) {
        app->screen = SCREEN_MENU;
        return;
    }

    if (app->phase == PHASE_CODE) {
        updateCodePhase(app);
    } else if (app->phase == PHASE_RUNNING) {
        app->runTimer += dt;
        if (app->runTimer > (linesToRun(app) + 1) * SECONDS_PER_LINE) {
            app->phase = PHASE_AIM;
        }
    } else if (app->phase == PHASE_AIM) {
        updateAimPhase(app, dt);
    } else if (app->phase == PHASE_ROLLING) {
        updateBalls(game, dt);
        if (!ballsMoving(game)) finishShot(app);
    }
    updateShotLog(app);

    // ---------- Draw ----------
    ClearBackground(COLOUR_BG);
    DrawRectangle(PANEL_X, 56, SCREEN_WIDTH - PANEL_X, SCREEN_HEIGHT - 56, COLOUR_PANEL);

    drawTable();
    drawBalls(game);
    drawTargetRing(game);
    if (app->phase == PHASE_AIM) drawCueStick(game, app->power);
    drawUnderTable(app);
    if (reached(app, CALL_LEVEL)) drawBallTooltip(game);

    if (app->phase == PHASE_LESSON) drawLessonPanel(app);
    else if (app->phase == PHASE_CODE || app->phase == PHASE_RUNNING) drawCodePanel(app);
    else drawShotPanel(app);

    drawTopBar(app);
    if (app->phase == PHASE_GAME_OVER) drawGameOver(app);
}
