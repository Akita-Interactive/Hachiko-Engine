#version 460

uniform float exponent;

layout (location = 0) in vec4 light_space_position;

out vec4 variance_shadow_map;

float PartialDerivative(float moment2, float moment1)
{
    float dx = dFdx(moment1);
    float dy = dFdy(moment1);
    
    return moment2 + 0.25*(dx*dx+dy*dy);
}

void main()
{
    // float depth = gl_FragCoord.z;

    // float moment1 = depth;
    // float moment2 = moment1 * moment1;

    // // Adjusting moments (this is sort of bias per pixel) using partial derivative
    // float dx = dFdx(depth);
    // float dy = dFdy(depth);
    // moment2 += 0.25*(dx*dx+dy*dy);

    // variance_shadow_map = vec4(moment1, moment2, 0.0, 0.0);

    float depth = light_space_position.z;

    float positive = exp(exponent * depth);
    float positive2 = PartialDerivative(positive*positive, positive);
    
    float negative = -exp(-exponent * depth);
    float negative2 = PartialDerivative(negative * negative, negative);

    variance_shadow_map = vec4(positive, positive2, negative, negative2);
}