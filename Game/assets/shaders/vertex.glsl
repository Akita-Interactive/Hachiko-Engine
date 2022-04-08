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

// Outputs
struct VertexData
{
    vec3 normal;
    vec3 pos;
    vec2 tex_coord;
};
out VertexData fragment;

void main()
{
    gl_Position = camera.proj * camera.view *  model * vec4(position, 1.0);

    fragment.normal = transpose(inverse(mat3(model))) * in_normal;
    fragment.pos = vec3(model * vec4(position, 1.0));
    fragment.tex_coord = in_tex_coord;   
}