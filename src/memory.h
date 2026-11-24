#ifndef MEMORY_H
#define MEMORY_H

struct App;

// ============================================================
// The memory panel (on the right while you aim): the shot's
// variables drawn like the diagrams in the Pointers lecture.
// Each variable is a box with its value and its address, and
// each pointer is an arrow to the box it points at.
// ============================================================

float drawMemoryPanel(App *app, float x, float y, float width);   // returns the height it used

#endif
