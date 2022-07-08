#version 460

#extension GL_ARB_shading_language_include : require

#include "/common/uniforms/camera_uniform.glsl"

// Inputs
in vec2 tex_coord;

uniform sampler2D diffuse;
uniform int diffuse_flag;
uniform vec4 img_color;

uniform vec2 factor;
uniform vec2 animation_index;

// Outputs
out vec4 color;

void main()
{
    // vec4(pow(color.rgb, vec3(GAMMA)), color.a);
    color = img_color;
    if (diffuse_flag > 0)
    {
        float u = (tex_coord.x + animation_index.x) * factor.x;
        float v = (tex_coord.y + animation_index.y) * factor.y;
        color *= texture2D(diffuse,  vec2(u, v));
    }
}