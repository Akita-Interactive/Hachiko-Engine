#ifndef _LIGHTS_INCLUDE_
#define _LIGHTS_INCLUDE_

#define MAX_POINT_LIGHTS 500
#define MAX_SPOT_LIGHTS 100
#define PI 3.141597

struct AmbientLight
{
    vec4 color;
    float intensity;
};

struct DirLight
{
    vec4 direction;
    vec4 color;
    float intensity;
};

struct PointLight
{
    vec4 position;
    vec4 color;
    float intensity;
    float radius;
};

struct SpotLight
{
    vec4 position;
    vec4 direction;
    vec4 color;
    float inner;
    float outer;
    float intensity;
    float radius;
};

#endif
