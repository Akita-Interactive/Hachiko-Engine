#version 460

out vec4 fragment_color;
in vec2 texture_coords;

uniform vec3 blur_scale;
uniform int blur_pixels;
uniform float blur_sigma;
layout (binding = 0) uniform sampler2D input_texture; 

const float pi = 3.14159265f;

void main()
{
    // Incremental Gaussian Coefficent Calculation (See GPU Gems 3 pp. 877 - 889):
    vec3 incremental_gaussian;
    incremental_gaussian.x = 1.0 / (sqrt(2.0 * pi) * blur_sigma);
    incremental_gaussian.y = exp(-0.5 / (blur_sigma * blur_sigma));
    incremental_gaussian.z = incremental_gaussian.y * incremental_gaussian.y;

    vec4 average_value = vec4(0.0);
    float coefficient_sum = 0.0;

    // Take the central sample first:
    average_value += texture2D(input_texture, texture_coords) * incremental_gaussian.x;
    coefficient_sum += incremental_gaussian.x;
    incremental_gaussian.xy *= incremental_gaussian.yz;

    // Go through the remaining vertical samples
    for (int i = 1; i <= blur_pixels; i++) 
    { 
        average_value += texture2D(input_texture, texture_coords - i * blur_scale.xy) * incremental_gaussian.x;         
        average_value += texture2D(input_texture, texture_coords + i * blur_scale.xy) * incremental_gaussian.x;         
        coefficient_sum += 2 * incremental_gaussian.x;
        incremental_gaussian.xy *= incremental_gaussian.yz;
    }

    fragment_color = average_value / coefficient_sum;
}
 