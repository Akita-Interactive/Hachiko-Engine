#ifndef _LIGHTS_UNIFORM_INCLUDE_
#define _LIGHTS_UNIFORM_INCLUDE_

uniform uint activate_IBL;
layout (binding = 7) uniform samplerCube diffuseIBL;
layout (binding = 8) uniform samplerCube prefilteredIBL;
layout (binding = 9) uniform sampler2D environmentBRDF;
uniform uint prefilteredIBL_numLevels;

layout(std140, binding = 2) uniform Lights
{
    AmbientLight ambient;
    DirLight directional;
    PointLight points[MAX_POINT_LIGHTS];
    SpotLight spots[MAX_SPOT_LIGHTS];
    uint n_points;
    uint n_spots;
} lights;

#endif
