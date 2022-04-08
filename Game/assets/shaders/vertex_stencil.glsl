# version 440

layout(location=0) in vec3 position;
layout(location=1) in vec3 in_normal;
layout(location=2) in vec2 in_tex_coord;

layout(std140, row_major, binding = 0) uniform Camera
{
    mat4 view;
    mat4 proj;
    vec3 pos;
} camera;

uniform mat4 model;

void main()
{
    mat4 outline_scale = mat4(1.05, 0.0, 0.0, 0.0,
                              0.0, 1.05, 0.0, 0.0,
                              0.0, 0.0, 1.05, 0.0,
                              0.0, 0.0, 0.0, 1.0);
    mat4 scaled = model * outline_scale;
    gl_Position = camera.proj * camera.view * scaled * vec4(position, 1.0);
}