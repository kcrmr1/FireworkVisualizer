#pragma once
#include <raylib.h>

typedef struct {
	bool fizzled;
	double vel[2]; // x: + is right; y: + is up
	double pos[2];
	Color color;
} Sparkle;