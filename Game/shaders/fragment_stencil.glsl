#version 460

#extension GL_ARB_shading_language_include : require

uniform vec4 in_color = vec4(1.0, 0.0, 0.0, 0.0);

// Outputs
out vec4 color;

void main()
{
    color = in_color;
}