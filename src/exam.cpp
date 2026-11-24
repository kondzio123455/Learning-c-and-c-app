#include "app.h"
#include "ui.h"
#include "fx.h"
#include "sound.h"
#include "exam_questions.h"
#include <cstdio>
#include <cstring>
#include <cstdlib>

// ============================================================
// Exam mode: practice for Coding Exam 2.
// 6 questions from Lab 1 and Lab 2, 1 hour, and a premade main that tests
// your functions, like the real exam. Your code is really compiled (with
// g++ or clang++) and run, and main prints yes or no for each test.
// ============================================================

const int EXAM_SIZE = 6;
const float EXAM_SECONDS = 60 * 60;

enum ExamState { EXAM_SETUP, EXAM_RUNNING, EXAM_MARKING, EXAM_RESULTS };

// The result of compiling and running one question's code
struct ExamRun {
    bool ran;           // has it been run at all?
    bool compiled;
    bool finished;      // main got to the end (false = it crashed or took too long)
    bool timedOut;
    bool indexing;      // a Lab 2 question that uses arr[i]
    bool untouched;     // the code is still the lab's starter stub (marked as not attempted)
    int passed;
    int total;
    char output[2500];  // what it printed, or the compiler's errors
};

struct Exam {
    ExamState state;
    bool timed;                  // the full exam, or one question with no timer
    int count;                   // how many questions (6, or 1)
    int questions[EXAM_SIZE];    // which questions from the bank
    CodeBox code[EXAM_SIZE];     // your code for each one
    ExamRun lastRun[EXAM_SIZE];  // the last time you clicked Run tests
    ExamRun marks[EXAM_SIZE];    // the final marking
    int current;
    float timeLeft;
    float timeUsed;
    int pendingRun;              // > 0: about to compile (draws "Compiling..." first)
    int markIndex;
    bool confirmFinish;
    int selected;                // results: the question shown
    float premadeScroll;
    float outputScroll;
    const char *compiler;        // "g++" or "clang++", or nullptr if there isn't one
    bool compilerChecked;
};

Exam exam;
char examProgram[40000];

// ============================================================
// Compiling and running
// ============================================================

const char *findCompiler() {
    if (system("g++ --version > exam_check.txt 2>&1") == 0) return "g++";
    if (system("clang++ --version > exam_check.txt 2>&1") == 0) return "clang++";
    return nullptr;
}

// Reads a whole text file (at most size - 1 characters)
void readFile(const char *name, char *out, int size) {
    out[0] = '\0';
    FILE *file = fopen(name, "r");
    if (file == nullptr) return;
    int length = (int)fread(out, 1, size - 1, file);
    out[length] = '\0';
    fclose(file);
}

void runExamCode(int slot, ExamRun *result) {
    ExamQuestion *q = getExamQuestion(exam.questions[slot]);
    const char *code = exam.code[slot].text;
    memset(result, 0, sizeof(ExamRun));
    result->ran = true;
    result->total = countTests(q);
    result->indexing = q->noIndexing && examUsesIndexing(code);

    if (exam.compiler == nullptr) {
        snprintf(result->output, sizeof(result->output), "No C++ compiler found, so the code can't be run.");
        return;
    }

    // Write the program, compile it, and run it
    buildExamProgram(q, code, examProgram, sizeof(examProgram));
    FILE *file = fopen("exam_test.cpp", "w");
    if (file == nullptr) return;
    fputs(examProgram, file);
    fclose(file);
    // Your code on its own too, so the compiler can show the line an error is on
    FILE *yours = fopen("your_code", "w");
    if (yours != nullptr) {
        fputs(code, yours);
        fputs("\n", yours);
        fclose(yours);
    }

#if defined(_WIN32)
    const char *program = "exam_test.exe";
    const char *runCommand = "exam_test.exe > exam_output.txt 2>&1";
#else
    const char *program = "exam_test";
    const char *runCommand = "./exam_test > exam_output.txt 2>&1";
#endif
    remove(program);
    char command[300];
    snprintf(command, sizeof(command), "%s -std=c++17 -O1 -w -pthread exam_test.cpp -o %s > exam_compile.txt 2>&1",
             exam.compiler, program);
    if (system(command) != 0) {
        // Keep the useful lines: the ones about your code, and the errors
        char errors[6000];
        readFile("exam_compile.txt", errors, sizeof(errors));
        int length = 0;
        int lines = 0;
        char *line = strtok(errors, "\n");
        while (line != nullptr && lines < 14) {
            bool useful = strstr(line, "your_code") != nullptr || strstr(line, "error") != nullptr || line[0] == ' ';
            if (useful) {
                length += snprintf(result->output + length, sizeof(result->output) - length, "%s\n", line);
                lines++;
                if (length >= (int)sizeof(result->output) - 1) break;
            }
            line = strtok(nullptr, "\n");
        }
        return;
    }
    result->compiled = true;
    system(runCommand);

    // Read what it printed: count the yes and no lines, and take out the markers
    char printed[2500];
    readFile("exam_output.txt", printed, sizeof(printed));
    result->finished = strstr(printed, "EXAM-DONE") != nullptr;
    result->timedOut = strstr(printed, "EXAM-TIMEOUT") != nullptr;
    int length = 0;
    char *line = strtok(printed, "\n");
    while (line != nullptr) {
        char *end = line + strlen(line) - 1;
        if (end >= line && *end == '\r') *end = '\0';   // Windows line endings
        if (strcmp(line, "yes") == 0) result->passed++;
        bool marker = strstr(line, "EXAM-DONE") != nullptr || strstr(line, "EXAM-TIMEOUT") != nullptr;
        if (!marker) length += snprintf(result->output + length, sizeof(result->output) - length, "%s\n", line);
        if (length >= (int)sizeof(result->output) - 1) break;
        line = strtok(nullptr, "\n");
    }
}

// Is the code still the starter stub? (spaces don't count)
bool sameIgnoringSpaces(const char *a, const char *b) {
    while (true) {
        while (*a == ' ' || *a == '\n' || *a == '\t') a++;
        while (*b == ' ' || *b == '\n' || *b == '\t') b++;
        if (*a != *b) return false;
        if (*a == '\0') return true;
        a++;
        b++;
    }
}

// One line saying how a run went
const char *runSummary(ExamRun *run, Color *colour) {
    *colour = COLOUR_BAD;
    if (!run->ran) {
        *colour = COLOUR_MUTED;
        return "Not run yet";
    }
    if (run->untouched) return "Not attempted - the code is still the lab's starter stub (0 marks)";
    if (!run->compiled) return "Didn't compile - the errors are below (your_code:LINE is the line in your code)";
    if (run->timedOut) return TextFormat("Took too long (over 10 seconds) - is there a loop that never ends? %d of %d tests passed", run->passed, run->total);
    if (!run->finished) return TextFormat("Crashed during the tests - maybe a nullptr or memory that doesn't exist. %d of %d passed", run->passed, run->total);
    if (run->passed == run->total) *colour = COLOUR_GOOD;
    else if (run->passed > 0) *colour = COLOUR_ACCENT;
    return TextFormat("%d of %d tests passed", run->passed, run->total);
}

// ============================================================
// Starting
// ============================================================

void setupExamSlot(int slot, int questionIndex) {
    exam.questions[slot] = questionIndex;
    CodeBox *box = &exam.code[slot];
    setupCodeBox(box, 120, "");
    box->size = 15;
    box->codeMode = true;
    const char *starter = getExamQuestion(questionIndex)->starter;
    snprintf(box->text, EDITOR_MAX_TEXT, "%s", starter);
    box->length = (int)strlen(box->text);
    box->cursor = 0;
    box->focused = true;
    memset(&exam.lastRun[slot], 0, sizeof(ExamRun));
}

// Picks a random question from one part of the labs (not one already chosen)
int pickQuestion(ExamPart part) {
    while (true) {
        int index = GetRandomValue(0, EXAM_QUESTION_COUNT - 1);
        if (getExamQuestion(index)->part != part) continue;
        bool used = false;
        for (int i = 0; i < exam.count; i++) {
            if (exam.questions[i] == index) used = true;
        }
        if (!used) return index;
    }
}

void startExam() {
    exam.timed = true;
    exam.count = 0;
    // 2 from Lab 1, 3 from Lab 2 Part 1, and 1 linked list
    ExamPart parts[EXAM_SIZE] = { PART_LAB1, PART_LAB1, PART_LAB2, PART_LAB2, PART_LAB2, PART_LISTS };
    for (int i = 0; i < EXAM_SIZE; i++) {
        int index = pickQuestion(parts[i]);
        setupExamSlot(i, index);
        exam.count++;
    }
    exam.current = 0;
    exam.timeLeft = EXAM_SECONDS;
    exam.timeUsed = 0;
    exam.pendingRun = 0;
    exam.confirmFinish = false;
    exam.premadeScroll = 0;
    exam.outputScroll = 0;
    exam.state = EXAM_RUNNING;
}

void startOneQuestion(int index) {
    exam.timed = false;
    exam.count = 1;
    setupExamSlot(0, index);
    exam.current = 0;
    exam.timeUsed = 0;
    exam.pendingRun = 0;
    exam.confirmFinish = false;
    exam.premadeScroll = 0;
    exam.outputScroll = 0;
    exam.state = EXAM_RUNNING;
}

// ============================================================
// Drawing pieces
// ============================================================

// Scrollable text in a box (the premade code, the output, the answers)
void drawScrollingCode(const char *text, Rectangle area, float *scroll, float size, bool colourYesNo) {
    DrawRectangleRec(area, COLOUR_CODE_BG);
    float contentHeight = countLines(text) * lineHeight(size) + 16;
    if (CheckCollisionPointRec(GetMousePosition(), area) && !buttonsAreBlocked()) *scroll -= GetMouseWheelMove() * 40;
    float maxScroll = contentHeight - area.height;
    if (maxScroll < 0) maxScroll = 0;
    if (*scroll > maxScroll) *scroll = maxScroll;
    if (*scroll < 0) *scroll = 0;

    beginClip(area.x, area.y, area.width, area.height);
    float y = area.y + 8 - *scroll;
    if (colourYesNo) {
        // Output: yes in green, no in red, the rest as it is
        const char *line = text;
        while (*line != '\0') {
            const char *end = strchr(line, '\n');
            int length = end ? (int)(end - line) : (int)strlen(line);
            char one[300];
            snprintf(one, sizeof(one), "%.*s", length, line);
            Color colour = COLOUR_TEXT;
            if (strcmp(one, "yes") == 0) colour = COLOUR_GOOD;
            if (strcmp(one, "no") == 0) colour = COLOUR_BAD;
            if (strstr(one, "error") != nullptr) colour = COLOUR_BAD;
            drawText(one, area.x + 10, y, size, colour);
            y += lineHeight(size);
            if (end == nullptr) break;
            line = end + 1;
        }
    } else {
        drawCode(text, area.x + 10, y, size, area.width - 20);
    }
    EndScissorMode();
    if (maxScroll > 0) {
        float barHeight = area.height * area.height / contentHeight;
        float barY = area.y + (area.height - barHeight) * (*scroll / maxScroll);
        DrawRectangle((int)(area.x + area.width - 6), (int)barY, 4, (int)barHeight, Color{ 90, 95, 110, 255 });
    }
}

const char *clockText(float seconds) {
    if (seconds < 0) seconds = 0;
    return TextFormat("%d:%02d", (int)seconds / 60, (int)seconds % 60);
}

// ============================================================
// The setup screen
// ============================================================

void drawExamSetup(App *app) {
    if (IsKeyPressed(KEY_ESCAPE)) {
        app->screen = SCREEN_MENU;
        return;
    }
    if (!exam.compilerChecked) {
        exam.compiler = findCompiler();
        exam.compilerChecked = true;
    }

    drawTitle("EXAM MODE - Coding Exam 2", 30, 14, 34, COLOUR_ACCENT);
    drawWrapped("Like the real exam: 6 questions in 1 hour, from Lab 1 and the Pointers & Linked List lab (Lab 2). "
                "You write each whole function, and a premade main tests it - your code is really compiled and run, "
                "and main prints yes or no for each test. Run the tests as often as you like. At the end every "
                "question is marked, and you can compare your code with a correct answer.",
                30, 62, SCREEN_WIDTH - 60.0f, 15, COLOUR_TEXT);

    if (exam.compiler != nullptr) {
        drawText(TextFormat("Compiler: %s found", exam.compiler), 30, 146, 15, COLOUR_GOOD);
    } else {
        drawWrapped("No C++ compiler found, so code can't be run. On Windows, start the game from the MSYS2 UCRT64 terminal. "
                    "On a Mac, install the command line tools: xcode-select --install", 30, 146, SCREEN_WIDTH - 60.0f, 15, COLOUR_BAD);
    }

    if (button(Rectangle{ 30, 180, 420, 56 }, "Start the exam (6 questions, 1 hour) >")) startExam();
    drawText("Each exam picks 2 Lab 1 questions, 3 from Lab 2 Part 1 and 1 linked list question.", 470, 199, 14, COLOUR_MUTED);

    // Or one question at a time, with no timer
    drawBold("Or practise one question (no timer):", 30, 258, 17, COLOUR_TEXT);
    const char *partNames[3] = { "Lab 1", "Lab 2 Part 1 (no arr[i])", "Lab 2 Part 2: linked lists" };
    float columnX[3] = { 30, 450, 870 };
    int row[3] = { 0, 0, 0 };
    for (int p = 0; p < 3; p++) drawText(partNames[p], columnX[p], 290, 15, COLOUR_ACCENT);
    for (int i = 0; i < EXAM_QUESTION_COUNT; i++) {
        ExamQuestion *q = getExamQuestion(i);
        int p = (int)q->part;
        Rectangle box = { columnX[p], 314 + row[p] * 29.0f, 390, 26 };
        const char *label = TextFormat("%s: %s", q->source, q->title);
        if (q->part == PART_LISTS) label = q->title;   // the column already says where these are from
        if (button(box, label)) startOneQuestion(i);
        row[p]++;
    }

    if (button(Rectangle{ SCREEN_WIDTH - 170.0f, SCREEN_HEIGHT - 60.0f, 150, 42 }, "Back")) app->screen = SCREEN_MENU;
}

// ============================================================
// Answering
// ============================================================

void finishExam() {
    exam.state = EXAM_MARKING;
    exam.markIndex = 0;
    exam.pendingRun = 0;
}

void drawExamRunning(float dt) {
    // The timer
    exam.timeUsed += dt;
    if (exam.timed) {
        exam.timeLeft -= dt;
        if (exam.timeLeft <= 0) {
            finishExam();
            return;
        }
    }
    if (!exam.timed && IsKeyPressed(KEY_ESCAPE)) {
        exam.state = EXAM_SETUP;
        return;
    }

    // Compile and run now (the "Compiling..." message was drawn last frame)
    if (exam.pendingRun == 1) {
        runExamCode(exam.current, &exam.lastRun[exam.current]);
        exam.pendingRun = 0;
        exam.outputScroll = 0;
        ExamRun *run = &exam.lastRun[exam.current];
        if (run->compiled && run->finished && run->passed == run->total) playSound(SOUND_CORRECT, 0.5f);
        else playSound(SOUND_WRONG, 0.4f);
    }

    int slot = exam.current;
    ExamQuestion *q = getExamQuestion(exam.questions[slot]);
    CodeBox *box = &exam.code[slot];
    bool waiting = exam.pendingRun > 0;

    // Typing, and Ctrl/Cmd + Enter runs the tests
    if (!waiting) {
        box->focused = true;
        updateCodeBox(box);
        bool control = IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL) ||
                       IsKeyDown(KEY_LEFT_SUPER) || IsKeyDown(KEY_RIGHT_SUPER);
        if (control && IsKeyPressed(KEY_ENTER)) exam.pendingRun = 2;
    }

    // ---------- Top bar ----------
    drawBar(0, 0, SCREEN_WIDTH, 56);
    if (exam.timed) {
        drawTitle("EXAM", 20, 12, 28, COLOUR_ACCENT);
        for (int i = 0; i < exam.count; i++) {
            Rectangle tab = { 110 + i * 84.0f, 10, 76, 36 };
            ExamRun *run = &exam.lastRun[i];
            Color colour = COLOUR_BUTTON;
            if (run->ran && run->compiled && run->finished && run->passed == run->total) colour = Color{ 40, 100, 55, 255 };
            else if (run->ran && run->passed > 0) colour = Color{ 110, 90, 30, 255 };
            else if (run->ran) colour = Color{ 110, 45, 45, 255 };
            DrawRectangleRec(tab, colour);
            if (i == exam.current) DrawRectangleLinesEx(tab, 2, COLOUR_ACCENT);
            const char *label = TextFormat("Q%d", i + 1);
            drawBold(label, tab.x + (tab.width - textWidth(label, 17)) / 2, tab.y + 9, 17, COLOUR_TEXT);
            if (!waiting && !buttonsAreBlocked() && CheckCollisionPointRec(GetMousePosition(), tab) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                exam.current = i;
                exam.premadeScroll = 0;
                exam.outputScroll = 0;
            }
        }
        Color timeColour = exam.timeLeft < 300 ? COLOUR_BAD : COLOUR_TEXT;
        drawText("Time left", 640, 8, 13, COLOUR_MUTED);
        drawBold(clockText(exam.timeLeft), 640, 24, 24, timeColour);
        const char *finishLabel = exam.confirmFinish ? "Click again to finish" : "Finish exam";
        if (!waiting && button(Rectangle{ SCREEN_WIDTH - 250.0f, 10, 230, 36 }, finishLabel)) {
            if (exam.confirmFinish) {
                finishExam();
                return;
            }
            exam.confirmFinish = true;
        }
    } else {
        drawTitle("PRACTISE ONE QUESTION", 20, 14, 24, COLOUR_ACCENT);
        drawText(TextFormat("No timer.  %s", clockText(exam.timeUsed)), 360, 21, 15, COLOUR_MUTED);
        if (!waiting && button(Rectangle{ SCREEN_WIDTH - 170.0f, 10, 150, 36 }, "Back")) {
            exam.state = EXAM_SETUP;
            return;
        }
    }

    // ---------- Left: the question and the premade code ----------
    float x = 20;
    float width = 560;
    float y = 68;
    drawText(q->source, x, y, 14, COLOUR_MUTED);
    drawBold(q->title, x, y + 18, 22, COLOUR_TEXT);
    y += 50;
    y += drawWrapped(q->description, x, y, width, 14, COLOUR_TEXT) + 10;
    if (q->noIndexing) {
        y += drawWrapped("Lab 2 rule: no array indexing (arr[i]) - use only pointer arithmetic.", x, y, width, 13, COLOUR_ACCENT) + 8;
    }
    drawText("Premade code (it's already there - you write the function):", x, y, 13, COLOUR_MUTED);
    y += 18;
    char premade[6000];
    premadeCodeText(q, premade, sizeof(premade));
    drawScrollingCode(premade, Rectangle{ x, y, width, SCREEN_HEIGHT - 12 - y }, &exam.premadeScroll, 13, false);

    // ---------- Right: your code, Run tests, and the output ----------
    float rx = 600;
    float rwidth = SCREEN_WIDTH - rx - 20;
    drawText("Your code:", rx, 66, 14, COLOUR_MUTED);
    box->area = Rectangle{ rx, 86, rwidth, 360 };
    drawCodeBox(box, -1);

    float buttonY = 456;
    if (!waiting && button(Rectangle{ rx, buttonY, 150, 38 }, "Reset code")) {
        snprintf(box->text, EDITOR_MAX_TEXT, "%s", q->starter);
        box->length = (int)strlen(box->text);
        box->cursor = 0;
        box->firstLine = 0;
    }
    drawText("Tab indents. Ctrl/Cmd + V pastes.", rx + 166, buttonY + 12, 13, COLOUR_MUTED);
    const char *shortcut = "or Ctrl/Cmd + Enter";
    drawText(shortcut, rx + rwidth - 85 - textWidth(shortcut, 11) / 2, buttonY + 41, 11, COLOUR_MUTED);
    if (!waiting && button(Rectangle{ rx + rwidth - 170, buttonY, 170, 38 }, "Run tests >")) exam.pendingRun = 2;

    // The output of the last run
    ExamRun *run = &exam.lastRun[slot];
    float oy = 512;
    Color summaryColour;
    const char *summary = runSummary(run, &summaryColour);
    drawBold("Output", rx, oy, 15, COLOUR_TEXT);
    oy += 2;
    float summaryHeight = drawWrapped(summary, rx + 70, oy, rwidth - 70, 14, summaryColour);
    oy += summaryHeight + 4;
    if (run->indexing) {
        oy += drawWrapped("Lab 2 rule: this uses arr[i]. Use pointer arithmetic instead, like *(arr + i).", rx, oy, rwidth, 13, COLOUR_ACCENT) + 4;
    }
    drawScrollingCode(run->output, Rectangle{ rx, oy, rwidth, SCREEN_HEIGHT - 12 - oy }, &exam.outputScroll, 13, true);

    // "Compiling..." while the run is about to happen
    if (waiting) {
        DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Fade(BLACK, 0.45f));
        const char *message = "Compiling and running your code...";
        drawPopupBox(Rectangle{ SCREEN_WIDTH / 2 - 260.0f, SCREEN_HEIGHT / 2 - 40.0f, 520, 80 });
        drawBold(message, SCREEN_WIDTH / 2.0f - textWidth(message, 20) / 2, SCREEN_HEIGHT / 2.0f - 12, 20, COLOUR_ACCENT);
        exam.pendingRun = 1;
    }
}

// ============================================================
// Marking, then the results
// ============================================================

void drawExamMarking() {
    if (exam.markIndex > 0) {
        // Mark one question per frame, so the screen can say how far it's got
        int slot = exam.markIndex - 1;
        runExamCode(slot, &exam.marks[slot]);
        // A stub can pass a test by luck (like return true;), so untouched code gets 0
        if (sameIgnoringSpaces(exam.code[slot].text, getExamQuestion(exam.questions[slot])->starter)) {
            exam.marks[slot].untouched = true;
            exam.marks[slot].passed = 0;
        }
    }
    if (exam.markIndex >= exam.count) {
        exam.state = EXAM_RESULTS;
        exam.selected = 0;
        exam.premadeScroll = 0;
        exam.outputScroll = 0;
        playSound(SOUND_UNLOCK, 0.6f);
        spawnConfetti(SCREEN_WIDTH / 2.0f, 200, 60);
        return;
    }
    const char *message = TextFormat("Marking question %d of %d...", exam.markIndex + 1, exam.count);
    drawBold(message, SCREEN_WIDTH / 2.0f - textWidth(message, 26) / 2, SCREEN_HEIGHT / 2.0f - 20, 26, COLOUR_ACCENT);
    exam.markIndex++;
}

// How much of a question is right, from 0 to 1
float questionMark(ExamRun *run) {
    if (!run->compiled || run->total == 0) return 0;
    return (float)run->passed / run->total;
}

void drawExamResults(App *app) {
    if (IsKeyPressed(KEY_ESCAPE)) {
        exam.state = EXAM_SETUP;
        return;
    }
    float total = 0;
    for (int i = 0; i < exam.count; i++) total += questionMark(&exam.marks[i]);
    int percent = (int)(total / exam.count * 100 + 0.5f);

    drawTitle(exam.timed ? "EXAM RESULTS" : "RESULT", 30, 12, 32, COLOUR_ACCENT);
    Color colour = percent >= 70 ? COLOUR_GOOD : (percent >= 40 ? COLOUR_ACCENT : COLOUR_BAD);
    drawBold(TextFormat("%d%%", percent), 290, 6, 44, colour);
    drawText(TextFormat("Time used: %s.  Click a question to compare your code with a correct answer.", clockText(exam.timeUsed)),
             30, 62, 15, COLOUR_TEXT);

    // The questions on the left
    for (int i = 0; i < exam.count; i++) {
        ExamRun *run = &exam.marks[i];
        ExamQuestion *q = getExamQuestion(exam.questions[i]);
        Rectangle row = { 30, 94 + i * 64.0f, 380, 58 };
        bool hover = CheckCollisionPointRec(GetMousePosition(), row);
        Color background = COLOUR_PANEL;
        if (i == exam.selected) background = Color{ 70, 62, 20, 255 };
        else if (hover) background = COLOUR_BUTTON_HOVER;
        DrawRectangleRec(row, background);
        drawText(TextFormat("Q%d  %s", i + 1, q->source), row.x + 10, row.y + 6, 13, COLOUR_MUTED);
        drawBold(q->title, row.x + 10, row.y + 22, 15, COLOUR_TEXT);
        Color markColour;
        runSummary(run, &markColour);
        const char *mark = TextFormat("%d/%d", run->passed, run->total);
        if (!run->compiled) mark = "didn't compile";
        if (run->untouched) mark = "not attempted";
        drawText(mark, row.x + row.width - 10 - textWidth(mark, 14), row.y + 40, 14, markColour);
        if (run->indexing) drawText("used arr[i]", row.x + 10, row.y + 40, 12, COLOUR_ACCENT);
        if (hover && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            exam.selected = i;
            exam.premadeScroll = 0;
            exam.outputScroll = 0;
        }
    }

    // The chosen question: your code next to a correct answer
    int s = exam.selected;
    ExamQuestion *q = getExamQuestion(exam.questions[s]);
    ExamRun *run = &exam.marks[s];
    float x = 430;
    float columnWidth = (SCREEN_WIDTH - x - 40) / 2;
    Color summaryColour;
    const char *summary = runSummary(run, &summaryColour);
    drawBold(q->title, x, 94, 18, COLOUR_TEXT);
    drawWrapped(summary, x, 118, SCREEN_WIDTH - x - 30, 14, summaryColour);
    drawText("Your code:", x, 146, 14, COLOUR_MUTED);
    drawScrollingCode(exam.code[s].text, Rectangle{ x, 166, columnWidth, 300 }, &exam.premadeScroll, 13, false);
    drawText("One correct answer:", x + columnWidth + 10, 146, 14, COLOUR_GOOD);
    drawScrollingCode(q->answer, Rectangle{ x + columnWidth + 10, 166, columnWidth, 300 }, &exam.outputScroll, 13, false);
    drawText("What your code printed:", x, 476, 14, COLOUR_MUTED);
    static float printedScroll = 0;
    drawScrollingCode(run->output, Rectangle{ x, 496, SCREEN_WIDTH - x - 30, 136 }, &printedScroll, 13, true);

    if (button(Rectangle{ 30, SCREEN_HEIGHT - 66.0f, 220, 46 }, exam.timed ? "Another exam" : "Try it again")) {
        if (exam.timed) startExam();
        else startOneQuestion(exam.questions[0]);
    }
    if (button(Rectangle{ 262, SCREEN_HEIGHT - 66.0f, 150, 46 }, "Exam menu")) exam.state = EXAM_SETUP;
    if (button(Rectangle{ SCREEN_WIDTH - 170.0f, SCREEN_HEIGHT - 66.0f, 150, 46 }, "Main menu")) {
        exam.state = EXAM_SETUP;
        app->screen = SCREEN_MENU;
    }
}

void examScreen(App *app) {
    drawBackground();
    if (exam.state == EXAM_SETUP) drawExamSetup(app);
    else if (exam.state == EXAM_RUNNING) drawExamRunning(GetFrameTime());
    else if (exam.state == EXAM_MARKING) drawExamMarking();
    else drawExamResults(app);
}
