#include "mathematics.hpp"

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

float dot(const Vec3& a, const Vec3& b) {
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

Mat4 mat4_unit =
{
	1.0f, 0, 0, 0,
	0, 1.0f, 0, 0,
	0, 0, 1.0f, 0,
	0, 0, 0, 1.0f,
};