#pragma once

#include "mathematics.hpp"

// 3d rendering stuff

typedef struct Vertex_Info {
	Vec3 position;
	Vec3 normal;
	// uv etc..
} Vertex_Info;

typedef struct Vertex_Info_Array {
	size_t capacity;
	size_t count;
	Vertex_Info* data;
} Vertex_Info_Array;

typedef struct Static_Model {
	Vertex_Info_Array mesh;
	//Mat4 projection;
} Static_Model;

enum Orientation {
	NO_ORIENTATION = 0,
	NORTH,
	EAST,
	SOUTH,
	WEST,
};

void construct_cube_triangles(Static_Model* model);
void construct_tile_triangles(Static_Model* model);
void construct_ramp_triangles(Static_Model* model, Orientation ramp_orientation);

void construct_normals(Static_Model* model);
