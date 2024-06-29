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
