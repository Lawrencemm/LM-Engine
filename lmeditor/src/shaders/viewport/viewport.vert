#version 440

layout(location = 0) in vec3 vpos;
layout(location = 1) in vec3 vnormal;

layout(binding = 0) uniform MeshUniform {
    vec3 colour;
    mat4 model;
    mat4 model_view_projection;
    vec3 camera_dir;
};

out gl_PerVertex {
    vec4 gl_Position;
};

layout(location = 0) out vec3 colour_out;
layout(location = 1) out vec3 normal_out;

void main() {
    gl_Position = model_view_projection * vec4(vpos, 1.f);
    colour_out = colour;
    normal_out = mat3(model) * vnormal;
}
