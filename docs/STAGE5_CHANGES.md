# Stage 5: progression

The rules and the questions are unchanged. XP is only added on top: it never changes what unlocks a level.

## New files

| File | What it does |
|---|---|
| `src/progression.h` / `src/progression.cpp` | XP and ranks (`giveXp`, `drawXpBar`, `rankOf`), the combo meter (`drawComboMeter`), the unlock card (`showUnlockCard`, `updateUnlockCard`, `drawUnlockCard`) and the skill tree (`drawSkillTree`) |

## How XP works
- A right answer: 20 + 10 per star (50 XP for 3 stars). In practice: 10 XP.
- Each of your own balls potted: 10 XP, plus 5 XP for each ball of a combo after the first.
- Unlocking a level: 100 XP. Winning a game: 150 XP.
- Rank 1 needs 200 XP, rank 2 needs 300, rank 3 needs 400... (`xpForRank`). The ranks have names, from
  Null Pointer to Pool Legend.
- The bar fills smoothly; when it passes the end of a rank it flashes, "RANK UP!" pops up with confetti and a fanfare.

## Changes to existing files

**Makefile**: `SRC` gains `src/progression.cpp`, `HEADERS` gains `src/progression.h`.

**src/questions.h / src/questions.cpp**
- `Profile` gains `int xp`. `setupProfile` sets it to 0.
- `saveProfile` writes it on a third line of `progress_Name.txt`; `readProgressFile` reads it if it's there, so saves
  from before XP still load (with 0 XP), and the old version of the game would just ignore the extra line.

**src/play.cpp**
- `completeLevel()` gives 100 XP and shows the **unlock card** instead of the old banner: the screen dims, a card
  flips over (squashing its width by cos(angle) makes it look like it turns), shows a glowing ball with the new
  level's number, and the text types itself out with a tick. Click (or Enter/Space) to skip the typing, then to close.
  The game waits while it's open.
- `runCode()` gives XP for a right answer; `finishShot()` gives XP for your own pots (and the combo bonus) and for a win.
- `playScreen()` draws the **combo meter** in the top-left of the table; `drawTopBar()` draws the shooter's **XP bar**.
- The old `drawBanner` was removed (and the `banner...` fields in `App`).

**src/main.cpp**
- The main loop calls `updateUnlockCard(dt)` and `drawUnlockCard()` (instead of the banner).
- The Progress screen shows the XP bar and the **skill tree**: every level is a node joined to the next, finished paths
  glow green with light flowing along them, the level you're on pulses, the upgrades are diamonds at the end of their
  chapters, and hovering a node shows its title. Clicking a node still opens it in Lessons.
- "Reset" also resets the player's XP.
