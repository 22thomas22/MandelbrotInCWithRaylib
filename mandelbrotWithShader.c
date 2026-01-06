#include "raylib.h"
#include <stdlib.h> // malloc, free, rand, exit
#include <stdio.h> // printf, scanf, fopen
#include <math.h> // sin, sqrt, pow, etc

//*
// Goals
// Make Windows Sceensaver
// Add other systems: https://www.jwz.org/xscreensaver/
// Run with GPU
// *//

#if defined(PLATFORM_DESKTOP)
#define GLSL_VERSION            330
#else   // PLATFORM_ANDROID, PLATFORM_WEB
#define GLSL_VERSION            100
#endif
// Mapping
static float lerp32(float p1, float p2, float x) {
	return p1 + (p2 - p1) * x;
}
static float map32(float Value, float ValMin, float ValMax, float MappedMin, float MappedMax) {
	float _scale = (MappedMax - MappedMin) / (ValMax - ValMin);
	return MappedMin + _scale * (Value - ValMin);
}

// Vector Helper Functions
static Vector2 V2_add(Vector2 a, Vector2 b) {
	return (Vector2) { a.x + b.x, a.y + b.y };
}
static Vector2 V2_sub(Vector2 a, Vector2 b) {
	return (Vector2) { a.x - b.x, a.y - b.y };
}
static Vector2 V2_lerp(Vector2 a, Vector2 b, float val) {
	return (Vector2){
		a.x + (b.x - a.x) * val,
		a.y + (b.y - a.y) * val
	};
}
static Vector2 V2_map(Vector2 value, Vector2 valMin, Vector2 valMax, Vector2 mappedMin, Vector2 mappedMax) {
	Vector2 _scale = {
		(mappedMax.x - mappedMin.x) / (valMax.x - valMin.x),
		(mappedMax.y - mappedMin.y) / (valMax.y - valMin.y)
	};
	return (Vector2) {
		mappedMin.x + _scale.x * (value.x - valMin.x),
		mappedMin.y + _scale.y * (value.y - valMin.y)
	};
}
static float V2_distBetween(Vector2 a, Vector2 b) {
	Vector2 diff = V2_sub(b, a);
	return sqrtf(diff.x * diff.x + diff.y * diff.y);
}
// Hacky vector pointer to duplicate singular data
typedef struct Vector2Ref {
	const float *x, *y;
} Vector2Ref;
static Vector2 V2fromRef(Vector2Ref v){
	return (Vector2) { *v.x, * v.y };
}

void V2_print(Vector2 v, bool addSpace) {
	printf("(%f, %f)", v.x, v.y);
	if (addSpace)
		printf("\n");
}
// Complex
typedef struct c32 {
	float r, i;
} c32;
static c32 c32_mul(c32 a, c32 b) {
	return (c32) {
		a.r * b.r - a.i * b.i,
		a.i * b.r + a.r * b.i
	};
}
static c32 c32_add(c32 a, c32 b) {
	return (c32) { a.r + b.r, a.i + b.i };
}
static float c32_abs_sq(c32 n) {
	return n.i * n.i + n.r * n.r;
}
static float c32_abs(c32 n) {
	return sqrtf(c32_abs_sq(n));
}
// Helper
static c32 recursePoint(int *Iteration, int _iterations, c32 Position) {
	c32 PositionDynamic = Position;
	while (++(*Iteration) < _iterations) {
		//z→z^2+c
		PositionDynamic = c32_add(c32_mul(PositionDynamic, PositionDynamic), Position);
		if (c32_abs_sq(PositionDynamic) > 4) {
			break;
		}
	}
	return PositionDynamic;
}

const float width = 600, height = 400;
bool moveWindow = true;
int main() {
	// variables with p... are for pixel calculations, variables with g.. are mapped from complex plane, w... deals with the window
	Vector2 gmiddle = { -0.3f, 0.f }; // use this to move the camera
	float gscale = 1; // use this to zoom the camera
	int iterations = 100;
	Vector2 poffset = { 0.f, 0.f };
	const Vector2 Vzero = { 0.f, 0.f };

	Vector2 windowSize = (Vector2){ width, height };
	Vector2 whalfWindowSize = (Vector2){ 0.5f * width, 0.5f * height };

	Vector2 pmouse, gmouse = {0.f, 0.f};

	Vector2Ref gscaleV = { &gscale, &gscale }; // for vector reference
	float psquare = (float)(width > height) ? height : width; // ensures the screen never stretches the fractal
	Vector2 psquareV = { psquare, psquare };

	InitWindow((int)width, (int)height, "mandelbrot set");

	//RenderTexture2D target = LoadRenderTexture(width, height);
	//Shader shader = LoadShader(0, TextFormat("./shader.fs", GLSL_VERSION));

	Image imageBuffer = GenImageColor((int)width, (int)height, BLACK);
	Texture displayTexture = LoadTextureFromImage(imageBuffer);
	
	if (width > height) { // fixes the center in the middle of the screen, even if viewed from a rectangular screen
		poffset.x = 0.5f * (width - height);
	} else {
		poffset.y = 0.5f * (height - width);
	}
	
	while (!WindowShouldClose()) {
		Vector2 wscreenMiddle = (Vector2){ GetMonitorWidth(0) * 0.5f, GetMonitorHeight(0) * 0.5f };
		Vector2 windowOffset = V2_add(V2_sub(GetWindowPosition(), wscreenMiddle), whalfWindowSize);
		pmouse = GetMousePosition();
		
		// precompute ingame bounds
		Vector2 gtopLeft = V2_sub(gmiddle, V2fromRef(gscaleV)), gbottomRight = V2_add(gmiddle, V2fromRef(gscaleV));
		
		if (moveWindow) {
			pmouse = V2_add(pmouse, windowOffset);
		}
		gmouse = V2_map(V2_sub(pmouse, poffset),
			Vzero,		psquareV,
			gtopLeft,	gbottomRight
		);
		
		ClearBackground(RAYWHITE);
		if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) || IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
			float scale = IsMouseButtonDown(MOUSE_BUTTON_LEFT) ? 0.9 : 1.1;
			gmiddle = V2_lerp(gmouse, gmiddle, scale);
			gscale *= scale;
		}
		
		for (int i = 0; i < width; i++) {
			for (int j = 0; j < height; j++) {
				Vector2 plocV = { i, j };
				if (moveWindow) {
					plocV = V2_add(plocV, windowOffset);
				}
				// Convert pixels to complex coordinates on the plane.
				Vector2 glocV = V2_map(V2_sub(plocV, poffset), // poffset pulls the square to the middle
					Vzero,		psquareV,
					gtopLeft,	gbottomRight
				);
				c32 glocC = { glocV.x, glocV.y };
				int iteration = 0;
				c32 gfinalPos = recursePoint(&iteration, iterations, glocC);
				/*if (V2_distBetween(gmouse, glocV) < 0.01) {
					ImageDrawPixel(&imageBuffer, i, j, RED);
				} else */if (iteration == iterations) {
					ImageDrawPixel(&imageBuffer, i, j, BLACK);
				} else {
					float scaled = 360.0 / (float)iterations * (float)iteration;
					ImageDrawPixel(&imageBuffer, i, j, ColorFromHSV(scaled, 1, 1));
				}
			}
		}
		
		UpdateTexture(displayTexture, imageBuffer.data);

		BeginDrawing();
		DrawTexture(displayTexture, 0, 0, WHITE);
		DrawFPS(10, 10);
		EndDrawing();
		
	}
	UnloadImage(imageBuffer);
	UnloadTexture(displayTexture);
	// cleanup
	CloseWindow();
	return 0;
}