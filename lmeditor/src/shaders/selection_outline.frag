#version 440

layout(location = 0) out vec4 frag_colour;

layout(binding = 0) uniform MeshUniform {
    mat4 unused_unused_proj_matrix;
    vec3 colour;
};

void main() {
    frag_colour = vec4(colour, 1.f);
}
