#include "app.h"
#include "ui.h"
#include "effects.h"
#include <cstdio>
#include <cstring>
#include <cmath>

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

void setPhase(App *app, Phase phase) {
    app->phase = phase;
    app->phaseTimer = 0;   // the panel fades in again
}

void setFeedback(App *app, const char *text, bool good) {
    snprintf(app->feedback, sizeof(app->feedback), "%s", text);
    app->feedbackGood = good;
}

// The question being asked right now: one of the level's two tasks, or a made-up one
Task *currentTask(App *app) {
    if (app->usingMadeUp) return &app->madeUp.task;
    return &getLevel(app->questionLevel)->tasks[app->version];
}

// Makes a new question in the lecturer's style for the current level
// (trying again if it happens to come out the same as the last one)
void useMadeUpQuestion(App *app) {
    char lastCode[sizeof(app->madeUp.code)];
    snprintf(lastCode, sizeof(lastCode), "%s", app->madeUp.code);
    for (int tries = 0; tries < 5; tries++) {
        if (!makeQuestion(app->questionLevel, &app->madeUp)) return;   // this level has none
        if (strcmp(app->madeUp.code, lastCode) != 0) break;
    }
    app->usingMadeUp = true;
}

// The banner that slides down from the top when you unlock something
void showBanner(App *app, const char *title, const char *text) {
    snprintf(app->bannerTitle, sizeof(app->bannerTitle), "%s", title);
    snprintf(app->bannerText, sizeof(app->bannerText), "%s", text);
    app->bannerTimer = 3.2f;
    spawnConfetti(SCREEN_WIDTH / 2.0f, 120, 70);
    playSound(SOUND_UNLOCK, 0.7f);
}

// Finishing a chapter unlocks an upgrade, like the upgrades in The Farmer Was Replaced
const char *upgradeFor(int levelsDone) {
    if (levelsDone == AIM_GUIDE_LEVEL) return "Upgrade unlocked: ghost ball! See exactly where the cue ball will hit.";
    if (levelsDone == BALL_PATH_LEVEL) return "Upgrade unlocked: ball path! See where the object ball will go.";
    if (levelsDone == GOLD_CUE_LEVEL) return "Upgrade unlocked: the golden cue!";
    return nullptr;
}

void completeLevel(App *app) {
    if (app->levelsDone >= LEVEL_COUNT) return;
    app->levelsDone++;
    saveProgress(app->levelsDone, app->stars);
    app->lessonShown = false;   // the next level's lesson will show
    app->askCount = 0;

    char title[80];
    char text[120];
    snprintf(title, sizeof(title), "LEVEL %d COMPLETE", app->levelsDone);
    const char *upgrade = upgradeFor(app->levelsDone);
    if (upgrade != nullptr) {
        snprintf(text, sizeof(text), "%s", upgrade);
    } else if (app->levelsDone < LEVEL_COUNT) {
        snprintf(text, sizeof(text), "Unlocked: Level %d - %s", app->levelsDone + 1, getLevel(app->levelsDone + 1)->title);
    } else {
        snprintf(text, sizeof(text), "Every concept unlocked! Review mode is on.");
    }
    showBanner(app, title, text);
}

// ============================================================
// Setting up a question
// ============================================================

// Clears the answer, hints and tiles for the question in app->questionLevel / app->version
void resetQuestion(App *app) {
    Task *task = currentTask(app);

    clearCodeBox(&app->answerBox);
    if (task->kind == TASK_TYPE) {
        app->answerBox.maxLines = 4;
        app->answerBox.placeholder = "Click here and type your code...";
    } else if (task->kind == TASK_TRACE) {
        app->answerBox.maxLines = 1;
        app->answerBox.placeholder = "Type your answer...";
    } else {
        app->answerBox.maxLines = 1;
        app->answerBox.placeholder = "Type the line number...";
    }
    app->answerBox.focused = task->kind != TASK_ORDER;
    app->callBox.focused = false;

    app->hintsShown = 0;
    app->chosenCount = 0;
    app->solved = false;
    app->wrongTimer = 0;
    setFeedback(app, "", false);
}

void startNewGame(App *app) {
    newGame(&app->game);
    app->gameInProgress = true;
    app->practising = false;
    app->logView = nullptr;
    prepareNextShot(app);
}

// Picks the question for the next shot
void prepareNextShot(App *app) {
    Game *game = &app->game;
    app->practising = false;
    app->reviewMode = app->levelsDone >= LEVEL_COUNT;

    app->usingMadeUp = false;
    if (app->reviewMode) {
        // Every level is done, so practise a random one (not the call level).
        // Half the time it's a made-up question, so it isn't the same over and over.
        app->questionLevel = GetRandomValue(1, LEVEL_COUNT);
        if (app->questionLevel == CALL_LEVEL) app->questionLevel = CALL_LEVEL + 1;
        app->version = GetRandomValue(0, 1);
        if (GetRandomValue(0, 1) == 0) useMadeUpQuestion(app);
    } else {
        app->questionLevel = currentLevel(app);
        // You don't call a ball on the break, so practise the level before instead
        if (game->isBreak && app->questionLevel == CALL_LEVEL) app->questionLevel = CALL_LEVEL - 1;
        // First the level's two tasks. After that (if you keep missing the shot),
        // a new made-up question each time, so you never get the same one twice in a row.
        if (app->askCount < 2 || app->questionLevel == CALL_LEVEL) {
            app->version = app->askCount % 2;
        } else {
            useMadeUpQuestion(app);
        }
        app->askCount++;
    }

    // After Level 3 you call a ball after every 2 of your own balls, and for the 8
    Player *shooter = &game->players[game->currentPlayer];
    bool goingForThe8 = !game->tableOpen && ballsLeft(game, shooter->group) == 0;
    bool callDue = game->potsSinceCall[game->currentPlayer] >= 2 || goingForThe8;
    app->callNeeded = !game->isBreak && app->levelsDone >= CALL_LEVEL && app->questionLevel != CALL_LEVEL && callDue;
    if (reached(app, STEP_LEVEL)) app->callBox.placeholder = "Ball *target = balls + ...;";
    else app->callBox.placeholder = "Ball *target = &balls[...];";

    resetQuestion(app);   // the call box keeps its text, so you only change the number
    app->power = 0;
    app->charging = false;
    app->aimX = RACK_X;
    app->aimY = RACK_Y;
    game->calledBall = -1;

    // The first time a level comes up, show its lesson
    bool newLevel = !app->reviewMode && app->questionLevel == currentLevel(app) && !app->lessonShown;
    if (newLevel) setPhase(app, PHASE_LESSON);
    else setPhase(app, PHASE_CODE);
}

// ============================================================
// Checking an answer
// ============================================================

// ORDER tasks: the clicked tiles joined together. Other tasks: what was typed.
void getAnswerText(App *app, char *out, int size) {
    Task *task = currentTask(app);
    out[0] = '\0';
    if (task->kind != TASK_ORDER) {
        snprintf(out, size, "%s", app->answerBox.text);
        return;
    }
    for (int i = 0; i < app->chosenCount; i++) {
        int length = (int)strlen(out);
        snprintf(out + length, size - length, "%s ", task->tiles[app->chosen[i]]);
    }
}

void runCode(App *app) {
    Game *game = &app->game;
    Level *level = getLevel(app->questionLevel);
    Task *task = currentTask(app);

    char answer[600];
    getAnswerText(app, answer, sizeof(answer));

    // 1. The answer to the question
    CheckResult result = checkAnswer(task, answer, level->noIndexing);
    if (!result.correct) {
        setFeedback(app, result.feedback, false);
        app->wrongTimer = 0.4f;   // shake the answer box
        playSound(SOUND_WRONG, 0.5f);
        return;
    }

    // At Level 3 the answer itself calls a ball
    int called = -1;
    if (app->questionLevel == CALL_LEVEL && !app->practising) called = result.number;

    // 2. The call line
    if (app->callNeeded && !app->practising) {
        CheckResult callResult = checkCallLine(app->callBox.text, reached(app, STEP_LEVEL));
        if (!callResult.correct) {
            setFeedback(app, callResult.feedback, false);
            app->callBox.focused = true;
            app->answerBox.focused = false;
            app->wrongTimer = 0.4f;
            playSound(SOUND_WRONG, 0.5f);
            return;
        }
        called = callResult.number;
    }

    // 3. Is the player allowed to call that ball?
    if (called != -1) {
        char why[150];
        if (!callIsAllowed(game, called, why, sizeof(why))) {
            setFeedback(app, why, false);
            playSound(SOUND_WRONG, 0.5f);
            return;
        }
        game->calledBall = called;
    }

    // Stars: 3 with no hints, 2 with one hint, 1 with more. The best result is kept.
    int earned = 3 - app->hintsShown;
    if (earned < 1) earned = 1;
    if (earned > app->stars[app->questionLevel]) {
        app->stars[app->questionLevel] = earned;
        saveProgress(app->levelsDone, app->stars);
    }
    playSound(SOUND_CORRECT, 0.6f);

    if (app->practising) {
        app->solved = true;
        setFeedback(app, "Correct!", true);
        spawnFloatingText("Correct!", SCREEN_WIDTH * 0.68f, 560, COLOUR_GOOD);
        spawnConfetti(SCREEN_WIDTH * 0.68f, 600, 30);
        return;
    }

    spawnFloatingText("Correct!", PANEL_X + 265.0f, 560, COLOUR_GOOD);
    spawnConfetti(PANEL_X + 265.0f, 600, 30);
    if (!UNLOCK_BY_POTTING && !app->reviewMode && app->questionLevel == currentLevel(app)) {
        completeLevel(app);
    }

    setFeedback(app, "Correct! Watch your code run...", true);
    app->runTimer = 0;
    setPhase(app, PHASE_RUNNING);
    app->answerBox.focused = false;
    app->callBox.focused = false;
}

// Called when the balls stop: apply the rules, then get the next shot ready
void finishShot(App *app) {
    Game *game = &app->game;
    int shooter = game->currentPlayer;
    int askedLevel = app->questionLevel;
    bool scratch = game->balls[0].pocketed;
    bool pottedOwn = endOfShot(game);
    app->logView = nullptr;   // show the newest log entry

    if (game->gameOver) {
        setPhase(app, PHASE_GAME_OVER);
        app->gameInProgress = false;
        playSound(SOUND_UNLOCK, 0.8f);
        spawnConfetti(SCREEN_WIDTH / 2.0f, 260, 120);
        return;
    }

    // Tell the players what happened
    float tableX = FELT_LEFT + FELT_WIDTH / 2.0f;
    if (scratch) {
        spawnFloatingText("Foul!", tableX, FELT_TOP + 60.0f, COLOUR_BAD);
    } else if (game->currentPlayer == shooter) {
        spawnFloatingText("Shoot again!", tableX, FELT_TOP + 60.0f, COLOUR_GOOD);
    }
    if (game->currentPlayer != shooter) {
        spawnFloatingText(TextFormat("%s's turn", game->players[game->currentPlayer].name), tableX, FELT_TOP + 100.0f, COLOUR_ACCENT);
    }

    // Like spending crops in The Farmer Was Replaced: potting one of your
    // balls after answering the current level unlocks the next concept
    if (UNLOCK_BY_POTTING && pottedOwn && !app->reviewMode && askedLevel == currentLevel(app)) {
        completeLevel(app);
    }
    prepareNextShot(app);
}

// ============================================================
// Updating
// ============================================================

const float SECONDS_PER_LINE = 0.4f;   // how long each line lights up when the code runs

int linesToRun(App *app) {
    Task *task = currentTask(app);
    int lines = countLines(app->answerBox.text);
    if (task->kind == TASK_ORDER) lines = app->chosenCount;
    if (app->callNeeded) lines++;
    return lines;
}

// Keyboard while answering: typing, Tab, and Ctrl/Cmd + Enter to run
void updateCodePhase(App *app) {
    if (app->solved) return;

    bool modifier = IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL) ||
                    IsKeyDown(KEY_LEFT_SUPER) || IsKeyDown(KEY_RIGHT_SUPER);
    if (modifier && IsKeyPressed(KEY_ENTER)) {
        runCode(app);
        return;
    }

    Task *task = currentTask(app);
    bool twoBoxes = app->callNeeded && !app->practising && task->kind != TASK_ORDER;
    if (IsKeyPressed(KEY_TAB) && twoBoxes) {
        app->answerBox.focused = !app->answerBox.focused;
        app->callBox.focused = !app->answerBox.focused;
    }
    if (app->callNeeded && task->kind == TASK_ORDER) app->callBox.focused = true;   // the only box to type in

    if (task->kind != TASK_ORDER) updateCodeBox(&app->answerBox);
    updateCodeBox(&app->callBox);
}

void updateAimPhase(App *app, float dt) {
    Vector2 mouse = GetMousePosition();
    bool onTable = mouse.x < PANEL_X - 10 && mouse.y > 60 && mouse.y < FELT_TOP + FELT_HEIGHT + RAIL + 10;

    // The aim follows the mouse while it's over the table
    if (onTable && !app->charging) {
        app->aimX = mouse.x;
        app->aimY = mouse.y;
    }

    // Hold the button to charge the power, let go to shoot
    if (onTable && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) app->charging = true;
    if (app->charging && IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
        app->aimX = mouse.x;
        app->aimY = mouse.y;
        app->power += 100 * dt;   // full power after 1 second
        if (app->power > 100) app->power = 100;
    }
    if (app->charging && IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
        app->charging = false;
        if (app->power > 2) {
            app->strikeTimer = 0;
            setPhase(app, PHASE_STRIKE);   // the stick moves forward first
        }
    }
}

// Up and Down move through the shot log by following prevPointer and nextPointer (Level 44)
void updateShotLog(App *app) {
    if (!reached(app, 44) || app->phase == PHASE_CODE) return;

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

    for (int p = 0; p < 2; p++) {
        Rectangle box = { 420.0f + p * 200, 5, 190, 46 };
        int left = ballsLeft(game, game->players[p].group);
        drawPlayerBox(&game->players[p], box, game->currentPlayer == p, game->tableOpen, left, true);
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

        // A curved arrow from the called ball's memory box to the ball on the table
        if (game->calledBall > 0 && !game->balls[game->calledBall].pocketed) {
            Ball *target = &game->balls[game->calledBall];
            Vector2 from = { memoryBoxX(game->calledBall) + 20, memoryY };
            Vector2 to = { target->x, target->y + BALL_RADIUS + 4 };
            float pulse = 0.3f + 0.15f * sinf((float)GetTime() * 5);
            DrawLineBezier(from, to, 1.5f, Fade(YELLOW, pulse));
        }
    } else {
        drawText("Level 3 unlocks: the balls array in memory", FELT_LEFT - RAIL, 470, 15, Color{ 80, 85, 100, 255 });
    }

    // Level 7: lastPotted (nullptr until something is potted)
    float infoX = 300;
    if (reached(app, 7)) {
        drawText("lastPotted:", infoX, 556, 15, COLOUR_MUTED);
        if (game->lastPotted == nullptr) drawText("nullptr", infoX + 110, 556, 15, COLOUR_ACCENT);
        else drawBall(game->lastPotted->number, infoX + 122, 565, 10);
    }

    // Level 16: find_max shows the highest ball left
    if (reached(app, 16)) {
        drawText(TextFormat("find_max: %d", highestBallLeft(game)), infoX + 160, 556, 15, COLOUR_MUTED);
    }

    // The players' potted balls (newest first once Level 17 is reached)
    bool newestFirst = reached(app, 17);
    drawTray(&game->players[0], FELT_LEFT - RAIL, 600, newestFirst);
    drawTray(&game->players[1], FELT_LEFT - RAIL, 630, newestFirst);

    // Level 40: the shot log (a linked list)
    Rectangle logArea = { 400, 595, 330, 118 };
    if (reached(app, 40)) {
        DoublyLinkedNode *newest = app->logView;
        if (newest == nullptr) {
            newest = game->shotLog.head;
            while (newest != nullptr && newest->nextPointer != nullptr) newest = newest->nextPointer;
        }
        drawShotLog(newest, logArea, reached(app, 44));
    } else {
        drawText("Level 40 unlocks: the shot log", logArea.x + 8, logArea.y + 8, 15, Color{ 80, 85, 100, 255 });
    }
}

// ============================================================
// Drawing: lessons
// ============================================================

// Explanation, example and "in the game" for one level. Returns the y just below it.
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
    if (button(Rectangle{ x, SCREEN_HEIGHT - 56.0f, 170, 40 }, "Show the slide")) app->showSlide = true;
    Rectangle tryButton = { SCREEN_WIDTH - 200.0f, SCREEN_HEIGHT - 56.0f, 180, 40 };
    if (button(tryButton, label)) {
        app->lessonShown = true;
        setPhase(app, PHASE_CODE);
    }
}

// ============================================================
// Drawing: the task (used by the game panel and the practice screen)
// ============================================================

// A coloured label saying what kind of task this is
float drawKindBadge(TaskKind kind, float x, float y) {
    const char *label = "TYPE THE CODE";
    Color colour = { 60, 110, 190, 255 };
    if (kind == TASK_TRACE) { label = "TRACE IT"; colour = Color{ 130, 80, 180, 255 }; }
    if (kind == TASK_BUG) { label = "FIND THE BUG"; colour = Color{ 190, 70, 70, 255 }; }
    if (kind == TASK_ORDER) { label = "PUT IN ORDER"; colour = Color{ 40, 140, 90, 255 }; }

    float width = textWidth(label, 13) + 18;
    DrawRectangleRounded(Rectangle{ x, y, width, 22 }, 0.5f, 8, colour);
    drawBold(label, x + 9, y + 4, 13, WHITE);
    return width;
}

bool tileIsChosen(App *app, int tile) {
    for (int i = 0; i < app->chosenCount; i++) {
        if (app->chosen[i] == tile) return true;
    }
    return false;
}

// ORDER tasks: clickable tiles, and the lines chosen so far. Returns the y below them.
float drawOrderTiles(App *app, Task *task, float x, float y, float width, bool locked) {
    drawText("Click the lines in the order they should run:", x, y, 14, COLOUR_MUTED);
    y += 20;

    // Short lines fit two to a row
    int tileCount = 0;
    int longest = 0;
    while (tileCount < 6 && task->tiles[tileCount] != nullptr) {
        int length = (int)strlen(task->tiles[tileCount]);
        if (length > longest) longest = length;
        tileCount++;
    }
    bool twoColumns = longest * charWidth(14) + 24 < (width - 10) / 2;
    float tileWidth = width;
    if (twoColumns) tileWidth = (width - 10) / 2;

    Vector2 mouse = GetMousePosition();
    for (int i = 0; i < tileCount; i++) {
        int column = 0;
        int row = i;
        if (twoColumns) {
            column = i % 2;
            row = i / 2;
        }
        Rectangle tile = { x + column * (tileWidth + 10), y + row * 30, tileWidth, 26 };
        bool used = tileIsChosen(app, i);
        bool hover = CheckCollisionPointRec(mouse, tile) && !used && !locked;

        Color background = COLOUR_CODE_BG;
        if (hover) background = Color{ 45, 50, 64, 255 };
        DrawRectangleRounded(tile, 0.2f, 6, background);
        if (used) {
            drawText(task->tiles[i], tile.x + 10, tile.y + 5, 14, Color{ 70, 74, 88, 255 });
        } else {
            drawCode(task->tiles[i], tile.x + 10, tile.y + 5, 14, tile.width - 20);
        }

        if (hover && IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && app->chosenCount < 6) {
            app->chosen[app->chosenCount] = i;
            app->chosenCount++;
        }
    }
    int rows = tileCount;
    if (twoColumns) rows = (tileCount + 1) / 2;
    y += rows * 30 + 8;

    // The chosen lines, in order. Click one to take it back out.
    drawText("Your code (click a line to take it out):", x, y, 14, COLOUR_MUTED);
    y += 20;
    int shownLines = app->chosenCount;
    if (shownLines < 3) shownLines = 3;
    float shake = 0;
    if (app->wrongTimer > 0) shake = sinf(app->wrongTimer * 60) * 6 * (app->wrongTimer / 0.4f);
    Rectangle box = { x + shake, y, width, shownLines * 24.0f + 12 };
    DrawRectangleRec(box, COLOUR_CODE_BG);
    DrawRectangleLinesEx(box, 2, Color{ 70, 75, 90, 255 });

    float highlightLine = -1;
    if (app->phase == PHASE_RUNNING && !app->practising) highlightLine = (int)(app->runTimer / SECONDS_PER_LINE);

    int removeIndex = -1;
    for (int i = 0; i < app->chosenCount; i++) {
        Rectangle line = { box.x + 4, box.y + 6 + i * 24.0f, box.width - 8, 22 };
        if (i == (int)highlightLine) DrawRectangleRec(line, Color{ 40, 90, 50, 255 });
        else if (!locked && CheckCollisionPointRec(mouse, line)) DrawRectangleRec(line, Color{ 45, 50, 64, 255 });
        drawText(TextFormat("%d", i + 1), line.x + 4, line.y + 3, 14, COLOUR_MUTED);
        drawCode(task->tiles[app->chosen[i]], line.x + 28, line.y + 3, 14, line.width - 40);
        if (!locked && CheckCollisionPointRec(mouse, line) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) removeIndex = i;
    }
    if (removeIndex != -1) {
        for (int i = removeIndex; i < app->chosenCount - 1; i++) app->chosen[i] = app->chosen[i + 1];
        app->chosenCount--;
    }
    return y + box.height + 8;
}

void nextPracticeQuestion(App *app);

// Everything for answering one task: the code, the answer, hints, feedback and buttons
void drawTaskArea(App *app, float x, float y, float width, float bottom) {
    Task *task = currentTask(app);
    bool running = app->phase == PHASE_RUNNING && !app->practising;
    bool locked = running || app->solved;

    // What kind of task, and how many stars it's worth right now
    float badgeWidth = drawKindBadge(task->kind, x, y);
    int worth = 3 - app->hintsShown;
    if (worth < 1) worth = 1;
    drawText("worth", x + badgeWidth + 12, y + 3, 14, COLOUR_MUTED);
    drawStars(x + badgeWidth + 70, y + 11, worth, 8);
    y += 32;

    // Made-up questions start with a learner note: a reminder and a small example
    if (task->note != nullptr && task->note[0] != '\0') {
        float noteHeight = wrappedHeight(task->note, width - 20, 14) + 12;
        DrawRectangle((int)x, (int)y, (int)width, (int)noteHeight, Color{ 48, 44, 24, 255 });
        DrawRectangle((int)x, (int)y, 4, (int)noteHeight, COLOUR_ACCENT);
        drawWrapped(task->note, x + 12, y + 6, width - 20, 14, COLOUR_TEXT);
        y += noteHeight + 8;
    }

    // The question and its code
    y += drawWrapped(task->task, x, y, width, 15, COLOUR_TEXT) + 6;
    float codeHeight = countLines(task->code) * lineHeight(14) + 14;
    DrawRectangle((int)x, (int)y, (int)width, (int)codeHeight, COLOUR_CODE_BG);
    if (task->kind == TASK_BUG) drawNumberedCode(task->code, x + 10, y + 7, 14);
    else drawCode(task->code, x + 10, y + 7, 14, width - 20);
    y += codeHeight + 10;

    // Which line is "running"? (the answer first, then the call line)
    int step = (int)(app->runTimer / SECONDS_PER_LINE);
    int answerLines = countLines(app->answerBox.text);
    if (task->kind == TASK_ORDER) answerLines = app->chosenCount;
    int answerHighlight = -1;
    int callHighlight = -1;
    if (running) {
        if (step < answerLines) answerHighlight = step;
        else if (app->callNeeded && step == answerLines) callHighlight = 0;
    }

    // The answer
    float shake = 0;
    if (app->wrongTimer > 0) shake = sinf(app->wrongTimer * 60) * 6 * (app->wrongTimer / 0.4f);
    if (task->kind == TASK_ORDER) {
        y = drawOrderTiles(app, task, x, y, width, locked);
    } else {
        const char *label = "Your code (it replaces the ____):";
        if (task->kind == TASK_TRACE) label = "Your answer:";
        if (task->kind == TASK_BUG) label = "The wrong line's number:";
        drawText(label, x, y, 14, COLOUR_MUTED);
        y += 20;
        int boxLines = answerLines;
        if (task->kind == TASK_TYPE && boxLines < 3) boxLines = 3;
        if (task->kind != TASK_TYPE) boxLines = 1;
        app->answerBox.area = Rectangle{ x + shake, y, width, boxLines * lineHeight(CODE_SIZE) + 16 };
        drawCodeBox(&app->answerBox, answerHighlight);
        y += app->answerBox.area.height + 10;
    }

    // The call line (only in the game, after every 2 of your balls and for the 8)
    bool showCall = app->callNeeded && !app->practising;
    if (!showCall && !app->practising && app->levelsDone >= CALL_LEVEL && app->questionLevel != CALL_LEVEL) {
        int potted = app->game.potsSinceCall[app->game.currentPlayer];
        drawText(TextFormat("No call this shot. You call a ball after every 2 of yours (%d more to go).", 2 - potted),
                 x, y, 13, COLOUR_MUTED);
        y += 22;
    }
    if (showCall) {
        drawText("Call your ball (Tab switches boxes):", x, y, 14, COLOUR_ACCENT);
        y += 20;
        app->callBox.area = Rectangle{ x, y, width, lineHeight(CODE_SIZE) + 16 };
        drawCodeBox(&app->callBox, callHighlight);
        y += app->callBox.area.height + 10;
    }

    // Click a box to type in it
    if (!locked && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        Vector2 mouse = GetMousePosition();
        if (task->kind != TASK_ORDER && CheckCollisionPointRec(mouse, app->answerBox.area)) {
            app->answerBox.focused = true;
            app->callBox.focused = false;
        } else if (showCall && CheckCollisionPointRec(mouse, app->callBox.area)) {
            app->callBox.focused = true;
            app->answerBox.focused = false;
        }
    }

    // The hints asked for so far. The 4th hint is the layout of the answer,
    // and it replaces the first three to save space.
    if (app->hintsShown == 4 && task->layout != nullptr) {
        drawText("Last hint - the layout:", x, y, 14, COLOUR_ACCENT);
        y += 20;
        float layoutHeight = countLines(task->layout) * lineHeight(14) + 12;
        DrawRectangle((int)x, (int)y, (int)width, (int)layoutHeight, COLOUR_CODE_BG);
        DrawRectangleLinesEx(Rectangle{ x, y, width, layoutHeight }, 1, Fade(COLOUR_ACCENT, 0.6f));
        drawCode(task->layout, x + 10, y + 6, 14, width - 20);
        y += layoutHeight + 6;
    } else {
        for (int h = 0; h < app->hintsShown && h < 3; h++) {
            y += drawWrapped(TextFormat("Hint %d: %s", h + 1, task->hints[h]), x, y, width, 14, COLOUR_ACCENT) + 2;
        }
    }

    // Feedback: why it's wrong (red) or "Correct!" (green)
    if (app->feedback[0] != '\0') {
        Color colour = COLOUR_BAD;
        if (app->feedbackGood) colour = COLOUR_GOOD;
        drawWrapped(app->feedback, x, y + 4, width, 15, colour);
    }

    // Buttons
    if (running) return;
    float buttonY = bottom - 2;
    if (app->solved) {
        if (button(Rectangle{ x + width - 150, buttonY, 150, 38 }, "Next >")) nextPracticeQuestion(app);
        return;
    }
    if (button(Rectangle{ x, buttonY, 100, 38 }, "Clear")) {
        clearCodeBox(&app->answerBox);
        app->chosenCount = 0;
        app->answerBox.focused = task->kind != TASK_ORDER;
    }
    const char *hintLabel = TextFormat("Hint (%d/4)", app->hintsShown);
    if (app->hintsShown == 3) hintLabel = "Show layout";
    if (app->hintsShown < 4) {
        if (button(Rectangle{ x + 112, buttonY, 150, 38 }, hintLabel)) app->hintsShown++;
    } else {
        drawText("No more hints", x + 120, buttonY + 11, 14, COLOUR_MUTED);
    }
    const char *shortcut = "or press Ctrl/Cmd + Enter";
    drawText(shortcut, x + width - textWidth(shortcut, 12), buttonY - 18, 12, COLOUR_MUTED);
    if (button(Rectangle{ x + width - 130, buttonY, 130, 38 }, "Run >")) runCode(app);
}

void drawCodePanel(App *app) {
    Level *level = getLevel(app->questionLevel);
    float x = PANEL_X + 20;
    float width = SCREEN_WIDTH - x - 20;
    float y = 66;

    const char *heading = TextFormat("Level %d - %s", app->questionLevel, chapterName(level->chapter));
    if (app->reviewMode) heading = TextFormat("Review - Level %d - %s", app->questionLevel, chapterName(level->chapter));
    if (app->usingMadeUp) heading = TextFormat("Level %d - a new question in the same style", app->questionLevel);
    drawText(heading, x, y, 14, COLOUR_MUTED);
    drawBold(level->title, x, y + 18, 22, COLOUR_TEXT);
    if (app->phase != PHASE_RUNNING) {
        if (button(Rectangle{ SCREEN_WIDTH - 230.0f, y, 100, 30 }, "Slide")) app->showSlide = true;
        if (button(Rectangle{ SCREEN_WIDTH - 120.0f, y, 100, 30 }, "Lesson")) {
            setPhase(app, PHASE_LESSON);
            return;
        }
    }
    drawTaskArea(app, x, y + 50, width, SCREEN_HEIGHT - 52.0f);
}

// The panel while aiming and while the balls roll: your code and its variables
void drawShotPanel(App *app) {
    Game *game = &app->game;
    Task *task = currentTask(app);
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

    // The answer that was accepted
    char answer[600];
    getAnswerText(app, answer, sizeof(answer));
    if (task->kind == TASK_ORDER) {
        // put each chosen tile on its own line
        answer[0] = '\0';
        for (int i = 0; i < app->chosenCount; i++) {
            int length = (int)strlen(answer);
            snprintf(answer + length, sizeof(answer) - length, "%s%s", task->tiles[app->chosen[i]], i < app->chosenCount - 1 ? "\n" : "");
        }
    }
    drawText("Your answer:", x, y, 14, COLOUR_MUTED);
    y += 20;
    int lines = countLines(answer);
    if (app->callNeeded) lines++;
    float codeHeight = lines * lineHeight(15) + 16;
    DrawRectangle((int)x, (int)y, (int)width, (int)codeHeight, COLOUR_CODE_BG);
    float codeY = y + 8;
    codeY += drawCode(answer, x + 10, codeY, 15, width - 20);
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

    // Power bar, instructions and upgrades
    if (app->phase == PHASE_AIM || app->phase == PHASE_STRIKE) {
        drawPowerBar(x, y, width - 60, app->power, false, true);
        y += 80;
        drawWrapped("Aim with the mouse. Hold the left button to charge the power, then let go to shoot.", x, y, width, 15, COLOUR_TEXT);
        y += 48;
        if (app->levelsDone >= AIM_GUIDE_LEVEL) drawText("Upgrade: ghost ball", x, y, 13, COLOUR_GOOD);
        if (app->levelsDone >= BALL_PATH_LEVEL) drawText("Upgrade: ball path", x + 170, y, 13, COLOUR_GOOD);
        if (app->levelsDone >= GOLD_CUE_LEVEL) drawText("Upgrade: golden cue", x + 330, y, 13, COLOUR_ACCENT);
    }

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
    float pulse = 1 + 0.04f * sinf((float)GetTime() * 4);
    drawBold(game->winMessage, x, y, 36 * pulse, COLOUR_ACCENT);
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

// The Slide window: what the slides (or lab sheet) actually say, so you don't
// have to open the PDF. Close it with the button or Esc.
void drawSlideOverlay(App *app) {
    Level *level = getLevel(app->questionLevel);
    if (app->screen == SCREEN_CODEX) level = getLevel(app->codexLevel);

    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Fade(BLACK, 0.7f));
    Rectangle box = { 140, 40, SCREEN_WIDTH - 280.0f, SCREEN_HEIGHT - 80.0f };
    DrawRectangleRec(box, COLOUR_PANEL);
    DrawRectangleLinesEx(box, 2, COLOUR_ACCENT);

    drawBold("FROM YOUR NOTES", box.x + 24, box.y + 18, 15, COLOUR_ACCENT);
    drawBold(level->slideTitle, box.x + 24, box.y + 40, 20, COLOUR_TEXT);
    Rectangle page = { box.x + 24, box.y + 76, box.width - 48, box.height - 150 };
    DrawRectangleRec(page, COLOUR_CODE_BG);
    drawCode(level->slideText, page.x + 14, page.y + 12, 14, page.width - 28);

    if (button(Rectangle{ box.x + box.width - 164, box.y + box.height - 56, 140, 40 }, "Close") ||
        IsKeyPressed(KEY_ESCAPE)) {
        app->showSlide = false;
    }
}

// The unlock banner slides down, waits, then slides back up
void drawBanner(App *app) {
    if (app->bannerTimer <= 0) return;
    float total = 3.2f;
    float shown = total - app->bannerTimer;
    float y = 70;
    if (shown < 0.3f) y = -110 + (shown / 0.3f) * 180;
    if (app->bannerTimer < 0.3f) y = 70 - (1 - app->bannerTimer / 0.3f) * 180;

    Rectangle box = { SCREEN_WIDTH / 2 - 330.0f, y, 660, 92 };
    DrawRectangleRounded(Rectangle{ box.x + 4, box.y + 6, box.width, box.height }, 0.2f, 8, Fade(BLACK, 0.5f));
    DrawRectangleRounded(box, 0.2f, 8, Color{ 50, 44, 18, 255 });
    float glow = 0.5f + 0.5f * sinf((float)GetTime() * 6);
    DrawRectangleRoundedLinesEx(box, 0.2f, 8, 3, Fade(COLOUR_ACCENT, 0.6f + 0.4f * glow));

    float titleWidth = textWidth(app->bannerTitle, 26);
    drawBold(app->bannerTitle, box.x + (box.width - titleWidth) / 2, box.y + 14, 26, COLOUR_ACCENT);
    float textW = textWidth(app->bannerText, 16);
    drawText(app->bannerText, box.x + (box.width - textW) / 2, box.y + 56, 16, COLOUR_TEXT);
}

// ============================================================
// The game screen: update, then draw
// ============================================================

void playScreen(App *app) {
    Game *game = &app->game;
    float dt = GetFrameTime();
    app->phaseTimer += dt;
    if (app->wrongTimer > 0) app->wrongTimer -= dt;

    // ---------- Update ----------
    bool blocked = app->showSlide;   // nothing else reacts while the Slide window is open
    if (!blocked && IsKeyPressed(KEY_ESCAPE)) {
        app->screen = SCREEN_MENU;
        return;
    }

    if (blocked) {
        // wait for the Slide window to close
    } else if (app->phase == PHASE_CODE) {
        updateCodePhase(app);
    } else if (app->phase == PHASE_RUNNING) {
        app->runTimer += dt;
        if (app->runTimer > (linesToRun(app) + 1) * SECONDS_PER_LINE) setPhase(app, PHASE_AIM);
    } else if (app->phase == PHASE_AIM) {
        updateAimPhase(app, dt);
    } else if (app->phase == PHASE_STRIKE) {
        app->strikeTimer += dt;
        if (app->strikeTimer >= 0.09f) {
            startShot(game, app->aimX, app->aimY, app->power);
            playSound(SOUND_CUE, 0.3f + app->power / 150);
            if (game->isBreak && app->power > 70) shakeScreen(5);
            setPhase(app, PHASE_ROLLING);
        }
    } else if (app->phase == PHASE_ROLLING) {
        updateBalls(game, dt);
        if (!ballsMoving(game)) finishShot(app);
    }
    updateShotLog(app);

    // ---------- Draw ----------
    ClearBackground(COLOUR_BG);
    DrawRectangle(PANEL_X, 56, SCREEN_WIDTH - PANEL_X, SCREEN_HEIGHT - 56, COLOUR_PANEL);

    // The table shakes a little on big hits
    Camera2D camera = {};
    camera.zoom = 1;
    camera.offset = updateShake(dt);
    BeginMode2D(camera);
    drawTable();
    if (app->phase == PHASE_AIM) {
        drawAimGuide(game, app->aimX, app->aimY, app->levelsDone >= AIM_GUIDE_LEVEL, app->levelsDone >= BALL_PATH_LEVEL);
    }
    drawBalls(game);
    drawTargetRing(game);
    if (app->phase == PHASE_AIM || app->phase == PHASE_STRIKE) {
        float strike = 0;
        if (app->phase == PHASE_STRIKE) strike = app->strikeTimer / 0.09f;
        drawCueStick(game, app->aimX, app->aimY, app->power, strike, app->levelsDone >= GOLD_CUE_LEVEL);
    }
    EndMode2D();

    drawUnderTable(app);
    if (reached(app, CALL_LEVEL)) drawBallTooltip(game);

    if (app->phase == PHASE_LESSON) drawLessonPanel(app);
    else if (app->phase == PHASE_CODE || app->phase == PHASE_RUNNING) drawCodePanel(app);
    else drawShotPanel(app);

    // Each new panel fades in
    if (app->phaseTimer < 0.25f) {
        DrawRectangle(PANEL_X, 56, SCREEN_WIDTH - PANEL_X, SCREEN_HEIGHT - 56, Fade(COLOUR_PANEL, 1 - app->phaseTimer / 0.25f));
    }

    drawTopBar(app);
    if (app->phase == PHASE_GAME_OVER) drawGameOver(app);
    if (app->showSlide) drawSlideOverlay(app);
}

// ============================================================
// Practice: answer questions for any level you've reached, without the table
// ============================================================

void startPractice(App *app, int level) {
    app->practising = true;
    app->usingMadeUp = false;
    app->showSlide = false;
    app->questionLevel = level;
    app->version = 0;
    app->callNeeded = false;
    resetQuestion(app);
    setPhase(app, PHASE_CODE);
    app->screen = SCREEN_PRACTICE;
}

// The next level you've reached (back to Level 1 after the last one)
void nextPracticeLevel(App *app) {
    app->questionLevel++;
    if (app->questionLevel > LEVEL_COUNT || !reached(app, app->questionLevel)) app->questionLevel = 1;
    app->version = 0;
    app->usingMadeUp = false;
    resetQuestion(app);
    setPhase(app, PHASE_CODE);
}

// After a correct answer: the other version, then new made-up questions for as long as you like
void nextPracticeQuestion(App *app) {
    if (!app->usingMadeUp && app->version == 0) {
        app->version = 1;
    } else {
        useMadeUpQuestion(app);
        if (!app->usingMadeUp) {   // this level has no made-up questions
            nextPracticeLevel(app);
            return;
        }
    }
    resetQuestion(app);
    setPhase(app, PHASE_CODE);
}

void practiceScreen(App *app) {
    float dt = GetFrameTime();
    app->phaseTimer += dt;
    if (app->wrongTimer > 0) app->wrongTimer -= dt;
    if (!app->showSlide) {
        if (IsKeyPressed(KEY_ESCAPE)) {
            app->screen = SCREEN_CODEX;
            return;
        }
        updateCodePhase(app);
    }

    Level *level = getLevel(app->questionLevel);
    ClearBackground(COLOUR_BG);
    drawBold("PRACTICE", 30, 18, 30, COLOUR_ACCENT);
    const char *kind = "the level's own question";
    if (app->usingMadeUp) kind = "a new question in the same style";
    drawText(TextFormat("Level %d - %s - %s", app->questionLevel, chapterName(level->chapter), kind), 30, 56, 15, COLOUR_MUTED);
    if (button(Rectangle{ SCREEN_WIDTH - 170.0f, 22, 150, 40 }, "Back")) app->screen = SCREEN_CODEX;
    if (button(Rectangle{ SCREEN_WIDTH - 360.0f, 22, 175, 40 }, "Next level >")) nextPracticeLevel(app);
    if (button(Rectangle{ SCREEN_WIDTH - 510.0f, 22, 135, 40 }, "Slide")) app->showSlide = true;

    // The lesson on the left, like the info pages in The Farmer Was Replaced
    DrawRectangle(20, 88, 470, SCREEN_HEIGHT - 100, COLOUR_PANEL);
    drawBold(level->title, 36, 102, 24, COLOUR_TEXT);
    drawLessonContent(level, 36, 140, 438);

    // The question on the right
    DrawRectangle(505, 88, SCREEN_WIDTH - 525, SCREEN_HEIGHT - 100, COLOUR_PANEL);
    drawTaskArea(app, 525, 102, SCREEN_WIDTH - 565.0f, SCREEN_HEIGHT - 62.0f);

    if (app->phaseTimer < 0.25f) {
        DrawRectangle(505, 88, SCREEN_WIDTH - 525, SCREEN_HEIGHT - 100, Fade(COLOUR_PANEL, 1 - app->phaseTimer / 0.25f));
    }
    if (app->showSlide) drawSlideOverlay(app);
}
