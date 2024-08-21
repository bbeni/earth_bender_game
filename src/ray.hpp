#pragma once
#include "mathematics.hpp"

// a box specified by two diagonally opposite corners
struct Box {
	Vec3 min;
	Vec3 max;
};

struct Ray {
	Vec3 origin;
	Vec3 direction;
};

// returns negative when inside
float dist_to_box(Box box, Vec3 point);

struct Ray_Cast_Result {
	bool did_hit;
	Vec3 hit_position;
	size_t hit_index;
};

Ray_Cast_Result ray_cast(Ray ray, Box* boxes, size_t count);