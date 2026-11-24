#ifndef SOUND_H
#define SOUND_H

#include "raylib.h"

// ============================================================
// Sound effects. Every sound is made in code, so the game needs no
// sound files. If you want real recordings instead, put a .wav (or
// .ogg) file with the sound's name in assets/sounds/ and it's used
// instead, e.g. assets/sounds/click.wav.
// ============================================================

enum SoundName {
    SOUND_CUE,        // the cue hits the cue ball       (strike)
    SOUND_CLICK,      // two balls hit                    (click)
    SOUND_CUSHION,    // a ball hits a cushion            (cushion)
    SOUND_POCKET,     // a ball drops into a pocket       (pocket)
    SOUND_CORRECT,    // a right answer                   (correct)
    SOUND_WRONG,      // a wrong answer                   (wrong)
    SOUND_UNLOCK,     // a new level or upgrade           (unlock)
    SOUND_HOVER,      // the mouse moves onto a button    (hover)
    SOUND_BUTTON,     // a button is clicked              (button)
    SOUND_COMBO,      // a combo goes up                  (combo)
    SOUND_FIREWORK,   // a firework bursts                (firework)
    SOUND_LEVEL_UP,   // the XP bar fills up              (levelup)
    SOUND_TYPE,       // one letter typed out             (type)
    SOUND_COUNT
};

void loadSounds();
void unloadSounds();
void playSound(SoundName name, float volume);                          // volume 0 to 1
void playSoundPitched(SoundName name, float volume, float pitch);      // pitch 1 = normal, 2 = an octave up

#endif
