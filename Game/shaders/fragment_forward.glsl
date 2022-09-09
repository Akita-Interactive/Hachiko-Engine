# version 460

#extension GL_ARB_shading_language_include : require

#include "/common/structs/vertex_data.glsl"
#include "/common/structs/material.glsl"
#include "/common/structs/lights.glsl"
#include "/common/uniforms/camera_uniform.glsl"
#include "/common/uniforms/material_and_texture_uniforms.glsl"
#include "/common/uniforms/lights_uniform.glsl"
#include "/common/functionality/material_property_functions.glsl"
#include "/common/functionality/lighting_functions.glsl"

// Inputs
in VertexData fragment;
in flat uint instance;

// Outputs
out vec4 color;

void main()
{
    Material material = materialsBuffer.materials[instance];
    
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

    vec3 normal = CalculateNormal(
        material.normal_flag,
        allMyTextures[material.normal_map.texIndex],
        fragment.tex_coord,
        material.normal_map.layerIndex,
        fragment.normal, 
        fragment.tangent); 
    
    vec3 view_direction = normalize(camera.pos - fragment.pos);
    
    vec3 hdr_color = GetAmbientLight(normal, reflect(-view_direction, normal), dot(normal, view_direction), pow(1.0 - smoothness, 2), diffuse.rgb, specular);
    hdr_color += DirectionalPBR(normal, view_direction, lights.directional, diffuse.rgb, specular, smoothness);
    
    for(uint i=0; i<lights.n_points; ++i)
    {
        hdr_color +=  PositionalPBR(fragment.pos, normal, view_direction, lights.points[i], diffuse.rgb, specular, smoothness);
    }

    for(uint i=0; i<lights.n_spots; ++i)
    {
        hdr_color +=  SpotPBR(fragment.pos, normal, view_direction, lights.spots[i], diffuse.rgb, specular, smoothness);
    }

    // Calculate emissive color:
    vec3 emissive = CalculateEmissive(
        allMyTextures[material.emissive_map.texIndex],
        fragment.tex_coord,
        material.emissive_map.layerIndex,
        material.emissive_color,
        material.emissive_flag
    );

    hdr_color += emissive;

    // Reinhard tone mapping
    vec3 ldr_color = hdr_color / (hdr_color + vec3(1.0));

    // Gamma correction & alpha from diffuse texture if it is transparent
    color = vec4(pow(ldr_color.rgb, vec3(1.0/2.2)), diffuse.a * material.is_transparent + (1 - material.is_transparent));
}