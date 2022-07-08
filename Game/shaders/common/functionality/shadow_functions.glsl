#ifndef _SHADOW_FUNCTIONS_INCLUDE_
#define _SHADOW_FUNCTIONS_INCLUDE_

float CalculateShadow(vec4 fragment_position_from_light, sampler2D shadow_map_texture, float bias)
{
    // Perform perspective division:
    // NOTE: On the tutorials it is said that since we are using an orthographic projection,
    // this division is actually meaningless as w is always 1.0. However this step is necessary
    // when using perspective projection so we do it anyways to support perspective projection
    // out of the box.
    vec3 projection_coordinates = fragment_position_from_light.xyz / fragment_position_from_light.w;

    // The projection_coordinates will be returned in the range [-1, 1]. Because the depth from 
    // the shadow map is in the range [0,1] and we also want to use projection_coordinates from 
    // the shadow map, we transform the NDC coordinates to the range [0,1]:
    projection_coordinates = projection_coordinates * 0.5 + 0.5;

    // With these projected coordinats we can sample the depth map as the resulting [0,1] 
    // coordinates from projection_coordinates directly correspond to the transformed NDC coords
    // from the first shadow depth render pass. This calculation gives us the closest depth from
    // the light's perspective:
    float closest_depth = texture(shadow_map_texture, projection_coordinates.xy).r;

    // We can get the current depth at this fragment by retrieving the projected vector's z coords
    // which equals to the depth of this fragment from the light's perspective:
    float current_depth = projection_coordinates.z;

    // Now with closest_depth and current_depth in our hands, we check whether current_depth is 
    // higher than closest_depth, and if so this fragment is in shadow:
    float shadow = current_depth - bias > closest_depth ? 1.0f : 0.0f;

    return shadow; 
}

float linstep(float low, float high, float value)
{
    return clamp((value - low) / (high - low), 0.0, 1.0);
}

float Chebyshev(float current_depth, vec2 moments, float min_variance_value, float light_bleeding_reduction_amount_value)
{
    if (current_depth <= moments.x)
    {
        return 1.0;
    }

    float variance = min(1.0f, max(moments.y - moments.x * moments.x, min_variance_value));

    float distance_depth = current_depth - moments.x;

    float p_max = variance / (variance + distance_depth * distance_depth);
          p_max = linstep(light_bleeding_reduction_amount_value, 1.0, p_max);

    return p_max;
}

float CalculateShadowVariance(sampler2D shadow_map_texture, vec4 fragment_position_from_light, float min_variance_value, float light_bleeding_reduction_amount_value)
{
    // Perform perspective division:
    // NOTE: On the tutorials it is said that since we are using an orthographic projection,
    // this division is actually meaningless as w is always 1.0. However this step is necessary
    // when using perspective projection so we do it anyways to support perspective projection
    // out of the box.
    vec3 projection_coordinates = fragment_position_from_light.xyz / fragment_position_from_light.w;

    // The projection_coordinates will be returned in the range [-1, 1]. Because the depth from 
    // the shadow map is in the range [0,1] and we also want to use projection_coordinates from 
    // the shadow map, we transform the NDC coordinates to the range [0,1]:
    projection_coordinates = projection_coordinates * 0.5 + vec3(0.5);

    // We can get the current depth at this fragment by retrieving the projected vector's z coords
    // which equals to the depth of this fragment from the light's perspective:
    float current_depth = projection_coordinates.z;

    vec2 moments = texture(shadow_map_texture, projection_coordinates.xy).xy;

    float shadow = Chebyshev(current_depth, moments, min_variance_value, light_bleeding_reduction_amount_value);

    return shadow;
}

float CalculateShadowExponentialVariance(sampler2D shadow_map_texture, vec4 fragment_position_from_light, float min_variance_value, float light_bleeding_reduction_amount_value, float shadow_bias_value, float exponent_value)
{
    // Perform perspective division:
    // NOTE: On the tutorials it is said that since we are using an orthographic projection,
    // this division is actually meaningless as w is always 1.0. However this step is necessary
    // when using perspective projection so we do it anyways to support perspective projection
    // out of the box.
    vec3 projection_coordinates = fragment_position_from_light.xyz / fragment_position_from_light.w;

    // Apply z bias:
    projection_coordinates.z -= shadow_bias_value;

    // The projection_coordinates will be returned in the range [-1, 1]. Because the depth from 
    // the shadow map is in the range [0,1] and we also want to use projection_coordinates from 
    // the shadow map, we transform the NDC coordinates to the range [0,1]:
    projection_coordinates = (projection_coordinates * 0.5) + vec3(0.5);

    float shadow = 0.0f;

    vec4 moments = texture(shadow_map_texture, projection_coordinates.xy);

    projection_coordinates = (projection_coordinates * 2.0) - vec3(1.0);

    float positive = exp(exponent_value * projection_coordinates.z);
    float negative = -exp(-exponent_value * projection_coordinates.z);

    float positive_shadow = Chebyshev(positive, moments.xy, min_variance_value, light_bleeding_reduction_amount_value);
    float negative_shadow = Chebyshev(negative, moments.zw, min_variance_value, light_bleeding_reduction_amount_value);
    
    shadow = min(positive_shadow, negative_shadow);
	
    return shadow;
}

#endif
