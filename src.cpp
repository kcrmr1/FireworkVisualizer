#include <raylib.h>
#include <cstdlib>
#include <iostream>
#include <math.h>
#include <time.h>
#include "sparkle.h"

constexpr int FRAMERATE = 60;
constexpr int NUM_SPARKLES = 10000;
constexpr int VEL_RANGE[2] = { 0, 1500 };
constexpr int THETA_RANGE[2] = { 0, 359 }; // degrees
constexpr int FLY_TIME_RANGE[2] = { 750, 1000 }; // ms
constexpr int BURST_TIME = 2000; // ms

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
		ClearBackground(BLACK);
		EndDrawing();

		// randomize firework launch values
		int xLaunch = GetRandomValue((int)(0.1 * width), (int)(0.9 * width));
		int yLaunch = GetRandomValue((int)(0.25 * height), (int)(0.75 * height));

		// prep sparkles
		for (int i = 0; i < NUM_SPARKLES; i++) {
			// instantiate sparkle
			Sparkle sparkle = {.angle = GetRandomValue(THETA_RANGE[0], THETA_RANGE[1]),
				.vel = (double)GetRandomValue(VEL_RANGE[0], VEL_RANGE[1])};
			sparkle.vel /= 10.;

			// randomize color
			int colorRand = GetRandomValue(1, 7);
			switch (colorRand) {
			case 1:
				sparkle.color = BLUE;
				break;
			case 2:
				sparkle.color = RED;
				break;
			case 3:
				sparkle.color = GREEN;
				break;
			case 4:
				sparkle.color = PURPLE;
				break;
			case 5:
				sparkle.color = WHITE;
				break;
			case 6:
				sparkle.color = GOLD;
				break;
			case 7:
				sparkle.color = PINK;
				break;
			}

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
		// draw firework flying anim
		while (GetTime() - startFly < randFlyTimeS && !WindowShouldClose()) {
			int currentHeight = (int)(height - (yLaunch * ((GetTime() - startFly) / randFlyTimeS)));
			BeginDrawing();
			ClearBackground(BLACK);
			DrawPixel(xLaunch, currentHeight, WHITE);
			EndDrawing();
		}

		// draw explosion anim
		double startBurst = GetTime();
		while (GetTime() - startBurst < (BURST_TIME / 1000.) && !WindowShouldClose()) {
			BeginDrawing();
			ClearBackground(BLACK);
			for (int i = 0; i < NUM_SPARKLES; i++) {
				Sparkle* currentSparkle = &sparkleMemory[i];
				// update position
				double unitVector[2] = { cos((*currentSparkle).angle * (PI / 180.)), sin((*currentSparkle).angle * (PI / 180.)) };
				double velFactor = (*currentSparkle).vel * GetFrameTime();
				double adjustedVector[2] = { unitVector[0] * velFactor, unitVector[1] * velFactor };
				(*currentSparkle).pos[0] += adjustedVector[0];
				(*currentSparkle).pos[1] += adjustedVector[1];

				// draw
				//DrawRectangle((*currentSparkle).pos[0], (*currentSparkle).pos[1], 10, 10, (*currentSparkle).color);
				DrawPixel((int)(*currentSparkle).pos[0], (int)(*currentSparkle).pos[1], (*currentSparkle).color);
			}
			EndDrawing();
		}
	}

	// de-initialization
	std::free(sparkleMemory);
	sparkleMemory = nullptr;
	CloseWindow();
}