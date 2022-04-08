# version 440
out vec4 color;

in vec3 tex_coords;

uniform samplerCube skybox;

void main()
{
    color = texture(skybox, tex_coords);
}