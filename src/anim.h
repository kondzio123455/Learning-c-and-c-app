#ifndef ANIM_H
#define ANIM_H

#include "raylib.h"

// ============================================================
// Animation helpers: easing curves and smooth movement.
// Easing functions take t from 0 to 1 and return a "shaped" 0 to 1.
// ============================================================

float clamp01(float t);
float lerpf(float a, float b, float t);

// Easing curves (t from 0 to 1)
float easeOutCubic(float t);      // fast start, gentle stop
float easeInOutCubic(float t);    // gentle start and stop
float easeOutBack(float t);       // overshoots a little, then settles (for pop-ins)
float easeOutElastic(float t);    // wobbles before settling (for bouncy popups)
float easeOutBounce(float t);     // drops and bounces, like a ball landing

// Smooth movement that doesn't depend on the frame rate:
// each call moves 'current' part of the way to 'target'. Bigger speed = snappier.
float smoothTowards(float current, float target, float speed, float dt);
Vector3 smoothTowards3(Vector3 current, Vector3 target, float speed, float dt);

#endif
