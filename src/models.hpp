// Copyright (C) 2024 Benjamin Froelich
// This file is part of https://github.com/bbeni/earth_bender_game
// For conditions of distribution and use, see copyright notice in project root.

#pragma once

#include "mathematics.hpp"
#include "ray.hpp"

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
	Box  bounding_box;
};

#define MAX_FRAMES_PER_ANIMATION 128
struct Animated_Model {
	size_t count;
	Mesh   meshes[MAX_FRAMES_PER_ANIMATION];
	Box    bounding_box;
	float  scale;
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
Static_Model load_model_bada_file(const char* file_path);
Animated_Model load_anim_bada_file(const char* file_path);

Box find_bounding_box(Mesh* mesh);

struct Box_Line_Model {
	Vec3 points[12 * 2]; // 12 edges * 2 points
};

//editor stuff
void construct_box_lines(Box_Line_Model* box);
Static_Model construct_box_lines();
