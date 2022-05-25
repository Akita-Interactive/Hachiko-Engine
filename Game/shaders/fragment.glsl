# version 460

#define DIFFUSE_SAMPLER 0
#define SPECULAR_SAMPLER 1
#define NORMAL_SAMPLER 2
#define METALLIC_SAMPLER 3
#define EMISSIVE_SAMPLER 4
#define N_2D_SAMPLERS 5

#define MAX_POINT_LIGHTS 4
#define MAX_SPOT_LIGHTS 4

#define PI 3.141597

struct AmbientLight
{
    vec4 color;
    float intensity;
};

struct DirLight
{
    vec4 direction;
    vec4 color;
    float intensity;
};

struct PointLight
{
    vec4 position;
    vec4 color;
    float intensity;
    float radius;
};

struct SpotLight
{
    vec4 position;
    vec4 direction;
    vec4 color;
    float inner;
    float outer;
    float intensity;
    float radius;
};

layout(std140, row_major, binding = 0) uniform Camera
{
    mat4 view;
    mat4 proj;
    vec3 pos;
} camera;

/* DEPRECATED
layout(std140, binding = 1) uniform Material
{
    vec4 diffuse_color;
    vec4 specular_color;
    vec4 emissive_color;
    uint diffuse_flag;
    uint specular_flag;
    uint normal_flag;
    uint metallic_flag;
    uint emissive_flag;
    float smoothness;
    float metalness_value;
    uint is_metallic;
    uint smoothness_alpha;
    uint is_transparent;
} material;
*/

layout(std140, binding = 2) uniform Lights
{
    AmbientLight ambient;
    DirLight directional;
    PointLight points[MAX_POINT_LIGHTS];
    SpotLight spots[MAX_SPOT_LIGHTS];
    uint n_points;
    uint n_spots;
} lights;

// Texture Batching
uniform sampler2DArray allMyTextures[gl_MaxTextureImageUnits-8];

struct TexAddress {
    int texIndex;
    int layerIndex;
};

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
    //uint padding0;
};

readonly layout(std430, binding = 1) buffer Materials {
    Material materials[];
} materialsBuffer;

// Inputs
struct VertexData
{
    vec3 normal;
    vec3 tangent;
    vec3 pos;
    vec2 tex_coord;
};
in VertexData fragment;
in flat uint instance;

// Outputs
out vec4 color;

vec3 SchlickFresnel(const vec3 f0, float half_vector)
{
    return f0 + (vec3(1.0) - f0) * pow(1.0 - half_vector, 5.0);
}

float GGX(const vec3 normal, const vec3 halfway_dir, float roughness)
{
    float a2 = pow(roughness, 2.0);
    float NdotH = max(dot(normal, halfway_dir), 0.00001);
    float NdotH2 = pow(NdotH, 2.0);
    
    float denom = PI * pow((NdotH2 * (a2 - 1.0) + 1.0), 2.0);

    return  a2 / denom;
}

float SmithVisibilityFunction(const vec3 normal, const vec3 view_dir, const vec3 light_dir, float roughness)
{
    float NdotV = max(dot(normal, view_dir), 0.00001);
    float NdotL = max(dot(normal, light_dir), 0.00001);

    float denom = NdotL * (NdotV * (1 - roughness) + roughness) + NdotV * (NdotL * (1 - roughness) + roughness);
    return 0.5 / denom;
}

vec3 PBR(const vec3 normal, const vec3 view_dir, const vec3 light_dir,  const vec3 light_color,
         const vec3 diffuse_color, const vec3 F0, float attenuation, float smoothness)
{
    vec3 reflect_dir = reflect(-light_dir, normal);
    // Should equal cos theta
    float NdL = max(dot(normal, light_dir), 0.00001);
    // Phong specular strength
    float VdR =  max(dot(view_dir, reflect_dir), 0.00001);

    float roughness = pow((1.0 - smoothness), 2.0);

    vec3 halfway_dir = normalize(light_dir + view_dir);
    float VdotH = max(dot(view_dir, halfway_dir), 0.00001);

    float NDF =  GGX(normal, halfway_dir, roughness);
    float SVF = SmithVisibilityFunction(normal, view_dir, light_dir, roughness);
    vec3 fresnel = SchlickFresnel(F0, VdotH);
    
    vec3 part1 = diffuse_color * (vec3(1.0) - F0);
    vec3 part2 = (fresnel*SVF*NDF)/4;

    

    return light_color * (diffuse_color * (vec3(1.0) - F0) + 0.25*fresnel*SVF*NDF) * NdL * attenuation;
}

vec3 DirectionalPBR(const vec3 normal, const vec3 view_dir, const DirLight light,
                    const vec3 diffuse_color, const vec3 F0, float smoothness)
{
    vec3 L = normalize(-light.direction.xyz);
    
    float attenuation = 1.0;
    // Input dir goes from light to fragment, swap it for calculations
    return PBR(normal, view_dir, L, light.color.rgb, diffuse_color, F0, attenuation, smoothness) * light.intensity;
}

float Attenuation(float distance)
{
    return 1.0/(1.0+0.3*distance+0.3*(distance*distance));
}

float EpicAttenuation(float distance, float radius)
{
    return max(pow(max(1 - pow(distance/radius, 4), 0.0), 2.0), 0.0) / (distance * distance + 1);
}

vec3 PositionalPBR(const vec3 frag_pos, const vec3 normal, const vec3 view_dir, const PointLight light, 
                   const vec3 diffuse_color, const vec3 F0, float smoothness)
{
    vec3 L = light.position.xyz - frag_pos;
    float light_distance = length(L);
    L = normalize(L);

    float radius = 250.0;
    float attenuation = EpicAttenuation(light_distance, light.radius);
    return PBR(normal, view_dir, L, light.color.rgb, diffuse_color, F0, attenuation, smoothness) * light.intensity;
}

// TODO: Test more
float SpotAttenuation(const vec3 L, const vec3 cone_direction, float radius)
{
     // Light direction from light to outside
    float distance = dot(-L, cone_direction);
    return EpicAttenuation(distance, radius);
}

float Cone(const vec3 L, const vec3 cone_direction, float inner, float outer)
{
    // Light direction from light to fragment (Reverse than pbr)
    float C = dot(-L, cone_direction);
    float c_inner = cos(inner);
    if (C > c_inner) return 1.0;
    float c_outer = cos(outer);
    if (C > c_outer) return (C - c_outer)/(c_inner - c_outer);
    return 0.0;
}

vec3 SpotPBR(const vec3 frag_pos, const vec3 normal, const vec3 view_dir, const SpotLight light, 
             const vec3 diffuse_color, const vec3 F0, float smoothness)
{
    // Not the same as spot light direction (L)
    vec3 L = light.position.xyz - frag_pos;
    L = normalize(L);
    vec3 cone_direction = normalize(light.direction.xyz);

    float attenuation = SpotAttenuation(L, cone_direction, light.radius);
    float cone = Cone(L, cone_direction, light.inner, light.outer);
    return PBR(normal, view_dir, L, light.color.rgb, diffuse_color, F0, attenuation * cone, smoothness) * light.intensity;
}

mat3 CreateTangentSpace(const vec3 normal, const vec3 tangent)
{
	vec3 ortho_tangent = normalize(tangent - dot(tangent, normal) * normal); // Gram-Schmidt
	vec3 bitangent = cross(normal, ortho_tangent);
	return mat3(tangent, bitangent, normal);
}


void main()
{
    vec3 norm = normalize(fragment.normal);
    vec3 view_dir = normalize(camera.pos - fragment.pos);
    
    Material material = materialsBuffer.materials[instance];

    if (material.normal_flag > 0)
    {
        mat3 tbn = CreateTangentSpace(normalize(fragment.normal), normalize(fragment.tangent));
	    vec3 fragmentNormal = tbn * (texture(allMyTextures[material.normal_map.texIndex], vec3(fragment.tex_coord, material.normal_map.layerIndex)).xyz * 2.0 - 1.0);
	    norm = normalize(fragmentNormal);
    }

    vec4 diffuse_color = material.diffuse_color;
    if (material.diffuse_flag > 0)
    {
        diffuse_color = pow(texture(allMyTextures[material.diffuse_map.texIndex], vec3(fragment.tex_coord, material.diffuse_map.layerIndex)), vec4(2.2, 2.2, 2.2, 1.0));
    }

    vec3 f0;
    vec3 Cd;
    float smoothness;

    if(material.is_metallic > 0)
    {
        vec4 texture_metal_color = texture(allMyTextures[material.metallic_map.texIndex], vec3(fragment.tex_coord, material.metallic_map.layerIndex));
        float metalnessMask = material.metallic_flag * texture_metal_color.r + (1 - material.metallic_flag) * material.metalness_value;
        Cd = diffuse_color.rgb * (1 - metalnessMask);
        f0 = vec3(0.04) * (1 - metalnessMask) + diffuse_color.rgb * metalnessMask;
        smoothness = material.smoothness * ((material.smoothness_alpha * texture_metal_color.a) + ((1 - material.smoothness_alpha)* diffuse_color.a));
    }
    else 
    {
        Cd = diffuse_color.rgb;
        vec4 texture_spec_color = texture(allMyTextures[material.specular_map.texIndex], vec3(fragment.tex_coord, material.specular_map.layerIndex));
        // If the flag is true (1) it will paint texture color, otherwise specular color
        vec4 colorSpecular = (material.specular_flag * texture_spec_color) + ((1 - material.specular_flag) * material.specular_color);
        f0 = colorSpecular.rgb;
        smoothness = material.smoothness * ((material.smoothness_alpha * colorSpecular.a) + ((1 - material.smoothness_alpha)* diffuse_color.a ));
    }
    
    vec3 hdr_color = vec3(0.0);
    hdr_color += DirectionalPBR(norm, view_dir, lights.directional, Cd, f0, smoothness);
    
    for(uint i=0; i<lights.n_points; ++i)
    {
        hdr_color +=  PositionalPBR(fragment.pos, norm, view_dir, lights.points[i], Cd, f0, smoothness);
    }

    for(uint i=0; i<lights.n_spots; ++i)
    {
        hdr_color +=  SpotPBR(fragment.pos, norm, view_dir, lights.spots[i], Cd, f0, smoothness);
        
    }   
    hdr_color += Cd * lights.ambient.color.rgb * lights.ambient.intensity;

    // Emissive map
    vec3 emissive = material.emissive_color.rgb;
    if(material.emissive_flag != 0)
    {
        emissive *= texture(allMyTextures[material.emissive_map.texIndex], vec3(fragment.tex_coord, material.emissive_map.layerIndex)).rgb;
    }

    hdr_color += emissive * material.emissive_color.a;

    // Reinhard tone mapping
    vec3 ldr_color = hdr_color / (hdr_color + vec3(1.0));

    // Gamma correction & alpha from diffuse texture if it is transparent
    color = vec4(pow(ldr_color.rgb, vec3(1.0/2.2)), diffuse_color.a * material.is_transparent + (1 - material.is_transparent));
}