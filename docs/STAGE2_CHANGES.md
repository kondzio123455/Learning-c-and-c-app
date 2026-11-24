# Stage 2: lighting, graphics and theme

Nothing in the rules, questions, physics or saving changed.

## New files

| File | What it does |
|---|---|
| `src/theme.h` | Every colour (moved here from `ui.h`), the fonts, corner sizes and the fade time |
| `shaders/lighting.vs` | Works out where each point is in the world and which way it faces |
| `shaders/lighting.fs` | A long pool-table lamp with three warm bulbs: diffuse light + shiny highlights (Blinn-Phong, like raylib's lighting example), light only in a cone under the shade, fog in the distance |
| `shaders/post.fs` | Bloom (bright parts glow) and a vignette (darker corners), run on the finished 3D picture |
| `assets/Inter-ExtraBold.ttf`, `assets/Inter-SemiBold.ttf`, `assets/Inter-OFL.txt` | The Inter font for titles and buttons (free, SIL Open Font License) |

## Changes to existing files

**Makefile**: `HEADERS` gains `src/theme.h`.

**src/ui.h**
- The `COLOUR_...` constants moved to `theme.h` (ui.h includes it, so every file still sees them).
- New: `drawTitle`, `titleWidth`, `drawGlowTitle`, `drawBackground`, `drawBar`, `drawPanel`, `drawPopupBox`,
  `drawPanelCover`, `drawSoftShadow`, `roundnessFor`.

**src/ui.cpp**
- `loadFonts()` also loads Inter (titles) and Inter SemiBold (buttons).
- New panel functions: rounded corners, a soft shadow (a few see-through boxes, each bigger), a faint edge.
- `button()` now glows brass and grows a little when the mouse is over it. Each button remembers how lit up it is
  (`glowFor`), so it fades smoothly with `smoothTowards`.

**src/render3d.cpp** (and `render3d.h`)
- The lighting shader lights the table, balls and cue. `setShine()` changes how shiny the next things are
  (felt and cushions matte, wood a little, balls very).
- The cue is now five lit tube meshes (`makeTubeMesh`) instead of flat-coloured cylinders.
- Soft shadows under the table and under every ball (`drawShadows`).
- The hanging lamp (`drawLampShade`, `drawLampGlow`). It's hidden when the camera is high up, so it can't block the table.
- `drawWithPost()` puts the 3D picture on the screen through `post.fs`.
- New `renderMenu3D()` / `drawMenuView()`: the menu's table, seen from a camera circling slowly round it. Its balls
  are hit every couple of seconds using the game's own `startShot` / `updateBalls` (in a separate `Game`).
- Ball spin is kept per table (`Spin`), so the menu's balls and the game's balls don't mix.

**src/pool.h / src/pool.cpp**
- `bool tableEffects`: when false, `updateBalls` makes no sounds, sparks or shake (used for the menu's table).
  The physics is unchanged (checked: the same shots end with every ball in exactly the same place).

**src/anim.h / src/anim.cpp**: new `easeOutBounce` (for the title).

**src/main.cpp**
- The 2D drifting menu balls were replaced: `menuScreen()` draws the 3D table behind the menu, darkened on the left.
- `drawMenuTitle()`: each letter of POINTER POOL drops in and bounces, one after another, then bobs gently, with a glow.
- In `main()`: `renderMenu3D()` before `BeginDrawing()` on the menu, and every new screen fades in from black
  (`fadeTimer`, `SCREEN_FADE_TIME` in theme.h).
- Panels, the player picker and headings use the new panel functions and Inter.

**src/play.cpp, src/exam.cpp, src/testmode.cpp**
- Flat `DrawRectangle(..., COLOUR_PANEL)` panels became `drawPanel(...)`, bars became `drawBar(...)`, windows over
  the screen became `drawPopupBox(...)`, `ClearBackground` became `drawBackground()`, and big headings use `drawTitle`.
  The play screen's right panel is `SIDE_PANEL`.

## Font
**Inter** by Rasmus Andersson (https://github.com/rsms/inter), SIL Open Font License. JetBrains Mono stays for normal
text and code, because the code box and lessons rely on every character being the same width.
