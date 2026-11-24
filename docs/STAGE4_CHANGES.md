# Stage 4: code editor and memory panel

Nothing about checking answers, stars, hints or the rules changed; this stage is only how they look.

## New files

| File | What it does |
|---|---|
| `src/memory.h` / `src/memory.cpp` | The memory panel on the shot panel. `power`, `powerPtr` and `target` are boxes with their value and address (made-up but consistent: 2000, 2008, 2016). `powerPtr`'s arrow points at `power`'s box. Values count up or down to new values. While you charge, the line `*powerPtr = 45;` lights up, the arrow glows and a bright dot carries the value to `power` |

## Changes to existing files

**Makefile**: `SRC` gains `src/memory.cpp`, `HEADERS` gains `src/memory.h`.

**src/editor.h**: `CodeBox` gains look-only fields: `caretX/caretY/caretPlaced` (the gliding cursor),
`lastChange` (solid while typing), `runBarY` (the gliding Run bar), `error` (the red wavy line). New `drawRunningBar()`.

**src/editor.cpp**
- `updateCodeBox()` notes when the text or cursor changes (typing clears the red line).
- `drawCodeBox()` now looks like a code editor: a gutter with line numbers (for boxes with more than one line), a faint
  band on the line you're typing on, a rounded box that glows while you type in it, a cursor that glides to where it
  should be and pulses gently, a glowing green bar that glides from line to line while the code runs, and a red wavy
  underline after a wrong answer (until you change it).

**src/app.h** / **src/main.cpp**: `App` gains `correctTimer`, `hintTimer` and `answerArea` (set up in `setupApp`).

**src/play.cpp**
- Right answer: a band of green light sweeps across the answer, green sparkles along its edge, then the code runs
  (the glowing bar) and "SHOT UNLOCKED!" pops up over the table.
- Wrong answer: the box shakes (as before), is underlined in red, and the explanation bounces in.
  A new hint or clue also bounces in (`bounceIn`).
- The shot panel's Name / Type / Value rows became the memory panel (`drawMemoryPanel`).
- The line-picking (ORDER) box uses the same glowing Run bar; code backgrounds are rounded (`drawCodeBackground`).

**src/ui.cpp / ui.h**: the power bar fills smoothly and its end glows while the value changes; new `drawCodeBackground`.

**src/fx.cpp / fx.h**: new `spawnSparkle` (tiny glowing sparks, for the right-answer sparkles).
