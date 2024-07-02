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
float dot(Vec3& a, Vec3& b);

typedef struct Mat4 {
	float u11, u12, u13, u14;
	float u21, u22, u23, u24;
	float u31, u32, u33, u34;
	float u41, u42, u43, u44;
} Mat4;

extern Mat4 mat4_unit;