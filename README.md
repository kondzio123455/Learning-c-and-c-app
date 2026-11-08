# Pointer Pool

A two-player 8-ball pool game for studying C and C++ pointers. Every question
comes from the lecture notes (Pointers, Memory Management, Classes, Pointers &
Linked Lists, Operator Overloading) and Labs 1 and 2. Inspired by *The Farmer Was
Replaced*: you unlock one concept at a time, write the code yourself, and the game
runs it.

## Build and run (macOS)

```bash
brew install raylib pkg-config
make run
```

Run it from this folder, because the font is loaded from `assets/`.

If you played an older version, open **Progress** and click **Reset progress**
once: the levels have been renumbered.

## How to play

- Two players on one computer. Before every shot you answer the current level's
  question, then aim with the mouse, hold the left button to charge, and let go.
- From Level 3 you **call your ball** after every 2 of your own balls (and when
  you go for the 8), in a second box: `Ball *target = &balls[3];`. From Level 13
  `[ ]` is locked (the Lab 2 rule), so it becomes `Ball *target = balls + 3;`.
- Pot the ball you called (or, on shots with no call, any of yours) and you shoot
  again. **Pot one of your own balls to unlock the next concept.**
- A dotted line shows where the cue ball will go. Finishing chapters unlocks
  upgrades: the ghost ball (Chapter 1), the ball path (Chapter 3) and the golden
  cue (Chapter 7).

## Studying for the test

- **Every task is from a specific slide or lab question**, using the lecturer's
  own code and names. Each lesson says where it comes from.
- **Four kinds of task:** type the missing code, trace what a variable holds
  (like the data tables on the slides), find the line that's wrong, and put lines
  in the right order.
- **No answers are given away.** A wrong answer explains what went wrong, not the
  fix. Hints appear only when you click Hint: three, each more specific, then a
  last one that shows the layout of the answer with blanks (or a part-done trace).
- **Stars:** 3 for solving without hints, 2 with one hint, 1 with more. Your best
  is kept.
- **The Slide button** shows what the slides (or lab sheet) actually say for that
  level, so you don't have to open the PDF. (Where a slide's data table shows the
  final answers of a trace, an earlier table is shown instead.)
- **New questions in the same style.** Every level can make endless new questions
  from the same slide or lab code, with new numbers, names or words. If you miss
  your shot, the level's two tasks come first, then a new one each time, with a
  learner note on top - never the same question twice in a row. Review mode (all
  levels done) and practice mix them in too.
- **Lessons & practice** (main menu): re-read any concept you've reached and
  practise it without playing pool: both versions, then new questions for as long
  as you like, or Next level.
- **Unlock all (revision)** on the Progress screen opens every lesson straight away.

## Controls

| Key | What it does |
|---|---|
| Ctrl/Cmd + Enter | Run your code (or click Run) |
| Arrow keys | Move the cursor in the code box (click to put it anywhere) |
| Cmd/Ctrl + Left/Right, Home, End | Jump to the start or end of the line |
| Backspace / Delete | Delete before / after the cursor |
| Tab | Switch between the answer box and the call box |
| Mouse | Aim; hold the left button to charge, let go to shoot |
| Up / Down | Scroll the shot log (from Level 44) |
| Esc | Back to the menu |
| F5 | Unlock every level (for demos) |

## How answers are checked

Typed code is never compiled. Instead:

1. Spaces, tabs and new lines are removed, so `* b = temp ;` and `*b=temp;` count
   as the same answer.
2. The answer is compared with each accepted answer (spaces removed too) using
   Lab 2's `compare_strings`. A `#` in an accepted answer means "any whole number",
   which is how the call line reads your ball's number.
3. If it doesn't match, a common mistake gets its own explanation; a missing `;`
   or `{` gets a "Nearly!"; anything else gets a general nudge for that kind of task.
4. In the Lab 2 chapters, any `[ ]` with something inside gets the "no indexing" message.

When an answer is right, the game runs the real version of that code from
`lab1.cpp` / `lab2.cpp`, so you see it working in the game.

All of this was checked before release: every accepted code answer compiles as
real C++ in its code, and every trace answer was confirmed by running the code.
The made-up questions work out their answers by running the same code (or the real
lab function). As a check, over a thousand of them were turned into real programs,
compiled and run, and every one printed exactly the answer the game expects.

## The 54 levels

| Chapter | Levels |
|---|---|
| 1. Pointer basics (Pointers lecture) | 1 `&`, 2 `*`, 3 call your ball, 4 tracing a pointer, 5 pass by value and by pointer, 6 pointer to a pointer, 7 `nullptr`, 8 reference vs pointer |
| 2. Lab 2: swaps | 9 swap_ints, 10 swap_chars |
| 3. Pointer arithmetic (lecture and Lab 2) | 11 `ptr + 1`, 12 tracing `ptr++`, 13 steps and `sizeof`, 14 printing with a pointer, 15 sum_array, 16 find_max, 17 reverse_array |
| 4. Lab 2: strings | 18 strlen_ptr, 19 count_vowels, 20 copy_string, 21 concat_strings, 22 compare_strings |
| 5. Memory management | 23 scope and dangling pointers, 24 `new` and `delete`, 25 memory leaks, 26 memory mistakes, 27 `new[]`, 28 2D arrays with `int**`, 29 `malloc` and `free`, 30 function pointers, 31 global variables |
| 6. Pointers and classes | 32 passing objects, 33 pointer to an object, 34 `new Point()`, 35 `this`, 36 copies and copy constructors, 37 arrays of object pointers, 38 destructors |
| 7. Linked lists (lecture and Lab 2 Part 2) | 39 nodes, 40 PrintList, 41 InsertAtEnd and `Node**`, 42 DeleteWithValue, 43 `clear()`, 44 prevPointer, 45 InsertAtBeginning, 46 InsertAtPosition |
| 8. Lab 1 review | 47 frequencyCount, 48 countDuplicates, 49 reverse, 50 rotateLeft, 51 twoMovies, 52 wordCounter, 53 PrintArray, 54 grid clusters (Q20-Q21) |

## Notes on the course material

- **Lab 2 Q6:** the sheet says `count_vowels("OpenAI")` is 3 ('O', 'e', 'A'), but
  the final 'I' is a vowel too, so the answer is 4. Level 19 points this out.
- **Linked list lecture:** the first `Node` calls its pointer `next`, while the full
  program calls it `nextPtr`. The game follows the full program and mentions both.
- **Lab answers:** the labs give prototypes and descriptions, not solutions. The
  answers here are standard correct solutions (compiled and tested), not an
  official answer sheet. Lab 1 Q20-Q21 only describe what to do, so Level 54 shows
  one correct way. Lab 1 Q21 (FindAllVariables) and Q22 (scope) aren't covered.
- **malloc:** the slides cast with `(int*)`. That's optional in C but needed in C++,
  so Level 29 accepts both.

## Files

| File | What it does |
|---|---|
| `src/main.cpp` | The window, main menu, lessons screen and progress tree |
| `src/play.cpp` | The game screen and practice: questions, hints, stars, shooting, banners |
| `src/pool.cpp` | Table, balls, physics, 8-ball rules and the aim guide |
| `src/effects.cpp` | Particles, floating text, screen shake, and sounds made in code |
| `src/questions.cpp` | The answer checker, layout hints, and saving progress to `progress.txt` |
| `src/practice.cpp` | Makes new questions in the lecturer's style for every level |
| `src/lessons_pointers.cpp` | Chapters 1-4: the Pointers lecture and Lab 2 Part 1 |
| `src/lessons_memory.cpp` | Chapters 5-6: Memory Management and Classes |
| `src/lessons_lists.cpp` | Chapters 7-8: linked lists and the Lab 1 review |
| `src/editor.cpp` | The code box: typing, cursor and syntax colouring |
| `src/ui.cpp` | Fonts, buttons, stars and pieces of the game screen |
| `src/lab1.cpp` / `src/lab2.cpp` | The lab functions the game really uses |
| `src/app.h` | The `App` struct that holds the whole app's state |

## Changing things

- **Edit or add a lesson:** the `lessons_*.cpp` files. Each level is one block,
  including what its Slide button shows.
- **Change the made-up questions:** `makeLevelN` in `src/practice.cpp`.
- **Unlock a concept after every correct answer instead of after potting:** set
  `UNLOCK_BY_POTTING` to `false` in `src/app.h`.
- **Start again:** Progress, then Reset progress (or delete `progress.txt`).

## Credits

Font: JetBrains Mono, under the SIL Open Font License (see `assets/OFL.txt`).
Sounds are generated in code, so there are no sound files.
