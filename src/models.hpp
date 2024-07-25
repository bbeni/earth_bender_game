#pragma once

#include "mathematics.hpp"

// 3d rendering stuff

struct Vertex_Info {
	Vec3 position;
	Vec3 normal;
	Vec2 uv;
};

// A mesh that consists of triangles
struct Mesh {
	size_t capacity;
	size_t count;
	Vertex_Info* data;
};

struct Static_Model {
	Mesh mesh;
};


#define MAX_FRAMES_PER_ANIMATION 128
struct Animated_Model {
	size_t count;
	Mesh meshes[MAX_FRAMES_PER_ANIMATION];
};


enum Ramp_Orientation {
	FLAT = 0,
	NORTH,
	EAST,
	SOUTH,
	WEST,
};


void construct_cube_triangles(Static_Model* model);
void construct_tile_triangles(Static_Model* model);
void construct_ramp_triangles(Static_Model* model, Ramp_Orientation ramp_orientation);

void construct_normals(Static_Model* model);

// make a cube with verts, normals and uvs set
void make_cube_model(Static_Model* model);


// loading function for my own 3d model format
Mesh load_mesh_bada_file(const char* file_path);

// @Temporary
Animated_Model load_anim_bada_file(const char* file_path);
