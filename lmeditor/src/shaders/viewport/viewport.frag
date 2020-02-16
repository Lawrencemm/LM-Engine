#version 440

layout(location = 0) in vec3 colour;
layout(location = 1) in vec3 normal;

layout(location = 0) out vec4 frag_colour;

layout(binding = 0) uniform MeshUniform {
    vec3 unused_colour;
    mat4 model;
    mat4 unused_proj_matrix;
    vec3 camera_dir;
};

void main() {
    float intensity = clamp(dot(normalize(normal), -camera_dir), 0.f, 1.f);
    intensity = max(0.1f, intensity);
    frag_colour = vec4(intensity * colour, 1.f);
}