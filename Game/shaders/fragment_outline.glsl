#version 460

layout (binding = 11) uniform sampler2D outline_texture;

out vec4 fragment_color;
in vec2 texture_coords;

void main()
{
    fragment_color = texture(outline_texture, texture_coords).rgba * 2;
}