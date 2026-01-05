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
// Hacky vector pointer to duplicate singular data
typedef struct {
	const float *x, *y;
} Vector2Ref;
static Vector2 V2fromRef(Vector2Ref v){
	return (Vector2) { *v.x, * v.y };
}
// Complex
typedef struct c32 {
	float r, i;
} c32;
static c32 c32_mul(c32 a, c32 b) {
	return (c32) {
		a.r* b.r - a.i * b.i,
			a.i* b.r + a.r * b.i
	};
}
static c32 c32_add(c32 a, c32 b) {
	return (c32) { a.r + b.r, a.i + b.i };
}
static float c32_abs_sq(c32 n) {
	return n.i * n.i + n.r + n.r;
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

float Width = 600, Height = 400;
Vector2 windowStart = {0.f, 0.f};
int main() {
	Vector2 position = { -0.3f, 0.f }; // use this to move the camera
	float scale = 1; // use this to zoom the camera
	int Iterations = 100;
	Vector2 offset = { 0.f, 0.f }; // this is to re-center the display for rectangular screens.

	Vector2Ref scaleV = { &scale, &scale }; // for vector reference

	InitWindow((int)Width, (int)Height, "mandelbrot set");

	Image imageBuffer = GenImageColor((int)Width, (int)Height, BLACK);
	Texture displayTexture = LoadTextureFromImage(imageBuffer);
	
	if (Width > Height) { // fixes the center in the middle of the screen, even if viewed from a rectangular screen
		offset.x = 0.5f * (Width - Height);
	} else {
		offset.y = 0.5f * (Height - Width);
	}

	float win = (float)(Width > Height) ? Height : Width;
	while (!WindowShouldClose()) {
		ClearBackground(RAYWHITE);
		if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
			Vector2 scaleV = { scale, scale };
			Vector2 mouseProjectedPosition = V2_map(GetMousePosition(), windowStart, GetWindowPosition(), V2_sub(position, scaleV), V2_add(position, scaleV));
			position = V2_lerp(position, GetMousePosition(), 0.1);
			
		}
		for (int i = 0; i < Width; i++) {
			for (int j = 0; j < Height; j++) {
				// Convert pixels to complex coordinates on the plane.
				float x = map32(i, offset.x, win + offset.x, position.x - scale, position.x + scale);
				float y = map32(j, offset.y, win + offset.y, position.y - scale, position.y + scale);
				c32 point = { x, y };
				int Iteration = 0;
				recursePoint(&Iteration, Iterations, point);

				if (Iteration == Iterations) {
					ImageDrawPixel(&imageBuffer, i, j, BLACK);
				} else {
					float scaled = 255.0 / Iterations * Iteration;
					ImageDrawPixel(&imageBuffer, i, j, ColorFromHSV(scaled, 255, 255));
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