#include "glad/glad.h"

#include "rendering_backend.hpp"

#include "shaders.hpp"

#include "resource_loading.hpp"

//#include <GL/gl.h>
#include <stdio.h>
#include <iostream>

#pragma comment(lib, "opengl32.lib")

#define GL_GLEXT_PROTOTYPES 1
#define GL3_PROTOTYPES 1

typedef HGLRC WINAPI wglCreateContextAttribsARB_type(HDC hdc, HGLRC hShareContext,
	const int* attribList);
wglCreateContextAttribsARB_type* wglCreateContextAttribsARB;

// See https://www.khronos.org/registry/OpenGL/extensions/ARB/WGL_ARB_create_context.txt for all values
#define WGL_CONTEXT_MAJOR_VERSION_ARB             0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB             0x2092
#define WGL_CONTEXT_PROFILE_MASK_ARB              0x9126

#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB          0x00000001

typedef BOOL WINAPI wglChoosePixelFormatARB_type(HDC hdc, const int* piAttribIList,
	const FLOAT* pfAttribFList, UINT nMaxFormats, int* piFormats, UINT* nNumFormats);
wglChoosePixelFormatARB_type* wglChoosePixelFormatARB;

// See https://www.khronos.org/registry/OpenGL/extensions/ARB/WGL_ARB_pixel_format.txt for all values
#define WGL_DRAW_TO_WINDOW_ARB                    0x2001
#define WGL_ACCELERATION_ARB                      0x2003
#define WGL_SUPPORT_OPENGL_ARB                    0x2010
#define WGL_DOUBLE_BUFFER_ARB                     0x2011
#define WGL_PIXEL_TYPE_ARB                        0x2013
#define WGL_COLOR_BITS_ARB                        0x2014
#define WGL_DEPTH_BITS_ARB                        0x2022
#define WGL_STENCIL_BITS_ARB                      0x2023

#define WGL_FULL_ACCELERATION_ARB                 0x2027
#define WGL_TYPE_RGBA_ARB                         0x202B


void setup_pixel_format(HDC hdc) {
	PIXELFORMATDESCRIPTOR pfd = {
		sizeof(PIXELFORMATDESCRIPTOR),
		1,
		PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
		PFD_TYPE_RGBA,
		24,
		0, 0, 0, 0, 0, 0,
		0,
		0,
		0,
		0, 0, 0, 0,
		32,
		0,
		0,
		PFD_MAIN_PLANE,
		0,
		0, 0, 0
	};

	int iPixelFormat = ChoosePixelFormat(hdc, &pfd);
	if (iPixelFormat == 0) {
		printf("Error: ChoosePixelFormat() failed\n");
		exit(1);
	}
	if (!SetPixelFormat(hdc, iPixelFormat, &pfd)) {
		printf("Error: SetPixelFormat() failed\n");
		exit(1);
	}
}


static void setup_open_gl_extensions(int major_version, int minor_version)
{
	if (major_version != 3 && minor_version != 3) {
		printf("Error: only version 3.3 supported for now\n");
		return;
	}

	// Before we can load extensions, we need a dummy OpenGL context, created using a dummy window.
	// We use a dummy window because you can only set the pixel format for a window once. For the
	// real window, we want to use wglChoosePixelFormatARB (so we can potentially specify options
	// that aren't available in PIXELFORMATDESCRIPTOR), but we can't load and use that before we
	// have a context.
	WNDCLASSA window_class = { 0 };
	window_class.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	window_class.lpfnWndProc = DefWindowProcA;
	window_class.hInstance = GetModuleHandle(0);
	window_class.lpszClassName = "Dummy_WGL_djuasiodwa";

	if (!RegisterClassA(&window_class)) {
		printf("Failed to register dummy OpenGL window.\n");
	}

	HWND dummy_window = CreateWindowExA(
		0,
		window_class.lpszClassName,
		"Dummy OpenGL Window",
		0,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		0,
		0,
		window_class.hInstance,
		0);

	if (!dummy_window) {
		printf("Failed to create dummy OpenGL window.\n");
	}

	HDC dummy_dc = GetDC(dummy_window);

	setup_pixel_format(dummy_dc);

	HGLRC dummy_context = wglCreateContext(dummy_dc);
	if (!dummy_context) {
		printf("Failed to create a dummy OpenGL rendering context.\n");
	}

	if (!wglMakeCurrent(dummy_dc, dummy_context)) {
		printf("Failed to activate dummy OpenGL rendering context.\n");
	}

	wglCreateContextAttribsARB = (wglCreateContextAttribsARB_type*)wglGetProcAddress(
		"wglCreateContextAttribsARB");
	wglChoosePixelFormatARB = (wglChoosePixelFormatARB_type*)wglGetProcAddress(
		"wglChoosePixelFormatARB");

	wglMakeCurrent(dummy_dc, 0);
	wglDeleteContext(dummy_context);
	ReleaseDC(dummy_window, dummy_dc);
	DestroyWindow(dummy_window);
}

void create_gl_context(Window_Info *info, int major_version, int minor_version) {

	if (info->window_handle == NULL) {
		printf("Error: window_handle is null. call create_gl_context() after create_window()\n");
		exit(1);
	}

	info->hdc = GetDC(info->window_handle);
	
	//setup_pixel_format(info->hdc);
	
	setup_open_gl_extensions(major_version, minor_version);

	// TODO gl init
	//glClearColor(0.0f, 0.5f, 0.5f, 1.0f); // Set background color to teal
	//glEnable(GL_DEPTH_TEST); // Enable depth testing

	//setup_projection_and_modelview(300, 300);
	//glDrawBuffer(GL_BACK);


	// Now we can choose a pixel format the modern way, using wglChoosePixelFormatARB.
	int pixel_format_attribs[] = {
		WGL_DRAW_TO_WINDOW_ARB,     GL_TRUE,
		WGL_SUPPORT_OPENGL_ARB,     GL_TRUE,
		WGL_DOUBLE_BUFFER_ARB,      GL_TRUE,
		WGL_ACCELERATION_ARB,       WGL_FULL_ACCELERATION_ARB,
		WGL_PIXEL_TYPE_ARB,         WGL_TYPE_RGBA_ARB,
		WGL_COLOR_BITS_ARB,         32,
		WGL_DEPTH_BITS_ARB,         24,
		WGL_STENCIL_BITS_ARB,       8,
		0
	};

	int pixel_format;
	UINT num_formats;
	wglChoosePixelFormatARB(info->hdc, pixel_format_attribs, 0, 1, &pixel_format, &num_formats);
	if (!num_formats) {
		printf("Error: wglChoosePixelFormatARB() failed for OpenGL 3.3 pixel format.\n");
		exit(1);
	}

	PIXELFORMATDESCRIPTOR pfd;
	DescribePixelFormat(info->hdc, pixel_format, sizeof(pfd), &pfd);
	if (!SetPixelFormat(info->hdc, pixel_format, &pfd)) {
		printf("Error: DescribePixelFormat() failed for OpenGL 3.3 pixel format.\n");
		exit(1);
	}

	// Specify that we want to create an OpenGL 3.3 core profile context
	int gl33_attribs[] = {
		WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
		WGL_CONTEXT_MINOR_VERSION_ARB, 3,
		WGL_CONTEXT_PROFILE_MASK_ARB,  WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
		0,
	};

	HGLRC gl33_context = wglCreateContextAttribsARB(info->hdc, 0, gl33_attribs);
	if (!gl33_context) {
		printf("Error: wglCreateContextAttribsARB() failed to create OpenGL 3.3 context.\n");
		exit(1);
	}

	if (!wglMakeCurrent(info->hdc, gl33_context)) {
		printf("Error: wglMakeCurrent() failed to activate OpenGL 3.3 rendering context.\n");
		exit(1);
	}

	wglMakeCurrent(info->hdc, gl33_context);
	info->gl_context = gl33_context;
}

void destroy_gl_context(Window_Info* info) {
	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(info->gl_context);
}


// immediate rendering stuff

GLuint immediate_vbo;
GLuint immediate_vao;


struct Immediate_Vert {
	Vec2 postition;
	Vec4 color;
};

#define MAX_IMMEDIATE_VERTICES 32768
Immediate_Vert immediate_vertices[MAX_IMMEDIATE_VERTICES];
size_t immediate_vertex_count = 0;

void immediate_vertex(Vec2 position, Vec4 color) {
	if (immediate_vertex_count >= MAX_IMMEDIATE_VERTICES) {
		printf("Error: immediate_vertex_count exceeded MAX_IMMEDIATE_VERTICES=%d\n", MAX_IMMEDIATE_VERTICES);
		exit(1);
	}
	immediate_vertices[immediate_vertex_count].postition = position;
	immediate_vertices[immediate_vertex_count].color = color;
	immediate_vertex_count++;
}

void immediate_shader_set_attributes(Immediate_Shader *shader) {

	GLsizei stride = sizeof(Immediate_Vert);
	
	check_gl_error_and_fail("start immediate_shader_attributes_do");

	glEnableVertexAttribArray(shader->position_location);
	glVertexAttribPointer(shader->position_location, 2, GL_FLOAT, GL_FALSE, stride, (GLvoid*) 0);

	glEnableVertexAttribArray(shader->color_location);
	glVertexAttribPointer(shader->color_location, 4, GL_FLOAT, GL_FALSE, stride, (GLvoid*) (2 * sizeof(GLfloat)));

	// TODO shaders attribures go here

	check_gl_error_and_fail("end immediate_shader_attributes_do");

}


void immediate_send() {

	if (immediate_vertex_count == 0) return;

	if (current_shader == NULL) {
		printf("Error: need to initialize current_shader before calling immediate_flush()\n");
		exit(1);
	}

	glUseProgram(current_shader->gl_id);

	check_gl_error_and_fail("after glUseProgram()");

	//glDisable(GL_DEPTH_TEST);
	//glDisable(GL_CULL_FACE);

	glBindVertexArray(immediate_vao);

	glBindBuffer(GL_ARRAY_BUFFER, immediate_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Immediate_Vert) * immediate_vertex_count, immediate_vertices, GL_DYNAMIC_DRAW);

	immediate_shader_set_attributes(current_shader);

	glDrawArrays(GL_TRIANGLES, 0, (GLsizei)immediate_vertex_count);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	immediate_vertex_count = 0;
}


void immediate_triangle() {

	// Render a simple triangle
	
	Vec4 color1 = { 1.0f, 0.0f, 0.0f, 1.0f };
	Vec4 color2 = { 0.0f, 1.0f, 0.0f, 1.0f };
	Vec4 color3 = { 0.0f, 0.0f, 1.0f, 1.0f };

	Vec2 p1 = { -0.5f, -0.5f };
	Vec2 p2 = { 0.5f, -0.5f };
	Vec2 p3 = { 0.0f, 0.5f };

	immediate_vertex(p1, color1);
	immediate_vertex(p2, color2);
	immediate_vertex(p3, color3);
}

void immediate_quad(Vec2 p1, Vec2 p2, Vec2 p3, Vec2 p4, Vec4 c1, Vec4 c2, Vec4 c3, Vec4 c4) {
	immediate_vertex(p1, c1);
	immediate_vertex(p2, c2);
	immediate_vertex(p4, c4);
	immediate_vertex(p4, c4);
	immediate_vertex(p2, c2);
	immediate_vertex(p3, c3);
}


void immediate_quad(Vec2 p1, Vec2 p2, Vec2 p3, Vec2 p4, Vec4 color) {
	immediate_vertex(p1, color);
	immediate_vertex(p2, color);
	immediate_vertex(p4, color);
	immediate_vertex(p4, color);
	immediate_vertex(p2, color);
	immediate_vertex(p3, color);
}

void immediate_quad(Vec2 pos, Vec2 size, Vec4 color) {

	Vec2 p1 = { pos.x - size.x, pos.y - size.y };
	Vec2 p2 = { pos.x + size.x, pos.y - size.y};
	Vec2 p3 = { pos.x + size.x, pos.y + size.y};
	Vec2 p4 = { pos.x - size.x, pos.y + size.y};
	immediate_quad(p1, p2, p3, p4, color);
}

void immediate_quad(float x, float y, float width, float height, Vec4 color) {
	Vec2 p1 = { x,		y };
	Vec2 p2 = { x,		y + height };
	Vec2 p3 = { x + width, y + height };
	Vec2 p4 = { x + width, y };
	immediate_quad(p1, p2, p3, p4, color);
}

GLuint load_texture_gpu(const Image* image) {
	
	GLuint texture_id;

	glGenTextures(1, &texture_id);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture_id);
	check_gl_error_and_fail("in load_texture() begin");

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	check_gl_error_and_fail("in load_texture() mid");
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image->w, image->h, 0, GL_RGB, GL_UNSIGNED_BYTE, image->data);

	check_gl_error_and_fail("in load_texture() end");
	
	return texture_id;
}

// send data to shader
void shader_init_model(Material_Shader* shader, Model_Info_For_Shading *model_info) {

	glUseProgram(shader->gl_id);
	check_gl_error_and_fail("after glUseProgram()");

	size_t vert_count = model_info->model.mesh.count;
	Vertex_Info* vertex_data = model_info->model.mesh.data;

	if (vert_count == 0) {
		return;
	}

	if (!model_info->initialized) {
		model_info->initialized = true;
		model_info->shader = shader;
		glGenVertexArrays(1, &model_info->shader_vao);
		glBindVertexArray(model_info->shader_vao);
		glGenBuffers(1, &model_info->shader_vbo);
	}

	GLuint vao = model_info->shader_vao;
	GLuint vbo = model_info->shader_vbo;

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex_Info) * vert_count, vertex_data, GL_DYNAMIC_DRAW);

	GLsizei stride = sizeof(Vertex_Info);

	check_gl_error_and_fail("start - shader_init_model locations");

	glEnableVertexAttribArray(shader->position_location);
	glVertexAttribPointer(shader->position_location, 3, GL_FLOAT, GL_FALSE, stride, (GLvoid*)0);

	check_gl_error_and_fail("mid1 - shader_init_model locations");

	glEnableVertexAttribArray(shader->normal_location);
	glVertexAttribPointer(shader->normal_location, 3, GL_FLOAT, GL_FALSE, stride, (GLvoid*)(3 * sizeof(GLfloat)));


	if (shader->flags & Shader_Flags::USES_TEXTURE) {
		check_gl_error_and_fail("mid2 - shader_init_model locations");

		glEnableVertexAttribArray(shader->uv_location);
		glVertexAttribPointer(shader->uv_location, 2, GL_FLOAT, GL_FALSE, stride, (GLvoid*)(6 * sizeof(GLfloat)));
	}

	check_gl_error_and_fail("end - shader_init_model locations");
}


// @CopyPasta from above
// send data to shader for animated model
void shader_init_animated_model(Material_Shader* shader, Animated_Model_Info_For_Shading* model_info) {

	glUseProgram(shader->gl_id);
	check_gl_error_and_fail("after glUseProgram()");

	size_t vert_count = model_info->model.meshes[0].count; // we have the same number for all meshes for now
	if (vert_count == 0) {
		return;
	}

	if (!model_info->initialized) {
		model_info->initialized = true;
		model_info->shader = shader;

		glGenVertexArrays(model_info->model.count, model_info->shader_vao);
		glBindVertexArray(model_info->shader_vao[0]);
		glGenBuffers(model_info->model.count, model_info->shader_vbo);
	}

	for (int i = 0; i < model_info->model.count; i++) {

		Vertex_Info* vertex_data = model_info->model.meshes[i].data;

		GLuint vao = model_info->shader_vao[i];
		GLuint vbo = model_info->shader_vbo[i];

		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex_Info) * vert_count, vertex_data, GL_DYNAMIC_DRAW);

		GLsizei stride = sizeof(Vertex_Info);

		check_gl_error_and_fail("start - shader_init_model locations");

		glEnableVertexAttribArray(shader->position_location);
		glVertexAttribPointer(shader->position_location, 3, GL_FLOAT, GL_FALSE, stride, (GLvoid*)0);

		check_gl_error_and_fail("mid1 - shader_init_model locations");

		glEnableVertexAttribArray(shader->normal_location);
		glVertexAttribPointer(shader->normal_location, 3, GL_FLOAT, GL_FALSE, stride, (GLvoid*)(3 * sizeof(GLfloat)));


		if (shader->flags & Shader_Flags::USES_TEXTURE) {
			check_gl_error_and_fail("mid2 - shader_init_model locations");

			glEnableVertexAttribArray(shader->uv_location);
			glVertexAttribPointer(shader->uv_location, 2, GL_FLOAT, GL_FALSE, stride, (GLvoid*)(6 * sizeof(GLfloat)));
		}

		check_gl_error_and_fail("end - shader_init_model locations");
	}
}

void shader_draw_call(Model_Info_For_Shading* model_info) {
	if (model_info->initialized) {
		Material_Shader* shader = model_info->shader;
		shader_draw_call(model_info, shader);
	}
}

// WARNING does not check if model_info or shader is initialized!
void shader_draw_call(Model_Info_For_Shading* model_info, Material_Shader* shader) {
	
	glUseProgram(shader->gl_id);

	GLuint draw_type = GL_TRIANGLES;

	if (shader->flags & Shader_Flags::USES_ALPHA) {
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_BLEND);
	}
	else {
		glDisable(GL_BLEND);
	}

	if (!(shader->flags & Shader_Flags::NO_SHADER_FLAGS)) {

		if (shader->flags & Shader_Flags::USES_TEXTURE && model_info->texture_color) {
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, model_info->texture_color->gpu_handle);
		}

		if (shader->flags & Shader_Flags::WIREFRAME) {
			draw_type = GL_LINE_STRIP;
			glLineWidth(2.0f);
			//glEnable(GL_LINE_SMOOTH);
			//glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
		}

		if (shader->flags & Shader_Flags::LINES) {
			draw_type = GL_LINES;
			glLineWidth(2.0f);
		}
	}


	GLuint vao = model_info->shader_vao;
	GLuint vbo = model_info->shader_vbo;
	size_t vert_count = model_info->model.mesh.count;

	check_gl_error_and_fail("before - shader_draw_call locations");


	glBindVertexArray(vao);
	//glBindBuffer(GL_ARRAY_BUFFER, vbo);

	check_gl_error_and_fail("mid - shader_draw_call locations");


	glDrawArrays(draw_type, 0, (GLsizei)vert_count);
	//glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	check_gl_error_and_fail("end - shader_draw_call locations");

}

// @CopyPasta from above
void shader_draw_call(Animated_Model_Info_For_Shading* model_info, int frame_index) {

	if (model_info->initialized) {

		Material_Shader* shader = model_info->shader;
		glUseProgram(shader->gl_id);

		if (shader->flags & Shader_Flags::USES_TEXTURE && model_info->texture_color) {
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, model_info->texture_color->gpu_handle);
		}

		glEnable(GL_DEPTH_TEST);

		assert(frame_index >= 0);
		assert(frame_index < model_info->model.count);

		int i = frame_index;
		GLuint vao = model_info->shader_vao[i];
		size_t vert_count = model_info->model.meshes[i].count;

		check_gl_error_and_fail("before - shader_draw_call locations");

		glBindVertexArray(vao);

		check_gl_error_and_fail("mid - shader_draw_call locations");

		glDrawArrays(GL_TRIANGLES, 0, (GLsizei)vert_count);

		glBindVertexArray(0);

		check_gl_error_and_fail("end - shader_draw_call locations");
	}
}


// general rendering stuff

void clear_it(float r, float g, float b, float a) {
	glClearColor(r, g, b, a);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void swap_buffers(Window_Info *info) {
	HDC dc = GetDC(info->window_handle);
	SwapBuffers(dc);
}

void adjust_viewport_size(int width, int height) {
	glViewport(0, 0, width, height);
	//glScissor(0, 0, width, height);

	//glFrustum(left * ratio, right * ratio, bottom, top, nearClip, farClip)
	//or
	//glOrtho(left * ratio, right * ratio, bottom, top, nearClip, farClip)
	//or
	//gluOrtho2D(left * ratio, right * ratio, bottom, top)
}



void backend_init(Window_Info* info) {

	if (info->gl_context) {
		info->hdc = GetDC(info->window_handle);
		//TODO windows_change_pixel_format(hdc);
	}
	else {
		create_gl_context(info, 3, 3);

		//load_wgl_functions();
		gladLoadGL();
	
		glGenVertexArrays(1, &immediate_vao);
		glBindVertexArray(immediate_vao);
		glGenBuffers(1, &immediate_vbo);

		check_gl_error_and_fail("in backend_init()");

		backend_create_shaders();

		current_shader = &immediate_shader_color;

	}
}



void check_gl_error_and_fail(const char* message) {
	GLenum error = glGetError();
	if (error != GL_NO_ERROR) {

		const char* error_hint;
		const char* enum_name;

		switch (error) {
		case GL_INVALID_ENUM:
			enum_name = "GL_INVALID_ENUM";
			error_hint = "Given when an enumeration parameter is not a legal enumeration for that function.This is given only for local problems; if the spec allows the enumeration in certain circumstances, where other parameters or state dictate those circumstances, then GL_INVALID_OPERATION is the result instead.";
			break;
		case GL_INVALID_VALUE:
			enum_name = "GL_INVALID_VALUE";
			error_hint = "Given when a value parameter is not a legal value for that function.This is only given for local problems; if the spec allows the value in certain circumstances, where other parameters or state dictate those circumstances, then GL_INVALID_OPERATION is the result instead.";
			break;
		case GL_INVALID_OPERATION:
			enum_name = "GL_INVALID_OPERATION";
			error_hint = "Given when the set of state for a command is not legal for the parameters given to that command.It is also given for commands where combinations of parameters define what the legal parameters are.";
			break;

		case GL_STACK_OVERFLOW:
			enum_name = "GL_STACK_OVERFLOW";
			error_hint = "Given when a stack pushing operation cannot be done because it would overflow the limit of that stack's size.";
			break;

		case GL_STACK_UNDERFLOW:
			enum_name = "GL_STACK_UNDERFLOW";
			error_hint = "Given when a stack popping operation cannot be done because the stack is already at its lowest point.";
			break;

		case GL_OUT_OF_MEMORY:
			enum_name = "GL_OUT_OF_MEMORY";
			error_hint = "Given when performing an operation that can allocate memory, and the memory cannot be allocated.The results of OpenGL functions that return this error are undefined; it is allowable for partial execution of an operation to happen in this circumstance.";
			break;

		case GL_INVALID_FRAMEBUFFER_OPERATION:
			enum_name = "GL_INVALID_FRAMEBUFFER_OPERATION";
			error_hint = "Given when doing anything that would attempt to read from or write / render to a framebuffer that is not complete.";
			break;

		//case GL_CONTEXT_LOST:
		//	enum_name = "GL_CONTEXT_LOST";
		//	error_hint = "(with OpenGL 4.5 or ARB_KHR_robustness)\n"
		//	"Given if the OpenGL context has been lost, due to a graphics card reset.";
		//  break;
		// 
		//case GL_TABLE_TOO_LARGE1:
		//	enum_name = "GL_TABLE_TOO_LARGE1";
		//	error_hint = "no hint";
		// break;

		default:
			enum_name = "UNKNOWN_ERROR_ENUM";
			error_hint = "The error enum that we got from open gl is not known (maybe new gl version?)";
			break;
		}

		printf("Error: %s [0x%x]: %s\n"
			   "Hint:  %s\n", enum_name, error, message, error_hint);
		exit(1);
	}
}


