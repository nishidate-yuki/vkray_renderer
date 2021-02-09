#version 460
#extension GL_EXT_ray_tracing : enable

#include "globals.glsl"

layout(location = 0) rayPayloadInEXT HitPayload payLoad;

void main()
{
    payLoad.contribution = vec3(0.0, 0.5, 0.2);
}
