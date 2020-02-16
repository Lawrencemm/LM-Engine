#version 440

layout(location = 0) in vec3 colour;
layout(location = 1) in vec3 normal;

layout(location = 0) out vec4 frag_colour;

layout(binding = 0) uniform MeshUniform {
    vec3 unused_colour;
    mat4 unused_model;
    mat4 unused_unused_proj_matrix;
    vec3 unused_camera_dir;
};

void main() {
    frag_colour = vec4(colour, 1.f);
}