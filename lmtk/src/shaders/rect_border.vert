#version 440

layout(binding = 0) uniform uniform_buffer {
    float xpos, ypos, width, height;
    vec4 colour;
    float border_thickness, xscale, yscale;
};

out gl_PerVertex {
    vec4 gl_Position;
};

void main() {
    float left = xpos;
    float right = xpos + width;
    float top = ypos;
    float bottom = ypos + height;

    float half_thickness = border_thickness / 2.f;
    vec2 points[8] = {
    vec2(left + half_thickness, top + border_thickness),
    vec2(left + half_thickness, bottom),
    vec2(left + border_thickness, bottom - half_thickness),
    vec2(right, bottom - half_thickness),
    vec2(right - half_thickness, bottom - border_thickness),
    vec2(right - half_thickness, top),
    vec2(right - border_thickness, top + half_thickness),
    vec2(left, top + half_thickness)
    };
    vec2 point = points[gl_VertexIndex];
    point.x *= xscale;
    point.y *= yscale;
    point -= 1.f;
    gl_Position = vec4(point, 0.f, 1.f);
}
