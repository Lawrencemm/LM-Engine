#version 440

layout(location = 0) in vec3 vpos;
layout(location = 1) in vec3 vcolour;

layout(binding = 0) uniform UniformBufferObject {
    mat4 model_view_projection;
    float point_size;
};

out gl_PerVertex {
    vec4 gl_Position;
    float gl_PointSize;
};

layout(location = 0) out vec4 fragColor;

void main() {
    gl_Position = model_view_projection * vec4(vpos, 1.f);
    gl_PointSize = point_size;
    fragColor = vec4(vcolour, 1.f);
}
