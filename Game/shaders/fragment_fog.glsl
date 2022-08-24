#version 460 core

#extension GL_ARB_shading_language_include : require

#include "/common/uniforms/camera_uniform.glsl"

out vec4 fragment_color;
in vec2 texture_coords;

// Fog
uniform vec3 fog_color;
uniform float fog_global_density;
uniform float fog_height_falloff;

layout (binding = 3) uniform sampler2D g_position;

void main()
{
    vec3  fragment_position = (texture(g_position, texture_coords)).rgb;

    float point_distance = distance(fragment_position, camera.pos);
    // Equivalent to -view_direction?

    vec3 ray_direction = normalize(fragment_position - camera.pos);
    float fog_density = (fog_global_density / fog_height_falloff) * exp(-camera.pos.y * fog_height_falloff) * (1.0 - exp(-point_distance * ray_direction.y * fog_height_falloff )) / ray_direction.y;
    // Simulate blend GL_ONE_MINUS_SRC_ALPHA Factor is equal to 1−alpha of the source color vector
    fragment_color = vec4(fog_color, fog_density);
}