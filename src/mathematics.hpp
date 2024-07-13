#pragma once

#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846  /* pi from math.h */
#endif

#define DEG_TO_RAD M_PI / 180

typedef struct Vec4 {
	float x, y, z, w;
} Vec4;

typedef struct Vec3 {
	float x, y, z;
	Vec3 operator+(const Vec3& other) const;
	Vec3 operator-(const Vec3& other) const;
	Vec3 operator*(float f) const;
	bool operator!=(const Vec3& other) const;
} Vec3;

typedef struct Vec2 {
	float x, y;
} Vec2;

void clamp(float* v, float lower, float upper);
void clamp(int* v, int lower, int upper);
float lerp(float lower, float upper, float t);
void move_towards(float* x, float target, float speed, float dt);

void move_towards(Vec3* vec, const Vec3& target, float speed, float dt);

float dot(const Vec3& a, const Vec3& b);
Vec3 cross(const Vec3& a, const Vec3& b);
void normalize_or_z_axis(Vec3* v);

// -pi to pi
float angle(const Vec2& a, const Vec2& b);


typedef struct Mat4 {
	// column major format (looks flipped here)

	float u11, u21, u31, u41;
	float u12, u22, u32, u42;
	float u13, u23, u33, u43;
	float u14, u24, u34, u44;

	Mat4 operator*(const Mat4& other) const;
	Vec4 operator*(const Vec4& other) const;
} Mat4;


extern const Mat4 mat4_unit;

float matrix_det(const Mat4& mat);

Mat4 matrix_transposed(const Mat4& mat);
Mat4 matrix_from_basis_vectors(Vec3 x, Vec3 y, Vec3 z);

// roatation around x-axis, y-axis then z-axis (alpha, beta, gamma).
Mat4 matrix_rotation_euler(float alpha, float beta, float gamma);
Mat4 matrix_translation(const Vec3& translation);
Mat4 matrix_scale(const Vec3& scales);
Mat4 matrix_scale(float scale);
Mat4 matrix_unit();

Mat4 matrix_look_at(Vec3 eye, Vec3 target, Vec3 up);
Mat4 matrix_camera(Vec3 pos, Vec3 looking_direction, Vec3 up);

Mat4 matrix_perspective(float vertical_fov, float aspect, float near, float far);
Mat4 matrix_perspective_orthographic(float left, float right, float top, float bottom, float near, float far);
