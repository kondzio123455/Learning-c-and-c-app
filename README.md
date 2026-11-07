# Pointer Pool

An 8-ball pool game for learning C and C++ pointers, built on the Lab 2
(Coding Exam 2) questions. Each level shows some code with a blank.
Pick an answer and watch what your code does to the table.

## Build and run (macOS)

```bash
brew install raylib pkg-config
make run
```

## Files

| File | What it does |
|---|---|
| `src/main.cpp` | Window, game loop, switching between the level and playing |
| `src/table.cpp` | The pool table: drawing, the rack, physics (game code) |
| `src/level.cpp` | The levels: the code shown, the answers, what each answer does |
| `src/lab2.cpp` | The Lab 2 functions (learning code) |

## Levels

1. `swap_ints` - put the 8-ball in the middle of the rack
