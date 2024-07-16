#ifndef SHADERS_H
#define SHADERS_H

#include "mathematics.hpp"

#include "glad/glad.h"
#include <string.h>
#include <stdio.h>

void backend_create_shaders();
void shader_uniform_set(GLuint shader_id, const char* value_name, const Mat4& mat4);
void shader_uniform_set(GLuint shader_id, const char* value_name, const Vec3& vec3);
void shader_uniform_set(GLuint shader_id, const char* value_name, const float value);
GLuint compile_shader(const char* shader_text, int shader_length = -1);

typedef struct Immediate_Shader {
	GLuint gl_id;
	
	GLuint position_location; // immediate_shader
	GLuint color_location;    // immediate_shader

} Immediate_Shader;

extern Immediate_Shader* current_shader;
extern Immediate_Shader immediate_shader_color;

typedef struct Shader {
	GLuint gl_id;

	GLuint position_location;
	GLuint normal_location;
	//GLuint uv;

} Shader;

extern Shader shader_phong;

typedef struct BRDF_Material_Shader {
	GLuint gl_id;

	GLuint position_location;
	GLuint normal_location;


} BRDF_Material_Shader;


// gets added to the source when you call compile_shader
static const char* _VERTEX_SHADER_HEADER = "\
#version 330\n\
#define VERTEX_SHADER\n\
#define COMMUNICATE out\n";

static const char* _FRAGMENT_SHADER_HEADER = "\
#version 330\n\
#define FRAGMENT_SHADER\n\
#define COMMUNICATE in\n";

static const char* _GENERIC_SHADER_HEADER = "#version330\n";

#endif //SHADERS_H