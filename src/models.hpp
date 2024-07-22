#pragma once

#include "mathematics.hpp"

// 3d rendering stuff

struct Vertex_Info {
	Vec3 position;
	Vec3 normal;
	Vec2 uv;
};

struct Vertex_Info_Array {
	size_t capacity;
	size_t count;
	Vertex_Info* data;
};

struct Model {
	Vertex_Info_Array mesh;
};

#define MAX_FRAMES_PER_ANIMATION 128
struct Animated_Model {
	size_t count;
	Vertex_Info_Array meshes[MAX_FRAMES_PER_ANIMATION];
};

enum Orientation {
	NO_ORIENTATION = 0,
	NORTH,
	EAST,
	SOUTH,
	WEST,
};

void construct_cube_triangles(Model* model);
void construct_tile_triangles(Model* model);
void construct_ramp_triangles(Model* model, Orientation ramp_orientation);

void construct_normals(Model* model);

// loading function for my own 3d model format
Vertex_Info_Array load_mesh_bada_file(const char* file_path);

// @Temporary
Animated_Model load_anim_bada_file(const char* file_path);
