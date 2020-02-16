#version 440

layout(location = 0) in vec2 vpos;
layout(location = 1) in vec2 texcoord_in;

layout(binding = 0) uniform uniform_buffer {
    mat3 transform;
};

out gl_PerVertex {
    vec4 gl_Position;
};

layout(location = 0) out vec2 texcoord;

void main() {
    vec3 transformed = transform * vec3(vpos, 1.f);
    gl_Position = vec4(transformed, 1.f);
    texcoord = texcoord_in;
}
