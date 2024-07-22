#include "mathematics.hpp"
#include <math.h>
#include <cassert>

void clamp(float* v, float lower, float upper) {
	if (*v < lower) {
		*v = lower;
		return;
	}

	if (*v > upper) {
		*v = upper;
	}
}

void clamp(int* v, int lower, int upper) {
	if (*v < lower) {
		*v = lower;
		return;
	}

	if (*v > upper) {
		*v = upper;
	}
}


float lerp(float lower, float upper, float t) {
	return lower * (1.0f - t) + upper * t;
}


void move_towards(float* x, float target, float speed, float dt) {
	if (*x == target) {
		return;
	}
	if (*x > target) {
		*x -= speed * dt;
		if (*x < target) {
			*x = target;
		}
	}
	else {
		*x += speed * dt;
		if (*x > target) {
			*x = target;
		}
	}
}

void move_towards(Vec3* vec, const Vec3& target, float speed, float dt) {
	move_towards(&vec->x, target.x, speed, dt);
	move_towards(&vec->y, target.y, speed, dt);
	move_towards(&vec->z, target.z, speed, dt);
}

// move an angle to a range [-pi, pi) closer to the mapped target angle the shorter way around the circle
// @Bug somehow there is and edge case where the target is PI and we go to -PI and never set to PI but it should flip it here
// for now just assume the user of this function is not dependent on the wrap around behaviour
void move_towards_on_circle(float* angle, float target,  float speed, float dt) {

	//*angle = fmod(*angle + M_PI, 2 * M_PI) - M_PI; // map to [-pi, pi)
	// we should use fmod.. but for now thats ok what is done here
	// maybe I am using fmod the wrong way.. the asserts fired using fmod..
	while (*angle >= M_PI) {
		*angle -= 2 * M_PI;
	}
	
	while (*angle < -M_PI) {
		*angle += 2 * M_PI;
	}

	assert(*angle < M_PI);
	assert(*angle >= -M_PI);
	
	if (*angle + M_PI < target) {
		move_towards(angle, target - 2*M_PI, speed, dt);
	} else if (*angle - M_PI > target) {
		move_towards(angle, target + 2*M_PI, speed, dt);
	}
	else {
		move_towards(angle, target, speed, dt);
	}
} 


float angle_between(const Vec2& a, const Vec2& b) {
	float det = a.x * b.y - a.y * b.x;
	float dot = a.x * b.x + a.y * b.y;
	return atan2f(det, dot);
}

void normalize_or_y_axis(Vec2* v) {
	float denom = sqrtf(v->x * v->x + v->y * v->y);
	if (denom == 0.0f) {
		*v = Vec2{ 0.0f, 1.0f };
		return;
	}
	float factor = 1 / denom;
	v->x *= factor;
	v->y *= factor;
}


float dot(const Vec3& a, const Vec3& b) {
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

float dot(const Vec2& a, const Vec2& b) {
	return a.x * b.x + a.y * b.y;
}

float length(const Vec3& vec) {
	return sqrtf(dot(vec, vec));
}

float length(const Vec2& vec) {
	return sqrtf(dot(vec, vec));
}

void normalize_or_z_axis(Vec3* v) {
	float denom = sqrtf(v->x * v->x + v->y * v->y + v->z * v->z);
	if (denom == 0.0f) {
		*v = Vec3{ 0.0f, 0.0f, 1.0f };
		return;
	}
	float factor = 1 / denom;
	v->x *= factor;
	v->y *= factor;
	v->z *= factor;
}

Vec3 cross(const Vec3& a, const Vec3& b) {
	Vec3 c = {a.y * b.z - a.z * b.y,
			  a.z * b.x - a.x * b.z,
			  a.x * b.y - a.y * b.x };
	return c;
}

Vec3 Vec3::operator+(const Vec3& other) const {
	Vec3 v = {
		x + other.x,
		y + other.y,
		z + other.z,
	};
	return v;
}

Vec3 Vec3::operator-(const Vec3& other) const {
	Vec3 v = {
		x - other.x,
		y - other.y,
		z - other.z,
	};
	return v;
}

Vec3 Vec3::operator*(float f) const {
	return Vec3{ f * x, f * y, f * z };
}

bool Vec3::operator!=(const Vec3& other) const {
	return x != other.x || y != other.y || z != other.z;
}



Vec2 Vec2::operator+(const Vec2& other) const {
	Vec2 v = {
		x + other.x,
		y + other.y,
	};
	return v;
}

Vec2 Vec2::operator-(const Vec2& other) const {
	Vec2 v = {
		x - other.x,
		y - other.y,
	};
	return v;
}

Vec2& Vec2::operator+=(const Vec2& other) {
	x += other.x;
	y += other.y;
	return *this;
}

Vec2& Vec2::operator-=(const Vec2& other) {
	x -= other.x;
	y -= other.y;
	return *this;
}


Vec2 Vec2::operator*(float f) const {
	return Vec2{ f * x, f * y};
}

bool Vec2::operator!=(const Vec2& other) const {
	return x != other.x || y != other.y;
}



const Mat4 mat4_unit =
{
	1.0f, 0, 0, 0,
	0, 1.0f, 0, 0,
	0, 0, 1.0f, 0,
	0, 0, 0, 1.0f,
};

Mat4 Mat4::operator*(const Mat4& other) const {
	Mat4 m = { 0 };

	m.u11 = u11 * other.u11 + u12 * other.u21 + u13 * other.u31 + u14 * other.u41;
	m.u12 = u11 * other.u12 + u12 * other.u22 + u13 * other.u32 + u14 * other.u42;
	m.u13 = u11 * other.u13 + u12 * other.u23 + u13 * other.u33 + u14 * other.u43;
	m.u14 = u11 * other.u14 + u12 * other.u24 + u13 * other.u34 + u14 * other.u44;

	m.u21 = u21 * other.u11 + u22 * other.u21 + u23 * other.u31 + u24 * other.u41;
	m.u22 = u21 * other.u12 + u22 * other.u22 + u23 * other.u32 + u24 * other.u42;
	m.u23 = u21 * other.u13 + u22 * other.u23 + u23 * other.u33 + u24 * other.u43;
	m.u24 = u21 * other.u14 + u22 * other.u24 + u23 * other.u34 + u24 * other.u44;

	m.u31 = u31 * other.u11 + u32 * other.u21 + u33 * other.u31 + u34 * other.u41;
	m.u32 = u31 * other.u12 + u32 * other.u22 + u33 * other.u32 + u34 * other.u42;
	m.u33 = u31 * other.u13 + u32 * other.u23 + u33 * other.u33 + u34 * other.u43;
	m.u34 = u31 * other.u14 + u32 * other.u24 + u33 * other.u34 + u34 * other.u44;

	m.u41 = u41 * other.u11 + u42 * other.u21 + u43 * other.u31 + u44 * other.u41;
	m.u42 = u41 * other.u12 + u42 * other.u22 + u43 * other.u32 + u44 * other.u42;
	m.u43 = u41 * other.u13 + u42 * other.u23 + u43 * other.u33 + u44 * other.u43;
	m.u44 = u41 * other.u14 + u42 * other.u24 + u43 * other.u34 + u44 * other.u44;

	return m;
}

Vec4 Mat4::operator*(const Vec4& other) const {
	Vec4 v = { 0 };

	v.x = u11 * other.x + u12 * other.y + u13 * other.z + u14 * other.w;
	v.y = u21 * other.x + u22 * other.y + u23 * other.z + u24 * other.w;
	v.z = u31 * other.x + u32 * other.y + u33 * other.z + u34 * other.w;
	v.w = u41 * other.x + u42 * other.y + u43 * other.z + u44 * other.w;

	return v;
}


float matrix_det(const Mat4& m) {
	// leibnitz fomula

	float result =
		m.u11 * m.u22 * m.u33 * m.u44 + m.u11 * m.u32 * m.u43 * m.u24 + m.u11 * m.u42 * m.u23 * m.u34 +
		m.u21 * m.u12 * m.u43 * m.u34 + m.u21 * m.u32 * m.u13 * m.u44 + m.u21 * m.u42 * m.u33 * m.u14 +
		m.u31 * m.u12 * m.u23 * m.u44 + m.u31 * m.u22 * m.u43 * m.u14 + m.u31 * m.u42 * m.u13 * m.u24 +
		m.u41 * m.u12 * m.u33 * m.u24 + m.u41 * m.u22 * m.u13 * m.u34 + m.u41 * m.u32 * m.u23 * m.u14 -
		m.u11 * m.u22 * m.u43 * m.u34 - m.u11 * m.u32 * m.u23 * m.u44 - m.u11 * m.u42 * m.u33 * m.u24 -
		m.u21 * m.u12 * m.u33 * m.u44 - m.u21 * m.u32 * m.u43 * m.u14 - m.u21 * m.u42 * m.u13 * m.u34 -
		m.u31 * m.u12 * m.u43 * m.u24 - m.u31 * m.u22 * m.u13 * m.u44 - m.u31 * m.u42 * m.u23 * m.u14 -
		m.u41 * m.u12 * m.u23 * m.u34 - m.u41 * m.u22 * m.u33 * m.u14 - m.u41 * m.u32 * m.u13 * m.u24;

	return result;
}


Mat4 matrix_transposed(const Mat4& mat) {
	Mat4 m = { 0 };
	m.u11 = mat.u11;
	m.u12 = mat.u21;
	m.u13 = mat.u31;
	m.u14 = mat.u41;
	
	m.u21 = mat.u12;
	m.u22 = mat.u22;
	m.u23 = mat.u32;
	m.u24 = mat.u42;
	
	m.u31 = mat.u13;
	m.u32 = mat.u23;
	m.u33 = mat.u33;
	m.u34 = mat.u43;

	m.u41 = mat.u14;
	m.u42 = mat.u24;
	m.u43 = mat.u34;
	m.u44 = mat.u44;
	return m;
}

Mat4 matrix_from_basis_vectors(Vec3 x, Vec3 y, Vec3 z) {
	Mat4 result = { 0 };
	result.u44 = 1.0f;

	result.u11 = x.x;
	result.u21 = x.y;
	result.u31 = x.z;

	result.u12 = y.x;
	result.u22 = y.y;
	result.u32 = y.z;

	result.u13 = z.x;
	result.u23 = z.y;
	result.u33 = z.z;

	return result;
}


// roatation around x-axis, y-axis then z-axis (alpha, beta, gamma).
Mat4 matrix_rotation_euler(float alpha, float beta, float gamma) {
	Mat4 m = { 0 };

	float sa = sinf(alpha);
	float ca = cosf(alpha);

	float sb = sinf(beta);
	float cb = cosf(beta);

	float sc = sinf(gamma);
	float cc = cosf(gamma);

	m.u11 = cb * cc;
	m.u12 = sa * sb * cc - ca * sc;
	m.u13 = ca * sb * cc + sa * sc;

	m.u21 = cb * sc;
	m.u22 = sa * sb * sc + ca * cc;
	m.u23 = ca * sb * sc - sa * cc;

	m.u31 = -sb;
	m.u32 = sa * cb;
	m.u33 = ca * cb;

	m.u44 = 1.0f;

	return m;
}

Mat4 matrix_translation(const Vec3& translation) {
	Mat4 m = { 0 };
	m.u11 = 1.0f;
	m.u22 = 1.0f;
	m.u33 = 1.0f;
	m.u14 = translation.x;
	m.u24 = translation.y;
	m.u34 = translation.z;
	m.u44 = 1.0f;
	return m;
}

Mat4 matrix_scale(const Vec3& scales) {
	Mat4 m = { 0 };
	m.u11 = scales.x;
	m.u22 = scales.y;
	m.u33 = scales.z;
	m.u44 = 1.0f;
	return m;
}

Mat4 matrix_scale(float scale) {
	Vec3 s = { scale, scale, scale };
	return matrix_scale(s);
}


Mat4 matrix_unit() {
	Mat4 m = { 0 };
	m.u11 = 1.0f;
	m.u22 = 1.0f;
	m.u33 = 1.0f;
	m.u44 = 1.0f;
	return m;
}


Mat4 matrix_camera(Vec3 pos, Vec3 looking_direction, Vec3 up) {
	return matrix_look_at(pos, pos + looking_direction, up);
}

Mat4 matrix_look_at(Vec3 eye, Vec3 target, Vec3 up) {

	Vec3 forward = target - eye;
	normalize_or_z_axis(&forward);
	Vec3 side = cross(forward, up);
	normalize_or_z_axis(&side);
	up = cross(side, forward);
	normalize_or_z_axis(&up);

	Mat4 result = { 0 };

	result.u11 = side.x;
	result.u12 = side.y;
	result.u13 = side.z;

	result.u21 = up.x;
	result.u22 = up.y;
	result.u23 = up.z;

	result.u31 = -forward.x;
	result.u32 = -forward.y;
	result.u33 = -forward.z;

	result.u14 = -dot(side, eye);
	result.u24 = -dot(up, eye);
	result.u34 = dot(forward, eye);

	result.u44 = 1.0f;

	return result;
}

// https://www.songho.ca/opengl/gl_projectionmatrix.html
// vertical_fov in degrees
Mat4 matrix_perspective(float vertical_fov, float aspect, float near, float far) {

	Mat4 mat = { 0 };

	float tan_half_fov = tanf(vertical_fov * 0.5f * DEG_TO_RAD);
	float top = near * tan_half_fov;
	float right = top * aspect;
	float inv_dist = 1.0f / (far - near);

	mat.u11 = near / right;
	mat.u22 = near / top;
	mat.u33 = -(near + far) * inv_dist;
	mat.u43 = -1.0f; // w = -z
	mat.u34 = -(2 * near * far) * inv_dist;
	mat.u44 = 0.0f;

	return mat;
}

// -1 to 1 
Mat4 matrix_perspective_orthographic(float left, float right, float top, float bottom, float near, float far) {
	Mat4 m = {0};

	m.u11 = 2.0f / (right - left);
	m.u22 = 2.0f / (top - bottom);
	m.u33 = -2.0f / (far - near);
	
	m.u14 = -(right + left) / (right - left);
	m.u24 = -(top + bottom) / (top - bottom);
	m.u34 = -(far + near) / (far - near);
	
	m.u44 = 1.0f;

	return m;
}