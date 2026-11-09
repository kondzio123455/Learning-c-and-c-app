#include "raylib.h"
#include "app.h"
#include "ui.h"
#include "effects.h"
#include <cmath>
#include <cstring>

// The whole app's state. Every screen function gets a pointer to it.
App app;

void setupApp(App *app) {
    app->screen = SCREEN_MENU;
    app->phase = PHASE_CODE;
    app->quit = false;
    app->gameInProgress = false;

    // Everyone who has played on this computer, and who Player 1 and 2 were last time
    int slot1, slot2;
    app->profileCount = loadProfiles(app->profiles, &slot1, &slot2);
    app->playerProfile[0] = nullptr;
    app->playerProfile[1] = nullptr;
    if (app->profileCount > 0) {
        app->playerProfile[0] = &app->profiles[slot1];   // a pointer to one element of the array
        app->playerProfile[1] = &app->profiles[slot2];
    }
    app->learner = app->playerProfile[0];
    app->pickingSlot = -1;
    app->confirmDelete = -1;
    setupCodeBox(&app->nameBox, 1, "Type a name...");
    app->afterWelcome = PHASE_CODE;
    app->showRewind = false;
    app->rewinding = false;
    app->shotSaved = false;
    app->questionLevel = 1;
    app->version = 0;
    app->reviewMode = false;
    app->callNeeded = false;
    app->practising = false;
    app->solved = false;
    app->hintsShown = 0;
    app->taskScroll = 0;
    app->viewTop = 0;
    app->viewBottom = SCREEN_HEIGHT;
    app->chosenCount = 0;
    app->usingMadeUp = false;
    app->showSlide = false;

    setupCodeBox(&app->answerBox, 4, "Click here and type your code...");
    setupCodeBox(&app->callBox, 1, "Ball *target = &balls[...];");
    app->feedback[0] = '\0';
    app->feedbackGood = false;

    app->runTimer = 0;
    app->power = 0;
    app->charging = false;
    app->strikeTimer = 0;
    app->aimX = RACK_X;
    app->aimY = RACK_Y;

    app->phaseTimer = 0;
    app->wrongTimer = 0;
    app->bannerTimer = 0;

    app->codexLevel = 1;
    app->confirmReset = false;
    app->confirmUnlockAll = false;
    app->logView = nullptr;
}

// ============================================================
// Players: each person has their own profile and save file
// ============================================================

// Which element of the profiles array a pointer points at (pointer arithmetic, Level 11)
int profileIndex(App *app, Profile *profile) {
    return (int)(profile - app->profiles);
}

void saveSlots(App *app) {
    saveProfileList(app->profiles, app->profileCount,
                    profileIndex(app, app->playerProfile[0]), profileIndex(app, app->playerProfile[1]));
}

// Puts a player in a slot. Changing who's playing starts a new game.
void choosePlayer(App *app, int slot, Profile *profile) {
    if (app->playerProfile[slot] != profile) app->gameInProgress = false;
    app->playerProfile[slot] = profile;
    // The very first player plays both sides until someone else joins
    if (app->playerProfile[1 - slot] == nullptr) app->playerProfile[1 - slot] = profile;
    app->learner = app->playerProfile[0];
    app->pickingSlot = -1;
    saveSlots(app);
}

// Adds a new player. Names are letters, digits and spaces (up to 12).
// If the name is already taken, that player is returned instead.
Profile *addProfile(App *app, const char *typed) {
    char name[NAME_LENGTH];
    int length = 0;
    for (const char *p = typed; *p != '\0' && length < NAME_LENGTH - 1; p++) {
        bool ok = (*p >= 'a' && *p <= 'z') || (*p >= 'A' && *p <= 'Z') || (*p >= '0' && *p <= '9') || *p == ' ';
        bool leadingSpace = *p == ' ' && length == 0;
        if (ok && !leadingSpace) {
            name[length] = *p;
            length++;
        }
    }
    while (length > 0 && name[length - 1] == ' ') length--;   // no spaces at the end
    name[length] = '\0';
    if (length == 0) return nullptr;

    for (int i = 0; i < app->profileCount; i++) {
        if (strcmp(app->profiles[i].name, name) == 0) return &app->profiles[i];
    }
    if (app->profileCount >= MAX_PROFILES) return nullptr;

    Profile *profile = &app->profiles[app->profileCount];
    setupProfile(profile, name);
    saveProfile(profile);
    app->profileCount++;
    return profile;
}

// Deletes a player who isn't playing right now, and their save file.
// The array closes the gap, so the slots' pointers are worked out again.
void deleteProfile(App *app, int index) {
    int slot1 = profileIndex(app, app->playerProfile[0]);
    int slot2 = profileIndex(app, app->playerProfile[1]);
    deleteProfileFile(&app->profiles[index]);
    for (int i = index; i < app->profileCount - 1; i++) {
        app->profiles[i] = app->profiles[i + 1];
    }
    app->profileCount--;
    if (slot1 > index) slot1--;
    if (slot2 > index) slot2--;
    app->playerProfile[0] = &app->profiles[slot1];
    app->playerProfile[1] = &app->profiles[slot2];
    app->learner = app->playerProfile[0];
    saveSlots(app);
}

// "Who is Player 1?": pick a name, add a new player, or delete one
void drawPlayerPicker(App *app) {
    int slot = app->pickingSlot;
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Fade(BLACK, 0.7f));
    Rectangle box = { 330, 50, 620, 620 };
    DrawRectangleRec(box, COLOUR_PANEL);
    DrawRectangleLinesEx(box, 2, COLOUR_ACCENT);
    float x = box.x + 30;
    float y = box.y + 24;

    drawBold(TextFormat("Who is Player %d?", slot + 1), x, y, 26, COLOUR_ACCENT);
    y += 42;
    y += drawWrapped("Everyone has their own saved progress. Pick your name, or add yourself as a new player.",
                     x, y, box.width - 60, 15, COLOUR_MUTED) + 16;

    Vector2 mouse = GetMousePosition();
    for (int i = 0; i < app->profileCount; i++) {
        Profile *profile = &app->profiles[i];
        Rectangle row = { x, y, 400, 36 };
        bool hover = CheckCollisionPointRec(mouse, row);
        DrawRectangleRec(row, hover ? COLOUR_BUTTON_HOVER : COLOUR_BUTTON);
        drawBold(profile->name, row.x + 12, row.y + 9, 17, COLOUR_TEXT);
        const char *where = TextFormat("Level %d", profile->levelsDone + 1);
        if (profile->levelsDone >= LEVEL_COUNT) where = "all done";
        drawText(where, row.x + 230, row.y + 10, 15, COLOUR_MUTED);
        int stars = 0;
        for (int n = 1; n <= LEVEL_COUNT; n++) stars += profile->stars[n];
        drawStar(row.x + 336, row.y + 18, 7, COLOUR_ACCENT);
        drawText(TextFormat("%d", stars), row.x + 350, row.y + 10, 15, COLOUR_MUTED);
        if (hover && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            choosePlayer(app, slot, profile);
            return;
        }

        // Players who aren't playing right now can be deleted (it needs two clicks)
        bool inUse = profile == app->playerProfile[0] || profile == app->playerProfile[1];
        if (!inUse) {
            const char *label = "Delete";
            if (app->confirmDelete == i) label = "Sure?";
            if (button(Rectangle{ x + 412, y, 150, 36 }, label)) {
                if (app->confirmDelete == i) {
                    deleteProfile(app, i);
                    app->confirmDelete = -1;
                    return;
                }
                app->confirmDelete = i;
            }
        }
        y += 44;
    }

    // A new player
    y += 10;
    if (app->profileCount < MAX_PROFILES) {
        drawText("New player's name:", x, y, 15, COLOUR_TEXT);
        y += 24;
        app->nameBox.focused = true;
        updateCodeBox(&app->nameBox);
        app->nameBox.area = Rectangle{ x, y, 400, 42 };
        drawCodeBox(&app->nameBox, -1);
        bool add = button(Rectangle{ x + 412, y, 150, 42 }, "Add");
        if (IsKeyPressed(KEY_ENTER)) add = true;
        if (add) {
            Profile *profile = addProfile(app, app->nameBox.text);
            if (profile != nullptr) {
                clearCodeBox(&app->nameBox);
                choosePlayer(app, slot, profile);
                return;
            }
        }
    } else {
        drawText("That's 8 players, the most there can be. Delete one to add another.", x, y, 15, COLOUR_MUTED);
    }

    // Cancel is only there once the slot has someone in it
    if (app->playerProfile[slot] != nullptr) {
        bool cancel = button(Rectangle{ box.x + box.width - 180, box.y + box.height - 60, 150, 40 }, "Cancel");
        if (cancel || IsKeyPressed(KEY_ESCAPE)) app->pickingSlot = -1;
    }
}

// The lessons and progress screens show one player at a time
void drawPlayerSwitch(App *app, float x, float y) {
    drawText(TextFormat("Showing: %s", app->learner->name), x, y + 10, 16, COLOUR_TEXT);
    if (app->playerProfile[0] != app->playerProfile[1]) {
        if (button(Rectangle{ x + 200, y, 180, 38 }, "Switch player")) {
            if (app->learner == app->playerProfile[0]) app->learner = app->playerProfile[1];
            else app->learner = app->playerProfile[0];
        }
    }
}

// ============================================================
// Main menu
// ============================================================

// A few balls drifting around behind the menu
const int MENU_BALLS = 7;
float menuX[MENU_BALLS], menuY[MENU_BALLS], menuVX[MENU_BALLS], menuVY[MENU_BALLS], menuRoll[MENU_BALLS];
int menuNumber[MENU_BALLS] = { 0, 1, 9, 8, 3, 14, 6 };

void setupMenuBalls() {
    for (int i = 0; i < MENU_BALLS; i++) {
        menuX[i] = (float)GetRandomValue(650, 1200);
        menuY[i] = (float)GetRandomValue(120, 640);
        menuVX[i] = (float)GetRandomValue(-60, 60);
        menuVY[i] = (float)GetRandomValue(-60, 60);
        menuRoll[i] = 0;
    }
}

void menuScreen(App *app) {
    float dt = GetFrameTime();
    ClearBackground(COLOUR_BG);

    // Move the drifting balls and bounce them off the edges of their area
    for (int i = 0; i < MENU_BALLS; i++) {
        menuX[i] += menuVX[i] * dt;
        menuY[i] += menuVY[i] * dt;
        if (menuX[i] < 640 || menuX[i] > 1240) menuVX[i] = -menuVX[i];
        if (menuY[i] < 100 || menuY[i] > 660) menuVY[i] = -menuVY[i];
        float speed = sqrtf(menuVX[i] * menuVX[i] + menuVY[i] * menuVY[i]);
        menuRoll[i] += speed * dt / 26;
        float dx = 1, dy = 0;
        if (speed > 0) { dx = menuVX[i] / speed; dy = menuVY[i] / speed; }
        drawBallRolling(menuNumber[i], menuX[i], menuY[i], 26, menuRoll[i], dx, dy, 0.9f);
    }

    float glow = 0.85f + 0.15f * sinf((float)GetTime() * 2);
    drawBold("POINTER POOL", 90, 120, 64, Fade(COLOUR_ACCENT, glow));
    drawText("Learn C and C++ pointers, one shot at a time.", 94, 200, 20, COLOUR_TEXT);
    drawText("Built from the Pointers, Memory, Classes and Linked List lectures,", 94, 232, 15, COLOUR_MUTED);
    drawText("and Labs 1 and 2.", 94, 252, 15, COLOUR_MUTED);

    // Nobody has played yet: ask for a name first
    if (app->profileCount == 0 && app->pickingSlot == -1) app->pickingSlot = 0;
    if (app->pickingSlot != -1) {
        drawPlayerPicker(app);
        return;
    }

    // Who's playing? Each player has their own saved progress.
    float x = 94;
    for (int s = 0; s < 2; s++) {
        float px = x + s * 330;
        drawText(TextFormat("Player %d:", s + 1), px, 297, 16, COLOUR_MUTED);
        drawBold(app->playerProfile[s]->name, px + 92, 296, 16, COLOUR_TEXT);
        if (button(Rectangle{ px + 222, 288, 90, 34 }, "Change")) {
            app->pickingSlot = s;
            app->confirmDelete = -1;
            clearCodeBox(&app->nameBox);
        }
    }

    const char *playLabel = "Play";
    if (app->gameInProgress) playLabel = "Continue";
    float y = 344;
    if (button(Rectangle{ x, y, 260, 48 }, playLabel)) {
        if (!app->gameInProgress) {
            startNewGame(app);
        } else if (app->shotSaved) {
            restoreShot(app);   // back from practice: the same shot as before
        }
        app->learner = app->playerProfile[app->game.currentPlayer];
        app->screen = SCREEN_PLAY;
    }
    if (app->gameInProgress && button(Rectangle{ x + 280, y, 200, 48 }, "New game")) {
        startNewGame(app);
        app->screen = SCREEN_PLAY;
    }
    if (button(Rectangle{ x, y + 64, 260, 48 }, "Lessons & practice")) app->screen = SCREEN_CODEX;
    if (button(Rectangle{ x, y + 128, 260, 48 }, "Progress")) {
        app->confirmReset = false;
        app->confirmUnlockAll = false;
        app->screen = SCREEN_PROGRESS;
    }
    if (button(Rectangle{ x, y + 192, 260, 48 }, "Quit")) app->quit = true;

    // Each player's progress (once, if the same person plays both sides)
    for (int s = 0; s < 2; s++) {
        Profile *profile = app->playerProfile[s];
        if (s == 1 && profile == app->playerProfile[0]) break;
        int stars = 0;
        for (int n = 1; n <= LEVEL_COUNT; n++) stars += profile->stars[n];
        drawStar(104, 618 + s * 28.0f, 8, COLOUR_ACCENT);
        drawText(TextFormat("%s: %d of %d concepts, %d stars", profile->name, profile->levelsDone, LEVEL_COUNT, stars),
                 120, 609 + s * 28.0f, 17, COLOUR_TEXT);
    }
    drawText("Two players, one computer. Write the code, then take the shot.", 94, 676, 15, COLOUR_MUTED);
}

// ============================================================
// Codex: re-read every concept you've reached, and practise it
// ============================================================

void codexScreen(App *app) {
    ClearBackground(COLOUR_BG);
    if (IsKeyPressed(KEY_ESCAPE) && !app->showSlide) app->screen = SCREEN_MENU;

    drawBold("Lessons", 30, 18, 34, COLOUR_ACCENT);
    drawText("All 54 levels are open here to read and practise. (The game still unlocks them one at a time.)", 30, 60, 16, COLOUR_MUTED);
    if (!app->showSlide) drawPlayerSwitch(app, SCREEN_WIDTH - 420.0f, 14);

    // The list on the left (54 levels, so two columns)
    Vector2 mouse = GetMousePosition();
    for (int n = 1; n <= LEVEL_COUNT; n++) {
        Level *level = getLevel(n);
        int column = 0;
        int row = n - 1;
        if (n > 27) {
            column = 1;
            row = n - 28;
        }
        float x = 20 + column * 205.0f;
        float y = 92 + row * 22.0f;
        Rectangle box = { x, y, 200, 21 };

        bool hover = CheckCollisionPointRec(mouse, box);
        if (n == app->codexLevel) DrawRectangleRec(box, Color{ 70, 62, 20, 255 });
        else if (hover) DrawRectangleRec(box, COLOUR_PANEL);
        drawText(TextFormat("%2d  %s", n, level->shortName), x + 6, y + 3, 14, COLOUR_TEXT);
        drawStars(x + 152, y + 10, app->learner->stars[n], 5);
        if (hover && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) app->codexLevel = n;
    }

    // The chosen concept on the right
    Level *level = getLevel(app->codexLevel);
    float x = 450;
    Rectangle page = { x - 20, 88, SCREEN_WIDTH - x, SCREEN_HEIGHT - 158.0f };
    DrawRectangleRec(page, COLOUR_PANEL);
    drawText(TextFormat("Level %d - Chapter %d: %s", app->codexLevel, level->chapter, chapterName(level->chapter)),
             x, 100, 14, COLOUR_MUTED);
    drawBold(level->title, x, 120, 28, COLOUR_TEXT);
    drawLessonContent(level, x, 162, SCREEN_WIDTH - x - 40);

    if (!app->showSlide) {
        if (button(Rectangle{ x - 20, SCREEN_HEIGHT - 56.0f, 220, 40 }, "Practise this >")) {
            if (app->gameInProgress && !app->shotSaved) saveShot(app);   // so Continue comes back to the same shot
            startPractice(app, app->codexLevel);
        }
        if (button(Rectangle{ x + 215, SCREEN_HEIGHT - 56.0f, 190, 40 }, "Show the slide")) app->showSlide = true;
        if (button(Rectangle{ SCREEN_WIDTH - 170.0f, SCREEN_HEIGHT - 56.0f, 150, 40 }, "Back")) app->screen = SCREEN_MENU;
    } else {
        drawSlideOverlay(app);
    }
}

// ============================================================
// Progress: the unlock tree
// ============================================================

void progressScreen(App *app) {
    ClearBackground(COLOUR_BG);
    if (IsKeyPressed(KEY_ESCAPE)) app->screen = SCREEN_MENU;

    drawBold("Progress", 30, 16, 34, COLOUR_ACCENT);
    drawText(TextFormat("%s: %d of %d concepts done. Pot one of your balls to unlock the next one.",
                        app->learner->name, app->learner->levelsDone, LEVEL_COUNT), 30, 56, 15, COLOUR_MUTED);
    drawPlayerSwitch(app, SCREEN_WIDTH - 420.0f, 14);

    Rectangle bar = { 30, 80, SCREEN_WIDTH - 60.0f, 10 };
    DrawRectangleRec(bar, COLOUR_PANEL);
    DrawRectangle(30, 80, (int)(bar.width * app->learner->levelsDone / LEVEL_COUNT), 10, COLOUR_GOOD);

    // One row per chapter (long chapters wrap onto a second row)
    float rowY = 104;
    int column = 0;
    Vector2 mouse = GetMousePosition();
    for (int n = 1; n <= LEVEL_COUNT; n++) {
        Level *level = getLevel(n);
        bool newChapter = (n == 1) || getLevel(n - 1)->chapter != level->chapter;
        if (newChapter) {
            if (n > 1) rowY += 64;
            column = 0;
            drawBold(TextFormat("%d. %s", level->chapter, chapterName(level->chapter)), 30, rowY + 6, 14, COLOUR_TEXT);
        } else if (column == 9) {
            rowY += 64;
            column = 0;
        }

        Rectangle node = { 230.0f + column * 112, rowY, 104, 54 };
        if (column > 0) DrawLineEx(Vector2{ node.x - 8, node.y + 27 }, Vector2{ node.x, node.y + 27 }, 2, Color{ 70, 75, 90, 255 });

        Color background = Color{ 40, 43, 52, 255 };
        Color border = Color{ 70, 75, 90, 255 };
        Color textColour = Color{ 90, 95, 110, 255 };
        if (n <= app->learner->levelsDone) {
            background = Color{ 30, 60, 40, 255 };
            border = COLOUR_GOOD;
            textColour = COLOUR_TEXT;
        } else if (n == currentLevel(app)) {
            float pulse = 0.6f + 0.4f * sinf((float)GetTime() * 4);
            background = Color{ 70, 62, 20, 255 };
            border = Fade(COLOUR_ACCENT, pulse);
            textColour = COLOUR_TEXT;
        }
        DrawRectangleRec(node, background);
        DrawRectangleLinesEx(node, 2, border);
        drawBold(TextFormat("%d", n), node.x + 6, node.y + 5, 14, textColour);
        drawText(level->shortName, node.x + 6, node.y + 23, 13, textColour);
        if (reached(app, n)) drawStars(node.x + 62, node.y + 12, app->learner->stars[n], 5);

        // Click a box you've reached to read it
        if (reached(app, n) && CheckCollisionPointRec(mouse, node) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            app->codexLevel = n;
            app->screen = SCREEN_CODEX;
        }
        column++;
    }

    // Upgrades, like the unlocks in The Farmer Was Replaced
    float upY = SCREEN_HEIGHT - 104.0f;
    drawBold("Upgrades:", 30, upY, 15, COLOUR_TEXT);
    const char *names[3] = { "Ghost ball (finish Ch 1)", "Ball path (finish Ch 3)", "Golden cue (finish Ch 7)" };
    int needs[3] = { AIM_GUIDE_LEVEL, BALL_PATH_LEVEL, GOLD_CUE_LEVEL };
    for (int i = 0; i < 3; i++) {
        bool unlocked = app->learner->levelsDone >= needs[i];
        Color colour = Color{ 90, 95, 110, 255 };
        if (unlocked) colour = COLOUR_GOOD;
        drawText(TextFormat("%s %s", unlocked ? "[x]" : "[ ]", names[i]), 140 + i * 300.0f, upY, 15, colour);
    }

    // Buttons
    if (button(Rectangle{ SCREEN_WIDTH - 170.0f, SCREEN_HEIGHT - 56.0f, 150, 40 }, "Back")) app->screen = SCREEN_MENU;

    const char *resetLabel = TextFormat("Reset %s", app->learner->name);
    if (app->confirmReset) resetLabel = "Click again to reset";
    if (button(Rectangle{ 30, SCREEN_HEIGHT - 56.0f, 240, 40 }, resetLabel)) {
        if (app->confirmReset) {
            app->learner->levelsDone = 0;
            for (int n = 0; n <= LEVEL_COUNT; n++) app->learner->stars[n] = 0;
            saveProfile(app->learner);
            app->learner->lessonShown = false;
            app->learner->askCount = 0;
            app->gameInProgress = false;
            app->confirmReset = false;
        } else {
            app->confirmReset = true;
        }
    }

    // For revising before a test: open every lesson straight away
    const char *unlockLabel = "Unlock all (revision)";
    if (app->confirmUnlockAll) unlockLabel = "Click again to unlock";
    if (app->learner->levelsDone < LEVEL_COUNT && button(Rectangle{ 290, SCREEN_HEIGHT - 56.0f, 260, 40 }, unlockLabel)) {
        if (app->confirmUnlockAll) {
            app->learner->levelsDone = LEVEL_COUNT;
            saveProfile(app->learner);
            app->confirmUnlockAll = false;
        } else {
            app->confirmUnlockAll = true;
        }
    }
}

// ============================================================
// main
// ============================================================

int main() {
    SetConfigFlags(FLAG_MSAA_4X_HINT);   // smoother circles
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Pointer Pool - learn C and C++ pointers");
    SetExitKey(KEY_NULL);                // Esc goes back instead of closing the window
    SetTargetFPS(60);                    // the friction is tuned for 60 frames per second

    loadFonts();
    loadSounds();
    setupTableTextures();
    setupLevels();
    setupApp(&app);
    setupMenuBalls();

    while (!WindowShouldClose() && !app.quit) {
        float dt = GetFrameTime();
        updateParticles(dt);
        updateFloatingTexts(dt);
        if (app.bannerTimer > 0) app.bannerTimer -= dt;

        // F5 unlocks every level for whoever's progress is showing (handy for demos)
        if (IsKeyPressed(KEY_F5) && app.learner != nullptr) {
            app.learner->levelsDone = LEVEL_COUNT;
            saveProfile(app.learner);
        }

        BeginDrawing();
        if (app.screen == SCREEN_MENU) menuScreen(&app);
        else if (app.screen == SCREEN_PLAY) playScreen(&app);
        else if (app.screen == SCREEN_CODEX) codexScreen(&app);
        else if (app.screen == SCREEN_PROGRESS) progressScreen(&app);
        else if (app.screen == SCREEN_PRACTICE) practiceScreen(&app);

        // Effects go on top of every screen
        drawParticles();
        drawFloatingTexts();
        drawBanner(&app);
        EndDrawing();
    }

    unloadTableTextures();
    unloadSounds();
    unloadFonts();
    CloseWindow();
    return 0;
}
