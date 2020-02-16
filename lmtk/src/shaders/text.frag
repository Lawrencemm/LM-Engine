#version 440

layout(binding = 1) uniform sampler2D glyph_atlas_sampler;

layout(location = 0) in vec2 tex_coord;
layout(location = 0) out vec4 out_colour;

layout(binding = 0) uniform uniform_buffer {
    mat4 transform;
    vec3 colour;
};

void main() {
    out_colour = vec4(colour, texture(glyph_atlas_sampler, tex_coord));
}
