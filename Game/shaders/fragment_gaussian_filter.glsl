#version 460

out vec4 fragment_color;
in vec2 texture_coords;

uniform vec3 blur_scale;
layout (binding = 0) uniform sampler2D input_texture; 


void main()
{
    vec4 color = vec4(0.0);

    // color += texture(input_texture, vec2(-3.0) * blur_scale.xy + texture_coords) * (1.0/64.0);
    // color += texture(input_texture, vec2(-2.0) * blur_scale.xy + texture_coords) * (6.0/64.0);
    // color += texture(input_texture, vec2(-1.0) * blur_scale.xy + texture_coords) * (15.0/64.0);
    // color += texture(input_texture, vec2(0.0) * blur_scale.xy + texture_coords)  * (20.0/64.0);
    // color += texture(input_texture, vec2(1.0) * blur_scale.xy + texture_coords)  * (15.0/64.0);
    // color += texture(input_texture, vec2(2.0) * blur_scale.xy + texture_coords)  * (6.0/64.0);
    // color += texture(input_texture, vec2(3.0) * blur_scale.xy + texture_coords)  * (1.0/64.0);


    const int num_indices = 21;
    // We assume that num_indices is an odd number:
    const int indices_half = (num_indices - 1) / 2;


    float coefficients[21] = 
    {
        0.000272337, 0.00089296, 0.002583865, 0.00659813, 0.014869116,
        0.029570767, 0.051898313, 0.080381679, 0.109868729, 0.132526984,
        0.14107424,
        0.132526984, 0.109868729, 0.080381679, 0.051898313, 0.029570767,
        0.014869116, 0.00659813, 0.002583865, 0.00089296, 0.000272337
    };


    for (int index = 0; index < 21; ++index)
    {
        color += texture(input_texture, vec2((index - indices_half)) * blur_scale.xy + texture_coords) * coefficients[index];
    }


    fragment_color = color;
}
 