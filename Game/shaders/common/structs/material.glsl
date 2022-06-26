#ifndef _MATERIAL_INCLUDE_
#define _MATERIAL_INCLUDE_

#include "/common/structs/tex_address.glsl"

struct Material {
    vec4 diffuse_color;
    vec4 specular_color;
    vec4 emissive_color;
    uint diffuse_flag;
    uint specular_flag;
    uint normal_flag;
    uint metallic_flag;
    uint emissive_flag;
    TexAddress diffuse_map;
    TexAddress specular_map;
    TexAddress normal_map;
    TexAddress metallic_map;
    TexAddress emissive_map;
    float smoothness;
    float metalness_value;
    uint is_metallic;
    uint smoothness_alpha;
    uint is_transparent;
    vec4 tint_color;
    //uint padding0;
};

#endif
