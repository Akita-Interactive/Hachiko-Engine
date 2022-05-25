# version 460
out vec4 color;

in vec3 tex_coords;

layout (binding = 0) uniform samplerCube skybox;

void main()
{
    color = texture(skybox, tex_coords);
}