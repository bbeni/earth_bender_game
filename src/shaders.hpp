#ifndef SHADERS_H
#define SHADERS_H


#include "glad/glad.h"
#include <string.h>
#include <stdio.h>

void backend_create_shaders();
void shader_uniform_set(GLuint shader_id, const char* value_name, const Mat4& mat4);
void shader_uniform_set(GLuint shader_id, const char* value_name, const Vec3& vec3);
void shader_uniform_set(GLuint shader_id, const char* value_name, const float value);

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


#endif //SHADERS_H

#ifdef SHADERS_IMPLEMENTATION
#undef SHADERS_IMPLEMENTATION

GLuint make_shader_object(const char* source, GLenum type) {
	GLuint shader_obj = glCreateShader(type);
	glShaderSource(shader_obj, 1, &source, NULL);
	glCompileShader(shader_obj);

	GLint sucess;
	glGetShaderiv(shader_obj, GL_COMPILE_STATUS, &sucess);
	if (!sucess) {
		char log_message[1024];
		glGetShaderInfoLog(shader_obj, 1024, NULL, log_message);
		printf("Error: OpenGL shader: %s\n", log_message);
		return 0;
	}

	return shader_obj;
}

GLuint compile_shader(const char* shader_text[2]) {
	
	GLuint shader_vert = make_shader_object(shader_text[0], GL_VERTEX_SHADER);
	GLuint shader_frag = make_shader_object(shader_text[1], GL_FRAGMENT_SHADER);
	GLuint shader_prog = glCreateProgram();

	glAttachShader(shader_prog, shader_vert);
	glAttachShader(shader_prog, shader_frag);
	glLinkProgram(shader_prog);

	GLint sucess = 0;
	glGetProgramiv(shader_prog, GL_LINK_STATUS, &sucess);

	printf("GLuints of shaders: %d %d %d\n", shader_vert, shader_frag, shader_prog);

	if (!sucess) {
		char log_message[1024];
		glGetShaderInfoLog(shader_prog, 1024, NULL, log_message);
		printf("Error: OpenGL shader: %s\n", log_message);
		return 0;
	}

	glDeleteShader(shader_vert);
	glDeleteShader(shader_frag);

	return shader_prog;
}

Immediate_Shader* current_shader;

Immediate_Shader immediate_shader_color;


#define MULTILINE_STRING(...) #__VA_ARGS__ // just quote the string

const char* immediate_shader_color_source[2] = {
MULTILINE_STRING(
	#version 330 core\n
	
	out vec4 iterated_color;\n
	layout(location = 0) in vec2 vert_position;\n
	layout(location = 1) in vec4 vert_color;\n
	
	uniform mat4 projection;\n
	
	void main() {\n
	   gl_Position = projection * vec4(vert_position, 0.0, 1.0);\n
	   iterated_color = vert_color;\n
	}\n
),
MULTILINE_STRING(
	#version 330 core\n

	out vec4 color;\n
	
	in vec4 iterated_color;\n
	
	void main() {\n
		color = iterated_color;\n
	}\n
)
};


Shader shader_phong;

// TODO goal is a phong shader
// 0 is vertexshader, 1 is fragmentshader
const char* shader_phong_source[2] = {
MULTILINE_STRING(
	#version 330 core\n
	\n
	out vec3 frag_pos;\n
	out vec3 frag_normal;\n
	\n
	layout(location = 0) in vec3 vert_position;\n
	layout(location = 1) in vec3 vert_normal;\n
	\n
	uniform mat4 projection;\n
	uniform mat4 model;\n
	uniform mat4 view;\n
	\n
	void main() {
		\n
		gl_Position = projection * view * model * vec4(vert_position, 1.0);\n
		frag_pos = vec3(model * vec4(vert_position, 1.0));\n
		frag_normal = vert_normal;\n
		//frag_normal.x = -frag_normal.x;\n
		//frag_normal.y = -frag_normal.y;\n
	}\n
),
MULTILINE_STRING(
	#version 330 core\n
	//precision mediump float;\n
	\n
	out vec4 color;\n
	in  vec3 frag_pos;\n
	in  vec3 frag_normal;\n
	\n
	uniform vec3 object_color;\n
	uniform vec3 light_position;\n
	uniform vec3 light_color;\n
	uniform float ambient_strength;\n
	\n
	void main() {\n
	   vec3 norm = normalize(frag_normal);\n
	   vec3 light_dir = normalize(light_position - frag_pos);\n
	   \n
	   float diff = max(dot(norm, light_dir), 0.0);\n
	   vec3 diffuse = diff * light_color;\n
		\n
	   vec3 ambient = ambient_strength * light_color;\n
		vec3 res = (ambient + diffuse) * object_color;\n
	   //color = vec4(0.5, 0.5, 1.0, 1.0);\n
	   //color = vec4(dot(norm, vec3(-1, 0, 0)), 0.5, 0.0, 1.0);\n
		color = vec4(res, 1.0);\n
	}\n
)
};


// TODO: save loactions in hash_table

void shader_uniform_set(GLuint shader_id, const char* value_name, const Mat4& mat4) {
	glUseProgram(shader_id); // temporary

	GLuint location = glGetUniformLocation(shader_id, value_name);
	check_gl_error_and_fail("location - shader_uniform_set(mat4)");
	if (location == -1) {
		printf("Error: OpenGL cannot find uniform location with name '%s' in shader_id %d.\n", value_name, shader_id);
		exit(1);
	}
	glUniformMatrix4fv(location, 1, GL_FALSE, &mat4.u11);
	check_gl_error_and_fail("end - shader_uniform_set(mat4)");
}

void shader_uniform_set(GLuint shader_id, const char* value_name, const Vec3& vec3) {
	glUseProgram(shader_id); // temporary

	GLuint location = glGetUniformLocation(shader_id, value_name);
	check_gl_error_and_fail("location - shader_uniform_set(vec3)");
	if (location == -1) {
		printf("Error: OpenGL cannot find uniform location with name '%s' in shader_id %d.\n", value_name, shader_id);
		exit(1);
	}

	glUniform3fv(location, 1, &vec3.x);
	check_gl_error_and_fail("end - shader_uniform_set(vec3)");
}

void shader_uniform_set(GLuint shader_id, const char* value_name, const float value) {
	glUseProgram(shader_id); // temporary

	GLuint location = glGetUniformLocation(shader_id, value_name);
	check_gl_error_and_fail("location - shader_uniform_set(float)");
	if (location == -1) {
		printf("Error: OpenGL cannot find uniform location with name '%s' in shader_id %d.\n", value_name, shader_id);
		exit(1);
	}

	glUniform1f(location, value);
	check_gl_error_and_fail("end - shader_uniform_set(float)");
}


void init_phong_uniforms();

void backend_create_shaders() {

	immediate_shader_color = { 0 };
	immediate_shader_color.gl_id = compile_shader(immediate_shader_color_source);
	immediate_shader_color.position_location = 0;
	immediate_shader_color.color_location = 1;

	Mat4 projection = matrix_unit();
	shader_uniform_set(immediate_shader_color.gl_id, "projection", projection);

	shader_phong = { 0 };
	shader_phong.gl_id = compile_shader(shader_phong_source);
	shader_phong.position_location = 0;
	shader_phong.normal_location = 1;

	init_phong_uniforms();
}

void init_phong_uniforms() {
	glUseProgram(shader_phong.gl_id);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	Vec3 light_pos = { 22.5f, 2.5f, 8.0f };
	Vec3 light_color = { 0.95f, 0.8f, 0.7f };
	Vec3 object_color = { 0.7f, 0.9f, 0.3f };
	float ambient_strength = 0.25f;

	float time = 0.0f;
	// Camera/view transformation
	//Vec3 camera_pos = { -18.5f * cosf(time), -10.0f, -18.5f * sinf(time) };
	//Vec3 camera_pos = { -20.0f,  5.0f, -20.0f };
	//Vec3 camera_target = { 0.0f, 6.0f, 0.0f };
	//Vec3 up_vector = { 0.0f, -1.0f, 0.0f };
	//Mat4 view = matrix_look_at(camera_pos, camera_target, up_vector);

	Mat4 view = matrix_camera(Vec3{ -3.0f, 3.0f, -3.0f }, Vec3{ 1.0f, 0.0f, 1.0f}, Vec3{0.0f, 1.0f, 0.0f});

	// Perspective projection
	float fov = 60.0f;
	float near_plane = 0.01f;
	float far_plane = 1000.0f;
	Mat4 projection = matrix_perspective(fov, 1.4f, near_plane, far_plane);

	//projection = matrix_perspective_orthographic(-3.5f, 1.5f, 7.0f, 3.0f, near_plane, far_plane);

	// Model transformation
	Mat4 model = matrix_scale(1.0f);

	shader_uniform_set(shader_phong.gl_id, "projection", projection);
	shader_uniform_set(shader_phong.gl_id, "model", model);
	shader_uniform_set(shader_phong.gl_id, "view", view);
	shader_uniform_set(shader_phong.gl_id, "light_position", light_pos);
	shader_uniform_set(shader_phong.gl_id, "light_color", light_color);
	shader_uniform_set(shader_phong.gl_id, "ambient_strength", ambient_strength);
	shader_uniform_set(shader_phong.gl_id, "object_color", object_color);

}

/*
// TODO delete:
// temporary 
void update_phong(float time) {
	glUseProgram(shader_phong.gl_id);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	// Lighting and material properties
	//Vec3 light_pos = { -0.3f, -2.0f + time*0.3f, 0.7f};
	Vec3 light_pos = {23.5f, 3.5f, 12.0f};

	//printf("light position: %f %f %f\n", light_pos.x, light_pos.y, light_pos.z);
	Vec3 light_color = { 0.95f, 0.8f, 0.7f };
	Vec3 object_color = { 0.7f, 0.9f, 0.3f };
	float ambient_strength = 0.01f;

	// Camera/view transformation
	Vec3 camera_pos = { -18.5f*cosf(time), -10.0f, -18.5f * sinf(time) };
	Vec3 camera_target = { 0.0f, 0.0f, 0.0f };
	Vec3 up_vector = { 0.0f, 1.0f, 0.0f };
	Mat4 view = matrix_look_at(camera_pos, camera_target, up_vector);

	// Perspective projection
	float fov = 60.0f;
	float near_plane = 0.01f;
	float far_plane = 100.0f;
	Mat4 projection = matrix_perspective(fov, 1.4f, near_plane, far_plane);

	//projection = matrix_perspective_orthographic(-1.5f, 1.5f, -1.0f, 1.0f, near_plane, far_plane);

	// Model transformation
	Mat4 model = matrix_scale(1.0f);

	shader_uniform_set(shader_phong.gl_id, "projection", projection);
	shader_uniform_set(shader_phong.gl_id, "model", model);
	shader_uniform_set(shader_phong.gl_id, "view", view);
	shader_uniform_set(shader_phong.gl_id, "light_position", light_pos);
	shader_uniform_set(shader_phong.gl_id, "light_color", light_color);
	shader_uniform_set(shader_phong.gl_id, "ambient_strength", ambient_strength);
	shader_uniform_set(shader_phong.gl_id, "object_color", object_color);

} */


#endif //SHADERS_IMPLEMENTATION
