#include "mathematics.hpp"
#include <math.h>

void clamp(float* v, float lower, float upper) {
	if (*v < lower) {
		*v = lower;
		return;
	}

	if (*v > upper) {
		*v = upper;
		return;
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



float dot(const Vec3& a, const Vec3& b) {
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

void normalize(Vec3* v) {
	float factor = 1 / sqrtf(v->x * v->x + v->y * v->y + v->z * v->z);
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
	v.y = u11 * other.x + u12 * other.y + u13 * other.z + u14 * other.w;
	v.z = u11 * other.x + u12 * other.y + u13 * other.z + u14 * other.w;
	v.w = u11 * other.x + u12 * other.y + u13 * other.z + u14 * other.w;

	return v;
}

Mat4 transpose(const Mat4& mat) {
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
	m.u41 = translation.x;
	m.u42 = translation.y;
	m.u43 = translation.z;
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

	Vec3 right = cross(looking_direction, up);

	normalize(&looking_direction);
	normalize(&up);
	normalize(&right);

	up = cross(right, looking_direction);

	Mat4 result = { 0 };

	result.u11 = -right.x;
	result.u12 = -right.y;
	result.u13 = -right.z;

	result.u21 = up.x;
	result.u22 = up.y;
	result.u23 = up.z;

	result.u31 = -looking_direction.x;
	result.u32 = -looking_direction.y;
	result.u33 = -looking_direction.z;

	result.u41 = -dot(right, pos);
	result.u42 = -dot(up, pos);
	result.u43 = dot(looking_direction, pos);

	result.u44 = 1.0f;
	return result;
}

Mat4 matrix_look_at(const Vec3& camera, const Vec3& target, const Vec3& up_axis) {

	Mat4 result = { 0 };

	Vec3 eye = target - camera;
	Vec3 forward = eye;
	normalize(&forward);

	Vec3 real_up = up_axis - forward * dot(up_axis, forward); 

	Vec3 right = cross(real_up, forward);
	normalize(&right);

	Vec3 new_up = cross(forward, right);

	result.u11 = right.x;
	result.u21 = right.y;
	result.u31 = right.z;
	
	result.u12 = new_up.x;
	result.u22 = new_up.y;
	result.u32 = new_up.z;
	
	result.u13 = forward.x;	
	result.u23 = forward.y;
	result.u33 = forward.z;
	
	result.u41 = -dot(right, eye);
	result.u42 = -dot(new_up, eye);
	result.u43 = -dot(forward, eye);

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
	mat.u34 = -1.0f; // w = -z
	mat.u43 = -(2 * near * far) * inv_dist;
	mat.u44 = 0.0f;

	return mat;
}

// -1 to 1 
Mat4 matrix_perspective_orthographic(float left, float right, float top, float bottom, float near, float far) {
	Mat4 m = {0};

	m.u11 = 2.0f / (right - left);
	m.u22 = 2.0f / (top - bottom);
	m.u33 = -2.0f / (far - near);
	
	m.u41 = -(right + left) / (right - left);
	m.u42 = -(top + bottom) / (top - bottom);
	m.u43 = -(far + near) / (far - near);
	
	m.u44 = 1.0f;

	return m;
}