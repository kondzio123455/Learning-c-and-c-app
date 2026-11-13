#include "app.h"
#include "ui.h"
#include "effects.h"
#include <cstdio>
#include <cstring>

// ============================================================
// Test mode: 10 questions from the chapters you pick, like the real test.
// No hints, no lessons or slides, and one try each. Marked at the end.
// ============================================================

const char *timeText(float seconds) {
    return TextFormat("%d:%02d", (int)seconds / 60, (int)seconds % 60);
}

// Picks the next question: a level from the chosen chapters (not Level 3 -
// calling a ball is only for the game), and half the time a made-up one
void nextTestQuestion(App *app) {
    int levels[LEVEL_COUNT];
    int count = 0;
    for (int n = 1; n <= LEVEL_COUNT; n++) {
        if (n != CALL_LEVEL && app->testChapters[getLevel(n)->chapter]) {
            levels[count] = n;
            count++;
        }
    }
    int previous = -1;
    if (app->testIndex > 0) previous = app->testQuestions[app->testIndex - 1].level;
    int level = levels[GetRandomValue(0, count - 1)];
    for (int tries = 0; tries < 5 && level == previous && count > 1; tries++) {
        level = levels[GetRandomValue(0, count - 1)];   // not the same level twice in a row
    }

    app->questionLevel = level;
    app->version = GetRandomValue(0, 1);
    app->usingMadeUp = false;
    if (GetRandomValue(0, 1) == 0) useMadeUpQuestion(app);
    resetQuestion(app);
    setPhase(app, PHASE_CODE);
}

void startTest(App *app) {
    app->testing = true;
    app->practising = true;   // no pool table and no call line
    app->callNeeded = false;
    app->rewinding = false;
    app->testState = TEST_RUNNING;
    app->testIndex = 0;
    app->testTime = 0;
    app->confirmStopTest = false;
    nextTestQuestion(app);
}

// Keeps the answer (and the question, for the results), then moves on
void recordTestAnswer(App *app, const char *answer, CheckResult result) {
    Task *task = currentTask(app);
    TestQuestion *q = &app->testQuestions[app->testIndex];
    q->level = app->questionLevel;
    q->correct = result.correct;
    q->kind = task->kind;
    snprintf(q->task, sizeof(q->task), "%s", task->task);
    snprintf(q->code, sizeof(q->code), "%s", task->code);
    snprintf(q->given, sizeof(q->given), "%s", answer[0] != '\0' ? answer : "(skipped)");
    snprintf(q->answer, sizeof(q->answer), "%s", task->answers[0]);
    snprintf(q->why, sizeof(q->why), "%s", result.correct ? "" : result.feedback);
    playSound(SOUND_CLICK, 0.6f);

    app->testIndex++;
    if (app->testIndex >= TEST_LENGTH) {
        app->testing = false;
        app->testState = TEST_RESULTS;
        app->testSelected = 0;
        for (int i = TEST_LENGTH - 1; i >= 0; i--) {
            if (!app->testQuestions[i].correct) app->testSelected = i;   // show the first one you got wrong
        }
        playSound(SOUND_UNLOCK, 0.6f);
    } else {
        nextTestQuestion(app);
    }
}

// Puts each statement of an answer on its own line (but not inside brackets, like a for loop)
void oneStatementPerLine(const char *text, char *out, int size) {
    int length = 0;
    int brackets = 0;
    for (const char *p = text; *p != '\0' && length < size - 2; p++) {
        if (*p == '(') brackets++;
        if (*p == ')') brackets--;
        out[length] = *p;
        length++;
        if (*p == ';' && brackets == 0 && *(p + 1) == ' ') {
            out[length] = '\n';
            length++;
            p++;
        }
    }
    out[length] = '\0';
}

// A green tick or a red cross
void drawMark(bool correct, float cx, float cy) {
    if (correct) {
        DrawCircleV(Vector2{ cx, cy }, 11, COLOUR_GOOD);
        DrawLineEx(Vector2{ cx - 5, cy }, Vector2{ cx - 1, cy + 5 }, 3, WHITE);
        DrawLineEx(Vector2{ cx - 1, cy + 5 }, Vector2{ cx + 6, cy - 5 }, 3, WHITE);
    } else {
        DrawCircleV(Vector2{ cx, cy }, 11, COLOUR_BAD);
        DrawLineEx(Vector2{ cx - 5, cy - 5 }, Vector2{ cx + 5, cy + 5 }, 3, WHITE);
        DrawLineEx(Vector2{ cx + 5, cy - 5 }, Vector2{ cx - 5, cy + 5 }, 3, WHITE);
    }
}

// A box of code with a coloured border (the player's answer, or the right one)
float drawAnswerBox(const char *answer, float x, float y, float width, Color border) {
    char text[500];
    oneStatementPerLine(answer, text, sizeof(text));
    float height = countLines(text) * lineHeight(14) + 14;
    DrawRectangle((int)x, (int)y, (int)width, (int)height, COLOUR_CODE_BG);
    DrawRectangleLinesEx(Rectangle{ x, y, width, height }, 2, border);
    drawCode(text, x + 10, y + 7, 14, width - 20);
    return height;
}

// ---------- Choosing the chapters ----------
void drawTestSetup(App *app) {
    if (IsKeyPressed(KEY_ESCAPE)) {
        app->screen = SCREEN_MENU;
        return;
    }
    drawBold("TEST MODE", 30, 16, 34, COLOUR_ACCENT);
    drawPlayerSwitch(app, SCREEN_WIDTH - 420.0f, 14);
    drawWrapped("10 questions from the chapters you pick: a mix of the levels' own questions and new ones in the "
                "same style. It works like the real test - no hints, no lessons or slides, and one try at each. "
                "Everything is marked at the end.", 30, 70, SCREEN_WIDTH - 60.0f, 16, COLOUR_TEXT);

    drawText("Chapters:", 30, 142, 16, COLOUR_MUTED);
    for (int c = 1; c <= CHAPTER_COUNT; c++) {
        int column = (c - 1) / 4;
        int row = (c - 1) % 4;
        Rectangle box = { 30 + column * 440.0f, 170 + row * 58.0f, 420, 48 };
        const char *mark = app->testChapters[c] ? "[x]" : "[ ]";
        if (button(box, TextFormat("%s  %d. %s", mark, c, chapterName(c)))) app->testChapters[c] = !app->testChapters[c];
    }
    if (button(Rectangle{ 30, 410, 140, 40 }, "All")) {
        for (int c = 1; c <= CHAPTER_COUNT; c++) app->testChapters[c] = true;
    }
    if (button(Rectangle{ 180, 410, 140, 40 }, "None")) {
        for (int c = 1; c <= CHAPTER_COUNT; c++) app->testChapters[c] = false;
    }

    bool anyChosen = false;
    for (int c = 1; c <= CHAPTER_COUNT; c++) {
        if (app->testChapters[c]) anyChosen = true;
    }
    if (anyChosen) {
        if (button(Rectangle{ 30, SCREEN_HEIGHT - 76.0f, 280, 52 }, "Start the test >")) startTest(app);
    } else {
        drawText("Pick at least one chapter.", 30, SCREEN_HEIGHT - 60.0f, 17, COLOUR_BAD);
    }
    if (button(Rectangle{ SCREEN_WIDTH - 170.0f, SCREEN_HEIGHT - 70.0f, 150, 44 }, "Back")) app->screen = SCREEN_MENU;
}

// ---------- Answering ----------
void drawTestRunning(App *app, float dt) {
    app->testTime += dt;
    updateCodePhase(app);                          // typing, and Ctrl/Cmd + Enter submits
    if (app->testState != TEST_RUNNING) return;    // that was the last answer

    drawBold("TEST", 30, 16, 30, COLOUR_ACCENT);
    drawText(TextFormat("Question %d of %d", app->testIndex + 1, TEST_LENGTH), 120, 26, 18, COLOUR_TEXT);
    // A dot for each question: answered, this one, still to come
    for (int i = 0; i < TEST_LENGTH; i++) {
        Vector2 centre = { 330 + i * 26.0f, 36 };
        if (i < app->testIndex) DrawCircleV(centre, 8, COLOUR_MUTED);
        else if (i == app->testIndex) DrawCircleV(centre, 9, COLOUR_ACCENT);
        else DrawCircleLines((int)centre.x, (int)centre.y, 8, COLOUR_MUTED);
    }
    drawBold(timeText(app->testTime), SCREEN_WIDTH - 380.0f, 24, 24, COLOUR_TEXT);

    const char *stopLabel = "Stop the test";
    if (app->confirmStopTest) stopLabel = "Click again to stop";
    if (button(Rectangle{ SCREEN_WIDTH - 260.0f, 18, 240, 40 }, stopLabel)) {
        if (app->confirmStopTest) {
            app->testing = false;
            app->testState = TEST_SETUP;
            return;
        }
        app->confirmStopTest = true;
    }

    DrawRectangle(150, 80, SCREEN_WIDTH - 300, SCREEN_HEIGHT - 92, COLOUR_PANEL);
    drawTaskArea(app, 175, 96, SCREEN_WIDTH - 350.0f, SCREEN_HEIGHT - 62.0f);
}

// ---------- The results ----------
void drawTestResults(App *app) {
    if (IsKeyPressed(KEY_ESCAPE)) {
        app->testState = TEST_SETUP;
        return;
    }
    int score = 0;
    for (int i = 0; i < TEST_LENGTH; i++) {
        if (app->testQuestions[i].correct) score++;
    }
    drawBold("TEST RESULTS", 30, 16, 30, COLOUR_ACCENT);
    Color scoreColour = COLOUR_BAD;
    if (score >= 5) scoreColour = COLOUR_ACCENT;
    if (score >= 7) scoreColour = COLOUR_GOOD;
    drawBold(TextFormat("%d / %d", score, TEST_LENGTH), 290, 8, 42, scoreColour);
    const char *message = "Click a question to see the right answer, then practise those levels.";
    if (score == TEST_LENGTH) message = "Perfect - every one right!";
    else if (score >= 7) message = "Well done. Have a look at the ones you missed.";
    drawText(TextFormat("%s, in %s.  %s", app->learner->name, timeText(app->testTime), message), 30, 62, 16, COLOUR_TEXT);

    // The ten questions on the left
    Vector2 mouse = GetMousePosition();
    for (int i = 0; i < TEST_LENGTH; i++) {
        TestQuestion *q = &app->testQuestions[i];
        Rectangle row = { 30, 98 + i * 52.0f, 380, 46 };
        bool hover = CheckCollisionPointRec(mouse, row);
        Color background = COLOUR_PANEL;
        if (i == app->testSelected) background = Color{ 70, 62, 20, 255 };
        else if (hover) background = COLOUR_BUTTON_HOVER;
        DrawRectangleRec(row, background);
        drawMark(q->correct, row.x + 22, row.y + 23);
        drawText(TextFormat("Q%d   Level %d", i + 1, q->level), row.x + 44, row.y + 5, 13, COLOUR_MUTED);
        drawText(getLevel(q->level)->title, row.x + 44, row.y + 23, 15, COLOUR_TEXT);
        if (hover && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) app->testSelected = i;
    }

    // The chosen question on the right: the question, your answer, and the right one
    TestQuestion *q = &app->testQuestions[app->testSelected];
    float x = 452;
    float width = SCREEN_WIDTH - x - 36;
    float y = 106;
    Rectangle page = { x - 16, 92, width + 32, SCREEN_HEIGHT - 172.0f };
    DrawRectangleRec(page, COLOUR_PANEL);
    BeginScissorMode((int)page.x, (int)page.y, (int)page.width, (int)page.height);
    drawText(TextFormat("Question %d - Level %d: %s", app->testSelected + 1, q->level, getLevel(q->level)->title), x, y, 14, COLOUR_MUTED);
    y += 22;
    y += drawWrapped(q->task, x, y, width, 15, COLOUR_TEXT) + 8;
    float codeHeight = countLines(q->code) * lineHeight(14) + 14;
    DrawRectangle((int)x, (int)y, (int)width, (int)codeHeight, COLOUR_CODE_BG);
    if (q->kind == TASK_BUG) drawNumberedCode(q->code, x + 10, y + 7, 14);
    else drawCode(q->code, x + 10, y + 7, 14, width - 20);
    y += codeHeight + 12;

    drawText("Your answer:", x, y, 14, COLOUR_MUTED);
    y += 20;
    y += drawAnswerBox(q->given, x, y, width, q->correct ? COLOUR_GOOD : COLOUR_BAD) + 12;
    if (q->correct) {
        drawText("Correct!", x, y, 16, COLOUR_GOOD);
    } else {
        drawText("The answer:", x, y, 14, COLOUR_GOOD);
        y += 20;
        y += drawAnswerBox(q->answer, x, y, width, COLOUR_GOOD) + 10;
        drawWrapped(q->why, x, y, width, 15, COLOUR_BAD);
    }
    EndScissorMode();

    if (button(Rectangle{ x - 16, SCREEN_HEIGHT - 70.0f, 270, 46 }, "Practise this level >")) startPractice(app, q->level);
    if (button(Rectangle{ x + 266, SCREEN_HEIGHT - 70.0f, 200, 46 }, "Another test")) app->testState = TEST_SETUP;
    if (button(Rectangle{ SCREEN_WIDTH - 170.0f, SCREEN_HEIGHT - 70.0f, 150, 46 }, "Back")) app->screen = SCREEN_MENU;
}

void testScreen(App *app) {
    ClearBackground(COLOUR_BG);
    if (app->testState == TEST_SETUP) drawTestSetup(app);
    else if (app->testState == TEST_RUNNING) drawTestRunning(app, GetFrameTime());
    else drawTestResults(app);
}
