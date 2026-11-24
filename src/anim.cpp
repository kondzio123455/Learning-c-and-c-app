#include "anim.h"
#include <cmath>

float clamp01(float t) {
    if (t < 0) return 0;
    if (t > 1) return 1;
    return t;
}

float lerpf(float a, float b, float t) {
    return a + (b - a) * t;
}

float easeOutCubic(float t) {
    t = clamp01(t);
    float u = 1 - t;
    return 1 - u * u * u;
}

float easeInOutCubic(float t) {
    t = clamp01(t);
    if (t < 0.5f) return 4 * t * t * t;
    float u = -2 * t + 2;
    return 1 - u * u * u / 2;
}

float easeOutBack(float t) {
    t = clamp01(t);
    const float c1 = 1.70158f;
    const float c3 = c1 + 1;
    float u = t - 1;
    return 1 + c3 * u * u * u + c1 * u * u;
}

float easeOutElastic(float t) {
    t = clamp01(t);
    if (t == 0 || t == 1) return t;
    const float c4 = (2 * PI) / 3;
    return powf(2, -10 * t) * sinf((t * 10 - 0.75f) * c4) + 1;
}

// 1 - e^(-speed * dt) is the fraction to move this frame. It gives the same
// motion at 30, 60 or 120 frames per second.
float smoothTowards(float current, float target, float speed, float dt) {
    float amount = 1 - expf(-speed * dt);
    return current + (target - current) * amount;
}

Vector3 smoothTowards3(Vector3 current, Vector3 target, float speed, float dt) {
    float amount = 1 - expf(-speed * dt);
    return Vector3{ current.x + (target.x - current.x) * amount,
                    current.y + (target.y - current.y) * amount,
                    current.z + (target.z - current.z) * amount };
}

// Like a ball dropped onto a table: it lands, bounces lower each time, then stops
float easeOutBounce(float t) {
    t = clamp01(t);
    const float n = 7.5625f;
    const float d = 2.75f;
    if (t < 1 / d) return n * t * t;
    if (t < 2 / d) { t -= 1.5f / d; return n * t * t + 0.75f; }
    if (t < 2.5f / d) { t -= 2.25f / d; return n * t * t + 0.9375f; }
    t -= 2.625f / d;
    return n * t * t + 0.984375f;
}
