# version 460

layout(location=0) in vec3 in_position;
layout(location=1) in vec2 in_tex_coord;

layout(std140, row_major, binding = 0) uniform Camera
{
    mat4 view;
    mat4 proj;
    vec3 pos;
} camera;

uniform mat4 model;

// Outputs
out vec2 tex_coord;

void main()
{
    gl_Position = camera.proj * camera.view * model * vec4(in_position, 1.0);
    tex_coord = in_tex_coord;
}