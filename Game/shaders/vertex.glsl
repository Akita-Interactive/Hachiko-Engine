# version 460

layout(location=0) in vec3 in_position;
layout(location=1) in vec3 in_normal;
layout(location=2) in vec2 in_tex_coord;
layout(location=3) in vec3 in_tangent;
layout(location=4) in ivec4 in_bone_indices;
layout(location=5) in vec4 in_bone_weights;
layout(location=6) in uint instance_idx;

#define MAX_BONES 64

uniform bool has_bones;

layout(std140, row_major, binding = 0) uniform Camera
{
    mat4 view;
    mat4 proj;
    vec3 pos;
} camera;

readonly layout(std430, row_major, binding = 3) buffer Transforms
{
 mat4 models[];
};

readonly layout(std430, row_major, binding = 4) buffer Palettes
{
 mat4 palettes[];
};

struct PalettePerInstance {
    uint  numBones;
    uint  paletteOffset;
    uint  padding0, padding1;
};

readonly layout(std430, row_major, binding = 5) buffer PalettesPerInstances
{
 PalettePerInstance paletteInstanceInfo[];
};

// Outputs
struct VertexData
{
    vec3 normal;
    vec3 tangent;
    vec3 pos;
    vec2 tex_coord;
};
out VertexData fragment;
out flat uint instance;

void main()
{
    vec4 position = vec4(in_position, 1.0);
    vec4 normal = vec4(in_normal, 0.0);
    vec4 tangent = vec4(in_tangent, 0.0);

    instance = gl_BaseInstance;

    if (has_bones)
    {
        PalettePerInstance palette_per_instance = paletteInstanceInfo[instance];
        mat4 skin_transform = palettes[palette_per_instance.paletteOffset + in_bone_indices[0]] * in_bone_weights[0] + palettes[palette_per_instance.paletteOffset + in_bone_indices[1]] * in_bone_weights[1] +
            palettes[palette_per_instance.paletteOffset + in_bone_indices[2]] * in_bone_weights[2] + palettes[palette_per_instance.paletteOffset + in_bone_indices[3]] * in_bone_weights[3];

        position = (skin_transform*vec4(in_position, 1.0));
        normal = (skin_transform*vec4(in_normal, 0.0));
        //tangent = (skin_transform*vec4(in_tangent, 0.0));
    }

    gl_Position = camera.proj * camera.view *  models[instance] * position;

    fragment.normal = transpose(inverse(mat3(models[instance]))) * normal.xyz;
    fragment.tangent = transpose(inverse(mat3(models[instance]))) * tangent.xyz;
    fragment.pos = vec3(models[instance] * position);
    fragment.tex_coord = in_tex_coord;
}