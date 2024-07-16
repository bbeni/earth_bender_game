#include "shaders.hpp"
#include "rendering_backend.hpp"
#include "resource_loading.hpp"

GLuint make_shader_object(const char* source, int size, GLenum type) {

	const char* sources[2] = {};
	int lengths[2] = {-1, -1};
	
	sources[1] = source;
	lengths[1] = size;

	switch(type) {
	case GL_VERTEX_SHADER:
		sources[0] = _VERTEX_SHADER_HEADER;
		lengths[0] = strlen(_VERTEX_SHADER_HEADER);
		break;
	case GL_FRAGMENT_SHADER:
		sources[0] = _FRAGMENT_SHADER_HEADER;
		lengths[0] = strlen(_FRAGMENT_SHADER_HEADER);
		break;
	default:
		sources[0] = _GENERIC_SHADER_HEADER;
		lengths[0] = strlen(_GENERIC_SHADER_HEADER);
		break;
	}


	GLuint shader_obj = glCreateShader(type);
	glShaderSource(shader_obj, 2, sources, lengths);
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

// return 0 on failure
GLuint compile_shader(const char* shader_text, int shader_length) {

	if (shader_length == -1) {
		shader_length = strlen(shader_text);
	}	

	GLuint shader_vert = make_shader_object(shader_text, shader_length, GL_VERTEX_SHADER);
	GLuint shader_frag = make_shader_object(shader_text, shader_length, GL_FRAGMENT_SHADER);
	GLuint shader_prog = glCreateProgram();

	// @MemoryLeak
	// TODO: fix shader objects cleanup
	if (shader_vert == 0 || shader_frag == 0) {
		return 0;
	}

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

const char* immediate_shader_color_source = {
MULTILINE_STRING(

	COMMUNICATE vec4 iterated_color;\n
	
	#ifdef VERTEX_SHADER\n

	layout(location = 0) in vec2 vert_position;\n
	layout(location = 1) in vec4 vert_color;\n
	
	uniform mat4 projection;\n
	
	void main() {\n
	   gl_Position = projection * vec4(vert_position, 0.0, 1.0);\n
	   iterated_color = vert_color;\n
	}\n

	#endif \n//VERTEX_SHADER

	#ifdef FRAGMENT_SHADER\n
	out vec4 color;\n
		
	void main() {\n
		color = iterated_color;\n
	}\n

	#endif //FRAGMENT_SHADER\n
)
};


Shader shader_phong;

// TODO goal is a phong shader
// 0 is vertexshader, 1 is fragmentshader
const char* shader_phong_source = {
MULTILINE_STRING(
	COMMUNICATE vec3 frag_pos;\n
	COMMUNICATE vec3 frag_normal;\n

	#ifdef VERTEX_SHADER \n

	layout(location = 0) in vec3 vert_position;\n
	layout(location = 1) in vec3 vert_normal;\n

	uniform mat4 projection;\n
	uniform mat4 model;\n
	uniform mat4 view;\n

	void main() {
		\n
		gl_Position = projection * view * model * vec4(vert_position, 1.0);\n
		frag_pos = vec3(model * vec4(vert_position, 1.0));\n
		frag_normal = vert_normal;\n
		//frag_normal.x = -frag_normal.x;\n
		//frag_normal.y = -frag_normal.y;\n
	}\n

	#endif \n// VERTEX_SHADER


	#ifdef FRAGMENT_SHADER \n

	out vec4 color;\n
	
	uniform vec3 object_color;\n
	uniform vec3 light_position;\n
	uniform vec3 light_color;\n
	uniform float ambient_strength;\n
	
	void main() {\n
	   vec3 norm = normalize(frag_normal);\n
	   vec3 light_dir = normalize(light_position - frag_pos);\n

	   float diff = max(dot(norm, light_dir), 0.0);\n
	   vec3 diffuse = diff * light_color;\n

	   vec3 ambient = ambient_strength * light_color;\n
	   vec3 res = (ambient + diffuse) * object_color;\n
	   color = vec4(res, 1.0);\n
	}\n

	#endif \n//FRAGMENT_SHADER
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

	// immediate color

	immediate_shader_color = { 0 };
	immediate_shader_color.gl_id = compile_shader(immediate_shader_color_source);
	immediate_shader_color.position_location = 0;
	immediate_shader_color.color_location = 1;

	Mat4 projection = matrix_unit();
	shader_uniform_set(immediate_shader_color.gl_id, "projection", projection);

	// phong

	shader_phong = { 0 };
	shader_phong.gl_id = compile_shader(shader_phong_source);
	shader_phong.position_location = 0;
	shader_phong.normal_location = 1;

	init_phong_uniforms();

	// brdf

	const char* source_path = "shaders/material_brdf.glsl";
	char* brdf_source;
	int brdf_source_length;

	if (!load_resource(source_path, &brdf_source_length,  &brdf_source)) {
		printf("Error: failed to load resource %f\n", source_path);
		exit(1);
	}

	compile_shader(brdf_source, brdf_source_length);

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
