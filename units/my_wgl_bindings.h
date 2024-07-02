// not used 
// we switched to glad now!

#ifndef MY_WGL_BINDINGS_H
#define MY_WGL_BINDINGS_H

#include <stdlib.h>
#include <windows.h>
#include <GL/gl.h>
#include <stdio.h>

// we use version 3.3 core

void load_wgl_functions();

#define GL_ARRAY_BUFFER 0x8892
#define GL_STATIC_DRAW 0x88E4


typedef signed long int GLsizeiptr;

// Defined function pointers for required OpenGL functions
typedef void (APIENTRY* PFNGLGENBUFFERSPROC) (GLsizei n, GLuint* buffers);
typedef void (APIENTRY* PFNGLBINDBUFFERPROC) (GLenum target, GLuint buffer);
typedef void (APIENTRY* PFNGLGENVERTEXARRAYSPROC) (GLsizei n, GLuint* arrays);
typedef void (APIENTRY* PFNGLBUFFERDATAPROC)(GLenum target, GLsizeiptr size, const void* data, GLenum usage);
typedef void (APIENTRY* PFNGLVERTEXATTRIBIPOINTERPROC)(GLuint index, GLint size, GLenum type, GLsizei stride, const void* pointer);
typedef void (APIENTRY* PFNGLENABLEVERTEXATTRIBARRAYPROC)(GLuint index);
typedef void (APIENTRY* PFNGLBINDVERTEXARRAYPROC)(GLuint array);

PFNGLBINDBUFFERPROC glBindBuffer = NULL;
PFNGLGENBUFFERSPROC glGenBuffers = NULL;
PFNGLGENVERTEXARRAYSPROC glGenVertexArrays = NULL;
PFNGLBUFFERDATAPROC glBufferData = NULL;
PFNGLVERTEXATTRIBIPOINTERPROC glVertexAttribIPointer = NULL;
PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray = NULL;
PFNGLBINDVERTEXARRAYPROC glBindVertexArray = NULL;

#endif //MY_WGL_BINDINGS_H

#ifdef MY_WGL_BINDINGS_IMPLEMENTATION
#undef MY_WGL_BINDINGS_IMPLEMENTATION

void load_wgl_functions() {
    glBindBuffer = (PFNGLBINDBUFFERPROC)wglGetProcAddress("glBindBuffer");
    glGenBuffers = (PFNGLGENBUFFERSPROC)wglGetProcAddress("glBufferData");
    glGenVertexArrays = (PFNGLGENVERTEXARRAYSPROC)wglGetProcAddress("glGenVertexArrays");
    glBufferData = (PFNGLBUFFERDATAPROC)wglGetProcAddress("glBufferData");
    glVertexAttribIPointer = (PFNGLVERTEXATTRIBIPOINTERPROC)wglGetProcAddress("glVertexAttribIPointer");
    glEnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC)wglGetProcAddress("glEnableVertexAttribArray");
    glBindVertexArray = (PFNGLBINDVERTEXARRAYPROC)wglGetProcAddress("glBindVertexArray");

    if (!glGenBuffers || !glBindBuffer || !glBindVertexArray) {
        printf("Error: Failed to load OpenGL functions.\n");
        exit(1);
    }
}

#endif //MY_WGL_BINDINGS_IMPLEMENTATION