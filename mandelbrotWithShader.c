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
int main() {
	
	InitWindow((int)width, (int)height, "mandelbrot set");

	RenderTexture2D target = LoadRenderTexture(width, height);
	Shader shader = LoadShader(0, "./shader.glsl");
	
	while (!WindowShouldClose()) {
		BeginTextureMode(target);
			ClearBackground(BLACK); // clear render texture
			DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), BLACK);
		EndTextureMode();
		BeginDrawing();
			ClearBackground(RAYWHITE); // clear screen background

			BeginShaderMode(shader);
				DrawTextureEx(target.texture, (Vector2) { 0.f, 0.f }, 0.0f, 1.0f, WHITE);
			EndShaderMode();
			DrawFPS(10, 10);
		EndDrawing();
	}
	
	UnloadShader(shader);
	UnloadRenderTexture(target);
	
	CloseWindow();
	return 0;
}