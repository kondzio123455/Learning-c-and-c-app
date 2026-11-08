#ifndef EFFECTS_H
#define EFFECTS_H

#include "raylib.h"

// ---------- Particles: sparks, confetti and dust ----------
void spawnSparks(float x, float y, Color colour, int count, float speed);
void spawnConfetti(float x, float y, int count);
void updateParticles(float dt);
void drawParticles();

// ---------- Floating text that rises and fades ("Correct!", "Foul!") ----------
void spawnFloatingText(const char *text, float x, float y, Color colour);
void updateFloatingTexts(float dt);
void drawFloatingTexts();

// ---------- Screen shake (for big breaks) ----------
void shakeScreen(float amount);
Vector2 updateShake(float dt);       // returns how far to move the table this frame

// ---------- Sound effects, made in code so no sound files are needed ----------
enum SoundName { SOUND_CUE, SOUND_CLICK, SOUND_CUSHION, SOUND_POCKET,
                 SOUND_CORRECT, SOUND_WRONG, SOUND_UNLOCK, SOUND_COUNT };
void loadSounds();
void unloadSounds();
void playSound(SoundName name, float volume);

#endif
