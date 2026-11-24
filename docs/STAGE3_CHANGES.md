# Stage 3: game feel and sounds

The physics and rules are unchanged (checked: the same shots end with every ball in exactly the same place).
`pool.cpp` now only says WHAT happened on the table; `fx.cpp` decides how it looks and sounds.

## New files (effects.cpp / effects.h were split into these)

| File | What it does |
|---|---|
| `src/fx.h` / `src/fx.cpp` | 3D particles (chalk dust on the strike, sparks on hard hits, a burst of light on a pot), 2D confetti and fireworks, popups that pop in ("NICE SHOT!", "COMBO x2", "+1"), the combo count, camera shake and hit-stop |
| `src/sound.h` / `src/sound.cpp` | Every sound, made in code by adding tones and noise. Each has 4 "voices" so it can overlap itself (the break has lots of clicks at once), a slightly different pitch each time, and can be replaced by a file in `assets/sounds/` |
| `assets/sounds/README.txt` | Which sound files you can add (optional) and where to get free ones |

## Changes to existing files

**Makefile**: `src/effects.cpp` became `src/fx.cpp src/sound.cpp` (and the headers the same).

**src/pool.cpp** (only the effect calls changed, not the physics)
- A cushion bounce calls `fxCushionHit(speed)`, a pot calls `fxBallPotted(game, number, pocket)`, two balls hitting
  call `fxBallsHit(x, y, speed)`. `#include "effects.h"` became `#include "fx.h"`.

**src/play.cpp**
- The strike calls `fxStrike(game, power)`: sound, chalk dust, shake, and hit-stop on hard shots and the break.
- New `stepBalls()` replaces the plain `updateBalls(game, dt)` while the balls roll:
  - during a **hit-stop** the balls don't move for a few frames (a big hit feels heavy);
  - when the 8 is rolling into a pocket, **slow motion**: time runs at a quarter speed, but the physics still moves in
    normal 1/60 s steps (so nothing about the shot changes) and the 3D view slides the balls between steps;
    the camera swings round the 8 and black film bars slide in;
  - otherwise it calls `updateBalls(game, dt)` exactly as before.
- A finished game launches fireworks (`fxWin()`); the winner's name uses the glowing title font.
- The 2D table shake was removed: the shake now moves the 3D camera instead.

**src/render3d.cpp** (and `render3d.h`)
- `shakenCamera()`: the camera nudged by the shake (never saved, so it can't drift).
- Glowing trails behind fast balls (`updateTrails`, `drawTrails`): a flat ribbon in the ball's colour.
- `setBallBlend` / `stopBallBlend` (smooth slow motion), `setCinematicCamera` (the swing round the 8), film bars.
- `tableToWorld()` for fx.cpp, and `drawParticles3D()` is called inside the 3D scene.

**src/ui.cpp**: buttons tick softly when the mouse arrives on them, and click when pressed.

**src/main.cpp**: `updateShake(dt)` and `updateHitStop(dt)` run once a frame with the particles.

**src/exam.cpp, src/testmode.cpp**: include `fx.h` and `sound.h` instead of `effects.h`.

## Sounds
No files are needed: every sound is generated. To use recordings, add files named as in `assets/sounds/README.txt`
(Kenney's "Interface Sounds" and "Impact Sounds" packs, or CC0 sounds on freesound.org).
