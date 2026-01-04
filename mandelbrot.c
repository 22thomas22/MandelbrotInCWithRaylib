#include "raylib.h"
#include <stdlib.h> // malloc, free, rand, exit
#include <stdio.h> // printf, scanf, fopen
#include <math.h> // sin, sqrt, pow, etc




typedef struct c32 {
	float r, i;
} c32;
c32 c32_mul(c32 a, c32 b) {
	return (c32){
		a.r * b.r - a.i * b.i,
		a.i * b.r + a.r * b.i
	};
}
c32 c32_add(c32 a, c32 b) {
	return (c32){ a.r + b.r,a.i + b.i };
}
float c32_abs(c32 n) {
	return sqrtf(n.i * n.i + n.r * n.r);
}

float map32(float Value, float ValMin, float ValMax, float MappedMin, float MappedMax) {
	float _scale = (MappedMax - MappedMin) / (ValMax - ValMin);
	return MappedMin + _scale * (Value - ValMin);
}

float Width = 600, Height = 400;
int Iterations = 100;

int main() {
	float OffsetX = 0.f, OffsetY = 0.f;
	float Scale = 1;
	Vector2 Position = {-0.3f, 0.f};
	InitWindow((int)Width, (int)Height, "mandelbrot set");
	if (Width > Height) { // fixes the center in the middle of the screen, even if viewed from a rectangular screen
		OffsetX = 0.5f * (Width - Height);
	} else {
		OffsetY = 0.5f * (Height - Width);
	}
	float Win = (float)(Width > Height) ? Height : Width;
	while (!WindowShouldClose()) {
		BeginDrawing();
		ClearBackground(RAYWHITE);
		for (int i = 0; i < Width; i++) {
			for (int j = 0; j < Height; j++) {
				float x = map32(i, 0.0 + OffsetX, Win + OffsetX, Position.x - Scale, Position.x + Scale);
				float y = map32(j, 0.0 + OffsetY, Win + OffsetY, Position.y - Scale, Position.y + Scale);
				c32 Point = { x, y };
				c32 ChangingPt = Point;
				int Iteration = 0;
				while (++Iteration < Iterations) {
					//z→z^2+c
					ChangingPt = c32_add(c32_mul(ChangingPt, ChangingPt), Point);
					if (c32_abs(ChangingPt) > 2) {
						break;
					}
				}
				if (c32_abs(ChangingPt) < 2) {
					DrawPixel(i, j, BLACK);
				} else {
					float scaled = 255 / Iterations * Iteration;
					DrawPixel(i, j, ColorFromHSV(scaled, 255, 255));
				}
			}
		}
		
		DrawRectangle(0, 0, 20, 20, BLUE);
		EndDrawing();
		
	}
	return 0;
}