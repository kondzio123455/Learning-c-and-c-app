# Pointer Pool

A two-player 8-ball pool game that teaches C and C++ pointers, built on the
Pointers lectures and Labs 1 and 2 (Coding Exams 1 and 2). Inspired by
*The Farmer Was Replaced*: you learn one small idea at a time, write the code
yourself, and the game runs it.

## Build and run (macOS)

```bash
brew install raylib pkg-config
make run
```

Run it from this folder, because the font is loaded from `assets/`.

## How to play

- Before every shot you get a coding question. Type the missing code in the box,
  then click **Run** (or press Ctrl/Cmd + Enter).
- From Level 3 you also **call your ball** in the second box, for example
  `Ball *target = &balls[3];`. From Level 14, `[ ]` is locked (the Lab 2 rule),
  so it becomes `Ball *target = balls + 3;`.
- Aim with the mouse, hold the left button to charge the power, and let go to shoot.
- **Pot one of your balls to unlock the next concept**, like spending crops on
  unlocks in The Farmer Was Replaced. Until then you practise the current one
  (each level has two versions of its task).
- **Esc** goes back to the menu. **Tab** switches between the two code boxes.
  **Up/Down** scroll the shot log (from Level 27). **F5** unlocks every level,
  which is handy for demos.

## Rules (simple 8-ball)

- Two players on one computer take turns.
- The table is open until the first ball is potted. That ball's group
  (solids 1-7 or stripes 9-15) goes to the player who potted it.
- Pot the ball you called and you shoot again. Otherwise the turn passes.
  (Before calling is unlocked: pot any of your own balls to shoot again.)
- Potting the cue ball is a foul: the turn passes and the cue ball goes back
  on the start line.
- Pot the 8 after clearing your group to win. Potting it early, or potting the
  cue ball on the same shot, loses.
- To keep the code simple, hitting the other group's ball first isn't a foul,
  and the 8 going down on the break is just put back on its spot.

## How answers are checked

Typed code is never compiled. Instead:

1. Spaces, tabs and new lines are removed, so `* b = temp ;` and `*b=temp;` count
   as the same answer.
2. The answer is compared with each accepted answer (spaces removed too) using
   Lab 2's `compare_strings`. A `#` in an accepted answer means "any whole number",
   which is how the call line reads the ball number.
3. If it doesn't match: common mistakes get their own hint (for example
   `*b = *a;` in swap_ints), a missing `;` or `{` gets a "Nearly!" hint, and
   anything else gets the task's general hint. You can try again as many times
   as you like.
4. From Level 14, any `[ ]` with something inside gets the Lab 2 "no indexing" hint.

When an answer is right, the game runs the real version of that code from
`lab1.cpp` / `lab2.cpp`, so you can see it working in the game.

## The 29 levels

| Chapter | Levels |
|---|---|
| 1. Pointer basics (Pointers lectures) | 1 `&`, 2 `*`, 3 call your ball, 4 pass by pointer, 5 `nullptr` |
| 2. Swaps (Lab 2 Q1-Q2) | 6 swap_ints, 7 swap_chars |
| 3. Arrays (Lab 1 Q13-Q18) | 8 frequencyCount, 9 countDuplicates, 10 twoMovies, 11 reverse, 12 rotateLeft, 13 wordCounter |
| 4. Pointer arithmetic (Lab 2, no `[ ]`) | 14 `*(arr + i)`, 15 sum_array, 16 find_max, 17 reverse_array |
| 5. Strings with pointers (Lab 2) | 18 strlen_ptr, 19 count_vowels, 20 copy_string, 21 concat_strings, 22 compare_strings |
| 6. Memory and linked lists | 23 new/delete, 24 `->`, 25 walking a list, 26 InsertAtEnd (`Node **`), 27 prevPointer, 28 InsertAtBeginning, 29 InsertAtPosition |

## Files

| File | What it does |
|---|---|
| `src/main.cpp` | The window, the main menu, the codex and the progress tree |
| `src/play.cpp` | The game screen: lesson, code, run, aim, roll, then the next shot |
| `src/pool.cpp` | Table, balls, physics and the 8-ball rules |
| `src/levels.cpp` | All 29 lessons: explanation, example, two tasks, answers and hints |
| `src/questions.cpp` | The answer checker, and saving progress to `progress.txt` |
| `src/editor.cpp` | The code box: typing, cursor and syntax colouring |
| `src/ui.cpp` | Fonts, buttons and pieces of the game screen |
| `src/lab1.cpp` | Lab 1 Q13-Q18, as used by the game |
| `src/lab2.cpp` | All of Lab 2, including the doubly linked list used for the shot log |
| `src/app.h` | The `App` struct that holds the whole app's state |

## Changing things

- **Edit or add a lesson:** `src/levels.cpp`.
- **Unlock a concept after every correct answer instead of after potting:** set
  `UNLOCK_BY_POTTING` to `false` in `src/app.h`.
- **Start again:** Progress, then Reset progress (or delete `progress.txt`).

## A note on the Lab 2 sheet

The sheet says `count_vowels("OpenAI")` is 3 ('O', 'e', 'A'), but the final 'I'
is a vowel too, so the answer is 4. The game's lesson points this out.

## Credits

Font: JetBrains Mono, under the SIL Open Font License (see `assets/OFL.txt`).
