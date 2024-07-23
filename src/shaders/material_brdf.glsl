// main shader code


#define BRDF_DISNEY  0
#define BRDF_LAMBERT 1
#define BRDF_DIFFUSE BRDF_LAMBERT


#define PI 3.14159265358979323846


COMMUNICATE vec3 frag_pos;
COMMUNICATE vec3 frag_normal;
COMMUNICATE vec2 frag_uv_coord;

#ifdef VERTEX_SHADER 

layout(location = 0) in vec3 vert_position;
layout(location = 1) in vec3 vert_normal;
layout(location = 2) in vec2 vert_uv_coord;

uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;

void main() {
	
	gl_Position = projection * view * model * vec4(vert_position, 1.0);
	frag_pos = vec3(model * vec4(vert_position, 1.0));
	
	frag_normal = normalize(mat3(model) * vert_normal).xyz;	
	frag_uv_coord = vert_uv_coord;
}

#endif // VERTEX_SHADER


// fragment shader is adapted from https://google.github.io/filament/Filament.md#materialsystem 
// which is licenced by https://github.com/google/filament/
//                                Apache License
//                          Version 2.0, January 2004
//                        http://www.apache.org/licenses/

#ifdef FRAGMENT_SHADER 



const vec3 Silver = vec3(0.97, 0.96, 0.91);
const vec3 Aluminum = vec3(0.91, 0.92, 0.92);
const vec3 Titanium = vec3(0.76, 0.73, 0.69);
const vec3 Iron = vec3(0.77, 0.78, 0.78);
const vec3 Platinum = vec3(0.83, 0.81, 0.78);
const vec3 Gold = vec3(1.00, 0.85, 0.57);
const vec3 Brass = vec3(0.98, 0.90, 0.59);
const vec3 Copper = vec3(0.97, 0.74, 0.62);


uniform sampler2D texture0;

out vec4 color;

uniform vec3 light_direction;
uniform vec3 light_color;
uniform float light_strength;
uniform float ambient_strength;

float D_GGX(float NoH, float a) {
    float a2 = a * a;
    float f = (NoH * a2 - NoH) * NoH + 1.0;
    return a2 / (PI * f * f);
}

vec3 F_Schlick(float u, vec3 f0) {
    return f0 + (vec3(1.0) - f0) * pow(1.0 - u, 5.0);
}

float F_Schlick(float u, float f0, float f90) {
    return f0 + (f90 - f0) * pow(1.0 - u, 5.0);
}

float Fd_Burley(float roughness, float NoV, float NoL, float LoH) {
    // Burley 2012, "Physically-Based Shading at Disney"
    float f90 = 0.5 + 2.0 * roughness * LoH * LoH;
    float lightScatter = F_Schlick(1.0, f90, NoL);
    float viewScatter  = F_Schlick(1.0, f90, NoV);
    return lightScatter * viewScatter * (1.0 / PI);
}

float V_SmithGGXCorrelated(float NoV, float NoL, float a) {
    float a2 = a * a;
    float GGXL = NoV * sqrt((-NoL * a2 + NoL) * NoL + a2);
    float GGXV = NoL * sqrt((-NoV * a2 + NoV) * NoV + a2);
    return 0.5 / (GGXV + GGXL);
}

float Fd_Lambert() {
    return 1.0 / PI;
}

// returns final color after lighting
vec3 BRDF(
    vec3 diffuse_color,
    vec3 n,
    vec3 v,
    vec3 l,
    float perceptual_roughness,
    vec3 f0
)
{
    vec3 h = normalize(v + l);

    float NoV = abs(dot(n, v)) + 1e-5;
    float NoL = clamp(dot(n, l), 0.0, 1.0);
    float NoH = clamp(dot(n, h), 0.0, 1.0);
    float LoH = clamp(dot(l, h), 0.0, 1.0);

    // perceptually linear roughness to roughness (see parameterization)
    float roughness = perceptual_roughness * perceptual_roughness;

    float D = D_GGX(NoH, roughness);
    vec3  F = F_Schlick(LoH, f0);
    float V = V_SmithGGXCorrelated(NoV, NoL, roughness);

    // specular BRDF
    vec3 specular = (D * V) * F;

    // diffuse BRDF
    vec3 diffuse = diffuse_color;
    #if BRDF_DIFFUSE == BRDF_DISNEY
        diffuse *= Fd_Burley(roughness, NoV, NoL, LoH);
    #else
        diffuse *= Fd_Lambert();
    #endif

    // apply lighting...

    return (specular + diffuse) * light_color * light_strength * NoL;

}

void main() {

	vec3 n = normalize(frag_normal);
    vec3 v = normalize(frag_pos);
	vec3 l = normalize(-light_direction);
    
    float roughness = 0.0;
    vec3 f0 = Gold;

    float diff = max(dot(n, l), 0.0);
	vec3 diffuse = diff * light_color;

    vec3 brdf_color = BRDF(diffuse, n, v, l, roughness, f0);

	//vec3 ambient = ambient_strength * light_color;
	//color = vec4(ambient + diffuse, 0) * texture(texture0, frag_uv_coord);
    color = vec4(brdf_color + ambient_strength, 1) * texture(texture0, frag_uv_coord);
}

#endif //FRAGMENT_SHADER