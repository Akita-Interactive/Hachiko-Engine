# version 460

// Input
in vec2 uv0;

// Texture
uniform sampler2D diffuseMap;

// Output
out vec4 output_color;

void main()
{	
    output_color = texture2D(diffuseMap,  uv0);
}