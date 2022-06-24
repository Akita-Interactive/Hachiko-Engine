# version 460

// Input
in vec2 uv0;

// Texture
uniform sampler2D diffuseMap;
uniform int has_texture;

// Animation
uniform int flip_x;
uniform int flip_y;
uniform float x_factor; // inverse of the number of columns
uniform float y_factor; // inverse of the number of rows
uniform vec2 animation_index;
uniform float animation_blend;

// Color Over Lifetime
uniform vec4 input_color;

// Output
out vec4 output_color;

void main()
{	
    const float epsilon = 0.001;

    // Flip UVs
	float u = flip_x * (1 - uv0.x) + (1 - flip_x) * uv0.x;
	float v = flip_y * (1 - uv0.y) + (1 - flip_y) * uv0.y;

    float u1 = (u + animation_index.x) * x_factor;
    float v1 = (v + animation_index.y) * y_factor;
    
    vec4 current_color = texture2D(diffuseMap,  vec2(u1, v1));
    vec4 next_color = vec4(0, 0, 0, 0);

    // Blend tiles
    if (animation_blend > epsilon)
    {
        vec2 next_index = vec2(0, 0);
        if ((animation_index.x + 1) <= (1 / x_factor) - 1)
        {
            next_index.x = animation_index.x + 1;
            next_index.y = animation_index.y;
        }
        else if ((animation_index.y + 1) <= (1 / y_factor) - 1)
        {
            next_index.x = 0;
            next_index.y = animation_index.y + 1;
        }

        float u2 = (u + next_index.x) * x_factor;
        float v2 = (v + next_index.y) * y_factor;
        next_color = texture2D(diffuseMap,  vec2(u2, v2));
    }

    output_color = has_texture * mix(current_color, next_color, animation_blend) * input_color + (1 - has_texture) * input_color;
}