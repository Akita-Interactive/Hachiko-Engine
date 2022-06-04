mat3 CreateTangentSpace(const vec3 normal, const vec3 tangent)
{
	vec3 ortho_tangent = normalize(tangent - dot(tangent, normal) * normal); // Gram-Schmidt
	vec3 bitangent = cross(normal, ortho_tangent);
	return mat3(tangent, bitangent, normal);
}

// TODO: Be sure that this is fully correct:
vec3 CalculateNormal(uint normal_flag, vec3 texture_normal_coords, vec3 fragment_normal_coords, vec3 fragment_tangent)
{
    vec3 normal = normalize(fragment_normal_coords);

    if (normal_flag > 0)
    {
        mat3 tbn = CreateTangentSpace(normal, normalize(fragment_tangent));
        vec3 texture_normal = normalize(texture_normal_coords.xyz);
	    vec3 fragment_normal = tbn * (texture_normal * 2.0 - 1.0);
	    normal = normalize(fragment_normal);
    }

    return normal;
}

// TODO: Check this function if it's fully correct:
void CalculateSpecularDiffuseSmoothness(
    uint material_diffuse_flag,
    uint material_metallic_flag,
    uint material_specular_flag,
    uint material_is_metallic,
    uint material_smoothness_alpha,
    vec4 material_diffuse_color,
    vec4 material_specular_color,
    float material_metalness_value,
    float material_smoothness,
    vec4 texture_metal_color,
    vec4 texture_specular_color,
    vec4 texture_diffuse_color,
    inout float smoothness, 
    inout vec3 diffuse, 
    inout vec3 specular)
{
    vec4 diffuse_temp = material_diffuse_flag * pow(texture_diffuse_color, vec4(2.2)) + // true
                        (1 - material_diffuse_flag) * material_diffuse_color;           // false

    if (material_is_metallic > 0)
    {
        // If it doesn't have metalness texture it adds the metalness value of the material:
        float metalness_mask = material_metallic_flag * texture_metal_color.r +         // true
                               (1 - material_metallic_flag) * material_metalness_value; // false
        
        // Cd:
        diffuse = diffuse_temp.rgb * (1 - metalness_mask);
        // f0:
        specular = vec3(0.04) * (1 - metalness_mask) + diffuse_temp.rgb * metalness_mask;

        smoothness = material_smoothness * ((material_smoothness_alpha * texture_metal_color.a) + // true
                     ((1 - material_smoothness_alpha) * diffuse_temp.a ));                        // false
    }
    else 
    {
        // Cd:
        diffuse = diffuse_temp.rgb;

        // If the flag is true (1) it will paint texture specular color, otherwise material specular color:
        vec4 color_specular = (material_specular_flag * texture_specular_color) +       // true
                              ((1 - material_specular_flag) * material_specular_color); // false
        
        // f0:
        specular = color_specular.rgb;

        smoothness = material_smoothness * ((material_smoothness_alpha * color_specular.a) + // true
                     ((1 - material_smoothness_alpha)* diffuse_temp.a ));                    // false
    } 
}

vec3 CalculateEmissive(vec3 texture_emissive_color, vec4 material_emissive_color, uint material_emissive_flag)
{
    vec3 emissive_color = material_emissive_color.rgb;

    if (material_emissive_flag != 0)
    {
        emissive_color *= texture_emissive_color;
    }

    emissive_color *= material_emissive_color.a;

    return emissive_color;
}