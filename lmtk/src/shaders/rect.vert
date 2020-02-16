#version 440

layout(binding = 0) uniform uniform_buffer {
    vec2 pos;
    vec2 size;
    vec4 colour;
};

out gl_PerVertex {
    vec4 gl_Position;
};

void main() {
    uint index_to_point[6] = { 0, 1, 2, 2, 3, 0 };
    vec2 points[4];
    points[0] = pos;
    points[1] = pos + vec2(0.f, size.y);
    points[2] = pos + size;
    points[3] = pos + vec2(size.x, 0.f);

    gl_Position = vec4(points[index_to_point[gl_VertexIndex]], 0.f, 1.f);
}
