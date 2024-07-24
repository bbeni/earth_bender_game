#include "models.hpp"

#include "data_stuff.h"
#include "resource_loading.hpp"

#include <stdio.h>
#include <cstdint>

void construct_cube_triangles(Model* model) {
	float s = 0.5f;

	Vertex_Info_Array* m = &model->mesh;

	for (int i = -1; i < 2; i += 2) {

		Vertex_Info v1 = { -s*i, -s*i, -s * i };
		Vertex_Info v2 = { s,      -s, -s * i };
		Vertex_Info v3 = { s*i,   s*i, -s * i };
		Vertex_Info v4 = { -s,      s, -s * i };

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
void construct_tile_triangles(Model* model) {

	construct_cube_triangles(model);
	Vertex_Info_Array* m = &model->mesh;

	for (int i = 0; i < m->count; i++) {
		if (m->data[i].position.z< 0.0f) {
			m->data[i].position.z *= 4.0f;
		}
	}
}


void construct_ramp_triangles(Model* model, Ramp_Orientation ramp_orientation) {

	construct_cube_triangles(model);
	Vertex_Info_Array* m = &model->mesh;

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
void construct_normals(Model* model) {

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

void make_cube_model(Model* model) {
	float s = 0.5f;
	Vertex_Info_Array* m = &model->mesh;

	for (int i = -1; i < 2; i += 2) {

		Vertex_Info v1 = { -s * i, -s * i, -s * i, 0, 0, 0, 0, 0 };
		Vertex_Info v2 = { s, -s, -s * i , 0, 0, 0, 0, 1 };
		Vertex_Info v3 = { s * i, s * i, -s * i , 0, 0, 0, 1, 1 };
		Vertex_Info v4 = { -s, s, -s * i , 0, 0, 0, 1, 0 };

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
		Vertex_Info v1 = { -s, -s * i, -s, 0, 0, 0, 0, 0 };
		Vertex_Info v4 = { s, -s * i, -s, 0, 0, 0, 0, 1 };
		Vertex_Info v3 = { s, -s * i,  s, 0, 0, 0, 1, 1 };
		Vertex_Info v2 = { -s, -s * i,  s, 0, 0, 0, 1, 0 };

		d_append(m, v1);
		d_append(m, v2);
		d_append(m, v4);
		d_append(m, v4);
		d_append(m, v2);
		d_append(m, v3);
	}


	for (int i = -1; i < 2; i += 2) {

		Vertex_Info v1 = { s * i, -s, -s, 0, 0, 0, 0, 0 };
		Vertex_Info v2 = { s * i,  s, -s, 0, 0, 0, 1, 0 };
		Vertex_Info v3 = { s * i,  s,  s, 0, 0, 0, 0, 1 };
		Vertex_Info v4 = { s * i, -s,  s, 0, 0, 0, 1, 1 };

		d_append(m, v1);
		d_append(m, v2);
		d_append(m, v4);
		d_append(m, v4);
		d_append(m, v2);
		d_append(m, v3);
	}

	construct_normals(model);
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
		model.meshes[i].count = 3 * faces_count;
		model.meshes[i].data = (Vertex_Info*)malloc(sizeof(Vertex_Info) * model.meshes[i].count);
		assert(model.meshes[i].data != NULL);
	}

	// we only have float data from now on
	float* f_data = (float*)data;

	for (int fn = 0; fn < frames_count; fn++) {

		Vertex_Info_Array* mesh = &model.meshes[fn];

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

	return model;
}


// read a .bada file generated with the blender script. will abort on error
Vertex_Info_Array load_mesh_bada_file(const char* file_path) {

	// load the animated model and extract first frame
	Animated_Model animated_model = load_anim_bada_file(file_path);
	return animated_model.meshes[0];
}