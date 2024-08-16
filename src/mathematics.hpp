#pragma once

#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846  /* pi from math.h */
#endif

#define DEG_TO_RAD M_PI / 180

struct Vec4 {
	float x, y, z, w;
};

struct Vec3 {
	float x, y, z;
	Vec3 operator+(const Vec3& other) const;
	Vec3 operator-(const Vec3& other) const;
	Vec3 operator*(float f) const;
	bool operator!=(const Vec3& other) const;
	Vec3& operator+=(const Vec3& other);
	Vec3& operator-=(const Vec3& other);
};

struct Vec2 {
	float x, y;
	Vec2 operator+(const Vec2& other) const;
	Vec2 operator-(const Vec2& other) const;
	Vec2 operator*(float f) const;
	bool operator!=(const Vec2& other) const;
	Vec2& operator+=(const Vec2& other);
	Vec2& operator-=(const Vec2& other);
};

void clamp(float* v, float lower, float upper);
void clamp(int* v, int lower, int upper);
float lerp(float lower, float upper, float t);
void move_towards(float* x, float target, float speed, float dt);
void move_towards(Vec3* vec, const Vec3& target, float speed, float dt);

// move towards the closer angle mapped from -pi to pi
void move_towards_on_circle(float* angle, float target,  float speed, float dt);

// common vector stuff
float dot(const Vec3& a, const Vec3& b);
float dot(const Vec2& a, const Vec2& b);
float length(const Vec3& vec);
float length(const Vec2& vec);

Vec3 cross(const Vec3& a, const Vec3& b);
void normalize_or_z_axis(Vec3* v);

// -pi to pi
float angle_between(const Vec2& a, const Vec2& b);
void normalize_or_y_axis(Vec2* v);
void normalize_or_zero(Vec2* v);
float length(const Vec2& vec);

struct Mat4 {
	// column major format (looks flipped here)

	float u11, u21, u31, u41;
	float u12, u22, u32, u42;
	float u13, u23, u33, u43;
	float u14, u24, u34, u44;

	Mat4 operator*(const Mat4& other) const;
	Vec4 operator*(const Vec4& other) const;
};


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

Mat4 matrix_perspective_projection(float vertical_fov, float aspect, float near, float far);
Mat4 matrix_orthographic_projection(float left, float right, float top, float bottom, float near, float far);

Mat4 model_rotation_0();
Mat4 model_rotation_90();
Mat4 model_rotation_180();
Mat4 model_rotation_270();

