#include <raylib.h>
#include <cstdlib>
#include <stdio.h>
#include <iostream>
#include <math.h>
#include <time.h>
#include "sparkle.h"

constexpr int FRAMERATE = 60;
constexpr int NUM_SPARKLES = 10000;
constexpr int VEL_RANGE[2] = { 1, 1500 };
constexpr int THETA_RANGE[2] = { 0, 359 }; // degrees
constexpr int FLY_TIME_RANGE[2] = { 750, 1000 }; // ms
constexpr int BURST_TIME = 750; // ms
constexpr int FIZZLE_TIME = 1250; // ms
constexpr int FIZZLE_CHANCE = 7; // %
constexpr Color BG_COLOR = DARKBLUE;
constexpr Color sparkleColors[] = {RED, GREEN, VIOLET, GOLD, PINK, ORANGE};

void drawSky();

int main() {
	// initialization
	InitWindow(0, 0, "firework visualizer");
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
		ClearBackground(BG_COLOR);
		EndDrawing();

		// randomize firework launch values
		int xLaunch = GetRandomValue((int)(0.1 * width), (int)(0.9 * width));
		int yLaunch = GetRandomValue((int)(0.25 * height), (int)(0.75 * height));

		// prep sparkles
		for (int i = 0; i < NUM_SPARKLES; i++) {
			// instantiate sparkle
			Sparkle sparkle = { .fizzled = false, .angle = GetRandomValue(THETA_RANGE[0], THETA_RANGE[1]),
				.vel = (double)GetRandomValue(VEL_RANGE[0], VEL_RANGE[1]) };
			sparkle.vel /= 10.;

			// randomize color
			int colorRand = GetRandomValue(1, sizeof(sparkleColors));
			sparkle.color = sparkleColors[colorRand];

			// set x and y pos
			sparkle.pos[0] = xLaunch;
			sparkle.pos[1] = height - yLaunch;

			// place sparkle in sparkle array
			sparkleMemory[i] = sparkle;
		}

		// randomize flying
		int randFlyTimeMS = GetRandomValue(FLY_TIME_RANGE[0], FLY_TIME_RANGE[1]);
		double randFlyTimeS = randFlyTimeMS / 1000.;
		double startFly = GetTime(); // s
		// draw pre-explosion flying anim
		while ((GetTime() - startFly) < randFlyTimeS && !WindowShouldClose()) {
			int currentHeight = (int)(height - (yLaunch * ((GetTime() - startFly) / randFlyTimeS)));
			BeginDrawing();
			ClearBackground(BG_COLOR);
			//DrawPixel(xLaunch, currentHeight, WHITE);
			DrawRectangle(xLaunch - 1, currentHeight - 1, 3, 3, WHITE);
			EndDrawing();
		}

		// draw explosion anim
		double startBurst = GetTime();
		while (GetTime() - startBurst < (BURST_TIME / 1000.) && !WindowShouldClose()) {
			BeginDrawing();
			ClearBackground(BG_COLOR);
			for (int i = 0; i < NUM_SPARKLES; i++) {
				Sparkle* currentSparkle = &sparkleMemory[i];

				// update position
				// u = < cos(theta), sin(theta) > identity for unit vector
				double unitVector[2] = { cos((*currentSparkle).angle * (PI / 180.)),
					sin((*currentSparkle).angle * (PI / 180.)) };
				double velFactor = (*currentSparkle).vel * GetFrameTime();
				double adjustedVector[2] = { unitVector[0] * velFactor, unitVector[1] * velFactor };
				(*currentSparkle).pos[0] += adjustedVector[0];
				(*currentSparkle).pos[1] += adjustedVector[1];

				// draw
				DrawPixel((int)(*currentSparkle).pos[0], 
					(int)(*currentSparkle).pos[1], (*currentSparkle).color);
			}
			EndDrawing();
		}

		// draw fizzle-away anim
		double startFizzle = GetTime();
		while (GetTime() - startFizzle < (FIZZLE_TIME / 1000.) && !WindowShouldClose()) {
			BeginDrawing();
			ClearBackground(BG_COLOR);
			for (int i = 0; i < NUM_SPARKLES; i++) {
				Sparkle* currentSparkle = &sparkleMemory[i];
				bool shouldFizzle = (GetRandomValue(1, 100) <= FIZZLE_CHANCE) || (*currentSparkle).fizzled;
				(*currentSparkle).fizzled = shouldFizzle;

				// check if sparkle already fizzled
				if (!shouldFizzle) {
					// update position
					// u = < cos(theta), sin(theta) > identity for unit vector
					double unitVector[2] = { cos((*currentSparkle).angle * (PI / 180.)),
						sin((*currentSparkle).angle * (PI / 180.)) };
					double velFactor = (*currentSparkle).vel * GetFrameTime();
					double adjustedVector[2] = { unitVector[0] * velFactor, unitVector[1] * velFactor };
					(*currentSparkle).pos[0] += adjustedVector[0];
					(*currentSparkle).pos[1] += adjustedVector[1];
					// draw
					DrawPixel((int)(*currentSparkle).pos[0], 
						(int)(*currentSparkle).pos[1], (*currentSparkle).color);
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

void drawSky() {

}