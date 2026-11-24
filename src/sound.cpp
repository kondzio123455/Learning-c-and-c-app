#include "sound.h"
#include <cmath>
#include <cstdio>

// ============================================================
// Making sounds in code. A sound is just a list of numbers (samples):
// 22050 of them for each second, each one how far the speaker is pushed.
// We add tones (sine waves) and noise (random numbers) into that list.
// ============================================================

const int SAMPLE_RATE = 22050;
const int MAX_SAMPLES = SAMPLE_RATE;   // sounds are at most 1 second long

float mix[MAX_SAMPLES];   // the sound being built
int mixLength = 0;

float randomUnit() {
    return GetRandomValue(-1000, 1000) / 1000.0f;
}

// Starts a new sound 'seconds' long, silent
void beginSound(float seconds) {
    mixLength = (int)(seconds * SAMPLE_RATE);
    if (mixLength > MAX_SAMPLES) mixLength = MAX_SAMPLES;
    for (int i = 0; i < mixLength; i++) mix[i] = 0;
}

// Adds a tone that starts at 'start' seconds and lasts 'length' seconds.
// Its pitch slides from pitch1 to pitch2 (in Hz), 'noise' mixes in hiss (0 to 1),
// 'decay' is how fast it fades, and 'bright' adds a higher harmonic (for bells).
void addTone(float start, float length, float pitch1, float pitch2, float volume, float noise, float decay, float bright) {
    int first = (int)(start * SAMPLE_RATE);
    int count = (int)(length * SAMPLE_RATE);
    float phase = 0;
    for (int i = 0; i < count && first + i < mixLength; i++) {
        float t = (float)i / count;                     // 0 to 1 through this tone
        float pitch = pitch1 + (pitch2 - pitch1) * t;
        phase += 2 * PI * pitch / SAMPLE_RATE;
        float wave = sinf(phase) + bright * 0.5f * sinf(phase * 2.01f) + bright * 0.25f * sinf(phase * 3.02f);
        float value = wave * (1 - noise) + randomUnit() * noise;
        float envelope = expf(-decay * t);              // fade out
        if (i < 30) envelope *= i / 30.0f;              // a tiny fade in, so it doesn't pop
        mix[first + i] += value * envelope * volume;
    }
}

// Turns the mix into a raylib Sound
Sound finishSound() {
    short *samples = (short *)MemAlloc(mixLength * sizeof(short));
    for (int i = 0; i < mixLength; i++) {
        float value = mix[i];
        if (value > 1) value = 1;
        if (value < -1) value = -1;
        samples[i] = (short)(value * 14000);
    }
    Wave wave = { (unsigned int)mixLength, (unsigned int)SAMPLE_RATE, 16, 1, samples };
    Sound sound = LoadSoundFromWave(wave);
    UnloadWave(wave);   // also frees the samples
    return sound;
}

// Each sound, built from tones. Musical notes: C5 = 523 Hz, E5 = 659, G5 = 784, C6 = 1047, E6 = 1319, G6 = 1568.
Sound makeSoundNamed(SoundName name) {
    switch (name) {
    case SOUND_CUE:        // a woody "tock" with a low thump
        beginSound(0.12f);
        addTone(0, 0.12f, 900, 700, 0.6f, 0.35f, 14, 0.3f);
        addTone(0, 0.10f, 160, 90, 0.7f, 0.1f, 9, 0);
        break;
    case SOUND_CLICK:      // a sharp, hard click (resin balls)
        beginSound(0.06f);
        addTone(0, 0.06f, 2600, 2100, 0.8f, 0.3f, 16, 0.6f);
        break;
    case SOUND_CUSHION:    // a soft rubbery thud
        beginSound(0.12f);
        addTone(0, 0.12f, 130, 80, 0.8f, 0.35f, 9, 0);
        break;
    case SOUND_POCKET:     // a clunk, then the ball rolling away under the table
        beginSound(0.45f);
        addTone(0, 0.14f, 170, 70, 0.9f, 0.3f, 8, 0);
        addTone(0.08f, 0.37f, 70, 55, 0.35f, 0.7f, 4, 0);
        break;
    case SOUND_CORRECT:    // three rising bell notes (C6 E6 G6)
        beginSound(0.6f);
        addTone(0.00f, 0.35f, 1047, 1047, 0.45f, 0, 5, 0.7f);
        addTone(0.08f, 0.35f, 1319, 1319, 0.45f, 0, 5, 0.7f);
        addTone(0.16f, 0.44f, 1568, 1568, 0.5f, 0, 4, 0.7f);
        break;
    case SOUND_WRONG:      // two low, buzzy notes going down
        beginSound(0.32f);
        addTone(0.00f, 0.14f, 220, 210, 0.5f, 0.1f, 3, 1.0f);
        addTone(0.14f, 0.18f, 185, 170, 0.5f, 0.1f, 4, 1.0f);
        break;
    case SOUND_UNLOCK:     // a bright arpeggio (C5 E5 G5 C6)
        beginSound(0.9f);
        addTone(0.00f, 0.5f, 523, 523, 0.4f, 0, 4, 0.6f);
        addTone(0.09f, 0.5f, 659, 659, 0.4f, 0, 4, 0.6f);
        addTone(0.18f, 0.5f, 784, 784, 0.4f, 0, 4, 0.6f);
        addTone(0.27f, 0.63f, 1047, 1047, 0.5f, 0, 3, 0.6f);
        break;
    case SOUND_HOVER:      // a tiny soft tick
        beginSound(0.03f);
        addTone(0, 0.03f, 1800, 1600, 0.25f, 0.2f, 10, 0);
        break;
    case SOUND_BUTTON:     // a short click
        beginSound(0.06f);
        addTone(0, 0.06f, 950, 700, 0.5f, 0.25f, 10, 0.4f);
        break;
    case SOUND_COMBO:      // a ding (played higher for bigger combos)
        beginSound(0.45f);
        addTone(0, 0.45f, 1568, 1568, 0.5f, 0, 5, 0.8f);
        addTone(0, 0.30f, 2093, 2093, 0.2f, 0, 6, 0);
        break;
    case SOUND_FIREWORK:   // a pop and a crackle
        beginSound(0.6f);
        addTone(0, 0.18f, 110, 60, 0.7f, 0.6f, 7, 0);
        addTone(0.05f, 0.55f, 3000, 2000, 0.25f, 0.95f, 5, 0);
        break;
    case SOUND_LEVEL_UP:   // a fast rising arpeggio with a long shimmer at the top
        beginSound(1.0f);
        addTone(0.00f, 0.3f, 523, 523, 0.35f, 0, 4, 0.6f);
        addTone(0.06f, 0.3f, 659, 659, 0.35f, 0, 4, 0.6f);
        addTone(0.12f, 0.3f, 784, 784, 0.35f, 0, 4, 0.6f);
        addTone(0.18f, 0.3f, 1047, 1047, 0.35f, 0, 4, 0.6f);
        addTone(0.24f, 0.76f, 1319, 1319, 0.45f, 0, 3, 0.8f);
        break;
    case SOUND_TYPE:       // a very short tick, for text typing itself out
        beginSound(0.02f);
        addTone(0, 0.02f, 1300, 1100, 0.2f, 0.3f, 8, 0);
        break;
    default:
        beginSound(0.05f);
        break;
    }
    return finishSound();
}

// The file name each sound can be replaced with, in assets/sounds/
const char *SOUND_FILES[SOUND_COUNT] = {
    "strike", "click", "cushion", "pocket", "correct", "wrong", "unlock",
    "hover", "button", "combo", "firework", "levelup", "type"
};

// Each sound has a few "voices" (copies), so the same sound can play again
// before it has finished (lots of clicks at once on the break)
const int VOICES = 4;
Sound sounds[SOUND_COUNT][VOICES];
int nextVoice[SOUND_COUNT];
bool soundsLoaded = false;

void loadSounds() {
    InitAudioDevice();
    if (!IsAudioDeviceReady()) return;   // no speakers: the game still works, silently

    for (int s = 0; s < SOUND_COUNT; s++) {
        // A real recording if there is one, otherwise make it
        const char *wav = TextFormat("assets/sounds/%s.wav", SOUND_FILES[s]);
        const char *ogg = TextFormat("assets/sounds/%s.ogg", SOUND_FILES[s]);
        if (FileExists(wav)) sounds[s][0] = LoadSound(wav);
        else if (FileExists(ogg)) sounds[s][0] = LoadSound(ogg);
        else sounds[s][0] = makeSoundNamed((SoundName)s);

        for (int v = 1; v < VOICES; v++) sounds[s][v] = LoadSoundAlias(sounds[s][0]);   // shares the same samples
        nextVoice[s] = 0;
    }
    soundsLoaded = true;
}

void unloadSounds() {
    if (soundsLoaded) {
        for (int s = 0; s < SOUND_COUNT; s++) {
            for (int v = 1; v < VOICES; v++) UnloadSoundAlias(sounds[s][v]);
            UnloadSound(sounds[s][0]);
        }
    }
    CloseAudioDevice();
}

void playSoundPitched(SoundName name, float volume, float pitch) {
    if (!soundsLoaded) return;
    if (volume > 1) volume = 1;
    if (volume <= 0) return;
    Sound *voice = &sounds[name][nextVoice[name]];
    nextVoice[name] = (nextVoice[name] + 1) % VOICES;
    SetSoundVolume(*voice, volume);
    SetSoundPitch(*voice, pitch);
    PlaySound(*voice);
}

// Plays a sound, a tiny bit higher or lower each time so it never sounds robotic
void playSound(SoundName name, float volume) {
    playSoundPitched(name, volume, 1.0f + GetRandomValue(-40, 40) / 1000.0f);
}
