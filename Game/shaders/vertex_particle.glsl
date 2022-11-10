# version 460

#extension GL_ARB_shading_language_include : require

#include "/common/uniforms/camera_uniform.glsl"

layout(location=0) in vec3 in_position;
layout(location=1) in vec2 in_tex_coord;

uniform mat4 model;
uniform int ignore_camera;

out vec2 uv0;

void main()
{
	gl_Position = (camera.proj * camera.view * model * vec4(in_position.xyz , 1.0)) * (1 - ignore_camera) + vec4(in_position.xyz , 1.0) * ignore_camera;
	uv0 = in_tex_coord;
}