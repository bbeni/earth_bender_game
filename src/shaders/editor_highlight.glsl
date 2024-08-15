// shader to highlight selected or hover or preview

COMMUNICATE vec3 frag_pos;
COMMUNICATE vec4 frag_highlight_color;

#ifdef VERTEX_SHADER

layout(location = 0) in vec3 vert_position;

uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;

uniform vec4 highlight_color;

void main() {
	
	gl_Position = projection * view * model * vec4(vert_position, 1.0);
	frag_pos = vec3(model * vec4(vert_position, 1.0));
	
	frag_highlight_color = highlight_color;
}


#endif // VERTEX_SHADER

#ifdef FRAGMENT_SHADER

out vec4 color;

void main() {
    color = frag_highlight_color;
}


#endif //FRAGMENT_SHADER