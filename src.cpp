#include <raylib.h>
#include <cstdlib>
#include <stdio.h>
#include <iostream>
#include <math.h>
#include <time.h>
#include "sparkle.h"

constexpr int FRAMERATE = 60;
constexpr int NUM_SPARKLES = 50000;
constexpr int VEL_RANGE[2] = { 1, 225 };
constexpr int THETA_RANGE[2] = { 0, 359 }; // degrees
constexpr int THETA_DOWN = 270;
constexpr int FLY_TIME_RANGE[2] = { 750, 1000 }; // ms
constexpr int BURST_TIME = 750; // ms
constexpr int FIZZLE_TIME = 2500; // ms
constexpr int FIZZLE_CHANCE = 7; // %
constexpr double GRAVITY = 125; // px / s^2
constexpr Color BG_COLOR = BLACK;
constexpr Color sparkleColors[] = {RED, GREEN, VIOLET, GOLD, PINK, ORANGE};

void drawSky();
void createSparkle(Sparkle* memoryPos);
void updatePos(Sparkle* s);
void applyGrav(Sparkle* s);
void drawSparkle(Sparkle* s);

int main() {
	// initialization
	InitWindow(0, 0, "firework visualizer");
	ToggleFullscreen();
	SetTargetFPS(FRAMERATE);
	SetRandomSeed(time(NULL));

	// get monitor size
	int monitor = GetCurrentMonitor();
	int width = GetMonitorWidth(monitor);
	int height = GetMonitorHeight(monitor);
	SetWindowSize(width, height);

	// allocate heap memory for program
	Sparkle* sparkleMemory = static_cast<Sparkle*>(std::malloc(NUM_SPARKLES * sizeof(Sparkle)));

	if (sparkleMemory == nullptr) {
		std::cout << "Memory allocation failure.";
		return 1;
	}

	while (!WindowShouldClose()) {
		// clear bg
		BeginDrawing();
		drawSky();
		EndDrawing();

		// randomize firework launch values
		int xLaunch = GetRandomValue((int)(0.1 * width), (int)(0.9 * width));
		int yLaunch = GetRandomValue((int)(0.25 * height), (int)(0.75 * height));

		// prep sparkles
		for (int i = 0; i < NUM_SPARKLES; i++) {
			// instantiate sparkle
			createSparkle(&sparkleMemory[i]);
			Sparkle* s = &sparkleMemory[i];

			// set x and y pos
			(*s).pos[0] = xLaunch;
			(*s).pos[1] = height - yLaunch;
		}

		// randomize flying
		int randFlyTimeMS = GetRandomValue(FLY_TIME_RANGE[0], FLY_TIME_RANGE[1]);
		double randFlyTimeS = randFlyTimeMS / 1000.;
		double startFly = GetTime(); // s
		// draw pre-explosion flying anim
		while ((GetTime() - startFly) < randFlyTimeS && !WindowShouldClose()) {
			int currentHeight = (int)(height - (yLaunch * ((GetTime() - startFly) / randFlyTimeS)));
			BeginDrawing();
			drawSky();
			//DrawPixel(xLaunch, currentHeight, WHITE);
			DrawRectangle(xLaunch - 1, currentHeight - 1, 3, 3, WHITE);
			EndDrawing();
		}

		// draw explosion anim
		double startBurst = GetTime();
		while (GetTime() - startBurst < (BURST_TIME / 1000.) && !WindowShouldClose()) {
			BeginDrawing();
			drawSky();
			for (int i = 0; i < NUM_SPARKLES; i++) {
				Sparkle* currentSparkle = &sparkleMemory[i];

				// update position
				applyGrav(currentSparkle);
				updatePos(currentSparkle);

				// draw
				drawSparkle(currentSparkle);
			}
			EndDrawing();
		}

		// draw fizzle-away anim
		double startFizzle = GetTime();
		while (GetTime() - startFizzle < (FIZZLE_TIME / 1000.) && !WindowShouldClose()) {
			BeginDrawing();
			drawSky();
			for (int i = 0; i < NUM_SPARKLES; i++) {
				Sparkle* currentSparkle = &sparkleMemory[i];
				bool shouldFizzle = (GetRandomValue(1, 100) <= FIZZLE_CHANCE) || (*currentSparkle).fizzled;
				(*currentSparkle).fizzled = shouldFizzle;

				// check if sparkle already fizzled
				if (!shouldFizzle) {
					// update position
					// u = < cos(theta), sin(theta) > identity for unit vector
					applyGrav(currentSparkle);
					updatePos(currentSparkle);
					// draw
					drawSparkle(currentSparkle);
				}
			}
			EndDrawing();
		}
	}

	// de-initialization
	std::free(sparkleMemory);
	sparkleMemory = nullptr;
	CloseWindow();
}

void createSparkle(Sparkle* memoryPos) {
	double smoothFactor = 100;
	double angle = GetRandomValue(THETA_RANGE[0]*smoothFactor, THETA_RANGE[1]*smoothFactor) / smoothFactor;
	double vel = GetRandomValue(VEL_RANGE[0]*smoothFactor, VEL_RANGE[1]*smoothFactor) / smoothFactor;
	int quadrant = ((int)angle / 90) + 1;
	double remAngle = angle - quadrant*90;

	double xVel;
	double yVel;
	// opp is y, adj is x
	if (quadrant == 1 || quadrant == 3) {
		yVel = sin(remAngle) * vel;
		xVel = cos(remAngle) * vel;
	} // opp is x, adj is y
	else {
		xVel = sin(remAngle) * vel;
		yVel = cos(remAngle) * vel;
	}
	yVel *= -1; // ACCOUNT FOR AXIS DIFF

	int colorRand = GetRandomValue(1, sizeof(sparkleColors));

	Sparkle s = { .fizzled = false, .vel = { xVel, yVel }, .color = sparkleColors[colorRand] };

	// place in memory
	(*memoryPos) = s;
}
void drawSky() {
	ClearBackground(BG_COLOR);
}
void applyGrav(Sparkle* s) {
	(*s).vel[1] += GRAVITY * GetFrameTime();
}
// u = < cos(theta), sin(theta) > identity for unit vector
void updatePos(Sparkle* s) {
	(*s).pos[0] += (*s).vel[0] * GetFrameTime();
	(*s).pos[1] += (*s).vel[1] * GetFrameTime();
}
void drawSparkle(Sparkle* s) {
	DrawPixel((int)(*s).pos[0], (int)(*s).pos[1], (*s).color);
	//DrawCircle((int)(*s).pos[0], (int)(*s).pos[1], 1, (*s).color);
}