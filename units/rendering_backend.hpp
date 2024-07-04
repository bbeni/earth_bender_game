#pragma once

#include "Windows.h"

#include "mathematics.hpp"
#include "input_and_window.hpp"
#include "data_stuff.h"
#include "shaders.hpp"

void create_gl_context(Window_Info* info, int major_version, int minor_version);
void destroy_gl_context(Window_Info* info);
void backend_init(Window_Info* info);

void check_gl_error_and_fail(const char* message);
void clear_it(float r, float g, float b, float a);
void swap_buffers(Window_Info* info);
void adjust_viewport_size(int width, int height);

// 2d rendering stuff

void immediate_triangle();
void immediate_quad(Vec2 p1, Vec2 p2, Vec2 p3, Vec2 p4, Vec4 color);
void immediate_quad(Vec2 p1, Vec2 p2, Vec2 p3, Vec2 p4, Vec4 c1, Vec4 c2, Vec4 c3, Vec4 c4);
void immediate_quad(Vec2 pos, Vec2 size, Vec4 color);

void immediate_send();

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

void construct_cube_triangles(Static_Model* model);
void construct_normals(Static_Model* model);

typedef struct Model_Info_For_Shading {
	Static_Model model; // needs be set by user

	bool initialized;  // set by shader_init_model()
	Shader* shader;    // set by shader_init_model()
	GLuint shader_vao; // set by shader_init_model()
	GLuint shader_vbo; // set by shader_init_model()

} Model_Info_For_Shading;


// send data to shader
void shader_init_model(Shader* shader, Model_Info_For_Shading* model_info);
// draw the model (shader is stored by shader_send_attributes)
void shader_draw_call(Model_Info_For_Shading* model_info);
//void shader_flush(Shader* shader);
