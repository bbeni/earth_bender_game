#include "models.hpp"

#include "data_stuff.h"
#include "resource_loading.hpp"

#include <stdio.h>
#include <cstdint>

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
		normalize_or_z_axis(&normal);
		model->mesh.data[base].normal = normal;
		model->mesh.data[base + 1].normal = normal;
		model->mesh.data[base + 2].normal = normal;
	}

}

// read a .bada file generated with the blender script. will abort on error
Vertex_Info_Array load_mesh_bada_file(const char* file_path) {

	// TODO: use read file directly
	char* data;
	int size;
	if (!load_resource(file_path, &size, (void**)&data)) {
		exit(1);
	}

	printf("INFO: %s has %d bytes of content\n", file_path, size);

	// check filemagic
	printf("INFO: supposed filemagic reversed is: 0x%4X\n", *(uint32_t*)data);
	uint32_t supposed_filemagic = *(uint32_t*)data;
	data += 4;
	assert(supposed_filemagic == 0xdabadaba);

	// extract faces count
	uint32_t faces_count = *(uint32_t*)data;
	data += 4;
	printf("INFO: we have %d faces in this file\n", faces_count);

	assert(*(uint32_t*)data == 1); // for now we support only triangles
	data += 4;

	// now we start with the trinangles
	Vertex_Info_Array mesh = { 0 };
	//mesh.triangle_count = faces_count;
	mesh.count = 3 * faces_count;
	mesh.data = (Vertex_Info*)malloc(sizeof(Vertex_Info) * mesh.count);

	// we only have float data for and from now
	float* f_data = (float*)data;
	for (int i = 0; i < faces_count; i++) {

		// skip material index for now
		f_data++;

		int base_index = i * 3;
		
		mesh.data[base_index].normal.x = *(f_data++);
		mesh.data[base_index].normal.y = *(f_data++);
		mesh.data[base_index].normal.z = *(f_data++);

		// copy the normal to other 2 vertex_info to mesh
		for (int j = 1; j < 3; j++) {
			mesh.data[base_index + j].normal = mesh.data[base_index].normal;
		}

		for (int j = 0; j < 3; j++) {
			mesh.data[base_index+j].position.x = *(f_data++);
			mesh.data[base_index+j].position.y = *(f_data++);
			mesh.data[base_index+j].position.z = *(f_data++);
		}

		// drain for now
		for (int j = 0; j < 3; j++) {
			f_data++;
			f_data++;
			//mesh.texture_coords[base_index + 0 + j * 2] = *(f_data++);
			//mesh.texture_coords[base_index + 1 + j * 2] = *(f_data++);
		}
	}
	
	uint32_t magic = *(uint32_t*)f_data;
	assert(*(uint32_t*)f_data == 0xdabadaba);
	return mesh;
}