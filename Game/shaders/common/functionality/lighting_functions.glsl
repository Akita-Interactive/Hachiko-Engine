#ifndef _LIGHTING_FUNCTIONS_INCLUDE_
#define _LIGHTING_FUNCTIONS_INCLUDE_

float Square(float a)
{
    return a * a;
}

vec3 SchlickFresnel(const vec3 f0, float LdotH)
{
    return f0 + (vec3(1.0) - f0) * pow(1.0 - LdotH, 5.0);
}

float GGX(const vec3 normal, const vec3 halfway_dir, float roughness)
{
    float a2 = Square(roughness);
    float NdotH = max(dot(normal, halfway_dir), 0.00001);
    
    return  a2 / (PI * Square(Square(NdotH) * (a2 - 1.0) + 1.0));
}

float SmithVisibilityFunction(const vec3 normal, const vec3 view_dir, const vec3 light_dir, float roughness)
{
    float NdotV = max(dot(normal, view_dir), 0.00001);
    float NdotL = max(dot(normal, light_dir), 0.00001);

    return 0.5 / (NdotL * (NdotV * (1 - roughness) + roughness) + NdotV * (NdotL * (1 - roughness) + roughness));
}

vec3 PBR(const vec3 normal, const vec3 view_dir, const vec3 light_dir,  const vec3 light_color,
         const vec3 diffuse_color, const vec3 F0, float attenuation, float smoothness)
{
    vec3 reflect_dir = reflect(-light_dir, normal);
    // Should equal cos theta
    float NdL = max(dot(normal, light_dir), 0.00001);
    // Phong specular strength
    float VdR =  max(dot(view_dir, reflect_dir), 0.00001);

    float roughness = max(Square(1.0 - smoothness), 0.00001);

    vec3 halfway_dir = normalize(light_dir + view_dir);
    float LdotH = max(dot(light_dir, halfway_dir), 0.00001);

    float NDF =  GGX(normal, halfway_dir, roughness);
    float SVF = SmithVisibilityFunction(normal, view_dir, light_dir, roughness);
    vec3 fresnel = SchlickFresnel(F0, LdotH);

    //return light_color * (diffuse_color * F0 + fresnel * SVF * NDF) * NdL * attenuation;
    return light_color * (diffuse_color * (vec3(1.0) - F0) + fresnel * SVF * NDF) * NdL * attenuation;
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
    return max(Square(max(1 - pow(distance/radius, 4), 0.0)), 0.0) / (distance * distance + 1);
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
    vec3 L = normalize(light.position.xyz - frag_pos);
    vec3 cone_direction = normalize(light.direction.xyz);

    float attenuation = SpotAttenuation(L, cone_direction, light.radius);
    float cone = Cone(L, cone_direction, light.inner, light.outer);
    return PBR(normal, view_dir, L, light.color.rgb, diffuse_color, F0, attenuation * cone, smoothness) * light.intensity;
}

vec3 GetAmbientLight(in vec3 normal, in vec3 R, float NdotV, float roughness, in vec3 diffuse_color, in vec3 specular_color)
{
    if (activate_IBL > 0)
    {
        vec3 irradiance = texture(diffuseIBL, normal).rgb;
        vec3 radiance = textureLod( prefilteredIBL, R, roughness * prefilteredIBL_numLevels).rgb;
        vec2 fab = texture( environmentBRDF, vec2(NdotV, roughness)).rg;

        vec3 diffuse = (diffuse_color * (1 - specular_color));

        return (diffuse * irradiance + radiance * (specular_color * fab.x + fab.y)) * lights.ambient.intensity;
    }
    return diffuse_color * lights.ambient.color.rgb * lights.ambient.intensity;
}

#endif
