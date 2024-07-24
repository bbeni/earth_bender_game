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


Material_Shader shader_phong;
Material_Shader shader_brdf;
Material_Shader shader_water;

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

// @Todo use world struct or something
// something is wrong anyway...
Vec3 light_direction = { 4.0f, -5.5f, -5.5f };

void init_phong_uniforms() {

	GLuint shader_id = shader_phong.gl_id;
	glUseProgram(shader_id);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	Vec3 light_color = { 0.95f, 0.8f, 0.7f };
	float light_strength = 0.12f;

	Vec3 object_color = { 0.7f, 0.9f, 0.3f };
	
	float time = 0.0f;

	Mat4 view = matrix_camera(Vec3{ -3.0f, 3.0f, -3.0f }, Vec3{ 1.0f, 0.0f, 1.0f}, Vec3{0.0f, 1.0f, 0.0f});

	// Perspective projection
	float fov = 60.0f;
	float near_plane = 0.01f;
	float far_plane = 1000.0f;

	Mat4 projection = matrix_perspective_projection(fov, 1.4f, near_plane, far_plane);
	//projection = matrix_perspective_orthographic(-3.5f, 1.5f, 7.0f, 3.0f, near_plane, far_plane);

	// Model transformation
	Mat4 model = matrix_scale(1.0f);

	shader_uniform_set(shader_id, "projection", projection);
	shader_uniform_set(shader_id, "model", model);
	shader_uniform_set(shader_id, "view", view);
	shader_uniform_set(shader_id, "light_direction", light_direction);
	shader_uniform_set(shader_id, "light_color", light_color);
	shader_uniform_set(shader_id, "light_strength", light_strength);
	//shader_uniform_set(shader_id, "ambient_strength", ambient_strength);
	shader_uniform_set(shader_id, "object_color", object_color);

}

void init_common_uniforms(GLuint shader_id) {

	Vec3 light_color = { 0.95f, 0.8f, 0.7f };
	float light_strength = 5.0f;

	Vec3 object_color = { 0.7f, 0.9f, 0.3f };
	float ambient_strength = 0.0f;

	
	Mat4 view = matrix_camera(Vec3{ -3.0f, 3.0f, -3.0f }, Vec3{ 1.0f, 0.0f, 1.0f }, Vec3{ 0.0f, 1.0f, 0.0f });

	// Perspective projection
	float fov = 60.0f;
	float near_plane = 0.01f;
	float far_plane = 1000.0f;

	Mat4 projection = matrix_perspective_projection(fov, 1.4f, near_plane, far_plane);
	//projection = matrix_perspective_orthographic(-3.5f, 1.5f, 7.0f, 3.0f, near_plane, far_plane);

	// Model transformation
	Mat4 model = matrix_scale(1.0f);

	shader_uniform_set(shader_id, "projection", projection);
	shader_uniform_set(shader_id, "model", model);
	shader_uniform_set(shader_id, "view", view);

	shader_uniform_set(shader_id, "light_direction", light_direction);
	shader_uniform_set(shader_id, "light_color", light_color);
	shader_uniform_set(shader_id, "light_strength", light_strength);

	shader_uniform_set(shader_id, "ambient_strength", ambient_strength);
}


void init_brdf_uniforms() {
	glUseProgram(shader_brdf.gl_id);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	init_common_uniforms(shader_brdf.gl_id);
}


void init_water_uniforms() {
	glUseProgram(shader_water.gl_id);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	init_common_uniforms(shader_water.gl_id);

	shader_uniform_set(shader_water.gl_id, "time", 0.5f);
}


GLuint load_shader_from_path(const char* source_path) {

	char* shader_source;
	int source_length;

	if (!load_resource(source_path, &source_length, &shader_source)) {
		printf("Error: failed to load resource %s\n", source_path);
		exit(1);
	}

	GLuint shader_id = compile_shader(shader_source, source_length);
	if (shader_id == 0) {
		printf("Error: failed to load shader '%s' (info: subtract 3 from line numbers beacuse of shader header).\n", source_path);
		exit(1);
	}

	return shader_id;
}

void backend_create_shaders() {

	// immediate color

	immediate_shader_color = { 0 };
	immediate_shader_color.gl_id = compile_shader(immediate_shader_color_source);
	immediate_shader_color.position_location = 0;
	immediate_shader_color.color_location = 1;

	// phong
	shader_phong = { 0 };
	shader_phong.gl_id = load_shader_from_path("shaders/material_phong.glsl");
	shader_phong.position_location = 0;
	shader_phong.normal_location = 1;
	init_phong_uniforms();

	// brdf
	shader_brdf = { 0 };
	shader_brdf.gl_id = load_shader_from_path("shaders/material_brdf.glsl");
	shader_brdf.position_location = 0;
	shader_brdf.normal_location = 1;
	shader_brdf.uv_location = 2;

	shader_brdf.flags = Shader_Flags::USES_TEXTURE;
	init_brdf_uniforms();

	// water brdf
	shader_water = { 0 };
	shader_water.gl_id = load_shader_from_path("shaders/material_water.glsl");
	shader_water.position_location = 0;
	shader_water.normal_location = 1;
	shader_water.uv_location = 2;

	shader_water.flags = Shader_Flags::USES_TEXTURE;
	init_water_uniforms();
}
