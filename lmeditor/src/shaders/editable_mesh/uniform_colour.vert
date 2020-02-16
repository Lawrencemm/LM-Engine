#version 440

layout(location = 0) in vec3 vpos;

layout(binding = 0) uniform MeshUniform {
    vec4 colour;
    mat4 model_view_projection;
};

out gl_PerVertex {
    vec4 gl_Position;
    float gl_PointSize;
};

layout(location = 0) out vec4 fragColor;

void main() {
    gl_Position = model_view_projection * vec4(vpos, 1.f);
    fragColor = colour;
}
