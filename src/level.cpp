#include "level.h"
#include "lab2.h"
#include "table.h"   // for SCREEN_WIDTH and SCREEN_HEIGHT

// ============================================================
// Level 1: swap_ints
// ============================================================

Level getLevel1() {
    Level level;
    level.title = "Level 1: swap_ints";
    level.task = "The 8-ball must sit in the middle of the\n"
                 "rack, but it's at the front. Fill in the\n"
                 "blank so swap_ints swaps the front ball\n"
                 "with the middle ball.";

    level.code[0] = "// Lab 2 Q1: swap two ints using pointers";
    level.code[1] = "void swap_ints(int *a, int *b) {";
    level.code[2] = "    int temp = *a;";
    level.code[3] = "    *a = *b;";
    level.code[4] = "    *b = %s;";
    level.code[5] = "}";
    level.code[6] = "";
    level.code[7] = "// rack + 4 is the address of slot 4";
    level.code[8] = "// (the same as &rack[4])";
    level.code[9] = "swap_ints(rack, rack + 4);";
    level.lineCount = 10;
    level.blankLine = 4;

    level.choices[0] = "temp";
    level.choices[1] = "*a";
    level.choices[2] = "a";
    level.choiceCount = 3;
    level.correctChoice = 0;

    return level;
}

// What the code does if the player picks "*a" for the blank.
// Their version still has "int temp = *a;", but temp is never used,
// so it is left out here.
void swap_ints_pickedStarA(int *a, int *b) {
    *a = *b;   // a gets b's value...
    *b = *a;   // ...then b copies a, which is now b's own value. a's old value is lost.
}

// Runs the player's answer on the rack and returns the message to show
const char *runLevel1Choice(int choice, int rack[]) {
    if (choice == 0) {
        // "temp" - the correct answer, so we run the real Lab 2 function
        swap_ints(rack, rack + 4);
        return "Correct! temp saved the 8-ball's number\n"
               "before *a was overwritten, so the two\n"
               "slots swapped.\n\n"
               "Press SPACE to break!";
    } else if (choice == 1) {
        // "*a" - runs, but gives the wrong result
        swap_ints_pickedStarA(rack, rack + 4);
        return "Two 1-balls and no 8-ball!\n"
               "*a was already changed on the line above,\n"
               "so *b copied the new value and the\n"
               "8-ball's number was lost.\n"
               "That's why temp saves it first.";
    } else {
        // "a" - this would not compile, so the rack doesn't change
        return "Compile error:\n"
               "assigning to 'int' from incompatible\n"
               "type 'int *'\n\n"
               "a is a pointer (an address). *b needs an\n"
               "int (a ball number), like temp.";
    }
}

// ============================================================
// Code panel
// ============================================================

Color PANEL_COLOUR = { 40, 40, 48, 255 };
Color CODE_BOX_COLOUR = { 22, 22, 28, 255 };
Color LIGHT_RED = { 255, 130, 130, 255 };

const int CODE_Y = 185;        // top of the code box
const int LINE_HEIGHT = 25;
const int BUTTON_Y = 470;

// Where answer button number 'index' goes.
// Drawing and clicking both use this, so they always line up.
Rectangle getChoiceButton(int index) {
    Rectangle button = { (float)(PANEL_X + 20 + index * 150), (float)BUTTON_Y, 130, 44 };
    return button;
}

int getClickedChoice(const Level &level) {
    if (!IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        return -1;
    }

    Vector2 mouse = GetMousePosition();
    for (int i = 0; i < level.choiceCount; i++) {
        if (CheckCollisionPointRec(mouse, getChoiceButton(i))) {
            return i;
        }
    }
    return -1;
}

void drawCodePanel(const Level &level, int pickedChoice, const char *message) {
    int x = PANEL_X + 20;

    // Panel background
    DrawRectangle(PANEL_X, 0, SCREEN_WIDTH - PANEL_X, SCREEN_HEIGHT, PANEL_COLOUR);

    // Title and task
    DrawText(level.title, x, 30, 28, RAYWHITE);
    DrawText(level.task, x, 80, 18, LIGHTGRAY);

    // Code box
    DrawRectangle(x, CODE_Y, 470, level.lineCount * LINE_HEIGHT + 20, CODE_BOX_COLOUR);

    for (int i = 0; i < level.lineCount; i++) {
        const char *line = level.code[i];
        Color colour = RAYWHITE;

        if (i == level.blankLine) {
            // Fill the blank with the picked answer, or ____ if nothing is picked yet
            const char *answer = "____";
            if (pickedChoice != -1) {
                answer = level.choices[pickedChoice];
            }
            line = TextFormat(level.code[i], answer);
            colour = YELLOW;
        } else if (line[0] == '/' && line[1] == '/') {
            // Comments are grey, like in VS Code
            colour = GRAY;
        }

        DrawText(line, x + 15, CODE_Y + 12 + i * LINE_HEIGHT, 20, colour);
    }

    // Answer buttons
    Vector2 mouse = GetMousePosition();
    for (int i = 0; i < level.choiceCount; i++) {
        Rectangle button = getChoiceButton(i);

        Color colour = DARKGRAY;
        if (i == pickedChoice && i == level.correctChoice) {
            colour = DARKGREEN;     // picked and right
        } else if (i == pickedChoice) {
            colour = MAROON;        // picked and wrong
        } else if (CheckCollisionPointRec(mouse, button)) {
            colour = GRAY;          // mouse is over it
        }

        DrawRectangleRounded(button, 0.3f, 8, colour);
        int textWidth = MeasureText(level.choices[i], 22);
        DrawText(level.choices[i], (int)(button.x + button.width / 2) - textWidth / 2, (int)button.y + 11, 22, RAYWHITE);
    }

    // Message under the buttons
    Color messageColour = RAYWHITE;
    if (pickedChoice == level.correctChoice) {
        messageColour = GREEN;
    } else if (pickedChoice != -1) {
        messageColour = LIGHT_RED;
    }
    DrawText(message, x, BUTTON_Y + 70, 18, messageColour);
}
