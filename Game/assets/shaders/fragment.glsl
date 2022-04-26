# version 440

#define DIFFUSE_SAMPLER 0
#define SPECULAR_SAMPLER 1
#define NORMAL_SAMPLER 2
#define N_2D_SAMPLERS 3

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

layout(std140, binding = 1) uniform Material
{
    vec4 diffuse_color;
    vec4 specular_color;
    uint diffuse_flag;
    uint specular_flag;
    uint normal_flag;
    float shininess;
} material;

layout(std140, binding = 2) uniform Lights
{
    AmbientLight ambient;
    DirLight directional;
    PointLight points[MAX_POINT_LIGHTS];
    SpotLight spots[MAX_SPOT_LIGHTS];
    uint n_points;
    uint n_spots;
} lights;

uniform sampler2D textures[N_2D_SAMPLERS];

// Inputs
struct VertexData
{
    vec3 normal;
    vec3 tangent;
    vec3 pos;
    vec2 tex_coord;
};
in VertexData fragment;

// Outputs
out vec4 color;

vec3 SchlickFresnel(const vec3 f0, float cos_theta)
{
    return f0 + (vec3(1.0) - f0) * pow(1.0 - cos_theta, 5.0);
}

vec3 PBR(const vec3 normal, const vec3 view_dir, const vec3 light_dir,  const vec3 light_color,
         const vec3 diffuse_color, const vec3 specular_color, float shininess, float attenuation)
{
    vec3 reflect_dir = reflect(-light_dir, normal);
    // Should equal cos theta
    float NdL = max(dot(normal, light_dir), 0.0001);
    // Phong specular strength
    float VdR =  max(dot(view_dir, reflect_dir), 0.0001);

    //vec3 halfway_dir = normalize(light_dir + view_dir);
    //float cos_theta = max(dot(view_dir, halfway_dir), 0.0);   
    //vec3 fresnel = SchlickFresnel(specular_color, cos_theta);

    vec3 fresnel = SchlickFresnel(specular_color, NdL);
    
    vec3 part1 = (diffuse_color * (vec3(1.0) - specular_color)) / PI;
    vec3 part2 = ((shininess + 2.0) / (2.0 * PI)) * fresnel * pow(VdR, shininess);

    return light_color * (part1 + part2) * NdL * attenuation;
}

vec3 DirectionalPBR(const vec3 normal, const vec3 view_dir, const DirLight light,
                    const vec3 diffuse_color, const vec3 specular_color, float shininess)
{
    vec3 L = normalize(-light.direction.xyz);
    
    float attenuation = 1.0;
    // Input dir goes from light to fragment, swap it for calculations
    return PBR(normal, view_dir, L, light.color.rgb, diffuse_color, specular_color, shininess, attenuation) * light.intensity;
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
                   const vec3 diffuse_color, const vec3 specular_color, float shininess)
{
    vec3 L = light.position.xyz - frag_pos;
    float light_distance = length(L);
    L = normalize(L);

    float radius = 250.0;
    float attenuation = EpicAttenuation(light_distance, light.radius);
    return PBR(normal, view_dir, L, light.color.rgb, diffuse_color, specular_color, shininess, attenuation) * light.intensity;
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
             const vec3 diffuse_color, const vec3 specular_color, float shininess)
{
    // Not the same as spot light direction (L)
    vec3 L = light.position.xyz - frag_pos;
    L = normalize(L);
    vec3 cone_direction = normalize(light.direction.xyz);

    float attenuation = SpotAttenuation(L, cone_direction, light.radius);
    float cone = Cone(L, cone_direction, light.inner, light.outer);
    return PBR(normal, view_dir, L, light.color.rgb, diffuse_color, specular_color, shininess, attenuation * cone) * light.intensity;
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
    if (material.normal_flag > 0)
    {
        mat3 tbn = CreateTangentSpace(normalize(fragment.normal), normalize(fragment.tangent));
	    vec3 fragmentNormal = tbn * (texture(textures[NORMAL_SAMPLER], fragment.tex_coord).xyz * 2.0 - 1.0);
	    norm = normalize(fragmentNormal);
    }

    vec3 view_dir = normalize(camera.pos - fragment.pos);
    
    vec3 diffuse_color = material.diffuse_color.rgb;
    if (material.diffuse_flag > 0)
    {
        diffuse_color = pow(texture(textures[DIFFUSE_SAMPLER], fragment.tex_coord).rgb, vec3(2.2));
    }

    float shininess = material.shininess;
    vec3 specular_color = material.specular_color.rgb;
    if (material.specular_flag > 0)
    {
        // Should we gaMma correct specular?
        // specular_color = pow(texture(textures[SPECULAR_SAMPLER], fragment.tex_coord).rgb, vec3(2.2));
        specular_color = texture(textures[SPECULAR_SAMPLER], fragment.tex_coord).rgb;
        // Use alpha as shininess?
        //shininess = texture(textures[SPECULAR_SAMPLER], fragment.tex_coord).a;
    }

    
    vec3 hdr_color = vec3(0.0);

    hdr_color += DirectionalPBR(norm, view_dir, lights.directional, diffuse_color, specular_color, shininess);
    
    for(uint i=0; i<lights.n_points; ++i)
    {
        hdr_color +=  PositionalPBR(fragment.pos, norm, view_dir, lights.points[i], diffuse_color, specular_color, shininess);
    }

    for(uint i=0; i<lights.n_spots; ++i)
    {
        hdr_color +=  SpotPBR(fragment.pos, norm, view_dir, lights.spots[i], diffuse_color, specular_color, shininess);
        
    }   
    hdr_color += diffuse_color * lights.ambient.color.rgb * lights.ambient.intensity;

    // Reinhard tone mapping
    vec3 ldr_color = hdr_color / (hdr_color + vec3(1.0));

    // Gamma correction & alpha from diffuse texture
    color = vec4(pow(ldr_color.rgb, vec3(1.0/2.2)), texture(textures[DIFFUSE_SAMPLER], fragment.tex_coord).a);
}
