#include "rendering_backend.hpp"

#include <GL/gl.h>
#include <stdio.h>
#include <iostream>

#pragma comment(lib, "opengl32.lib")
//#pragma comment(lib, "glu32.lib")

void setup_projection_and_modelview(int window_width, int window_height) {

	// Set up the projection matrix
	//glMatrixMode(GL_PROJECTION);
	//glLoadIdentity();
	//gluPerspective(45.0, 4.0 / 3.0, 1.0, 100.0);

	// Set up the modelview matrix
	//glMatrixMode(GL_MODELVIEW);
	//glLoadIdentity();
	//gluLookAt(0.0, 0.0, 5.0,   // Eye position
	//		  0.0, 0.0, 0.0,   // Look-at position
	//	      0.0, 1.0, 0.0);  // Up direction

	//glViewport(0, 0, window_width, window_height);
	//glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

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

void create_gl_context(Window_Info *info, int major_version, int minor_version) {
	info->hdc = GetDC(info->window_handle);
	setup_pixel_format(info->hdc);

	HGLRC hglrc = wglCreateContext(info->hdc);
	wglMakeCurrent(info->hdc, hglrc);
	info->gl_context = hglrc;
	

	// OpenGL initialization code here
	const GLubyte* renderer = glGetString(GL_RENDERER);
	const GLubyte* version = glGetString(GL_VERSION);
	std::cout << "Renderer: " << renderer << std::endl;
	std::cout << "OpenGL version supported: " << version << std::endl;


	// TODO gl init
	//glClearColor(0.0f, 0.5f, 0.5f, 1.0f); // Set background color to teal
	//glEnable(GL_DEPTH_TEST); // Enable depth testing

	//setup_projection_and_modelview(300, 300);
	//glDrawBuffer(GL_BACK);
}

void destroy_gl_context(Window_Info* info) {
	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(info->gl_context);
}

void immediate_triangle() {

	// Render a simple triangle
	glBegin(GL_TRIANGLE_STRIP);
	
	glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
	glVertex3f(-0.5f, -0.5f, 0.0f);
	glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
	glVertex3f(0.5f, -0.5f, 0.0f);
	glColor4f(0.0f, 0.0f, 1.0f, 1.0f);
	glVertex3f(0.0f, 0.5f, 0.0f);
	glEnd();
}

void immediate_quad() {
	glBegin(GL_TRIANGLES);

	glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
	glVertex3f(-0.5f, -0.5f, 0.0f);
	glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
	glVertex3f(0.5f, -0.5f, 0.0f);
	glColor4f(0.0f, 0.0f, 1.0f, 1.0f);
	glVertex3f(0.5f, 0.5f, 0.0f);
	
	glColor4f(0.0f, 0.0f, 1.0f, 1.0f);
	glVertex3f(0.5f, 0.5f, 0.0f);
	glColor4f(1.0f, 1.0f, 0.0f, 0.5f);
	glVertex3f(-0.5f, 0.5f, 0.0f);
	glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
	glVertex3f(-0.5f, -0.5f, 0.0f);

	glEnd();
}


void immediate_quad(Vec3 p1, Vec3 p2, Vec3 p3, Vec3 p4, Vec4 color) {
	glBegin(GL_TRIANGLES);

	glColor4f(color.x, color.y, color.z, color.w);
	glVertex3f(p1.x, p1.y, p1.z);
	glVertex3f(p2.x, p2.y, p2.z);
	glVertex3f(p4.x, p4.y, p4.z);

	glVertex3f(p2.x, p2.y, p2.z);
	glVertex3f(p3.x, p3.y, p3.z);
	glVertex3f(p4.x, p4.y, p4.z);

	glEnd();
}

void immediate_quad(Vec2 pos, Vec2 size, Vec4 color) {

	Vec3 p1 = { pos.x - size.x, pos.y - size.y, 0.0f };
	Vec3 p2 = { pos.x + size.x, pos.y - size.y, 0.0f };
	Vec3 p3 = { pos.x + size.x, pos.y + size.y, 0.0f };
	Vec3 p4 = { pos.x - size.x, pos.y + size.y, 0.0f };
	immediate_quad(p1, p2, p3, p4, color);
}


void backend_init(Window_Info *info) {

	if (info->gl_context) {
		info->hdc = GetDC(info->window_handle);
		//TODO windows_change_pixel_format(hdc);
	}
	else {
		create_gl_context(info, 3, 3);

		
		//glGenVertexArrays(1, *opengl_is_stupid_vao);
		//glGenBuffers(1, *immediate_vbo);

		//backend_init_shaders();
		//DumpGLErrors("context");
	}
}

void clear_it(float r, float g, float b, float a) {
	glClearColor(r, g, b, a);
	glClear(GL_COLOR_BUFFER_BIT);
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


