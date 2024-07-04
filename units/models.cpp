#include "models.hpp"

#include "data_stuff.h"

void construct_cube_triangles(Static_Model* model) {
	float s = 0.5f;

	Vertex_Info_Array* m = &model->mesh;

	for (int i = -1; i < 2; i += 2) {

		Vertex_Info v1 = { -s, -s, -s * i };
		Vertex_Info v2 = { s, -s, -s * i };
		Vertex_Info v3 = { s,  s, -s * i };
		Vertex_Info v4 = { -s,  s, -s * i };

		// seems legit
		d_append(m, v1);
		d_append(m, v2);
		d_append(m, v4);
		d_append(m, v4);
		d_append(m, v2);
		d_append(m, v3);
	}

	for (int i = -1; i < 2; i += 2) {

		// reverse corners for normals
		Vertex_Info v1 = { -s, -s * i, -s };
		Vertex_Info v4 = { s, -s * i, -s };
		Vertex_Info v3 = { s, -s * i,  s };
		Vertex_Info v2 = { -s, -s * i,  s };

		d_append(m, v1);
		d_append(m, v2);
		d_append(m, v4);
		d_append(m, v4);
		d_append(m, v2);
		d_append(m, v3);
	}


	for (int i = -1; i < 2; i += 2) {

		Vertex_Info v1 = { s * i, -s, -s };
		Vertex_Info v2 = { s * i,  s, -s };
		Vertex_Info v3 = { s * i,  s,  s };
		Vertex_Info v4 = { s * i, -s,  s };

		d_append(m, v1);
		d_append(m, v2);
		d_append(m, v4);
		d_append(m, v4);
		d_append(m, v2);
		d_append(m, v3);
	}

}


// TODO these functions are kinda dirty... should use transformation matrices?
void construct_tile_triangles(Static_Model* model) {

	construct_cube_triangles(model);
	Vertex_Info_Array* m = &model->mesh;

	for (int i = 0; i < m->count; i++) {
		if (m->data[i].position.y < 0.0f) {
			m->data[i].position.y *= 4.0f;
		}
	}
}


void construct_ramp_triangles(Static_Model* model, Orientation ramp_orientation) {

	construct_cube_triangles(model);
	Vertex_Info_Array* m = &model->mesh;



	for (int i = 0; i < m->count; i++) {
		if (m->data[i].position.y < 0.0f) {
			m->data[i].position.y *= 3.0f;
		}

		if (ramp_orientation == Orientation::NORTH)
		{
			if (m->data[i].position.y > 0.0f && m->data[i].position.z > 0.0f) {
				m->data[i].position.y *= 2.0f;
			}
		}
		if (ramp_orientation == Orientation::SOUTH)
		{
			if (m->data[i].position.y > 0.0f && m->data[i].position.z < 0.0f) {
				m->data[i].position.y *= 2.0f;
			}
		}
		if (ramp_orientation == Orientation::EAST)
		{
			if (m->data[i].position.y > 0.0f && m->data[i].position.x > 0.0f) {
				m->data[i].position.y *= 2.0f;
			}
		}
		if (ramp_orientation == Orientation::WEST)
		{
			if (m->data[i].position.y > 0.0f && m->data[i].position.x < 0.0f) {
				m->data[i].position.y *= 2.0f;
			}
		}
	}
}


// calculate normals by face normal of triangles;
void construct_normals(Static_Model* model) {

	assert(model->mesh.count % 3 == 0 && "vertex count needs be multiple of 3 for triangles.\n");

	size_t count = model->mesh.count;

	for (int i = 0; i < count / 3; i++) {
		size_t base = i * 3;
		Vec3 a = model->mesh.data[base].position;
		Vec3 b = model->mesh.data[base + 1].position;
		Vec3 c = model->mesh.data[base + 2].position;

		Vec3 ab = b - a;
		Vec3 ac = c - a;

		Vec3 normal = cross(ab, ac);
		normalize(&normal);
		model->mesh.data[base].normal = normal;
		model->mesh.data[base + 1].normal = normal;
		model->mesh.data[base + 2].normal = normal;
	}

}