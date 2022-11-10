#version 460

#extension GL_ARB_shading_language_include : require

#include "/common/structs/vertex_data.glsl"
#include "/common/structs/bones.glsl"
#include "/common/uniforms/camera_uniform.glsl"

layout(location=0) in vec3 in_position;
layout(location=1) in vec3 in_normal;
layout(location=2) in vec2 in_tex_coord;
layout(location=3) in vec3 in_tangent;
layout(location=4) in ivec4 in_bone_indices;
layout(location=5) in vec4 in_bone_weights;
layout(location=6) in uint instance_idx;

uniform bool has_bones;
uniform float outline_thickness;

readonly layout(std430, row_major, binding = 3) buffer Transforms
{
 mat4 models[];
};

readonly layout(std430, row_major, binding = 4) buffer Palettes
{
 mat4 palettes[];
};

readonly layout(std430, row_major, binding = 5) buffer PalettesPerInstances
{
 PalettePerInstance paletteInstanceInfo[];
};

void main()
{
    vec4 position = vec4(in_position, 1.0);
    vec4 normal = (vec4(in_normal, 0.0));

    uint instance = gl_BaseInstance;

    if (has_bones)
    {
        PalettePerInstance palette_per_instance = paletteInstanceInfo[instance];
        mat4 skin_transform = palettes[palette_per_instance.paletteOffset + in_bone_indices[0]] * in_bone_weights[0] + palettes[palette_per_instance.paletteOffset + in_bone_indices[1]] * in_bone_weights[1] +
            palettes[palette_per_instance.paletteOffset + in_bone_indices[2]] * in_bone_weights[2] + palettes[palette_per_instance.paletteOffset + in_bone_indices[3]] * in_bone_weights[3];

        position = (skin_transform * position);
        normal = (skin_transform * normal);
    }
    
    vec4 current_position = models[instance] * position;
    vec4 extrusion = normalize(vec4(inverse(transpose((mat3(models[instance])))) * normal.xyz, 0.0)) * outline_thickness;

    gl_Position = camera.proj * camera.view * vec4(extrusion.xyz + current_position.xyz, 1.0) ;
}