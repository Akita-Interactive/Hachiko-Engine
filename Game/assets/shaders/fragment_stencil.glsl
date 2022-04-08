# version 440

layout(std140, row_major, binding = 0) uniform Camera
{
    mat4 view;
    mat4 proj;
    vec3 pos;
} camera;

// Outputs
out vec4 color;

void main()
{
    color = vec4(1.0, 0.0, 0.0, 1.0);
}