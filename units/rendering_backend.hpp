#pragma once

#include "Windows.h"

#include "mathematics.hpp"
#include "input_and_window.hpp"

void create_gl_context(Window_Info* info, int major_version, int minor_version);
void destroy_gl_context(Window_Info* info);
void backend_init(Window_Info* info);
void clear_it(float r, float g, float b, float a);
void swap_buffers(Window_Info* info);
void adjust_viewport_size(int width, int height);

void immediate_triangle();
void immediate_quad(Vec2 p1, Vec2 p2, Vec2 p3, Vec2 p4, Vec4 color);
void immediate_quad(Vec2 p1, Vec2 p2, Vec2 p3, Vec2 p4, Vec4 c1, Vec4 c2, Vec4 c3, Vec4 c4);
void immediate_quad(Vec2 pos, Vec2 size, Vec4 color);

void immediate_send();
