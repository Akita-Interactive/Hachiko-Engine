#version 460

out vec4 fragment_color;
in vec2 texture_coords;

layout (binding = 0) uniform sampler2D input_texture; 

uniform float texture_width_inverse;
uniform float texture_height_inverse;

const float EDGE_THRESHOLD_MIN = 0.0312;
const float EDGE_THRESHOLD_MAX = 0.1250;
const int NUM_ITERATIONS = 12;
const float ITERATION_QUALITY[NUM_ITERATIONS] = {
    1.0,
    1.0,
    1.0,
    1.0,
    1.0,
    1.5,
    2.0,
    2.0,
    2.0,
    2.0,
    4.0,
    8.0
};
const float SUBPIXEL_QUALITY = 0.75;

float ConvertRGBToLuma(vec3 rgb_color)
{
    return sqrt(dot(rgb_color, vec3(0.299, 0.587, 0.114)));
}

float GetOffsettedLuma(ivec2 offset) 
{
    return ConvertRGBToLuma(
        textureOffset(input_texture, texture_coords, offset).rgb);
}

void main()
{
    const vec3 color_center = texture(input_texture, texture_coords).rgb; 
    
    // This is the luma at the current fragment:
    const float luma_center = ConvertRGBToLuma(color_center);

    // Luma at down, up, left and right neighbors of the current fragment:
    const float luma_down  = GetOffsettedLuma(ivec2( 0, -1));
    const float luma_up    = GetOffsettedLuma(ivec2( 0,  1));
    const float luma_left  = GetOffsettedLuma(ivec2(-1,  0));
    const float luma_right = GetOffsettedLuma(ivec2( 1,  0));

    // Get maximum and minimum lumas around center, including center:
    const float luma_min = 
        min(luma_center,min(min(luma_down,luma_up),min(luma_left,luma_right)));
    const float luma_max = 
        max(luma_center,max(max(luma_down,luma_up),max(luma_left,luma_right)));
    
    // Compute the difference between min and max lumas:
    const float luma_range = luma_max - luma_min;

    // If the contrast in area is lower then our threshold, or the area is 
    // dark, don't touch the actual color of the fragment:
    if (luma_range < max(EDGE_THRESHOLD_MIN, luma_max * EDGE_THRESHOLD_MAX))
    {
        fragment_color = vec4(color_center, 1.0f);
        return;
    }

    // Luma at down left, up left, down right and up right neighbors of the 
    // current fragment (diagonal neighbors):
    const float luma_down_left  = GetOffsettedLuma(ivec2(-1, -1));
    const float luma_up_right   = GetOffsettedLuma(ivec2( 1,  1));
    const float luma_up_left    = GetOffsettedLuma(ivec2(-1,  1));
    const float luma_down_right = GetOffsettedLuma(ivec2( 1, -1));

    // Combine previous four neighbors (not the diagonal ones). These will be 
    // used for future calculations:
    const float luma_down_up    = luma_down + luma_up;
    const float luma_left_right = luma_left + luma_right;

    // Combine the corners as well:
    const float luma_left_corners   = luma_down_left + luma_up_left;
    const float luma_down_corners   = luma_down_left + luma_down_right;
    const float luma_right_corners  = luma_down_right + luma_up_right;
    const float luma_up_corners     = luma_up_right + luma_up_left;

    // Compute an estimation of the gradient along the horizontal and vertical 
    // axis:
    const float edge_horizontal = 
        abs(-2.0 * luma_left + luma_left_corners) +
        abs(-2.0 * luma_center + luma_down_up ) * 2.0 +
        abs(-2.0 * luma_right + luma_right_corners);
    const float edge_vertical = 
        abs(-2.0 * luma_up + luma_up_corners) +
        abs(-2.0 * luma_center + luma_left_right) * 2.0 +
        abs(-2.0 * luma_down + luma_down_corners);

    // Is the local edge horizontal or vertical ?
    const bool is_horizontal = (edge_horizontal >= edge_vertical);

    // Select the two neighboring texel luma in the opposite direction to the 
    // local edge:
    const float luma1 = is_horizontal ? luma_down : luma_left;
    const float luma2 = is_horizontal ? luma_up : luma_right;
    
    // Compute the gradients in these directions:
    const float gradient1 = luma1 - luma_center;
    const float gradient2 = luma2 - luma_center;

    // Find which direction is the steepest:
    const float abs_gradient1 = abs(gradient1);
    const float abs_gradient2 = abs(gradient2);
    const bool is_1_steepest = (abs_gradient1 >= abs_gradient2);

    const float gradient_scaled = 
        0.25 * (is_1_steepest ? abs_gradient1 : abs_gradient2);

    // Choose one pixel amount of step size according to the edge direction:
    float step_length = is_horizontal 
        ? texture_height_inverse 
        : texture_width_inverse;
    // Average luma in the selected direction:
    float luma_local_average = 0.0;

    if (is_1_steepest)
    {
        // Flip the direction:
        step_length = -step_length;

        luma_local_average = 0.5 * (luma1 + luma_center);
    }
    else
    {
        luma_local_average = 0.5 * (luma2 + luma_center);
    }
    
    // Shift texture coordinates (UV) in the correct direction by half a
    // pixel:
    vec2 current_texture_coords = texture_coords;

    if (is_horizontal)
    {
        current_texture_coords.y += step_length * 0.5;
    }
    else
    {
        current_texture_coords.x += step_length * 0.5;
    }

    // Compute the offset (for each iteration step) in the right direction:
    vec2 offset = is_horizontal 
        ? vec2(texture_width_inverse, 0.0)
        : vec2(0.0, texture_height_inverse);
    // Compute the UVs to explore each side of the edge, orthogonally.
    vec2 texture_coords1 = current_texture_coords - offset;
    vec2 texture_coords2 = current_texture_coords + offset;

    // Read the lumas at both ends of the exploration segment, and compute the 
    // difference with respect to the local average luma (delta):
    float luma_end1 = 
        ConvertRGBToLuma(texture(input_texture, texture_coords1).rgb) - 
        luma_local_average;
    float luma_end2 = 
        ConvertRGBToLuma(texture(input_texture, texture_coords2).rgb) -
        luma_local_average;

    // If the luma deltas at the current ends are larger than the local gradient
    // that means we have reached the side of the edge:
    bool reached1 = abs(luma_end1) >= gradient_scaled;
    bool reached2 = abs(luma_end2) >= gradient_scaled;
    bool reached_both = reached1 && reached2;

    if (!reached1)
    {
        texture_coords1 -= offset;
    }
    if (!reached2)
    {
        texture_coords2 += offset;
    }

    // If both sides are not reached yet, continue exploring:
    if (!reached_both)
    {
        for (int i = 2; i < NUM_ITERATIONS; i++)
        {
            // If needed read luma in 1st direction, compute the difference wrt
            // local average:
            if (!reached1)
            {
                luma_end1 = ConvertRGBToLuma(
                    texture(input_texture, texture_coords1).rgb);
                luma_end1 -= luma_local_average;
            }
            // If needed read luma in 2nd direction, compute the difference wrt
            // local average:
            if (!reached2)
            {
                luma_end2 = ConvertRGBToLuma(
                    texture(input_texture, texture_coords2).rgb);
                luma_end2 -= luma_local_average;
            }

            // If the luma deltas at the current ends are larger than the local 
            // gradient that means we have reached the side of the edge:
            reached1 = abs(luma_end1) >= gradient_scaled;
            reached2 = abs(luma_end2) >= gradient_scaled;
            reached_both = reached1 && reached2;

            if (!reached1)
            {
                texture_coords1 -= offset * ITERATION_QUALITY[i];
            }
            if (!reached2)
            {
                texture_coords2 += offset * ITERATION_QUALITY[i];
            }

            // Stop the exploration if the both sides have been reached:
            if (reached_both)
            {
                break;
            }
        }
    }

    // Compute the distances to the each end of the edge:
    const float distance1 = is_horizontal 
        ? (texture_coords.x - texture_coords1.x)
        : (texture_coords.y - texture_coords1.y);
    const float distance2 = is_horizontal 
        ? (texture_coords2.x - texture_coords.x)
        : (texture_coords2.y - texture_coords.y);

    // Is distance1 closer?
    const bool is_direction1 = distance1 < distance2;
    const float distance_final = is_direction1 ? distance1 : distance2;

    // Compute lenght of the edge:
    const float edge_length = (distance1 + distance2);

    // UV Offset: read in the direction of the closest side of the edge:
    const float pixel_offset = -distance_final / edge_length + 0.5;

    // Is the luma at center smaller than the average?
    const bool is_luma_center_smaller = luma_center < luma_local_average;

    // If the luma at center is smaller than its neighbor, the difference of 
    // lumas at each end should be positive, meaning same variation:
    // (in the direction of the closer side of the edge)
    const bool correct_variation = (
        (is_direction1 ? luma_end1 : luma_end2) < 0.0) 
            != is_luma_center_smaller;
    
    // If the luma variation is incorrect, do not add offset:
    float final_offset = correct_variation ? pixel_offset : 0.0;

    ////////////////////////////////////////////////////////////////////////////

    // Sub-pixel shifting:
    // This additional computational step allows us to handle sub-pixel aliasing
    // for example when the thin lines are aliased. In such cases we compute an
    // average luma over the 3x3 neighborhood.

    // Compute the full weighted average of the luma over the 3x3 neighborhood:
    const float luma_average = 
        (1.0/12.0) * 
        (2.0 * (luma_down_up + luma_left_right) + 
            luma_left_corners + luma_right_corners);

    // Calculate the ratio of the difference between the global average and the 
    // center luma, over the range in the 3x3 neighborhood:
    const float subpixel_offset1 = 
        clamp(abs(luma_average - luma_center) / luma_range, 0.0, 1.0);
    const float subpixel_offset2 = 
        (-2.0 * subpixel_offset1 + 3.0) * subpixel_offset1 * subpixel_offset1;

    
    // Compute a subpixel offset based on this delta:
    const float subpixel_offset_final = 
        subpixel_offset2 * subpixel_offset2 * SUBPIXEL_QUALITY;

    // Pick the biggest of the two offsets:
    final_offset = max(final_offset, subpixel_offset_final);

    ////////////////////////////////////////////////////////////////////////////

    // Compute the final texture coordinates:
    vec2 final_texture_coords = texture_coords;

    if (is_horizontal)
    {
        final_texture_coords.y += final_offset * step_length;
    } 
    else
    {
        final_texture_coords.x += final_offset * step_length;
    }

    // Read the color at the new UV coordinates, and use it.
    vec3 final_color = texture(input_texture, final_texture_coords).rgb;

    fragment_color = vec4(final_color, 1.0);
}
