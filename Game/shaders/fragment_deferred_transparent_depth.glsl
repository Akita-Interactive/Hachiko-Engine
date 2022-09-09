#version 460 core

#extension GL_ARB_shading_language_include : require

#include "/common/structs/vertex_data.glsl"

// G Buffer textures:
layout (location = 3) out vec4 g_position;

// Inputs
in VertexData fragment;

void main()
{
    // Store the fragment position in rgb channels of g buffer texture for position:
    g_position.xyz = fragment.pos.xyz;
}