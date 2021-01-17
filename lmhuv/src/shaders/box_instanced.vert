#version 440

layout(location = 0) in vec3 vpos;
layout(location = 1) in vec3 vnormal;
layout(location = 2) in vec3 colour;
layout(location = 3) in mat4 model;

layout(binding = 0) uniform MeshUniform {
    mat4 view_projection;
    vec3 camera_dir;
};

out gl_PerVertex {
    vec4 gl_Position;
};

layout(location = 0) out vec3 colour_out;
layout(location = 1) out vec3 normal_out;

void main() {
    mat4 mvp = view_projection * model;
    gl_Position = mvp * vec4(vpos, 1.f);
    colour_out = colour;
    normal_out = transpose(inverse(mat3(model))) * vnormal;
}
