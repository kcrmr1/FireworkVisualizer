#pragma once
#include <raylib.h>

typedef struct {
	bool fizzled;
	int angle;
	double pos[2];
	double vel;
	Color color;
} Sparkle;