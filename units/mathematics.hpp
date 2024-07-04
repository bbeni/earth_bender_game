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
} Vec3;

typedef struct Vec2 {
	float x, y;
} Vec2;

void clamp(float* v, float lower, float upper);
float lerp(float lower, float upper, float t);

float dot(const Vec3& a, const Vec3& b);
Vec3 cross(const Vec3& a, const Vec3& b);
void normalize(Vec3* v);


typedef struct Mat4 {
	// row major format

	float u11, u12, u13, u14;
	float u21, u22, u23, u24;
	float u31, u32, u33, u34;
	float u41, u42, u43, u44;

	Mat4 operator*(const Mat4& other) const;
	Vec4 operator*(const Vec4& other) const;
} Mat4;


extern const Mat4 mat4_unit;

Mat4 transpose(const Mat4& mat);

// roatation around x-axis, y-axis then z-axis (alpha, beta, gamma).
Mat4 matrix_rotation_euler(float alpha, float beta, float gamma);
Mat4 matrix_translation(const Vec3& translation);
Mat4 matrix_scale(const Vec3& scales);
Mat4 matrix_scale(float scale);
Mat4 matrix_unit();

Mat4 matrix_look_at(const Vec3& from, const Vec3& to, const Vec3& up);
Mat4 matrix_perspective(float vertical_fov, float aspect, float near, float far);
Mat4 matrix_perspective_orthographic(float left, float right, float top, float bottom, float near, float far);
