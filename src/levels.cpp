#include "questions.h"

// ============================================================
// All the lessons, following the lecture notes and Labs 1 and 2.
//
// Each level has: a title, where it comes from in the notes, a short
// explanation, a tiny example, how the game uses it, and two versions
// of a task. Each task lists accepted answers, common mistakes with
// their own hints, and a general hint.
//
// Spaces don't matter in answers. A # in an answer means "any number".
// ============================================================

void setupLevels() {
    Level *level;
    Task *task;

    // =========================================================
    // Chapter 1: Pointer basics
    // =========================================================

    // ---------- Level 1: & ----------
    level = getLevel(1);
    level->chapter = 1;
    level->title = "Addresses with &";
    level->shortName = "&";
    level->source = "Pointers lecture: Addressing Concept, Twin Operators";
    level->explanation = "Every variable lives somewhere in memory, at an address. A pointer is a variable that stores an address. "
                         "Put & in front of a variable to get its address, and put a * in the pointer's type: "
                         "int *ptr is a pointer to an int.";
    level->example = "int x = 10;\n"
                     "int *ptr = &x;   // ptr holds the address of x";
    level->inGame = "Your shot power is stored in a variable called power. From now on the power bar shows "
                    "int *powerPtr = &power; because the game reads the power through that pointer.";

    task = &level->tasks[0];
    task->task = "Make powerPtr point at power. Type the line that declares powerPtr and stores the address of power in it.";
    task->code = "int power = 50;\n"
                 "____";
    task->answers[0] = "int *powerPtr = &power;";
    task->mistakes[0] = { "int *powerPtr = power;", "That stores the value 50, not where power lives. Put & in front of power to get its address." };
    task->mistakes[1] = { "int powerPtr = &power;", "powerPtr needs a * in its type to be a pointer: int *powerPtr" };
    task->mistakes[2] = { "int *powerPtr = *power;", "* is for reading through a pointer. To get an address, use &power." };
    task->hint = "The pattern is: type *name = &variable;";

    task = &level->tasks[1];
    task->task = "Make scorePtr point at score. Type the line that declares scorePtr and stores the address of score in it.";
    task->code = "int score = 0;\n"
                 "____";
    task->answers[0] = "int *scorePtr = &score;";
    task->mistakes[0] = { "int *scorePtr = score;", "That stores the value 0, not the address. Put & in front of score." };
    task->mistakes[1] = { "int scorePtr = &score;", "scorePtr needs a * in its type to be a pointer: int *scorePtr" };
    task->mistakes[2] = { "int *scorePtr = *score;", "* is for reading through a pointer. To get an address, use &score." };
    task->hint = "Copy the example: int *ptr = &x; but with scorePtr and score.";

    // ---------- Level 2: * ----------
    level = getLevel(2);
    level->chapter = 1;
    level->title = "Changing a value with *";
    level->shortName = "*";
    level->source = "Pointers lecture: Twin Operators, An Illustration";
    level->explanation = "Put * in front of a pointer to reach the variable it points at. This is called dereferencing. "
                         "*ptr = 99; changes the variable that ptr points at. The pointer still holds the same address.";
    level->example = "int x = 10;\n"
                     "int *ptr = &x;\n"
                     "*ptr = 99;       // x is now 99";
    level->inGame = "Your code sets the power for this shot. Aim with the mouse, then click once to shoot at that power.";

    task = &level->tasks[0];
    task->task = "Set this shot's power to 80 through powerPtr (don't write to power directly).";
    task->code = "int power = 50;\n"
                 "int *powerPtr = &power;\n"
                 "____";
    task->answers[0] = "*powerPtr = 80;";
    task->mistakes[0] = { "powerPtr = 80;", "Without the * you change the address stored in powerPtr, not the power. Use *powerPtr." };
    task->mistakes[1] = { "power = 80;", "That works in C, but this task is to change it through the pointer: *powerPtr = 80;" };
    task->mistakes[2] = { "&powerPtr = 80;", "& gives an address. To change the value powerPtr points at, use *." };
    task->mistakes[3] = { "int *powerPtr = 80;", "powerPtr already exists. Don't declare it again, just use *powerPtr." };
    task->hint = "Put * in front of the pointer, then = and the new value.";
    task->power = 80;

    task = &level->tasks[1];
    task->task = "Set this shot's power to 60 through powerPtr (don't write to power directly).";
    task->code = "int power = 50;\n"
                 "int *powerPtr = &power;\n"
                 "____";
    task->answers[0] = "*powerPtr = 60;";
    task->mistakes[0] = { "powerPtr = 60;", "Without the * you change the address stored in powerPtr, not the power. Use *powerPtr." };
    task->mistakes[1] = { "power = 60;", "That works in C, but this task is to change it through the pointer: *powerPtr = 60;" };
    task->mistakes[2] = { "&powerPtr = 60;", "& gives an address. To change the value powerPtr points at, use *." };
    task->mistakes[3] = { "int *powerPtr = 60;", "powerPtr already exists. Don't declare it again, just use *powerPtr." };
    task->hint = "Put * in front of the pointer, then = and the new value.";
    task->power = 60;

    // ---------- Level 3: call your ball ----------
    level = getLevel(3);
    level->chapter = 1;
    level->title = "Call your ball";
    level->shortName = "call ball";
    level->source = "Pointers lecture: Pointer Arithmetic and Array (ptr = &(a[2]))";
    level->explanation = "All 16 balls are stored in one array: Ball balls[16]. balls[14] is ball 14, and &balls[14] is its "
                         "address (look at the row of boxes under the table). A pointer to a Ball has the type Ball *. "
                         "Pointing target at a ball is how you call your shot: pot the ball you called and you shoot again.";
    level->example = "float a[4];\n"
                     "float *ptr = &(a[2]);       // from the lecture\n"
                     "Ball *target = &balls[3];   // call ball 3";
    level->inGame = "From now on you call a ball before every shot (except the break). Your call line stays in its box, "
                    "so next time you only change the number.";

    task = &level->tasks[0];
    task->task = "Call the ball you want to pot by pointing target at it. For ball 3 you'd type Ball *target = &balls[3]; "
                 "On an open table you can call any ball except the 8.";
    task->code = "Ball balls[16];   // balls[0] is the cue ball\n"
                 "____";
    task->answers[0] = "Ball *target = &balls[#];";
    task->answers[1] = "Ball *target = &(balls[#]);";
    task->mistakes[0] = { "Ball *target = balls[#];", "balls[3] is the ball itself. Put & in front to get its address: &balls[3]" };
    task->mistakes[1] = { "Ball target = &balls[#];", "target must be a pointer: Ball *target" };
    task->mistakes[2] = { "target = &balls[#];", "Start with the type, so target gets declared: Ball *target = ..." };
    task->hint = "Type Ball *target = &balls[N]; with the number of the ball you want.";

    task = &level->tasks[1];
    task->task = "Call the next ball you want to pot. Hover over a ball to see its number and address.";
    task->code = "Ball balls[16];   // balls[0] is the cue ball\n"
                 "____";
    task->answers[0] = "Ball *target = &balls[#];";
    task->answers[1] = "Ball *target = &(balls[#]);";
    task->mistakes[0] = { "Ball *target = balls[#];", "balls[3] is the ball itself. Put & in front to get its address: &balls[3]" };
    task->mistakes[1] = { "Ball target = &balls[#];", "target must be a pointer: Ball *target" };
    task->mistakes[2] = { "target = &balls[#];", "Start with the type, so target gets declared: Ball *target = ..." };
    task->hint = "Type Ball *target = &balls[N]; with the number of the ball you want.";

    // The call line, used before every shot after Level 3
    task = getCallTask(false);
    task->answers[0] = "Ball *target = &balls[#];";
    task->answers[1] = "Ball *target = &(balls[#]);";
    task->mistakes[0] = { "Ball *target = balls[#];", "balls[3] is the ball itself. Put & in front to get its address: &balls[3]" };
    task->mistakes[1] = { "Ball target = &balls[#];", "target must be a pointer: Ball *target" };
    task->mistakes[2] = { "target = &balls[#];", "Start with the type, so target gets declared: Ball *target = ..." };
    task->hint = "Call your ball: Ball *target = &balls[N]; with the number of the ball you want.";

    task = getCallTask(true);
    task->answers[0] = "Ball *target = balls + #;";
    task->answers[1] = "Ball *target = (balls + #);";
    task->answers[2] = "Ball *target = # + balls;";
    task->mistakes[0] = { "Ball *target = *(balls + #);", "*(balls + 3) is the ball itself. Leave out the * to get its address: balls + 3" };
    task->mistakes[1] = { "Ball target = balls + #;", "target must be a pointer: Ball *target" };
    task->mistakes[2] = { "Ball *target = &(balls + #);", "balls + 3 is already an address, so you don't need the &." };
    task->mistakes[3] = { "target = balls + #;", "Start with the type, so target gets declared: Ball *target = ..." };
    task->hint = "Call your ball with pointer arithmetic: Ball *target = balls + N;";

    // ---------- Level 4: pass by pointer ----------
    level = getLevel(4);
    level->chapter = 1;
    level->title = "Pass by pointer";
    level->shortName = "pass ptr";
    level->source = "Pointers & Linked List lecture: Pass-by-Value vs Pass-by-Reference";
    level->explanation = "When you pass a normal int to a function, the function gets a copy, so the original doesn't change. "
                         "Pass the variable's address instead (&x), and the function can change the original through the pointer. "
                         "Watch the brackets: (*numPtr)++ adds 1 to the value.";
    level->example = "void increment(int *numPtr) {\n"
                     "    (*numPtr)++;\n"
                     "}\n"
                     "increment(&x);   // x goes up by 1";
    level->inGame = "Every time you pot one of your balls, the game calls addPoint(&player.score).";

    task = &level->tasks[0];
    task->task = "Finish addPoint so it adds 1 to the score that score points at.";
    task->code = "void addPoint(int *score) {\n"
                 "    ____\n"
                 "}";
    task->answers[0] = "(*score)++;";
    task->answers[1] = "*score = *score + 1;";
    task->answers[2] = "*score += 1;";
    task->answers[3] = "++(*score);";
    task->answers[4] = "++*score;";
    task->mistakes[0] = { "*score++;", "*score++ moves the pointer forward instead of changing the score (the ++ happens to score first). Wrap it in brackets: (*score)++;" };
    task->mistakes[1] = { "score++;", "score++ moves the pointer to the next address. Use (*score)++ to change the value it points at." };
    task->mistakes[2] = { "score = score + 1;", "That changes the address in score. Use * to change the value: *score = *score + 1;" };
    task->hint = "You want to add 1 to the value that score points at. Remember the brackets.";

    task = &level->tasks[1];
    task->task = "Finish addPoints so it adds points onto the score that score points at.";
    task->code = "void addPoints(int *score, int points) {\n"
                 "    ____\n"
                 "}";
    task->answers[0] = "*score += points;";
    task->answers[1] = "*score = *score + points;";
    task->answers[2] = "(*score) += points;";
    task->mistakes[0] = { "score += points;", "That moves the pointer. Add to the value it points at: *score += points;" };
    task->mistakes[1] = { "*score = points;", "That replaces the score instead of adding to it. Use +=." };
    task->mistakes[2] = { "score = score + points;", "That changes the address. Use *score on both sides." };
    task->hint = "Use *score to reach the value, then add points onto it.";

    // ---------- Level 5: nullptr ----------
    level = getLevel(5);
    level->chapter = 1;
    level->title = "Pointing at nothing: nullptr";
    level->shortName = "nullptr";
    level->source = "Pointers lecture: Pointer to nullptr";
    level->explanation = "A pointer that doesn't point at anything yet should be set to nullptr. "
                         "Using a pointer that is nullptr crashes your program, so check it first with if (ptr != nullptr).";
    level->example = "int *ptr = nullptr;     // points at nothing yet\n"
                     "if (ptr != nullptr) {\n"
                     "    cout << *ptr;\n"
                     "}";
    level->inGame = "lastPotted points at the last ball potted. At the start of a game it's nullptr, "
                    "so the game checks it before showing it under the table.";

    task = &level->tasks[0];
    task->task = "Type the if line that checks lastPotted points at something before the game shows it.";
    task->code = "Ball *lastPotted = nullptr;\n"
                 "// ... later in the game ...\n"
                 "____\n"
                 "    showLastPotted(lastPotted);\n"
                 "}";
    task->answers[0] = "if (lastPotted != nullptr) {";
    task->answers[1] = "if (lastPotted != NULL) {";
    task->mistakes[0] = { "if (lastPotted == nullptr) {", "== checks that it IS empty. Show the ball only when it's NOT empty: !=" };
    task->mistakes[1] = { "if (*lastPotted != nullptr) {", "*lastPotted is the ball itself. Compare the pointer: lastPotted != nullptr" };
    task->mistakes[2] = { "if (lastPotted = nullptr) {", "A single = sets the pointer to nullptr! To compare, use !=" };
    task->hint = "Compare the pointer with nullptr using !=, inside if ( ) {";

    task = &level->tasks[1];
    task->task = "Ball 14 just went down a pocket. Type the line that sets target back to nullptr.";
    task->code = "Ball *target = &balls[14];\n"
                 "// ... ball 14 goes down a pocket ...\n"
                 "____";
    task->answers[0] = "target = nullptr;";
    task->answers[1] = "target = NULL;";
    task->mistakes[0] = { "*target = nullptr;", "*target is the ball itself. To empty the pointer, set target, not *target." };
    task->mistakes[1] = { "Ball *target = nullptr;", "target already exists. Don't declare it again, just assign: target = nullptr;" };
    task->mistakes[2] = { "target == nullptr;", "== compares. To set it, use a single =." };
    task->hint = "Assign nullptr to the pointer itself (no *).";

    // =========================================================
    // Chapter 2: Swaps
    // =========================================================

    // ---------- Level 6: swap_ints ----------
    level = getLevel(6);
    level->chapter = 2;
    level->title = "swap_ints";
    level->shortName = "swap_ints";
    level->source = "Lab 2 Q1; Pointers lecture: swap with pass by pointer";
    level->explanation = "To swap two values you need a third variable to hold one of them for a moment, or it gets overwritten. "
                         "With pointers, a function can swap the caller's own variables.";
    level->example = "void swap(int *x, int *y) {   // from the lecture\n"
                     "    int z = *x;\n"
                     "    *x = *y;\n"
                     "    *y = z;\n"
                     "}";
    level->inGame = "When your turn ends, the game calls swap_ints(&currentPlayer, &waitingPlayer).";

    task = &level->tasks[0];
    task->task = "Type the 3 lines inside swap_ints that swap the values a and b point at, using a variable called temp.";
    task->code = "void swap_ints(int *a, int *b) {\n"
                 "    ____\n"
                 "}";
    task->answers[0] = "int temp = *a; *a = *b; *b = temp;";
    task->answers[1] = "int temp = *b; *b = *a; *a = temp;";
    task->mistakes[0] = { "int temp = *a; *a = *b; *b = *a;", "*a was already overwritten on the line before, so *b = *a copies the wrong value. That's what temp is for: *b = temp;" };
    task->mistakes[1] = { "int temp = a; a = b; b = temp;", "That swaps the addresses inside the function only. Use * to swap the values: *a and *b." };
    task->mistakes[2] = { "*a = *b; *b = *a;", "Without temp, a's value is lost on the first line. Save it first: int temp = *a;" };
    task->hint = "Save *a in temp, copy *b into *a, then put temp into *b.";

    task = &level->tasks[1];
    task->task = "One line is missing from swap_ints. Type it.";
    task->code = "void swap_ints(int *a, int *b) {\n"
                 "    int temp = *a;\n"
                 "    ____\n"
                 "    *b = temp;\n"
                 "}";
    task->answers[0] = "*a = *b;";
    task->mistakes[0] = { "a = b;", "That copies the address. Use *a = *b; to copy the value." };
    task->mistakes[1] = { "*b = *a;", "Other way round: a gets b's value first: *a = *b;" };
    task->mistakes[2] = { "temp = *b;", "temp already holds a's value. Now copy b's value into a." };
    task->hint = "a needs b's value. Use * on both.";

    // ---------- Level 7: swap_chars ----------
    level = getLevel(7);
    level->chapter = 2;
    level->title = "swap_chars";
    level->shortName = "swap_chars";
    level->source = "Lab 2 Q2";
    level->explanation = "Pointers work the same way for every type. A char * points at a char, "
                         "so swap_chars is swap_ints with char instead of int.";
    level->example = "char a = 'A', b = 'B';\n"
                     "swap_chars(&a, &b);   // now a = 'B', b = 'A'";
    level->inGame = "Groups are stored as chars: 'S' for solids and 'T' for stripes. When the first ball goes down, "
                    "the game calls swap_chars if the shooter needs the other group.";

    task = &level->tasks[0];
    task->task = "Type the 3 lines inside swap_chars, using a char called temp.";
    task->code = "void swap_chars(char *a, char *b) {\n"
                 "    ____\n"
                 "}";
    task->answers[0] = "char temp = *a; *a = *b; *b = temp;";
    task->answers[1] = "char temp = *b; *b = *a; *a = temp;";
    task->mistakes[0] = { "int temp = *a; *a = *b; *b = temp;", "So close! temp should be a char, the same type as the values it holds." };
    task->mistakes[1] = { "char temp = a; a = b; b = temp;", "That swaps the addresses. Use *a and *b to swap the chars." };
    task->mistakes[2] = { "char temp = *a; *a = *b; *b = *a;", "*a was already overwritten. Use temp on the last line." };
    task->hint = "Same as swap_ints, but temp is a char.";

    task = &level->tasks[1];
    task->task = "Type the call that swaps the two players' groups.";
    task->code = "char p1Group = 'S';\n"
                 "char p2Group = 'T';\n"
                 "____";
    task->answers[0] = "swap_chars(&p1Group, &p2Group);";
    task->answers[1] = "swap_chars(&p2Group, &p1Group);";
    task->mistakes[0] = { "swap_chars(p1Group, p2Group);", "swap_chars needs the addresses so it can change them: &p1Group and &p2Group." };
    task->mistakes[1] = { "swap_chars(*p1Group, *p2Group);", "p1Group isn't a pointer, so * doesn't work here. Use & to get its address." };
    task->hint = "Pass the address of each variable with &.";

    // =========================================================
    // Chapter 3: Arrays (Lab 1, Q13 to Q18)
    // =========================================================

    // ---------- Level 8: frequencyCount ----------
    level = getLevel(8);
    level->chapter = 3;
    level->title = "Counting in an array";
    level->shortName = "freqCount";
    level->source = "Lab 1 Q13";
    level->explanation = "Loop over an array with an index i from 0 up to size - 1, and use arr[i] to read element i. "
                         "To count how many elements equal a value, add 1 to count each time one matches.";
    level->example = "int count = 0;\n"
                     "for (int i = 0; i < size; i++) {\n"
                     "    if (arr[i] == 5) count++;\n"
                     "}";
    level->inGame = "The game counts how many of your group are left with frequencyCount (each ball on the table "
                    "is stored as 1 for solid or 2 for stripe). Your box at the top now shows how many you have left.";

    task = &level->tasks[0];
    task->task = "Type the line inside the loop that adds 1 to count when arr[i] equals value.";
    task->code = "int frequencyCount(int size, int arr[], int value) {\n"
                 "    int count = 0;\n"
                 "    for (int i = 0; i < size; i++) {\n"
                 "        ____\n"
                 "    }\n"
                 "    return count;\n"
                 "}";
    task->answers[0] = "if (arr[i] == value) count++;";
    task->answers[1] = "if (arr[i] == value) { count++; }";
    task->answers[2] = "if (arr[i] == value) count = count + 1;";
    task->answers[3] = "if (arr[i] == value) count += 1;";
    task->answers[4] = "if (value == arr[i]) count++;";
    task->mistakes[0] = { "if (arr[i] = value) count++;", "A single = sets arr[i]. To compare, use ==." };
    task->mistakes[1] = { "if (arr == value) count++;", "Compare one element, arr[i], not the whole array." };
    task->mistakes[2] = { "if (arr[i] == value) count--;", "count-- takes 1 away. You want count++." };
    task->hint = "If arr[i] is equal to value, add 1 to count.";

    task = &level->tasks[1];
    task->task = "Type the for line that loops i from 0 up to (but not including) size.";
    task->code = "int count = 0;\n"
                 "____\n"
                 "    if (arr[i] == value) count++;\n"
                 "}";
    task->answers[0] = "for (int i = 0; i < size; i++) {";
    task->answers[1] = "for (int i = 0; i < size; ++i) {";
    task->mistakes[0] = { "for (int i = 0; i <= size; i++) {", "i <= size goes one past the end (the last element is size - 1). Use i < size." };
    task->mistakes[1] = { "for (int i = 1; i < size; i++) {", "Arrays start at 0, so start i at 0." };
    task->mistakes[2] = { "for (i = 0; i < size; i++) {", "Declare i in the loop: for (int i = 0; ..." };
    task->hint = "Start i at 0, keep going while i < size, add 1 to i each time, and end the line with {";

    // ---------- Level 9: countDuplicates ----------
    level = getLevel(9);
    level->chapter = 3;
    level->title = "Comparing pairs";
    level->shortName = "dupes";
    level->source = "Lab 1 Q14";
    level->explanation = "To compare every pair of elements, use two loops. The inner loop starts at j = i + 1, "
                         "so each pair is checked once and an element is never compared with itself.";
    level->example = "for (int i = 0; i < size; i++) {\n"
                     "    for (int j = i + 1; j < size; j++) {\n"
                     "        // compare arr[i] with arr[j]\n"
                     "    }\n"
                     "}";
    level->inGame = "Before every game, countDuplicates checks the rack so no ball number appears twice. "
                    "The result is the first line of the shot log.";

    task = &level->tasks[0];
    task->task = "Type the inner for line so that j starts just after i.";
    task->code = "int countDuplicates(int size, int arr[]) {\n"
                 "    int count = 0;\n"
                 "    for (int i = 0; i < size; i++) {\n"
                 "        ____\n"
                 "            if (arr[i] == arr[j]) count++;\n"
                 "        }\n"
                 "    }\n"
                 "    return count;\n"
                 "}";
    task->answers[0] = "for (int j = i + 1; j < size; j++) {";
    task->answers[1] = "for (int j = i + 1; j < size; ++j) {";
    task->answers[2] = "for (int j = 1 + i; j < size; j++) {";
    task->mistakes[0] = { "for (int j = 0; j < size; j++) {", "Starting j at 0 compares each element with itself, and checks every pair twice. Start at i + 1." };
    task->mistakes[1] = { "for (int j = i; j < size; j++) {", "j = i compares arr[i] with itself. Start at i + 1." };
    task->mistakes[2] = { "for (int i = i + 1; i < size; i++) {", "The inner loop needs its own variable: j." };
    task->hint = "Like the outer loop, but with j, starting at i + 1.";

    task = &level->tasks[1];
    task->task = "Type the if line that adds 1 to count when arr[i] and arr[j] match.";
    task->code = "for (int i = 0; i < size; i++) {\n"
                 "    for (int j = i + 1; j < size; j++) {\n"
                 "        ____\n"
                 "    }\n"
                 "}";
    task->answers[0] = "if (arr[i] == arr[j]) count++;";
    task->answers[1] = "if (arr[i] == arr[j]) { count++; }";
    task->answers[2] = "if (arr[j] == arr[i]) count++;";
    task->answers[3] = "if (arr[i] == arr[j]) count += 1;";
    task->mistakes[0] = { "if (arr[i] == arr[i]) count++;", "That compares an element with itself, so it's always true. Use arr[j] for the second one." };
    task->mistakes[1] = { "if (arr[i] = arr[j]) count++;", "A single = copies. To compare, use ==." };
    task->hint = "Compare arr[i] with arr[j] using ==.";

    // ---------- Level 10: twoMovies ----------
    level = getLevel(10);
    level->chapter = 3;
    level->title = "Pairs that add up";
    level->shortName = "twoMovies";
    level->source = "Lab 1 Q17";
    level->explanation = "The same two loops can find two different elements that add up to a target. "
                         "As soon as you find a pair, return true. If the loops finish without finding one, return false.";
    level->example = "for (int i = 0; i < size; i++)\n"
                     "    for (int j = i + 1; j < size; j++)\n"
                     "        if (arr[i] + arr[j] == 10) return true;\n"
                     "return false;";
    level->inGame = "This one isn't used by the game, but it's straight from the Coding Exam 1 prep.";

    task = &level->tasks[0];
    task->task = "Type the if line that returns true when movie i and movie j add up to flightLength.";
    task->code = "bool twoMovies(int flightLength, int movieLengths[], int size) {\n"
                 "    for (int i = 0; i < size; i++) {\n"
                 "        for (int j = i + 1; j < size; j++) {\n"
                 "            ____\n"
                 "        }\n"
                 "    }\n"
                 "    return false;\n"
                 "}";
    task->answers[0] = "if (movieLengths[i] + movieLengths[j] == flightLength) return true;";
    task->answers[1] = "if (movieLengths[i] + movieLengths[j] == flightLength) { return true; }";
    task->answers[2] = "if (movieLengths[j] + movieLengths[i] == flightLength) return true;";
    task->answers[3] = "if (flightLength == movieLengths[i] + movieLengths[j]) return true;";
    task->mistakes[0] = { "if (movieLengths[i] + movieLengths[i] == flightLength) return true;", "That's the same movie twice. Use j for the second movie." };
    task->mistakes[1] = { "if (movieLengths[i] + movieLengths[j] = flightLength) return true;", "A single = assigns. To compare, use ==." };
    task->mistakes[2] = { "if (movieLengths[i] + movieLengths[j] == flightLength) return false;", "Finding a pair means the answer is yes: return true." };
    task->hint = "Add movieLengths[i] and movieLengths[j], compare with flightLength, and return true.";

    task = &level->tasks[1];
    task->task = "Pool version: type the if line that returns true when two balls' numbers add up to 15.";
    task->code = "bool comboOf15(int nums[], int size) {\n"
                 "    for (int i = 0; i < size; i++) {\n"
                 "        for (int j = i + 1; j < size; j++) {\n"
                 "            ____\n"
                 "        }\n"
                 "    }\n"
                 "    return false;\n"
                 "}";
    task->answers[0] = "if (nums[i] + nums[j] == 15) return true;";
    task->answers[1] = "if (nums[i] + nums[j] == 15) { return true; }";
    task->answers[2] = "if (nums[j] + nums[i] == 15) return true;";
    task->answers[3] = "if (15 == nums[i] + nums[j]) return true;";
    task->mistakes[0] = { "if (nums[i] + nums[i] == 15) return true;", "That uses the same ball twice. Use nums[j] for the second." };
    task->mistakes[1] = { "if (nums[i] + nums[j] = 15) return true;", "A single = assigns. To compare, use ==." };
    task->hint = "Add nums[i] and nums[j], compare with 15, and return true.";

    // ---------- Level 11: reverse ----------
    level = getLevel(11);
    level->chapter = 3;
    level->title = "Reversing an array";
    level->shortName = "reverse";
    level->source = "Lab 1 Q15";
    level->explanation = "To reverse an array in place, swap the first and last elements, then the second and second-last, "
                         "and so on, stopping at the middle. The partner of element i is element size - 1 - i. "
                         "You can reuse swap_ints from Level 6: give it the two addresses.";
    level->example = "// size 5: swap 0 with 4, then 1 with 3 (2 stays put)\n"
                     "swap_ints(&arr[0], &arr[4]);";
    level->inGame = "Your potted balls under the table are now shown newest first: the game reverses a copy of your tray.";

    task = &level->tasks[0];
    task->task = "Type the line inside the loop that swaps element i with its partner at the other end, using swap_ints.";
    task->code = "void reverse(int size, int arr[]) {\n"
                 "    for (int i = 0; i < size / 2; i++) {\n"
                 "        ____\n"
                 "    }\n"
                 "}";
    task->answers[0] = "swap_ints(&arr[i], &arr[size - 1 - i]);";
    task->answers[1] = "swap_ints(&arr[i], &arr[size - i - 1]);";
    task->answers[2] = "swap_ints(&arr[size - 1 - i], &arr[i]);";
    task->answers[3] = "swap_ints(&arr[size - i - 1], &arr[i]);";
    task->answers[4] = "int temp = arr[i]; arr[i] = arr[size - 1 - i]; arr[size - 1 - i] = temp;";
    task->answers[5] = "int temp = arr[i]; arr[i] = arr[size - i - 1]; arr[size - i - 1] = temp;";
    task->mistakes[0] = { "swap_ints(&arr[i], &arr[size - i]);", "When i is 0, arr[size - i] is one past the end. The last element is size - 1, so the partner is size - 1 - i." };
    task->mistakes[1] = { "swap_ints(arr[i], arr[size - 1 - i]);", "swap_ints needs addresses: put & in front of each element." };
    task->mistakes[2] = { "swap_ints(&arr[i], &arr[size - 1]);", "That always swaps with the last element. The partner moves too: size - 1 - i." };
    task->hint = "swap_ints(&arr[i], &arr[partner]); and the partner of i is size - 1 - i.";

    task = &level->tasks[1];
    task->task = "Type the for line so the loop stops at the middle of the array.";
    task->code = "void reverse(int size, int arr[]) {\n"
                 "    ____\n"
                 "        swap_ints(&arr[i], &arr[size - 1 - i]);\n"
                 "    }\n"
                 "}";
    task->answers[0] = "for (int i = 0; i < size / 2; i++) {";
    task->answers[1] = "for (int i = 0; i < size / 2; ++i) {";
    task->mistakes[0] = { "for (int i = 0; i < size; i++) {", "Going all the way swaps every pair twice, which puts the array back how it was. Stop at the middle: size / 2." };
    task->mistakes[1] = { "for (int i = 0; i <= size / 2; i++) {", "<= goes one too far for even sizes. Use i < size / 2." };
    task->hint = "A normal for loop, but stop at size / 2.";

    // ---------- Level 12: rotateLeft ----------
    level = getLevel(12);
    level->chapter = 3;
    level->title = "Shifting elements";
    level->shortName = "rotateLeft";
    level->source = "Lab 1 Q16";
    level->explanation = "To rotate left by one, save the first element, move every other element one place to the left, "
                         "then put the saved one at the end. {1, 2, 3, 4} becomes {2, 3, 4, 1}.";
    level->example = "int first = arr[0];\n"
                     "// ... shift everything one place left ...\n"
                     "// ... then put first at the end";
    level->inGame = "Not used by the game - it's Coding Exam 1 practice.";

    task = &level->tasks[0];
    task->task = "Type the line inside the loop that moves each element one place to the left.";
    task->code = "int first = arr[0];\n"
                 "for (int i = 0; i < size - 1; i++) {\n"
                 "    ____\n"
                 "}\n"
                 "arr[size - 1] = first;";
    task->answers[0] = "arr[i] = arr[i + 1];";
    task->answers[1] = "arr[i] = arr[1 + i];";
    task->mistakes[0] = { "arr[i + 1] = arr[i];", "That copies to the right. Moving left means arr[i] gets the element after it." };
    task->mistakes[1] = { "arr[i] = arr[i - 1];", "arr[i - 1] is the one before. You want the one after: arr[i + 1]." };
    task->mistakes[2] = { "arr[i] = arr[i];", "That copies an element onto itself. Use the next one: arr[i + 1]." };
    task->hint = "Element i gets the value of element i + 1.";

    task = &level->tasks[1];
    task->task = "Type the last line, which puts the saved first element at the end.";
    task->code = "int first = arr[0];\n"
                 "for (int i = 0; i < size - 1; i++) {\n"
                 "    arr[i] = arr[i + 1];\n"
                 "}\n"
                 "____";
    task->answers[0] = "arr[size - 1] = first;";
    task->mistakes[0] = { "arr[size] = first;", "arr[size] is past the end. The last element is arr[size - 1]." };
    task->mistakes[1] = { "first = arr[size - 1];", "Other way round: the last element gets first." };
    task->mistakes[2] = { "arr[0] = first;", "first goes at the end, not back at the start." };
    task->hint = "The last element is arr[size - 1].";

    // ---------- Level 13: wordCounter ----------
    level = getLevel(13);
    level->chapter = 3;
    level->title = "Counting words";
    level->shortName = "wordCount";
    level->source = "Lab 1 Q18";
    level->explanation = "A word is one or more letters. Walk along the chars and remember whether you're inside a word. "
                         "A new word starts when you find a letter and you weren't already in a word: that's when to count.";
    level->example = "char s[] = \"hi there\";\n"
                     "// words start at the 'h' and the 't'\n"
                     "// so there are 2 words";
    level->inGame = "Not used by the game - it's the last Lab 1 question. Next chapter: no more [ ]!";

    task = &level->tasks[0];
    task->task = "Type the if line that adds 1 to count when a letter starts a new word.";
    task->code = "int wordCounter(int size, char characters[]) {\n"
                 "    int count = 0;\n"
                 "    bool inWord = false;\n"
                 "    for (int i = 0; i < size; i++) {\n"
                 "        char c = characters[i];\n"
                 "        bool isLetter = (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');\n"
                 "        ____\n"
                 "        inWord = isLetter;\n"
                 "    }\n"
                 "    return count;\n"
                 "}";
    task->answers[0] = "if (isLetter && !inWord) count++;";
    task->answers[1] = "if (!inWord && isLetter) count++;";
    task->answers[2] = "if (isLetter && inWord == false) count++;";
    task->answers[3] = "if (isLetter && !inWord) { count++; }";
    task->answers[4] = "if (isLetter == true && inWord == false) count++;";
    task->mistakes[0] = { "if (isLetter) count++;", "That counts every letter. Only count when a letter starts a word: isLetter && !inWord" };
    task->mistakes[1] = { "if (!isLetter && inWord) count++;", "That counts where words end, so a word at the very end is missed. Count where they start instead." };
    task->mistakes[2] = { "if (isLetter || !inWord) count++;", "|| means 'or'. You need both to be true: &&" };
    task->hint = "Count when isLetter is true AND inWord is false.";

    task = &level->tasks[1];
    task->task = "Type the line that works out whether c is a letter (A to Z, or a to z).";
    task->code = "for (int i = 0; i < size; i++) {\n"
                 "    char c = characters[i];\n"
                 "    ____\n"
                 "    if (isLetter && !inWord) count++;\n"
                 "    inWord = isLetter;\n"
                 "}";
    task->answers[0] = "bool isLetter = (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');";
    task->answers[1] = "bool isLetter = (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');";
    task->answers[2] = "bool isLetter = c >= 'A' && c <= 'Z' || c >= 'a' && c <= 'z';";
    task->answers[3] = "bool isLetter = c >= 'a' && c <= 'z' || c >= 'A' && c <= 'Z';";
    task->mistakes[0] = { "bool isLetter = (c >= 'A' || c <= 'Z') || (c >= 'a' || c <= 'z');", "c >= 'A' || c <= 'Z' is true for every char. Inside each bracket use && (both must be true)." };
    task->mistakes[1] = { "bool isLetter = (c >= \"A\" && c <= \"Z\") || (c >= \"a\" && c <= \"z\");", "Use single quotes for one character: 'A', not \"A\"." };
    task->hint = "c is a capital if c >= 'A' && c <= 'Z'. Do the same for small letters and join the two with ||.";

    // =========================================================
    // Chapter 4: Pointer arithmetic (Lab 2: no [ ] allowed)
    // =========================================================

    // ---------- Level 14: pointer arithmetic ----------
    level = getLevel(14);
    level->chapter = 4;
    level->title = "Pointer arithmetic";
    level->shortName = "ptr + i";
    level->source = "Pointers lecture: Pointer Arithmetic and Array, More Pointer Arithmetic";
    level->explanation = "An array's name is the address of its first element, and ptr + 1 is the next element, "
                         "so *(arr + i) is the same as arr[i]. C moves by the size of the type: a Ball is 24 bytes, "
                         "so balls + 14 is 1000 + 14 * 24 = 1336. From now on [ ] is locked, like the Lab 2 rule.";
    level->example = "int arr[] = {4, 6, 7};\n"
                     "int *ptr = arr;        // same as &arr[0]\n"
                     "cout << *(ptr + 2);    // prints 7, same as arr[2]";
    level->inGame = "Your call line has to change too: [ ] is locked, so call ball 14 with Ball *target = balls + 14;";

    task = &level->tasks[0];
    task->task = "Read element 2 of the rack without [ ]: type the line that stores it in a new int called ball.";
    task->code = "int rack[15] = {1, 9, 2, 10, 8, 3, 11, 4, 12, 5, 6, 13, 7, 14, 15};\n"
                 "____";
    task->answers[0] = "int ball = *(rack + 2);";
    task->answers[1] = "int ball = *(2 + rack);";
    task->mistakes[0] = { "int ball = rack + 2;", "rack + 2 is an address. Put * in front to read the value: *(rack + 2)" };
    task->mistakes[1] = { "int ball = *rack + 2;", "*rack + 2 reads element 0 and adds 2 to it. Put the brackets round rack + 2 first." };
    task->mistakes[2] = { "int ball = &(rack + 2);", "& gives an address. To read the value, use *." };
    task->hint = "The value of element i is *(rack + i).";

    task = &level->tasks[1];
    task->task = "Make ptr point at element 3 of the rack, without [ ] or &.";
    task->code = "int rack[15] = {1, 9, 2, 10, 8, 3, 11, 4, 12, 5, 6, 13, 7, 14, 15};\n"
                 "int *ptr;\n"
                 "____";
    task->answers[0] = "ptr = rack + 3;";
    task->answers[1] = "ptr = 3 + rack;";
    task->mistakes[0] = { "ptr = *(rack + 3);", "*(rack + 3) is the value at element 3, not its address. Leave out the *." };
    task->mistakes[1] = { "int *ptr = rack + 3;", "ptr already exists. Just assign it: ptr = rack + 3;" };
    task->mistakes[2] = { "ptr = rack;", "That's element 0. Add 3 to move along." };
    task->hint = "rack + 3 is the address of element 3.";

    // ---------- Level 15: sum_array ----------
    level = getLevel(15);
    level->chapter = 4;
    level->title = "sum_array";
    level->shortName = "sum_array";
    level->source = "Lab 2 Q9; Pointers lecture exercises";
    level->explanation = "Now use pointer arithmetic in a loop. *(arr + i) reads element i without [ ]. "
                         "Add each one onto sum, and return sum at the end.";
    level->example = "int arr[] = {4, 6, 7, 8, 9};\n"
                     "for (int i = 0; i < 5; i++) {\n"
                     "    cout << *(arr + i) << \" \";   // prints each one\n"
                     "}";
    level->inGame = "At the end of a game, the results screen adds up the numbers of each player's potted balls with sum_array.";

    task = &level->tasks[0];
    task->task = "Type the line inside the loop that adds element i onto sum, without [ ].";
    task->code = "int sum_array(int *arr, int size) {\n"
                 "    int sum = 0;\n"
                 "    for (int i = 0; i < size; i++) {\n"
                 "        ____\n"
                 "    }\n"
                 "    return sum;\n"
                 "}";
    task->answers[0] = "sum += *(arr + i);";
    task->answers[1] = "sum = sum + *(arr + i);";
    task->answers[2] = "sum += *(i + arr);";
    task->mistakes[0] = { "sum += arr + i;", "arr + i is an address. Use *(arr + i) for the value." };
    task->mistakes[1] = { "sum += *arr + i;", "*arr + i adds i to the first element. Put the brackets round arr + i." };
    task->mistakes[2] = { "sum = *(arr + i);", "That replaces sum each time. Add onto it with +=." };
    task->hint = "Add *(arr + i) onto sum with +=.";

    task = &level->tasks[1];
    task->task = "This version walks a pointer p along the array. Type the line that adds the element p points at onto sum.";
    task->code = "int sum_array(int *arr, int size) {\n"
                 "    int sum = 0;\n"
                 "    for (int *p = arr; p < arr + size; p++) {\n"
                 "        ____\n"
                 "    }\n"
                 "    return sum;\n"
                 "}";
    task->answers[0] = "sum += *p;";
    task->answers[1] = "sum = sum + *p;";
    task->mistakes[0] = { "sum += p;", "p is an address. Use *p to get the element it points at." };
    task->mistakes[1] = { "sum = *p;", "That replaces sum each time. Add onto it with +=." };
    task->mistakes[2] = { "sum += *(p + 1);", "p already moves along by itself (p++). Just use *p." };
    task->hint = "The element p points at is *p.";

    // ---------- Level 16: find_max ----------
    level = getLevel(16);
    level->chapter = 4;
    level->title = "find_max";
    level->shortName = "find_max";
    level->source = "Lab 2 Q3";
    level->explanation = "To find the biggest value, start with the first element as the biggest so far. "
                         "Then compare every other element with it, and keep the larger one.";
    level->example = "int max = *arr;                // the first element\n"
                     "if (*(arr + 1) > max) {\n"
                     "    max = *(arr + 1);\n"
                     "}";
    level->inGame = "Under the table you can now see the highest-numbered ball still on the table, found with find_max.";

    task = &level->tasks[0];
    task->task = "Type the if line that updates max when element i is bigger.";
    task->code = "int find_max(int *arr, int size) {\n"
                 "    int max = *arr;\n"
                 "    for (int i = 1; i < size; i++) {\n"
                 "        ____\n"
                 "    }\n"
                 "    return max;\n"
                 "}";
    task->answers[0] = "if (*(arr + i) > max) max = *(arr + i);";
    task->answers[1] = "if (*(arr + i) > max) { max = *(arr + i); }";
    task->answers[2] = "if (max < *(arr + i)) max = *(arr + i);";
    task->mistakes[0] = { "if (*(arr + i) < max) max = *(arr + i);", "< keeps the smaller one. Use > to keep the biggest." };
    task->mistakes[1] = { "if (arr + i > max) max = arr + i;", "arr + i is an address. Compare the value: *(arr + i)" };
    task->mistakes[2] = { "max = *(arr + i);", "Without the if, max just ends up as the last element." };
    task->hint = "If element i is bigger than max, max becomes element i. Element i is *(arr + i).";

    task = &level->tasks[1];
    task->task = "Type the first line of find_max: start max at the first element, without [ ].";
    task->code = "int find_max(int *arr, int size) {\n"
                 "    ____\n"
                 "    for (int i = 1; i < size; i++) {\n"
                 "        if (*(arr + i) > max) max = *(arr + i);\n"
                 "    }\n"
                 "    return max;\n"
                 "}";
    task->answers[0] = "int max = *arr;";
    task->answers[1] = "int max = *(arr + 0);";
    task->mistakes[0] = { "int max = 0;", "Starting at 0 goes wrong if every number is negative. Lab 2 starts with the first element: *arr" };
    task->mistakes[1] = { "int max = arr;", "arr is the address. *arr is the first element." };
    task->mistakes[2] = { "max = *arr;", "max doesn't exist yet. Declare it: int max = *arr;" };
    task->hint = "The first element is *arr.";

    // ---------- Level 17: reverse_array ----------
    level = getLevel(17);
    level->chapter = 4;
    level->title = "reverse_array";
    level->shortName = "rev_array";
    level->source = "Lab 2 Q4";
    level->explanation = "Same idea as Level 11, but with two pointers instead of indexes. start points at the first element "
                         "and end at the last. Swap what they point at, then move start forward and end back until they meet.";
    level->example = "int *start = arr;\n"
                     "int *end = arr + size - 1;   // the last element\n"
                     "start++;                     // move towards the middle\n"
                     "end--;";
    level->inGame = "This is the function the game really uses to show your potted balls newest first.";

    task = &level->tasks[0];
    task->task = "Type the line inside the loop that swaps the two elements, using swap_ints.";
    task->code = "void reverse_array(int *arr, int size) {\n"
                 "    int *start = arr;\n"
                 "    int *end = arr + size - 1;\n"
                 "    while (start < end) {\n"
                 "        ____\n"
                 "        start++;\n"
                 "        end--;\n"
                 "    }\n"
                 "}";
    task->answers[0] = "swap_ints(start, end);";
    task->answers[1] = "swap_ints(end, start);";
    task->mistakes[0] = { "swap_ints(&start, &end);", "start and end are already addresses. Pass them as they are: swap_ints(start, end);" };
    task->mistakes[1] = { "swap_ints(*start, *end);", "*start is a value. swap_ints needs the addresses: start and end." };
    task->mistakes[2] = { "swap_ints(arr, end);", "arr never moves. Use start, which moves towards the middle." };
    task->hint = "start and end are already pointers, so pass them straight to swap_ints.";

    task = &level->tasks[1];
    task->task = "Type the line that makes end point at the last element.";
    task->code = "void reverse_array(int *arr, int size) {\n"
                 "    int *start = arr;\n"
                 "    ____\n"
                 "    while (start < end) {\n"
                 "        swap_ints(start, end);\n"
                 "        start++;\n"
                 "        end--;\n"
                 "    }\n"
                 "}";
    task->answers[0] = "int *end = arr + size - 1;";
    task->answers[1] = "int *end = arr + (size - 1);";
    task->answers[2] = "int *end = size - 1 + arr;";
    task->mistakes[0] = { "int *end = arr + size;", "arr + size is one past the end. The last element is arr + size - 1." };
    task->mistakes[1] = { "int *end = *(arr + size - 1);", "That's the value of the last element. Leave out the * to get its address." };
    task->mistakes[2] = { "int end = arr + size - 1;", "end must be a pointer: int *end" };
    task->hint = "The last element is at arr + size - 1.";

    // =========================================================
    // Chapter 5: Strings with pointers (Lab 2)
    // =========================================================

    // ---------- Level 18: strlen_ptr ----------
    level = getLevel(18);
    level->chapter = 5;
    level->title = "strlen_ptr";
    level->shortName = "strlen_ptr";
    level->source = "Lab 2 Q5";
    level->explanation = "A C string is an array of chars that ends with a hidden '\\0'. To find its length, "
                         "walk a pointer along the string and count, until *str is '\\0'.";
    level->example = "char name[] = \"Kon\";   // stored as K o n \\0\n"
                     "char *p = name;        // p points at 'K'\n"
                     "p++;                   // now p points at 'o'";
    level->inGame = "Every letter in this font is the same width, so the game centres player names using "
                    "strlen_ptr(name) times the width of one letter.";

    task = &level->tasks[0];
    task->task = "Type the while line that keeps going until str reaches the '\\0' at the end.";
    task->code = "int strlen_ptr(char *str) {\n"
                 "    int length = 0;\n"
                 "    ____\n"
                 "        length++;\n"
                 "        str++;\n"
                 "    }\n"
                 "    return length;\n"
                 "}";
    task->answers[0] = "while (*str != '\\0') {";
    task->answers[1] = "while (*str != 0) {";
    task->answers[2] = "while (*str) {";
    task->mistakes[0] = { "while (str != '\\0') {", "str is the address. Compare the character it points at: *str" };
    task->mistakes[1] = { "while (*str == '\\0') {", "== '\\0' is only true at the very end. Keep going while it's NOT the end: !=" };
    task->mistakes[2] = { "while (*str != \"\\0\") {", "Use single quotes for one character: '\\0'" };
    task->hint = "Keep going while the character str points at is not '\\0'.";

    task = &level->tasks[1];
    task->task = "Type the line inside the loop that moves str on to the next character.";
    task->code = "int strlen_ptr(char *str) {\n"
                 "    int length = 0;\n"
                 "    while (*str != '\\0') {\n"
                 "        length++;\n"
                 "        ____\n"
                 "    }\n"
                 "    return length;\n"
                 "}";
    task->answers[0] = "str++;";
    task->answers[1] = "++str;";
    task->answers[2] = "str = str + 1;";
    task->answers[3] = "str += 1;";
    task->mistakes[0] = { "*str++;", "Nearly! The * isn't needed (it reads the char and throws it away). Just str++;" };
    task->mistakes[1] = { "(*str)++;", "That changes the character itself (K would become L!). Move the pointer instead: str++;" };
    task->mistakes[2] = { "length++;", "length is already counted. Now move the pointer on." };
    task->hint = "Pointer arithmetic: add 1 to the pointer.";

    // ---------- Level 19: count_vowels ----------
    level = getLevel(19);
    level->chapter = 5;
    level->title = "count_vowels";
    level->shortName = "vowels";
    level->source = "Lab 2 Q6";
    level->explanation = "Walk the string the same way as strlen_ptr, but check each character first. To check for "
                         "several letters, compare *str with each one and join the checks with ||. "
                         "(The Lab 2 sheet says \"OpenAI\" has 3 vowels, but the I counts too, so it's really 4.)";
    level->example = "if (*str == 'x' || *str == 'X') {\n"
                     "    count++;\n"
                     "}";
    level->inGame = "Not used by the game, but it's on Lab 2.";

    task = &level->tasks[0];
    task->task = "Type the if line that adds 1 to count when *str is a small vowel. Check them in the order a, e, i, o, u.";
    task->code = "int count_vowels(char *str) {\n"
                 "    int count = 0;\n"
                 "    while (*str != '\\0') {\n"
                 "        ____\n"
                 "        str++;\n"
                 "    }\n"
                 "    return count;\n"
                 "}";
    task->answers[0] = "if (*str == 'a' || *str == 'e' || *str == 'i' || *str == 'o' || *str == 'u') count++;";
    task->answers[1] = "if (*str == 'a' || *str == 'e' || *str == 'i' || *str == 'o' || *str == 'u') { count++; }";
    task->mistakes[0] = { "if (*str == 'a' || 'e' || 'i' || 'o' || 'u') count++;", "Each check needs its own *str ==. On its own, 'e' is always true, so this counts every character." };
    task->mistakes[1] = { "if (*str == 'a' && *str == 'e' && *str == 'i' && *str == 'o' && *str == 'u') count++;", "&& needs ALL of them to be true at once, which never happens. Use || (or)." };
    task->mistakes[2] = { "if (str == 'a' || str == 'e' || str == 'i' || str == 'o' || str == 'u') count++;", "str is the address. Compare the character: *str" };
    task->hint = "Five checks like *str == 'a', joined with ||, then count++;";

    task = &level->tasks[1];
    task->task = "Type the if line that adds 1 to count when *str is 'a' or 'A'.";
    task->code = "int count_a(char *str) {\n"
                 "    int count = 0;\n"
                 "    while (*str != '\\0') {\n"
                 "        ____\n"
                 "        str++;\n"
                 "    }\n"
                 "    return count;\n"
                 "}";
    task->answers[0] = "if (*str == 'a' || *str == 'A') count++;";
    task->answers[1] = "if (*str == 'A' || *str == 'a') count++;";
    task->answers[2] = "if (*str == 'a' || *str == 'A') { count++; }";
    task->answers[3] = "if (*str == 'A' || *str == 'a') { count++; }";
    task->mistakes[0] = { "if (*str == 'a' || 'A') count++;", "Each check needs its own *str ==: *str == 'a' || *str == 'A'" };
    task->mistakes[1] = { "if (*str == \"a\" || *str == \"A\") count++;", "Use single quotes for a char: 'a'" };
    task->mistakes[2] = { "if (*str == 'a' && *str == 'A') count++;", "A character can't be both at once. Use || (or)." };
    task->hint = "Two checks joined with ||.";

    // ---------- Level 20: copy_string ----------
    level = getLevel(20);
    level->chapter = 5;
    level->title = "copy_string";
    level->shortName = "copy_str";
    level->source = "Lab 2 Q7";
    level->explanation = "Copy one character at a time from src to dest, moving both pointers on each time. "
                         "Stop at src's '\\0', then put a '\\0' at the end of dest so it's a proper string too.";
    level->example = "char word[] = \"cat\";\n"
                     "char *p = word;\n"
                     "*p = 'b';            // word is now \"bat\"";
    level->inGame = "When someone wins, the game starts the win message by copying their name with copy_string.";

    task = &level->tasks[0];
    task->task = "Type the line inside the loop that copies one character from src to dest.";
    task->code = "void copy_string(char *dest, char *src) {\n"
                 "    while (*src != '\\0') {\n"
                 "        ____\n"
                 "        dest++;\n"
                 "        src++;\n"
                 "    }\n"
                 "    *dest = '\\0';\n"
                 "}";
    task->answers[0] = "*dest = *src;";
    task->mistakes[0] = { "dest = src;", "That copies the address, so dest just points at src. Copy the character: *dest = *src;" };
    task->mistakes[1] = { "*src = *dest;", "Other way round: dest gets src's character." };
    task->mistakes[2] = { "dest* = src*;", "The * goes in front: *dest = *src;" };
    task->hint = "Write the character src points at into the place dest points at.";

    task = &level->tasks[1];
    task->task = "Type the line after the loop that ends dest with '\\0'.";
    task->code = "void copy_string(char *dest, char *src) {\n"
                 "    while (*src != '\\0') {\n"
                 "        *dest = *src;\n"
                 "        dest++;\n"
                 "        src++;\n"
                 "    }\n"
                 "    ____\n"
                 "}";
    task->answers[0] = "*dest = '\\0';";
    task->answers[1] = "*dest = 0;";
    task->mistakes[0] = { "dest = '\\0';", "Put * in front: you're writing a character to where dest points." };
    task->mistakes[1] = { "*dest = \"\\0\";", "Use single quotes for one character: '\\0'" };
    task->mistakes[2] = { "*src = '\\0';", "src already ends with '\\0'. It's dest that needs one." };
    task->hint = "Write '\\0' to where dest points.";

    // ---------- Level 21: concat_strings ----------
    level = getLevel(21);
    level->chapter = 5;
    level->title = "concat_strings";
    level->shortName = "concat";
    level->source = "Lab 2 Q8";
    level->explanation = "To add one string onto the end of another, first move dest along to its '\\0'. "
                         "Then copy src from there - and you already have a function that copies: copy_string.";
    level->example = "char dest[20] = \"Hi \";\n"
                     "// move dest to the '\\0' after \"Hi \",\n"
                     "// then copy \"Kon\" there: \"Hi Kon\"";
    level->inGame = "The game adds \" wins!\" to the winner's name with concat_strings.";

    task = &level->tasks[0];
    task->task = "Type the while line that moves dest along to the end of its string.";
    task->code = "void concat_strings(char *dest, char *src) {\n"
                 "    ____\n"
                 "    copy_string(dest, src);\n"
                 "}";
    task->answers[0] = "while (*dest != '\\0') dest++;";
    task->answers[1] = "while (*dest != '\\0') { dest++; }";
    task->answers[2] = "while (*dest) dest++;";
    task->answers[3] = "while (*dest != 0) dest++;";
    task->mistakes[0] = { "while (dest != '\\0') dest++;", "Compare the character, not the address: *dest" };
    task->mistakes[1] = { "while (*dest != '\\0') *dest++;", "Nearly! Leave out the * in front of dest++." };
    task->mistakes[2] = { "while (*src != '\\0') src++;", "It's dest you need to move to the end, so src can be copied there." };
    task->hint = "The same loop as strlen_ptr, but with dest and without counting.";

    task = &level->tasks[1];
    task->task = "Type the line that copies src onto the end of dest, reusing copy_string.";
    task->code = "void concat_strings(char *dest, char *src) {\n"
                 "    while (*dest != '\\0') dest++;\n"
                 "    ____\n"
                 "}";
    task->answers[0] = "copy_string(dest, src);";
    task->mistakes[0] = { "copy_string(src, dest);", "copy_string(dest, src): the destination comes first." };
    task->mistakes[1] = { "copy_string(*dest, *src);", "Pass the pointers, not the characters: copy_string(dest, src);" };
    task->mistakes[2] = { "copy_string(&dest, &src);", "dest and src are already pointers. Pass them as they are." };
    task->hint = "copy_string(destination, source);";

    // ---------- Level 22: compare_strings ----------
    level = getLevel(22);
    level->chapter = 5;
    level->title = "compare_strings";
    level->shortName = "compare";
    level->source = "Lab 2 Q10";
    level->explanation = "Walk both strings together while their characters match. Stop when they differ or the first one ends, "
                         "and return the difference between the two characters: negative if the first string comes first, "
                         "0 if they're equal, positive if it comes after.";
    level->example = "'a' - 'b'      // is -1, so \"apple\" < \"banana\"\n"
                     "'c' - 'c'      // is 0: the same character";
    level->inGame = "When you click Run, the game's answer checker compares your answer with the right ones using compare_strings.";

    task = &level->tasks[0];
    task->task = "Type the return line: the difference between the two current characters.";
    task->code = "int compare_strings(char *str1, char *str2) {\n"
                 "    while (*str1 != '\\0' && *str1 == *str2) {\n"
                 "        str1++;\n"
                 "        str2++;\n"
                 "    }\n"
                 "    ____\n"
                 "}";
    task->answers[0] = "return *str1 - *str2;";
    task->mistakes[0] = { "return str1 - str2;", "That subtracts the addresses. Use the characters: *str1 - *str2" };
    task->mistakes[1] = { "return *str2 - *str1;", "Other way round: first minus second, so \"apple\" vs \"banana\" gives a negative number." };
    task->mistakes[2] = { "return 0;", "That says every pair of strings is equal. Return the difference: *str1 - *str2" };
    task->hint = "Subtract the second character from the first.";

    task = &level->tasks[1];
    task->task = "Type the while line: keep going while str1 hasn't ended and the two characters match.";
    task->code = "int compare_strings(char *str1, char *str2) {\n"
                 "    ____\n"
                 "        str1++;\n"
                 "        str2++;\n"
                 "    }\n"
                 "    return *str1 - *str2;\n"
                 "}";
    task->answers[0] = "while (*str1 != '\\0' && *str1 == *str2) {";
    task->answers[1] = "while (*str1 == *str2 && *str1 != '\\0') {";
    task->answers[2] = "while (*str1 && *str1 == *str2) {";
    task->answers[3] = "while (*str1 != 0 && *str1 == *str2) {";
    task->mistakes[0] = { "while (*str1 == *str2) {", "If both strings are the same, this runs past the '\\0' at the end. Also check *str1 != '\\0'." };
    task->mistakes[1] = { "while (*str1 != '\\0' || *str1 == *str2) {", "|| keeps going if either is true. You need both: &&" };
    task->mistakes[2] = { "while (str1 == str2) {", "That compares the addresses. Compare the characters: *str1 == *str2" };
    task->hint = "Two checks joined with &&: str1 hasn't reached '\\0', and *str1 == *str2.";

    // =========================================================
    // Chapter 6: Memory and linked lists
    // =========================================================

    // ---------- Level 23: new and delete ----------
    level = getLevel(23);
    level->chapter = 6;
    level->title = "new and delete";
    level->shortName = "new/delete";
    level->source = "Memory Management lecture; Pointers & Linked List lecture";
    level->explanation = "new makes memory on the heap that stays until you delete it, even after the function ends. "
                         "Every new needs exactly one delete, or you leak memory. Memory from new[] (an array) "
                         "is freed with delete[]. Don't use memory after you've deleted it.";
    level->example = "int *ptr = new int;\n"
                     "*ptr = 42;\n"
                     "cout << *ptr;     // 42\n"
                     "delete ptr;";
    level->inGame = "Every entry in the shot log is made with new. When a new game starts, the old entries are freed with delete.";

    task = &level->tasks[0];
    task->task = "Type the line that frees the memory once you're done with it.";
    task->code = "int *p = new int;\n"
                 "*p = 5;\n"
                 "cout << *p << endl;\n"
                 "____";
    task->answers[0] = "delete p;";
    task->mistakes[0] = { "delete *p;", "delete takes the pointer itself: delete p;" };
    task->mistakes[1] = { "delete[] p;", "delete[] is for arrays made with new[]. This was one int: delete p;" };
    task->mistakes[2] = { "free(p);", "free goes with malloc in C. Memory from new is freed with delete." };
    task->mistakes[3] = { "p = nullptr;", "That forgets the address but doesn't free the memory - it's a leak! Use delete p;" };
    task->hint = "delete, then the pointer.";

    task = &level->tasks[1];
    task->task = "This array was made with new[]. Type the line that frees it.";
    task->code = "int numItems;\n"
                 "cin >> numItems;\n"
                 "int *arr = new int[numItems];\n"
                 "// ... use the array ...\n"
                 "____";
    task->answers[0] = "delete[] arr;";
    task->mistakes[0] = { "delete arr;", "Memory from new[] must be freed with delete[]." };
    task->mistakes[1] = { "delete[] *arr;", "Give delete[] the pointer itself: delete[] arr;" };
    task->mistakes[2] = { "free(arr);", "free goes with malloc. Memory from new[] is freed with delete[]." };
    task->hint = "Arrays made with new[] are freed with delete[].";

    // ---------- Level 24: -> ----------
    level = getLevel(24);
    level->chapter = 6;
    level->title = "-> for pointers to structs";
    level->shortName = "->";
    level->source = "Pointers & Linked List lecture: newNode->data";
    level->explanation = "When a pointer points at a struct, use -> to reach one of its fields. "
                         "cue->x means (*cue).x: go to the struct the pointer points at, then take its x.";
    level->example = "Ball *cue = balls + 0;   // a pointer to the cue ball\n"
                     "cue->x = 220;            // same as (*cue).x = 220;";
    level->inGame = "After a scratch, the game puts the cue ball back on the start line with code like cue->x = ... and cue->y = ...";

    task = &level->tasks[0];
    task->task = "Type the line that sets the cue ball's y to 260, through the pointer cue.";
    task->code = "Ball *cue = balls + 0;\n"
                 "cue->x = 220;\n"
                 "____";
    task->answers[0] = "cue->y = 260;";
    task->answers[1] = "(*cue).y = 260;";
    task->mistakes[0] = { "cue.y = 260;", "cue is a pointer, so use -> to reach its fields: cue->y" };
    task->mistakes[1] = { "*cue.y = 260;", "The . happens before the *, so this doesn't work. Use cue->y (or (*cue).y)." };
    task->mistakes[2] = { "cue->x = 260;", "That's x again. This time set y." };
    task->hint = "pointer->field = value;";

    task = &level->tasks[1];
    task->task = "Type the line that marks the target ball as potted (set its pocketed field to true), through the pointer target.";
    task->code = "Ball *target = balls + 14;\n"
                 "____";
    task->answers[0] = "target->pocketed = true;";
    task->answers[1] = "(*target).pocketed = true;";
    task->mistakes[0] = { "target.pocketed = true;", "target is a pointer, so use ->: target->pocketed" };
    task->mistakes[1] = { "*target.pocketed = true;", "The . happens before the *. Use target->pocketed" };
    task->mistakes[2] = { "target->pocketed == true;", "== compares. To set it, use a single =." };
    task->hint = "pointer->field = value;";

    // ---------- Level 25: walking a linked list ----------
    level = getLevel(25);
    level->chapter = 6;
    level->title = "Walking a linked list";
    level->shortName = "walk list";
    level->source = "Pointers & Linked List lecture: Traversing the List (PrintList)";
    level->explanation = "A linked list is a chain of nodes. Each node holds its data and nextPtr, a pointer to the next node. "
                         "The last node's nextPtr is nullptr. To visit every node, start at the head and keep following nextPtr "
                         "until you reach nullptr.";
    level->example = "struct Node {\n"
                     "    int data;\n"
                     "    Node *nextPtr;   // the next node, or nullptr\n"
                     "};";
    level->inGame = "The shot log under the table is a linked list: one node per shot.";

    task = &level->tasks[0];
    task->task = "Type the line that moves curNodePtr on to the next node.";
    task->code = "void PrintList(Node *headPtr) {\n"
                 "    Node *curNodePtr = headPtr;\n"
                 "    while (curNodePtr != nullptr) {\n"
                 "        cout << curNodePtr->data << \" \";\n"
                 "        ____\n"
                 "    }\n"
                 "}";
    task->answers[0] = "curNodePtr = curNodePtr->nextPtr;";
    task->answers[1] = "curNodePtr = (*curNodePtr).nextPtr;";
    task->mistakes[0] = { "curNodePtr++;", "Nodes aren't next to each other in memory like an array, so ++ won't work. Follow the link: curNodePtr->nextPtr" };
    task->mistakes[1] = { "curNodePtr->nextPtr = curNodePtr;", "Other way round: curNodePtr gets its next node." };
    task->mistakes[2] = { "curNodePtr = nextPtr;", "nextPtr belongs to the node: curNodePtr->nextPtr" };
    task->hint = "curNodePtr becomes the node its nextPtr points at.";

    task = &level->tasks[1];
    task->task = "Type the while line that keeps going until the end of the list.";
    task->code = "void PrintList(Node *headPtr) {\n"
                 "    Node *curNodePtr = headPtr;\n"
                 "    ____\n"
                 "        cout << curNodePtr->data << \" \";\n"
                 "        curNodePtr = curNodePtr->nextPtr;\n"
                 "    }\n"
                 "}";
    task->answers[0] = "while (curNodePtr != nullptr) {";
    task->answers[1] = "while (curNodePtr != NULL) {";
    task->answers[2] = "while (curNodePtr) {";
    task->mistakes[0] = { "while (curNodePtr->nextPtr != nullptr) {", "That stops one node early, so the last node isn't printed." };
    task->mistakes[1] = { "while (curNodePtr == nullptr) {", "That only runs when the list is empty. Use !=" };
    task->mistakes[2] = { "while (headPtr != nullptr) {", "headPtr never changes, so this never stops. Check curNodePtr." };
    task->hint = "Keep going while curNodePtr is not nullptr.";

    // ---------- Level 26: InsertAtEnd and Node ** ----------
    level = getLevel(26);
    level->chapter = 6;
    level->title = "InsertAtEnd and Node **";
    level->shortName = "InsertEnd";
    level->source = "Pointers lecture: pointer to pointer; Pointers & Linked List lecture: InsertAtEnd";
    level->explanation = "A pointer to a pointer (Node **) lets a function change the caller's own head pointer. "
                         "*ptrToHeadPtr is the head pointer itself. If the list is empty, the new node becomes the head. "
                         "Otherwise, walk to the last node and link the new one after it.";
    level->example = "int i = 5;\n"
                     "int *ptr = &i;\n"
                     "int **pptr = &ptr;    // a pointer to a pointer\n"
                     "**pptr = 7;           // i is now 7";
    level->inGame = "Every shot is added to the end of the shot log.";

    task = &level->tasks[0];
    task->task = "The list is empty. Type the line that makes newNode the head.";
    task->code = "void InsertAtEnd(Node **ptrToHeadPtr, int newData) {\n"
                 "    Node *newNode = new Node();\n"
                 "    newNode->data = newData;\n"
                 "    newNode->nextPtr = nullptr;\n"
                 "    if (*ptrToHeadPtr == nullptr) {\n"
                 "        ____\n"
                 "        return;\n"
                 "    }\n"
                 "    // ... otherwise walk to the end ...\n"
                 "}";
    task->answers[0] = "*ptrToHeadPtr = newNode;";
    task->mistakes[0] = { "ptrToHeadPtr = newNode;", "That only changes the function's own copy. Put * in front to change the caller's head: *ptrToHeadPtr" };
    task->mistakes[1] = { "**ptrToHeadPtr = newNode;", "**ptrToHeadPtr is the head node itself, not the head pointer. Use one *." };
    task->mistakes[2] = { "Node *head = newNode;", "That makes a new local variable. Change the caller's head through ptrToHeadPtr." };
    task->hint = "*ptrToHeadPtr is the head pointer. Set it to newNode.";

    task = &level->tasks[1];
    task->task = "curNodePtr is now the last node. Type the line that links newNode after it.";
    task->code = "    Node *curNodePtr = *ptrToHeadPtr;\n"
                 "    while (curNodePtr->nextPtr != nullptr) {\n"
                 "        curNodePtr = curNodePtr->nextPtr;\n"
                 "    }\n"
                 "    ____\n"
                 "}";
    task->answers[0] = "curNodePtr->nextPtr = newNode;";
    task->mistakes[0] = { "curNodePtr = newNode;", "That just moves your local pointer. Link the nodes: curNodePtr->nextPtr = newNode;" };
    task->mistakes[1] = { "newNode->nextPtr = curNodePtr;", "Other way round: the last node points to the new one." };
    task->mistakes[2] = { "*ptrToHeadPtr = newNode;", "That would replace the whole list with one node. Link it after the last node instead." };
    task->hint = "The last node's nextPtr should point at newNode.";

    // ---------- Level 27: doubly linked list ----------
    level = getLevel(27);
    level->chapter = 6;
    level->title = "Doubly linked: prevPointer";
    level->shortName = "prevPtr";
    level->source = "Lab 2 Part 2: DoublyLinkedList and DoublyLinkedNode";
    level->explanation = "In a doubly linked list each node has two pointers: nextPointer to the node after it, and "
                         "prevPointer to the node before it. That means you can walk backwards too. "
                         "When you add a node, set both links.";
    level->example = "class DoublyLinkedNode {\n"
                     "public:\n"
                     "    int data;\n"
                     "    DoublyLinkedNode *prevPointer;\n"
                     "    DoublyLinkedNode *nextPointer;\n"
                     "};";
    level->inGame = "The shot log is a doubly linked list, so now you can scroll back through it with the Up and Down arrows.";

    task = &level->tasks[0];
    task->task = "newNode has just been linked after curNodePtr. Type the line that points newNode back at curNodePtr.";
    task->code = "// InsertAtEnd for the doubly linked list\n"
                 "while (curNodePtr->nextPointer != nullptr) {\n"
                 "    curNodePtr = curNodePtr->nextPointer;\n"
                 "}\n"
                 "curNodePtr->nextPointer = newNode;\n"
                 "____";
    task->answers[0] = "newNode->prevPointer = curNodePtr;";
    task->mistakes[0] = { "curNodePtr->prevPointer = newNode;", "Other way round: the NEW node points back at curNodePtr." };
    task->mistakes[1] = { "newNode->nextPointer = curNodePtr;", "nextPointer goes forwards. Pointing back uses prevPointer." };
    task->mistakes[2] = { "newNode->prevPointer = newNode;", "A node can't be its own previous node. Point it at curNodePtr." };
    task->hint = "newNode's prevPointer should point at curNodePtr.";

    task = &level->tasks[1];
    task->task = "Type the line that moves curNodePtr back one node, so the list prints backwards.";
    task->code = "// print the list backwards, starting at the last node\n"
                 "while (curNodePtr != nullptr) {\n"
                 "    cout << curNodePtr->data << \" \";\n"
                 "    ____\n"
                 "}";
    task->answers[0] = "curNodePtr = curNodePtr->prevPointer;";
    task->mistakes[0] = { "curNodePtr = curNodePtr->nextPointer;", "nextPointer goes forwards. Use prevPointer to go back." };
    task->mistakes[1] = { "curNodePtr--;", "Nodes aren't in an array, so -- doesn't work. Follow prevPointer." };
    task->mistakes[2] = { "curNodePtr->prevPointer = curNodePtr;", "Other way round: curNodePtr gets its previous node." };
    task->hint = "Follow the backwards link: prevPointer.";

    // ---------- Level 28: InsertAtBeginning ----------
    level = getLevel(28);
    level->chapter = 6;
    level->title = "InsertAtBeginning";
    level->shortName = "InsertBeg";
    level->source = "Lab 2 Part 2; Pointers & Linked List lecture: insertAtBeginning";
    level->explanation = "To add a node at the front: point the new node forward at the old head, "
                         "point the old head back at the new node, then make the new node the head.";
    level->example = "// the singly linked version from the lecture:\n"
                     "Node *newNode = new Node(newData);\n"
                     "newNode->next = head;\n"
                     "head = newNode;";
    level->inGame = "Not used by the game, but it's part of Lab 2.";

    task = &level->tasks[0];
    task->task = "Type the line that points the new node forward at the old head.";
    task->code = "void DoublyLinkedList::InsertAtBeginning(int newData) {\n"
                 "    DoublyLinkedNode *newNode = new DoublyLinkedNode(newData);\n"
                 "    ____\n"
                 "    if (head != nullptr) {\n"
                 "        head->prevPointer = newNode;\n"
                 "    }\n"
                 "    head = newNode;\n"
                 "}";
    task->answers[0] = "newNode->nextPointer = head;";
    task->mistakes[0] = { "head->nextPointer = newNode;", "Other way round: the new node points forward at head." };
    task->mistakes[1] = { "newNode = head;", "That loses your new node. Link it: newNode->nextPointer = head;" };
    task->mistakes[2] = { "newNode->prevPointer = head;", "The old head comes AFTER the new node, so use nextPointer." };
    task->hint = "newNode's nextPointer should point at head.";

    task = &level->tasks[1];
    task->task = "Type the line that points the old head back at the new node.";
    task->code = "void DoublyLinkedList::InsertAtBeginning(int newData) {\n"
                 "    DoublyLinkedNode *newNode = new DoublyLinkedNode(newData);\n"
                 "    newNode->nextPointer = head;\n"
                 "    if (head != nullptr) {\n"
                 "        ____\n"
                 "    }\n"
                 "    head = newNode;\n"
                 "}";
    task->answers[0] = "head->prevPointer = newNode;";
    task->mistakes[0] = { "newNode->prevPointer = head;", "The old head points BACK at the new node: head->prevPointer = newNode;" };
    task->mistakes[1] = { "head = newNode;", "That happens on the last line. First link the old head back to newNode." };
    task->mistakes[2] = { "head->nextPointer = newNode;", "Going back uses prevPointer." };
    task->hint = "head's prevPointer should point at newNode.";

    // ---------- Level 29: InsertAtPosition ----------
    level = getLevel(29);
    level->chapter = 6;
    level->title = "InsertAtPosition";
    level->shortName = "InsertPos";
    level->source = "Lab 2 Part 2";
    level->explanation = "To insert in the middle, walk to the node just before the position (curNodePtr). "
                         "The new node goes between curNodePtr and the node after it, so four links change: "
                         "two on the new node, and one on each neighbour.";
    level->example = "// before:  A <-> C\n"
                     "// after:   A <-> B <-> C\n"
                     "// B->prevPointer = A;   B->nextPointer = C;\n"
                     "// A->nextPointer = B;   C->prevPointer = B;";
    level->inGame = "Not used by the game - but with this you've done all of Lab 2. Well done!";

    task = &level->tasks[0];
    task->task = "Type the line that points the new node forward at the node after curNodePtr.";
    task->code = "// curNodePtr is the node just before the position\n"
                 "newNode->prevPointer = curNodePtr;\n"
                 "____\n"
                 "if (curNodePtr->nextPointer != nullptr) {\n"
                 "    curNodePtr->nextPointer->prevPointer = newNode;\n"
                 "}\n"
                 "curNodePtr->nextPointer = newNode;";
    task->answers[0] = "newNode->nextPointer = curNodePtr->nextPointer;";
    task->mistakes[0] = { "newNode->nextPointer = curNodePtr;", "curNodePtr is the node BEFORE. Forward is the node after it: curNodePtr->nextPointer" };
    task->mistakes[1] = { "curNodePtr->nextPointer = newNode;", "That's the last line. Do it now and you lose the node after curNodePtr. First point newNode at it." };
    task->mistakes[2] = { "newNode->nextPointer = newNode;", "A node can't point at itself. Use the node after curNodePtr." };
    task->hint = "The node after curNodePtr is curNodePtr->nextPointer.";

    task = &level->tasks[1];
    task->task = "Type the last link: curNodePtr points forward at the new node.";
    task->code = "// curNodePtr is the node just before the position\n"
                 "newNode->prevPointer = curNodePtr;\n"
                 "newNode->nextPointer = curNodePtr->nextPointer;\n"
                 "if (curNodePtr->nextPointer != nullptr) {\n"
                 "    curNodePtr->nextPointer->prevPointer = newNode;\n"
                 "}\n"
                 "____";
    task->answers[0] = "curNodePtr->nextPointer = newNode;";
    task->mistakes[0] = { "newNode->nextPointer = curNodePtr;", "Forward from curNodePtr to newNode: curNodePtr->nextPointer = newNode;" };
    task->mistakes[1] = { "curNodePtr->prevPointer = newNode;", "newNode comes AFTER curNodePtr, so use nextPointer." };
    task->mistakes[2] = { "curNodePtr = newNode;", "That just moves your pointer. Link the nodes instead." };
    task->hint = "curNodePtr's nextPointer should point at newNode.";
}
