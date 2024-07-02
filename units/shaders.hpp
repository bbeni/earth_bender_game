#ifndef SHADERS_H
#define SHADERS_H


#include "glad/glad.h"
#include <string.h>
#include <stdio.h>

typedef struct Shader {
	GLuint opengl_handle;
	GLuint position_location;
	GLuint color_location;
} Shader;

void backend_create_shaders();

extern Shader* current_shader;

extern Shader shader_color;

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

Shader* current_shader;

Shader shader_color;

// 0 is vertexshader, 1 is fragmentshader
const char* shader_color_text[2] = {
	"#version 330 core\n"
	"\n"
	"out vec4 iterated_color;\n"
	"layout(location = 0) in vec2 vert_position;\n"
	"layout(location = 1) in vec4 vert_color;\n"
	"\n"
	"uniform mat4 projection;\n"
	"\n"
	"void main() {\n"
	"   gl_Position = projection * vec4(vert_position, 0.0, 1.0);\n"
	"   iterated_color = vert_color;\n"
	"}\n"
	"\n"

	,

	"#version 330 core\n"
	"\n"
	"out vec4 color;\n"
	"in vec4 iterated_color;\n"
	"\n"
	"void main() {\n"
	"	color = iterated_color;\n"
	"}\n"
};

void backend_create_shaders() {

	shader_color = { 0 };
	shader_color.opengl_handle = compile_shader(shader_color_text);
	shader_color.position_location = 0;
	shader_color.color_location = 1;


	const char value_name[] = "projection";
	GLuint projection_location = glGetUniformLocation(shader_color.opengl_handle, value_name);
	/*if (projection_location == GL_INVALID_VALUE || projection_location == GL_INVALID_OPERATION) {
		printf("Error: OpenGL shader location with glGetUniformLocation(): '%s' not found.", value_name);
		exit(1);
	}*/
	glUseProgram(shader_color.opengl_handle);
	glUniformMatrix4fv(projection_location, 1, GL_FALSE, &mat4_unit.u11);

	//check_gl_error_and_fail("end - backend_create_shaders()");
}

#endif //SHADERS_IMPLEMENTATION
