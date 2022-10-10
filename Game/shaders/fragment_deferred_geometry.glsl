#version 460 core

#extension GL_ARB_shading_language_include : require

#include "/common/structs/vertex_data.glsl"
#include "/common/structs/material.glsl"
#include "/common/functionality/material_property_functions.glsl"
#include "/common/uniforms/material_and_texture_uniforms.glsl"

// G Buffer textures:
layout (location = 0) out vec4 g_diffuse;
layout (location = 1) out vec4 g_specular_smoothness;
layout (location = 2) out vec4 g_normal;
layout (location = 3) out vec4 g_position;
layout (location = 4) out vec4 g_emissive;

// Inputs
in VertexData fragment;
in flat uint instance;

layout (binding = 10) uniform sampler2D noise;

void main()
{
    Material material = materialsBuffer.materials[instance];

    // Disolving check
    bool modify_emisive = false;
    if (material.dissolve_progress < 1.0)
    {
        float noise_value = texture(noise, fragment.tex_coord).r;
        if (noise_value >= material.dissolve_progress)
        {
            discard;
        }
        else if (noise_value >= material.dissolve_progress - 0.05)
        {
            modify_emisive = true;
        }
    }

    // Temporary values that will be calculated:
    float smoothness;
    vec4 diffuse;
    vec3 specular;
    
    // Calculate specular color, diffuse color and smoothness based on material properties and textures:
    CalculateSpecularDiffuseSmoothness(
        material.diffuse_flag,
        material.metallic_flag,
        material.specular_flag,
        material.is_metallic,
        material.diffuse_color,
        material.specular_color,
        material.metalness_value,
        material.smoothness,
        allMyTextures[material.metallic_map.texIndex],
        allMyTextures[material.specular_map.texIndex],
        allMyTextures[material.diffuse_map.texIndex],
        fragment.tex_coord,
        material.metallic_map.layerIndex,
        material.specular_map.layerIndex,
        material.diffuse_map.layerIndex,
        material.tint_color,
        smoothness, 
        diffuse, 
        specular);

    // Store diffuse color in rgb channels of g buffer texture for diffuse: 
    g_diffuse.rgb = diffuse.rgb;
    g_diffuse.a = 1.0f;

    // Store specular color in rgb channels of g buffer texture for specular and smoothness:
    g_specular_smoothness.rgb = specular;
    // Store smoothness in a channel of g buffer texture for specular and smoothness:
    g_specular_smoothness.a = smoothness;

    // Store the fragment normal in rgb channels of g buffer texture for normal:
    g_normal.xyz = CalculateNormal(
        material.normal_flag, 
        allMyTextures[material.normal_map.texIndex],
        fragment.tex_coord,
        material.normal_map.layerIndex,
        fragment.normal, 
        fragment.tangent); 

    // Store the fragment position in rgb channels of g buffer texture for position:
    g_position.xyz = fragment.pos.xyz;

    // Store the fragment emissive color in rgb channels of g buffer texture for emissive:
    if (modify_emisive)
    {
        g_emissive.xyz = vec3(1.0, 1.0, 1.0);
    }
    else
    {
        g_emissive.xyz = CalculateEmissive(
            allMyTextures[material.emissive_map.texIndex],
            fragment.tex_coord,
            material.emissive_map.layerIndex,
            material.emissive_color,
            material.emissive_flag
        );
    }
}