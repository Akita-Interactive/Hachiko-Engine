#version 460 

out float out_fragment_color;
in vec2 texture_coords;

layout (binding = 0) uniform sampler2D texture_noise;
layout (binding = 1) uniform sampler2D g_normal;
layout (binding = 2) uniform sampler2D g_position;

const int kernel_size = 64;

uniform float frame_buffer_width;
uniform float frame_buffer_height;

const float noise_texture_size = 4.0;
const float noise_texture_size_inv = 1.0 / noise_texture_size;
const vec2 noise_scale = vec2(
    frame_buffer_width * noise_texture_size_inv, 
    frame_buffer_height * noise_texture_size_inv);

uniform float radius;
uniform float bias;
uniform float power;
uniform mat4 camera_view;
uniform mat4 camera_proj;
uniform vec3 samples[kernel_size];

void main()
{
    vec3 fragment_position = (camera_view*texture(g_position, texture_coords)).xyz;
    vec3 fragment_normal = mat3(camera_view)*normalize(texture(g_normal, texture_coords).xyz*2.0-1.0);
    vec3 random_vec = normalize(texture(texture_noise, texture_coords * noise_scale).xyz);

    // Create TBN change-of-basis matrix. From tangent-space to view-space:
    vec3 tangent = normalize((random_vec - fragment_normal) * dot(random_vec, fragment_normal));
    vec3 bitangent = normalize(cross(fragment_normal, tangent));
    mat3 tbn = mat3(tangent, bitangent, fragment_normal);

    // iterate over the sample kernel and calculate occlusion factor
    float occlusion = 0.0;
    for(int i = 0; i < kernel_size; ++i)
    {
        // Get sample position:
        vec3 sample_position = tbn * samples[i]; // from tangent to view-space
        sample_position = fragment_position + sample_position * radius; 
        
        // Project sample position (to sample texture) (to get position on screen/texture):
        vec4 offset = camera_proj*vec4(sample_position, 1.0);
        
        offset.xyz /= offset.w; // Perspective division
        offset.xyz = offset.xyz * 0.5 + 0.5; // Transform to range 0.0 - 1.0
        
        // Get sample depth:
        float sample_depth = (camera_view * texture(g_position, offset.xy)).z; // Get depth value of kernel sample

        // Range check & accumulate:
        float rangeCheck = smoothstep(0.0, 1.0, radius / abs(sample_position.z - sample_depth));
        occlusion += (sample_depth - sample_position.z >= bias ? 1.0 : 0.0)* rangeCheck;
    }

    out_fragment_color = pow(1.0 - (occlusion / kernel_size), power);
}