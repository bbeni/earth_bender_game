// main shader code
COMMUNICATE vec3 frag_pos;
COMMUNICATE vec3 frag_normal;

#ifdef VERTEX_SHADER
layout(location = 0) in vec3 vert_position;
layout(location = 1) in vec3 vert_normal;

uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;

void main() {
	gl_Position = projection * view * model * vec4(vert_position, 1.0);
	frag_pos = vec3(model * vec4(vert_position, 1.0));
	frag_normal = vert_normal;
}

#endif //VERTEX_SHADER

#ifdef FRAGMENT_SHADER

out vec4 color;

uniform vec3 object_color;
uniform vec3 light_position;
uniform vec3 light_color;
uniform float ambient_strength;

void main() {
	vec3 norm = normalize(frag_normal);
	vec3 light_dir = normalize(light_position - frag_pos);
	
	float diff = max(dot(norm, light_dir), 0.0);
	vec3 diffuse = diff * light_color;
	
	vec3 ambient = ambient_strength * light_color;
	vec3 res = (ambient + diffuse) * object_color;
	color = vec4(res, 1.0);
}

#endif //FRAGMENT_SHADER