#include "models.hpp"

#include "data_stuff.h"
#include "resource_loading.hpp"

#include <stdio.h>
#include <cstdint>

void construct_cube_triangles(Static_Model* model) {
	float s = 0.5f;

	Mesh* m = &model->mesh;

	for (int i = -1; i < 2; i += 2) {

		Vertex_Info v1 = { -s*i, -s*i, -s * i };
		Vertex_Info v2 = { s,      -s, -s * i };
		Vertex_Info v3 = { s*i,   s*i, -s * i };
		Vertex_Info v4 = { -s,      s, -s * i };

		// seems legit
		array_add(m, v1);
		array_add(m, v2);
		array_add(m, v4);
		array_add(m, v4);
		array_add(m, v2);
		array_add(m, v3);
	}

	for (int i = -1; i < 2; i += 2) {

		// reverse corners for normals
		Vertex_Info v1 = { -s, -s * i, -s };
		Vertex_Info v4 = { s, -s * i, -s };
		Vertex_Info v3 = { s, -s * i,  s };
		Vertex_Info v2 = { -s, -s * i,  s };

		array_add(m, v1);
		array_add(m, v2);
		array_add(m, v4);
		array_add(m, v4);
		array_add(m, v2);
		array_add(m, v3);
	}


	for (int i = -1; i < 2; i += 2) {

		Vertex_Info v1 = { s * i, -s, -s };
		Vertex_Info v2 = { s * i,  s, -s };
		Vertex_Info v3 = { s * i,  s,  s };
		Vertex_Info v4 = { s * i, -s,  s };

		array_add(m, v1);
		array_add(m, v2);
		array_add(m, v4);
		array_add(m, v4);
		array_add(m, v2);
		array_add(m, v3);
	}

}


// TODO these functions are kinda dirty... should use transformation matrices?
void construct_tile_triangles(Static_Model* model) {

	construct_cube_triangles(model);
	Mesh* m = &model->mesh;

	for (int i = 0; i < m->count; i++) {
		if (m->data[i].position.z< 0.0f) {
			m->data[i].position.z *= 4.0f;
		}
	}
}


void construct_ramp_triangles(Static_Model* model, Ramp_Orientation ramp_orientation) {

	construct_cube_triangles(model);
	Mesh* m = &model->mesh;

	for (int i = 0; i < m->count; i++) {
		if (m->data[i].position.z < 0.0f) {
			m->data[i].position.z *= 3.0f;
		}

		if (ramp_orientation == Ramp_Orientation::NORTH)
		{
			if (m->data[i].position.z > 0.0f && m->data[i].position.y > 0.0f) {
				m->data[i].position.z *= 2.0f;
			}
		}
		if (ramp_orientation == Ramp_Orientation::SOUTH)
		{
			if (m->data[i].position.z > 0.0f && m->data[i].position.y < 0.0f) {
				m->data[i].position.z *= 2.0f;
			}
		}
		if (ramp_orientation == Ramp_Orientation::EAST)
		{
			if (m->data[i].position.z > 0.0f && m->data[i].position.x > 0.0f) {
				m->data[i].position.z *= 2.0f;
			}
		}
		if (ramp_orientation == Ramp_Orientation::WEST)
		{
			if (m->data[i].position.z > 0.0f && m->data[i].position.x < 0.0f) {
				m->data[i].position.z *= 2.0f;
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

void make_cube_model(Static_Model* model) {
	float s = 0.5f;
	Mesh* m = &model->mesh;

	for (int i = -1; i < 2; i += 2) {

		Vertex_Info v1 = { -s * i, -s * i, -s * i, 0, 0, 0, 0, 0 };
		Vertex_Info v2 = { s, -s, -s * i , 0, 0, 0, 0, 1 };
		Vertex_Info v3 = { s * i, s * i, -s * i , 0, 0, 0, 1, 1 };
		Vertex_Info v4 = { -s, s, -s * i , 0, 0, 0, 1, 0 };

		// seems legit
		array_add(m, v1);
		array_add(m, v2);
		array_add(m, v4);
		array_add(m, v4);
		array_add(m, v2);
		array_add(m, v3);
	}

	for (int i = -1; i < 2; i += 2) {

		// reverse corners for normals
		Vertex_Info v1 = { -s, -s * i, -s, 0, 0, 0, 0, 0 };
		Vertex_Info v4 = { s, -s * i, -s, 0, 0, 0, 0, 1 };
		Vertex_Info v3 = { s, -s * i,  s, 0, 0, 0, 1, 1 };
		Vertex_Info v2 = { -s, -s * i,  s, 0, 0, 0, 1, 0 };

		array_add(m, v1);
		array_add(m, v2);
		array_add(m, v4);
		array_add(m, v4);
		array_add(m, v2);
		array_add(m, v3);
	}


	for (int i = -1; i < 2; i += 2) {

		Vertex_Info v1 = { s * i, -s, -s, 0, 0, 0, 0, 0 };
		Vertex_Info v2 = { s * i,  s, -s, 0, 0, 0, 1, 0 };
		Vertex_Info v3 = { s * i,  s,  s, 0, 0, 0, 0, 1 };
		Vertex_Info v4 = { s * i, -s,  s, 0, 0, 0, 1, 1 };

		array_add(m, v1);
		array_add(m, v2);
		array_add(m, v4);
		array_add(m, v4);
		array_add(m, v2);
		array_add(m, v3);
	}

	construct_normals(model);

	model->bounding_box = find_bounding_box(&model->mesh);
}



static const uint32_t BADA_FILE_MAGIC = 0xdabadaba;
static const uint32_t BADA_VERSION = 2;

// read a .bada file generated with the blender script. will abort on error
Animated_Model load_anim_bada_file(const char* file_path) {

	// TODO: use read file directly
	char* data;
	int size;
	if (!load_resource(file_path, &size, &data)) {
		assert(false && "failed to load bada file");
	}

	printf("Info: %s has %d bytes of content\n", file_path, size);

	// check filemagic
	printf("Info: supposed filemagic reversed is: 0x%4X\n", *(uint32_t*)data);
	uint32_t supposed_filemagic = *(uint32_t*)data;
	data += 4;
	assert(supposed_filemagic == BADA_FILE_MAGIC);

	// check version number
	int32_t supposed_version = *(int32_t*)data;
	data += 4;
	if (supposed_version > BADA_VERSION) {
		printf("Error: Loading .bada file '%s': the version is too high so we don't know how to load it. Epected %d got %d (a currupted file is also likely..)\n", file_path, BADA_VERSION, supposed_version);
		assert(false);
	}

	if (supposed_version < BADA_VERSION) {
		printf("Error: Loading .bada file '%s': the version is too low and we don't support migrations for now. Epected %d got %d\n", file_path, BADA_VERSION, supposed_version);
		assert(false);
	}

	// flags
	assert(*(uint32_t*)data == 1); // for now we support only triangles
	data += 4;

	// reserved skipped for now
	data += 4 * 4;

	// extract frames count
	int32_t frames_count = *(int32_t*)data;
	data += 4;
	
	// extract faces count
	int32_t faces_count = *(int32_t*)data;
	data += 4;

	printf("Info: we have %d frames, %d faces per fram, in bada version %d file\n", frames_count, faces_count, supposed_version);

	Animated_Model model = { 0 };
	model.count = frames_count;

	for (int i = 0; i < frames_count; i++) {
		model.meshes[i].capacity = 3 * faces_count;
		model.meshes[i].count = 3 * faces_count;
		model.meshes[i].data = (Vertex_Info*)malloc(sizeof(Vertex_Info) * model.meshes[i].count);
		assert(model.meshes[i].data != NULL);
	}

	// we only have float data from now on
	float* f_data = (float*)data;

	for (int fn = 0; fn < frames_count; fn++) {

		Mesh* mesh = &model.meshes[fn];

		for (int i = 0; i < faces_count; i++) {

			// skip material index for now
			f_data++;

			int base_index = i * 3;

			mesh->data[base_index].normal.x = *(f_data++);
			mesh->data[base_index].normal.y = *(f_data++);
			mesh->data[base_index].normal.z = *(f_data++);

			// copy the normal to other 2 vertex_info to mesh
			for (int j = 1; j < 3; j++) {
				mesh->data[base_index + j].normal = mesh->data[base_index].normal;
			}

			for (int j = 0; j < 3; j++) {
				mesh->data[base_index + j].position.x = *(f_data++);
				mesh->data[base_index + j].position.y = *(f_data++);
				mesh->data[base_index + j].position.z = *(f_data++);
			}

			for (int j = 0; j < 3; j++) {
				mesh->data[base_index + j].uv.x = *(f_data++);
				mesh->data[base_index + j].uv.y = *(f_data++);
				//printf("uv: (%f, %f)\n", mesh->data[base_index + j].uv.x, mesh->data[base_index + j].uv.y);
			}
		}
	}

	uint32_t magic = *(uint32_t*)f_data;
	assert(*(uint32_t*)f_data == 0xdabadaba);

	model.bounding_box = find_bounding_box(&model.meshes[0]);
	model.scale = 1.0f;

	return model;
}

Box find_bounding_box(Mesh* mesh) {

	// find minimal bounding box
	float min_x = 0, min_y = 0, min_z = 0;
	float max_x = 0, max_y = 0, max_z = 0;

	for (int i = 0; i < mesh->count; i++) {
		Vec3 point = mesh->data[i].position;
		if (point.x > max_x) max_x = point.x;
		if (point.y > max_y) max_y = point.y;
		if (point.z > max_z) max_z = point.z;

		if (point.x < min_x) min_x = point.x;
		if (point.y < min_y) min_y = point.y;
		if (point.z < min_z) min_z = point.z;
	}
	
	Box bounding_box;

	bounding_box.min = Vec3{ min_x, min_y, min_z };
	bounding_box.max = Vec3{ max_x, max_y, max_z };

	return bounding_box;
}

// read a .bada file generated with the blender script. will abort on error
Static_Model load_model_bada_file(const char* file_path) {

	Static_Model model = { 0 };

	// load the animated model and extract first frame
	Animated_Model animated_model = load_anim_bada_file(file_path);

	model.mesh = animated_model.meshes[0];

	model.bounding_box = find_bounding_box(&model.mesh);

	return model;
}


void construct_box_lines(Box_Line_Model* box)
{
	// one corner 3 connections
	box->points[0] = Vec3{ 0.5f, 0.5f, 0.5f };
	box->points[1] = Vec3{ 0.5f, 0.5f, -0.5f};
	box->points[2] = Vec3{ 0.5f, 0.5f, 0.5f };
	box->points[3] = Vec3{ 0.5f, -0.5f, 0.5f };
	box->points[4] = Vec3{ 0.5f, 0.5f, 0.5f };
	box->points[5] = Vec3{ -0.5f, 0.5f, 0.5f };

	// opposite corner 3 connections
	box->points[6] = Vec3{ -0.5f, -0.5f, -0.5f };
	box->points[7] = Vec3{ -0.5f, -0.5f, 0.5f };
	box->points[8] = Vec3{ -0.5f, -0.5f, -0.5f };
	box->points[9] = Vec3{ -0.5f, 0.5f, -0.5f };
	box->points[10] = Vec3{ -0.5f, -0.5f, -0.5f };
	box->points[11] = Vec3{ 0.5f, -0.5f, -0.5f };


	// zig zag connections
	box->points[12] = Vec3{ 0.5f, 0.5f, -0.5f };
	box->points[13] = Vec3{ 0.5f, -0.5f, -0.5f };
	box->points[14] = Vec3{ 0.5f, 0.5f, -0.5f };
	box->points[15] = Vec3{ -0.5f, 0.5f, -0.5f };
	
	box->points[16] = Vec3{ 0.5f, -0.5f, 0.5f };
	box->points[17] = Vec3{ 0.5f, -0.5f, -0.5f };
	box->points[18] = Vec3{ 0.5f, -0.5f, 0.5f };
	box->points[19] = Vec3{ -0.5f, -0.5f, 0.5f };

	box->points[20] = Vec3{ -0.5f, 0.5f, 0.5f };
	box->points[21] = Vec3{ -0.5f, -0.5f, 0.5f };
	box->points[22] = Vec3{ -0.5f, 0.5f, 0.5f };
	box->points[23] = Vec3{ -0.5f, 0.5f, -0.5f };

}


// note this does not use faces or normals
Static_Model construct_box_lines() {
	
	Static_Model model = {0};

	Box_Line_Model line_model;
	construct_box_lines(&line_model);

	Mesh* m = &model.mesh;

	for (int i = 0; i < 12 *2; i += 1) {

		Vec3 p = line_model.points[i];
		Vertex_Info vi = {p.x, p.y, p.z, 0, 0, 0, 0, 0};
		array_add(m, vi);
	}

	assert(model.mesh.count == 12 * 2);

	model.bounding_box = find_bounding_box(m);

	return model;
} 

