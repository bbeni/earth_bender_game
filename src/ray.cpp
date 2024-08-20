#include "ray.hpp"

#include <cassert>
#include <cfloat>

#define TOLARANCE 0.0000001f

float dist_to_box(Box box, Vec3 point) {

	float dx = fmaxf(box.min.x - point.x, point.x - box.max.x);
	float dy = fmaxf(box.min.y - point.y, point.y - box.max.y);
	float dz = fmaxf(box.min.z - point.z, point.z - box.max.z);

	if (dx < 0 && dy < 0 && dz < 0) {
		// we are inside the box
		float min_dist_sq = fminf(dx * dx, dy * dy);
		min_dist_sq = fminf(min_dist_sq, dz * dz);
		return -sqrtf(min_dist_sq);
	}

	dx = fmaxf(0, dx);
	dy = fmaxf(0, dy);
	dz = fmaxf(0, dz);

	return sqrtf(dx * dx + dy + dy + dz * dz);
}

bool ray_does_hit(Ray ray, Box box, Vec3 *out_position) {
	float dist = dist_to_box(box, ray.origin);
	Vec3 possible_point = ray.origin + ray.direction * dist;
	float dist2 = dist_to_box(box, possible_point);
	if (dist2 < TOLARANCE) {
		if (out_position != NULL) {
			*out_position = possible_point;
		}
		return true;
	}
	return false;
}

Ray_Cast_Result ray_cast(Ray ray, Box* boxes, size_t count)
{
	auto res = Ray_Cast_Result{ 0 };

	float min_dist = FLT_MAX;

	for (int i = 0; i < count; i++) {
		// first find out if we are inside the bounding sphere of the box
		Box* b = &boxes[i];
		Vec3 center = (b->max + b->min) * 0.5f;
		float radius = length(b->max - b->min) * 0.5f;
		float t = dot(ray.direction, center - ray.origin);
		float x = length(ray.origin + ray.direction * t - center);
		float f = x;
		if (x <= radius) {
			// we hit the sphere
			// TODO: check for actual hit and continue
			
			// TODO: get actual hit position not center of sphere
			float center_dist = length(center - ray.origin);
			if (center_dist > min_dist) {
				// we are further away than the last hit
				assert(res.did_hit);
				continue;
			}
			min_dist = center_dist;

			res.did_hit = true;
			res.hit_position = center;
			res.hit_index = i;
			break;
		}
	}

	return res;
}
