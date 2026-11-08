#include "effects.h"
#include "ui.h"
#include <cmath>
#include <cstdio>

// ============================================================
// Particles
// ============================================================

struct Particle {
    float x, y;
    float vx, vy;
    float life;        // seconds left
    float maxLife;
    float size;
    float gravity;     // confetti falls, sparks don't
    Color colour;
};

const int MAX_PARTICLES = 400;
Particle particles[MAX_PARTICLES];

// Finds a free slot (life <= 0), or -1 if they're all busy
int freeParticle() {
    for (int i = 0; i < MAX_PARTICLES; i++) {
        if (particles[i].life <= 0) return i;
    }
    return -1;
}

float randomFloat(float low, float high) {
    return low + (high - low) * GetRandomValue(0, 1000) / 1000.0f;
}

void spawnSparks(float x, float y, Color colour, int count, float speed) {
    for (int n = 0; n < count; n++) {
        int i = freeParticle();
        if (i == -1) return;
        float angle = randomFloat(0, 6.283f);
        float s = randomFloat(0.3f, 1.0f) * speed;
        particles[i] = Particle{ x, y, cosf(angle) * s, sinf(angle) * s, 0.35f, 0.35f, randomFloat(1.5f, 3), 0, colour };
    }
}

void spawnConfetti(float x, float y, int count) {
    Color colours[5] = { COLOUR_ACCENT, COLOUR_GOOD, { 90, 160, 255, 255 }, { 255, 110, 150, 255 }, WHITE };
    for (int n = 0; n < count; n++) {
        int i = freeParticle();
        if (i == -1) return;
        float angle = randomFloat(-2.6f, -0.5f);   // mostly upwards
        float s = randomFloat(150, 420);
        particles[i] = Particle{ x, y, cosf(angle) * s, sinf(angle) * s, 1.4f, 1.4f, randomFloat(2.5f, 4.5f), 500,
                                 colours[GetRandomValue(0, 4)] };
    }
}

void updateParticles(float dt) {
    for (int i = 0; i < MAX_PARTICLES; i++) {
        Particle *p = &particles[i];
        if (p->life > 0) {
            p->x += p->vx * dt;
            p->y += p->vy * dt;
            p->vy += p->gravity * dt;
            p->vx *= 0.98f;
            p->life -= dt;
        }
    }
}

void drawParticles() {
    for (int i = 0; i < MAX_PARTICLES; i++) {
        Particle *p = &particles[i];
        if (p->life > 0) {
            float fade = p->life / p->maxLife;
            DrawRectangle((int)(p->x - p->size / 2), (int)(p->y - p->size / 2), (int)p->size, (int)p->size,
                          Fade(p->colour, fade));
        }
    }
}

// ============================================================
// Floating text
// ============================================================

struct FloatingText {
    char text[40];
    float x, y;
    float life;
    Color colour;
};

const int MAX_TEXTS = 8;
FloatingText texts[MAX_TEXTS];

void spawnFloatingText(const char *text, float x, float y, Color colour) {
    for (int i = 0; i < MAX_TEXTS; i++) {
        if (texts[i].life <= 0) {
            snprintf(texts[i].text, sizeof(texts[i].text), "%s", text);
            texts[i].x = x;
            texts[i].y = y;
            texts[i].life = 1.4f;
            texts[i].colour = colour;
            return;
        }
    }
}

void updateFloatingTexts(float dt) {
    for (int i = 0; i < MAX_TEXTS; i++) {
        if (texts[i].life > 0) {
            texts[i].y -= 40 * dt;   // drift upwards
            texts[i].life -= dt;
        }
    }
}

void drawFloatingTexts() {
    for (int i = 0; i < MAX_TEXTS; i++) {
        if (texts[i].life > 0) {
            float fade = texts[i].life;
            if (fade > 1) fade = 1;
            float size = 28;
            float width = textWidth(texts[i].text, size);
            drawBold(texts[i].text, texts[i].x - width / 2 + 2, texts[i].y + 2, size, Fade(BLACK, fade * 0.6f));
            drawBold(texts[i].text, texts[i].x - width / 2, texts[i].y, size, Fade(texts[i].colour, fade));
        }
    }
}

// ============================================================
// Screen shake
// ============================================================

float shakeAmount = 0;

void shakeScreen(float amount) {
    if (amount > shakeAmount) shakeAmount = amount;
}

Vector2 updateShake(float dt) {
    Vector2 offset = { 0, 0 };
    if (shakeAmount > 0.1f) {
        offset.x = randomFloat(-shakeAmount, shakeAmount);
        offset.y = randomFloat(-shakeAmount, shakeAmount);
        shakeAmount -= shakeAmount * 6 * dt;   // dies away quickly
    } else {
        shakeAmount = 0;
    }
    return offset;
}

// ============================================================
// Sounds: each one is a short wave built from sine waves and noise
// ============================================================

Sound sounds[SOUND_COUNT];
bool soundsLoaded = false;
const int SAMPLE_RATE = 22050;

// Makes one sound. tone1/tone2 are pitches in Hz (the pitch slides from one to the
// other), noise is how much hiss to mix in, decay is how fast it fades out.
Sound makeSound(float seconds, float tone1, float tone2, float noise, float decay) {
    int frames = (int)(seconds * SAMPLE_RATE);
    short *samples = (short *)MemAlloc(frames * sizeof(short));
    float phase = 0;

    for (int i = 0; i < frames; i++) {
        float t = (float)i / frames;                       // 0 to 1 through the sound
        float pitch = tone1 + (tone2 - tone1) * t;
        phase += 6.283f * pitch / SAMPLE_RATE;
        float value = sinf(phase) * (1 - noise) + randomFloat(-1, 1) * noise;
        float envelope = expf(-decay * t);                 // fade out
        if (i < 40) envelope *= i / 40.0f;                 // tiny fade in, so it doesn't pop
        samples[i] = (short)(value * envelope * 12000);
    }

    Wave wave = { (unsigned int)frames, (unsigned int)SAMPLE_RATE, 16, 1, samples };
    Sound sound = LoadSoundFromWave(wave);
    UnloadWave(wave);
    return sound;
}

void loadSounds() {
    InitAudioDevice();
    if (!IsAudioDeviceReady()) return;   // no speakers: the game still works, silently

    sounds[SOUND_CUE] = makeSound(0.08f, 240, 120, 0.5f, 9);
    sounds[SOUND_CLICK] = makeSound(0.05f, 2200, 1800, 0.35f, 10);
    sounds[SOUND_CUSHION] = makeSound(0.09f, 140, 90, 0.4f, 8);
    sounds[SOUND_POCKET] = makeSound(0.22f, 160, 60, 0.5f, 6);
    sounds[SOUND_CORRECT] = makeSound(0.35f, 880, 1320, 0.0f, 4);
    sounds[SOUND_WRONG] = makeSound(0.25f, 180, 120, 0.15f, 5);
    sounds[SOUND_UNLOCK] = makeSound(0.7f, 523, 1046, 0.0f, 3);
    soundsLoaded = true;
}

void unloadSounds() {
    if (soundsLoaded) {
        for (int i = 0; i < SOUND_COUNT; i++) UnloadSound(sounds[i]);
    }
    CloseAudioDevice();
}

void playSound(SoundName name, float volume) {
    if (!soundsLoaded) return;
    if (volume > 1) volume = 1;
    SetSoundVolume(sounds[name], volume);
    PlaySound(sounds[name]);
}
