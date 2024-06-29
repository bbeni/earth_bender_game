#pragma once

#include "Windows.h"

#include "mathematics.hpp"


typedef struct Window_Info {
	HWND window_handle;
	HDC hdc;
	HGLRC gl_context;

} Window_Info;

void create_gl_context(Window_Info* info, int major_version, int minor_version);
void destroy_gl_context(Window_Info* info);
void backend_init(Window_Info* info);
void clear_it(float r, float g, float b, float a);
void swap_buffers(Window_Info* info);
void adjust_viewport_size(int width, int height);

void immediate_triangle();
void immediate_quad();
void immediate_quad(Vec3 p1, Vec3 p2, Vec3 p3, Vec3 p4, Vec4 color);
void immediate_quad(Vec2 pos, Vec2 size, Vec4 color);
