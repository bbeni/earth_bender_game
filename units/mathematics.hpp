#pragma once


typedef struct Vec4 {
	float x, y, z, w;
} Vec4;

typedef struct Vec3 {
	float x, y, z;
} Vec3;

typedef struct Vec2 {
	float x, y;
} Vec2;

void clamp(float* v, float lower, float upper);