#include "fx.h"
#include "sound.h"
#include "render3d.h"
#include "anim.h"
#include "ui.h"
#include "rlgl.h"
#include <cmath>
#include <cstdio>

// From pool.cpp
extern Color ballColours[8];
extern float pocketX[6], pocketY[6];

float randomFloat(float low, float high) {
    return low + (high - low) * GetRandomValue(0, 10000) / 10000.0f;
}

// ============================================================
// 3D particles: chalk dust, sparks and pot bursts, in the 3D scene
// ============================================================

struct Particle3D {
    Vector3 position;
    Vector3 velocity;     // 3D units per second
    float life;           // seconds left
    float maxLife;
    float size;
    float grow;           // how fast it gets bigger (chalk dust spreads out)
    float gravity;        // pulls it down (sparks fall, dust floats)
    Color colour;
    bool glow;            // added on top (bright, like light) instead of painted over
};

const int MAX_3D = 600;
Particle3D particles3D[MAX_3D];

// Finds a free slot (life <= 0), or -1 if they're all busy
int free3D() {
    for (int i = 0; i < MAX_3D; i++) {
        if (particles3D[i].life <= 0) return i;
    }
    return -1;
}

void spawn3D(Vector3 position, Vector3 velocity, float life, float size, float grow, float gravity, Color colour, bool glow) {
    int i = free3D();
    if (i == -1) return;
    particles3D[i] = Particle3D{ position, velocity, life, life, size, grow, gravity, colour, glow };
}

// A random direction, mostly sideways, a bit up
Vector3 randomSpray(float speed, float up) {
    float angle = randomFloat(0, 2 * PI);
    float s = randomFloat(0.3f, 1.0f) * speed;
    return Vector3{ cosf(angle) * s, randomFloat(0.2f, 1.0f) * up, sinf(angle) * s };
}

void drawParticles3D() {
    rlDrawRenderBatchActive();
    rlDisableDepthMask();   // see-through: they mustn't hide each other (but balls still hide them)
    for (int pass = 0; pass < 2; pass++) {
        // First the soft ones (painted over), then the glowing ones (added on top, like light)
        if (pass == 1) BeginBlendMode(BLEND_ADDITIVE);
        for (int i = 0; i < MAX_3D; i++) {
            Particle3D *p = &particles3D[i];
            if (p->life <= 0 || p->glow != (pass == 1)) continue;
            float fade = p->life / p->maxLife;
            DrawSphereEx(p->position, p->size, 4, 6, Fade(p->colour, fade * (float)p->colour.a / 255));
        }
        if (pass == 1) EndBlendMode();
    }
    rlDrawRenderBatchActive();
    rlEnableDepthMask();
}

// ============================================================
// 2D particles: confetti and fireworks, over the whole screen
// ============================================================

enum ParticleKind { CONFETTI, SPARK, ROCKET };

struct Particle {
    ParticleKind kind;
    float x, y;
    float vx, vy;
    float life;
    float maxLife;
    float size;
    float gravity;
    float angle, spin;    // confetti tumbles
    Color colour;
};

const int MAX_PARTICLES = 700;
Particle particles[MAX_PARTICLES];
float fireworksLeft = 0;   // seconds of fireworks still to go
float nextRocket = 0;

int freeParticle() {
    for (int i = 0; i < MAX_PARTICLES; i++) {
        if (particles[i].life <= 0) return i;
    }
    return -1;
}

void spawnParticle(Particle p) {
    int i = freeParticle();
    if (i != -1) particles[i] = p;
}

void spawnConfetti(float x, float y, int count) {
    Color colours[5] = { COLOUR_ACCENT, COLOUR_GOOD, { 90, 160, 255, 255 }, { 255, 110, 150, 255 }, WHITE };
    for (int n = 0; n < count; n++) {
        float angle = randomFloat(-2.6f, -0.5f);   // mostly upwards
        float s = randomFloat(150, 420);
        spawnParticle(Particle{ CONFETTI, x, y, cosf(angle) * s, sinf(angle) * s, 1.4f, 1.4f, randomFloat(3, 6), 500,
                                randomFloat(0, 360), randomFloat(-600, 600), colours[GetRandomValue(0, 4)] });
    }
}

void spawnSparkle(float x, float y, Color colour) {
    for (int n = 0; n < 3; n++) {
        float angle = randomFloat(0, 2 * PI);
        float s = randomFloat(30, 120);
        float life = randomFloat(0.4f, 0.8f);
        spawnParticle(Particle{ SPARK, x, y, cosf(angle) * s, sinf(angle) * s - 40, life, life, randomFloat(1.5f, 2.5f), 60, 0, 0, colour });
    }
}

// A firework bursts: a ball of glowing sparks in one colour, falling as they fade
void burstFirework(float x, float y) {
    Color colour = ColorFromHSV(randomFloat(0, 360), 0.65f, 1.0f);
    for (int n = 0; n < 60; n++) {
        float angle = randomFloat(0, 2 * PI);
        float s = randomFloat(60, 300);
        float life = randomFloat(0.9f, 1.5f);
        spawnParticle(Particle{ SPARK, x, y, cosf(angle) * s, sinf(angle) * s, life, life, randomFloat(2, 3.5f), 160, 0, 0, colour });
    }
    playSoundPitched(SOUND_FIREWORK, 0.45f, randomFloat(0.85f, 1.15f));
}

void launchRocket() {
    // From the bottom, mostly at the sides so the "you won" box stays readable
    float x = GetRandomValue(0, 1) ? randomFloat(80, 320) : randomFloat(960, 1200);
    if (GetRandomValue(0, 3) == 0) x = randomFloat(380, 900);
    float fuse = randomFloat(0.7f, 1.0f);
    spawnParticle(Particle{ ROCKET, x, (float)SCREEN_HEIGHT, randomFloat(-60, 60), randomFloat(-760, -620), fuse, fuse, 3, 250, 0, 0,
                            Color{ 255, 230, 180, 255 } });
}

void fxWin() {
    fireworksLeft = 4.0f;
    nextRocket = 0;
}

void updateParticles(float dt) {
    // Fireworks: a new rocket every so often
    if (fireworksLeft > 0) {
        fireworksLeft -= dt;
        nextRocket -= dt;
        if (nextRocket <= 0) {
            launchRocket();
            nextRocket = randomFloat(0.15f, 0.35f);
        }
    }

    for (int i = 0; i < MAX_PARTICLES; i++) {
        Particle *p = &particles[i];
        if (p->life <= 0) continue;
        p->x += p->vx * dt;
        p->y += p->vy * dt;
        p->vy += p->gravity * dt;
        p->vx *= 0.985f;
        p->angle += p->spin * dt;
        p->life -= dt;
        if (p->kind == ROCKET && p->life <= 0) burstFirework(p->x, p->y);   // the fuse ran out
    }

    for (int i = 0; i < MAX_3D; i++) {
        Particle3D *p = &particles3D[i];
        if (p->life <= 0) continue;
        p->position = Vector3{ p->position.x + p->velocity.x * dt, p->position.y + p->velocity.y * dt, p->position.z + p->velocity.z * dt };
        p->velocity.y -= p->gravity * dt;
        p->velocity.x *= 1 - 2 * dt;   // air slows them down
        p->velocity.z *= 1 - 2 * dt;
        if (p->position.y < 0.002f && p->gravity > 0) {   // land on the felt
            p->position.y = 0.002f;
            p->velocity.y = 0;
        }
        p->size += p->grow * dt;
        p->life -= dt;
    }
}

void drawParticles() {
    for (int pass = 0; pass < 2; pass++) {
        if (pass == 1) BeginBlendMode(BLEND_ADDITIVE);   // sparks and rockets glow
        for (int i = 0; i < MAX_PARTICLES; i++) {
            Particle *p = &particles[i];
            if (p->life <= 0 || (p->kind == CONFETTI) == (pass == 1)) continue;
            float fade = p->life / p->maxLife;
            if (p->kind == CONFETTI) {
                Rectangle piece = { p->x, p->y, p->size, p->size * 0.6f };
                DrawRectanglePro(piece, Vector2{ p->size / 2, p->size * 0.3f }, p->angle, Fade(p->colour, fade));
            } else if (p->kind == SPARK) {
                DrawCircleV(Vector2{ p->x, p->y }, p->size * 3, Fade(p->colour, fade * 0.07f));   // a soft halo
                DrawCircleV(Vector2{ p->x, p->y }, p->size * (0.5f + 0.5f * fade), Fade(p->colour, fade));
            } else {
                // A rocket: a bright head with a short tail behind it
                DrawLineEx(Vector2{ p->x - p->vx * 0.05f, p->y - p->vy * 0.05f }, Vector2{ p->x, p->y }, 3, Fade(p->colour, 0.5f));
                DrawCircleV(Vector2{ p->x, p->y }, 3, p->colour);
            }
        }
        if (pass == 1) EndBlendMode();
    }
}

// ============================================================
// Popups
// ============================================================

struct Popup {
    char text[40];
    float x, y;
    float age;
    float life;
    float size;
    Color colour;
};

const int MAX_POPUPS = 12;
Popup popups[MAX_POPUPS];

void spawnPopup(const char *text, float x, float y, Color colour, float size) {
    // Use a free slot, or replace the oldest
    int slot = 0;
    for (int i = 0; i < MAX_POPUPS; i++) {
        if (popups[i].age >= popups[i].life) { slot = i; break; }
        if (popups[i].age > popups[slot].age) slot = i;
    }
    Popup *p = &popups[slot];
    snprintf(p->text, sizeof(p->text), "%s", text);
    p->x = x;
    p->y = y;
    p->age = 0;
    p->life = size >= 40 ? 1.5f : 1.3f;
    p->size = size;
    p->colour = colour;
}

void spawnFloatingText(const char *text, float x, float y, Color colour) {
    spawnPopup(text, x, y, colour, 30);
}

void updateFloatingTexts(float dt) {
    for (int i = 0; i < MAX_POPUPS; i++) {
        if (popups[i].age < popups[i].life) {
            popups[i].age += dt;
            popups[i].y -= 30 * dt;   // drift upwards
        }
    }
}

void drawFloatingTexts() {
    for (int i = 0; i < MAX_POPUPS; i++) {
        Popup *p = &popups[i];
        if (p->age >= p->life) continue;
        float pop = easeOutBack(clamp01(p->age / 0.3f));        // grows past full size, then settles
        float fade = clamp01((p->life - p->age) / 0.4f);         // fades out at the end
        float size = p->size * (0.4f + 0.6f * pop);
        float width = titleWidth(p->text, size);
        float x = p->x - width / 2;
        float y = p->y - size / 2;
        // A dark outline, so it can be read over anything
        for (int k = 0; k < 4; k++) {
            float ox = (k % 2 == 0 ? -1.0f : 1.0f) * size * 0.05f;
            float oy = (k < 2 ? -1.0f : 1.0f) * size * 0.05f;
            drawTitle(p->text, x + ox, y + oy + 2, size, Fade(BLACK, 0.5f * fade));
        }
        if (p->size >= 40) drawGlowTitle(p->text, x, y, size, Fade(p->colour, fade), fade);
        else drawTitle(p->text, x, y, size, Fade(p->colour, fade));
    }
}

// ============================================================
// Combo: own balls potted in a row, in one turn
// ============================================================

int combo = 0;
int comboPlayer = -1;
int comboShot = -10;   // the shot the last one was potted on

int comboCount(Game *game) {
    // The combo is still going if the same player is shooting, on the same shot or the next one
    // (a smaller shot number means a new game has started)
    if (game->currentPlayer != comboPlayer) return 0;
    if (game->shotNumber < comboShot || game->shotNumber > comboShot + 1) return 0;
    return combo;
}

// ============================================================
// Things that happen on the table
// ============================================================

void fxStrike(Game *game, float power) {
    Ball *cue = &game->balls[0];
    float speed = sqrtf(cue->vx * cue->vx + cue->vy * cue->vy);
    playSound(SOUND_CUE, 0.3f + power / 150);
    if (speed <= 0) return;

    // A puff of blue chalk dust where the tip hit the ball, blown back towards the player
    float dx = cue->vx / speed;
    float dy = cue->vy / speed;
    Vector3 contact = tableToWorld(cue->x - dx * BALL_RADIUS, cue->y - dy * BALL_RADIUS, BALL_RADIUS);
    int puffs = 10 + (int)(power / 6);
    for (int n = 0; n < puffs; n++) {
        Vector3 v = randomSpray(0.25f, 0.25f);
        v.x -= dx * randomFloat(0.1f, 0.5f);
        v.z -= dy * randomFloat(0.1f, 0.5f);
        spawn3D(contact, v, randomFloat(0.5f, 0.9f), 0.008f, 0.05f, -0.05f, Color{ 160, 190, 240, 150 }, false);
    }

    // A hard shot shakes the table and stops time for a moment
    shakeScreen(power / 30);
    if (game->isBreak && power > 70) {
        shakeScreen(4);
        hitStop(0.09f);
    } else if (power > 80) {
        hitStop(0.05f);
    }
}

void fxBallsHit(float x, float y, float speed) {
    playSound(SOUND_CLICK, speed / 900);   // harder hits are louder
    if (speed < 650) return;

    // Hard hits throw sparks, shake the camera, and the biggest ones freeze time for a moment
    Vector3 at = tableToWorld(x, y, BALL_RADIUS);
    int count = (int)((speed - 650) / 50);
    if (count > 24) count = 24;
    for (int n = 0; n < count; n++) {
        spawn3D(at, randomSpray(1.6f, 1.2f), randomFloat(0.2f, 0.4f), 0.006f, -0.01f, 6, Color{ 255, 220, 150, 255 }, true);
    }
    shakeScreen(speed / 450);
    if (speed > 1100) hitStop(0.03f + fminf(0.04f, (speed - 1100) / 25000));
}

void fxCushionHit(float speed) {
    playSound(SOUND_CUSHION, speed / 1200);
}

void fxBallPotted(Game *game, int number, int pocket) {
    playSound(SOUND_POCKET, 0.9f);

    // A burst of light in the ball's colour comes up out of the pocket
    Color colour = ballColours[number % 8];
    if (number == 0) colour = WHITE;
    if (number == 8) colour = Color{ 170, 120, 255, 255 };
    Vector3 at = tableToWorld(pocketX[pocket], pocketY[pocket], 4);
    for (int n = 0; n < 30; n++) {
        spawn3D(at, randomSpray(0.7f, 1.8f), randomFloat(0.4f, 0.8f), 0.012f, -0.01f, 3, colour, true);
    }
    for (int n = 0; n < 16; n++) {   // and a quick white ring flashing outwards
        float angle = 2 * PI * n / 16;
        spawn3D(at, Vector3{ cosf(angle) * 1.2f, 0.05f, sinf(angle) * 1.2f }, 0.25f, 0.01f, 0, 0, WHITE, true);
    }

    // Fouls and the 8 are shown when the shot ends; only the shooter's own balls score here
    if (number == 0 || number == 8) return;
    Player *shooter = &game->players[game->currentPlayer];
    bool own = game->tableOpen || groupOf(number) == shooter->group;
    if (!own) return;

    if (comboCount(game) == 0) combo = 0;   // a new turn: start counting again
    combo++;
    comboPlayer = game->currentPlayer;
    comboShot = game->shotNumber;

    Vector2 onScreen = tableToScreen(pocketX[pocket], pocketY[pocket], 0);
    spawnPopup("+1", onScreen.x, onScreen.y - 24, COLOUR_ACCENT, 30);
    float cx = TABLE_VIEW.x + TABLE_VIEW.width / 2;
    float cy = TABLE_VIEW.y + 70;
    if (combo == 1) {
        spawnPopup("NICE SHOT!", cx, cy, COLOUR_GOOD, 50);
    } else {
        spawnPopup(TextFormat("COMBO x%d", combo), cx, cy, COLOUR_ACCENT, 56);
        playSoundPitched(SOUND_COMBO, 0.6f, 1.0f + 0.12f * (combo - 2));   // higher each time
    }
}

// ============================================================
// Screen shake: "trauma" goes up with each hit and slowly drains away.
// The shake is trauma squared, so small hits barely shake and big ones really do.
// Smooth wobbly waves (not random jumps) move the camera, so it feels like a jolt.
// ============================================================

float trauma = 0;       // 0 to 1
float shakeTime = 0;

void shakeScreen(float amount) {
    trauma = fminf(1, trauma + amount / 10);
}

void updateShake(float dt) {
    trauma = fmaxf(0, trauma - 1.5f * dt);
    shakeTime += dt;
}

Vector2 getShake() {
    float strength = trauma * trauma;
    float x = sinf(shakeTime * 53) * 0.6f + sinf(shakeTime * 31 + 1.3f) * 0.4f;
    float y = sinf(shakeTime * 47 + 2.1f) * 0.6f + sinf(shakeTime * 37 + 0.4f) * 0.4f;
    return Vector2{ x * strength, y * strength };
}

// ============================================================
// Hit-stop: the balls freeze for a split second on a big hit
// ============================================================

float hitStopTimer = 0;

void hitStop(float seconds) {
    if (seconds > hitStopTimer) hitStopTimer = seconds;
}

void updateHitStop(float dt) {
    if (hitStopTimer > 0) hitStopTimer -= dt;
}

bool hitStopping() {
    return hitStopTimer > 0;
}
