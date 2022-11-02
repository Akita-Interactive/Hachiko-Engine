#version 460

layout(location = 0) in vec3 input_position;
layout(location = 1) in vec2 input_texture_coords;

out vec2 texture_coords;

void main()
{
    gl_Position = vec4(input_position, 1.0f);
    texture_coords = input_texture_coords;
}