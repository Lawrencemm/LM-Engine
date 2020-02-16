#version 440

layout(binding = 0) uniform uniform_buffer {
    vec2 pos;
    vec2 size;
    vec4 colour;
};

layout(location = 0) out vec4 out_colour;

void main() {
    out_colour = colour;
}
