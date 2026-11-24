# Stage 1: 3D table and camera

The physics and the 8-ball rules are unchanged: `pool.cpp` still moves the balls in 2D table pixels.
Only the drawing of the table changed. Each frame, the 2D ball positions are drawn as 3D spheres
(100 table pixels = 1 unit in 3D).

## New files

| File | What it does |
|---|---|
| `src/render3d.h` / `src/render3d.cpp` | The 3D table, balls, cue, aim guide, called-ball ring and the camera. It draws into a render texture, which `playScreen` puts on the screen |
| `src/anim.h` / `src/anim.cpp` | Easing helpers (`easeOutCubic`, `easeOutBack`, ...) and `smoothTowards`, which glides a value or a camera position at the same speed at any frame rate |

## Changes to existing files

**Makefile**
- `SRC` gains `src/render3d.cpp src/anim.cpp`; `HEADERS` gains `src/render3d.h src/anim.h`.

**src/main.cpp**
- `#include "render3d.h"`.
- In `main()`: `setupRender3D();` after `setupTableTextures();`, and `unloadRender3D();` before `unloadTableTextures();`.
- In the game loop, just before `BeginDrawing()`:
  `if (app.screen == SCREEN_PLAY) renderTable3D(&app, GetFrameTime());`
  (3D must be drawn before the scaled 2D frame starts).

**src/play.cpp**
- `#include "render3d.h"`.
- `updateAimPhase()` was rewritten. In top-down view the aim follows the mouse on the table. In the 3D views the
  mouse's left/right movement (or A/D, arrow keys; Shift is slow) turns the aim angle. The aim no longer
  moves while you're charging.
- `drawUnderTable()`: the arrow from the memory row to the called ball ends at `tableToScreen(target->x, target->y, 0)`.
- `drawShotPanel()`: the aim instructions depend on `isTopDownView()`.
- `playScreen()`: the 2D `drawTable / drawAimGuide / drawBalls / drawTargetRing / drawCueStick` block is replaced by
  `drawTableView();`. The T key calls `toggleTopDownView()` (not while typing code).

**src/pool.h / src/pool.cpp**
- New `aimContact()`: the "what does the aim line hit first" maths, taken out of `drawAimGuide()` so the 3D aim guide can
  use it too. `drawAimGuide()` now calls it. The result is the same.
- The pocket and collision sparks use `tableToScreen()` so they appear where the 3D ball is.

**src/ui.h / src/ui.cpp**
- New `getViewScale()` (the 3D image is made at the window's real resolution, so it stays sharp).
- `drawBallTooltip()` uses `ballUnderMouse()` (a ray from the mouse into the 3D scene) and shows the box by the mouse.

**README.md**: the new controls and files.

## Cameras
- **Aiming:** low, behind the cue ball, looking along the shot.
- **Balls moving:** an angled view of the whole table.
- **T:** straight down (toggle). The camera always glides to the new view.
