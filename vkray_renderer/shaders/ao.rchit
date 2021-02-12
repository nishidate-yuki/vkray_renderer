#version 460
#extension GL_EXT_ray_tracing : enable
#extension GL_EXT_nonuniform_qualifier : enable

#include "globals.glsl"

struct Vertex
{
    vec3 pos;     // 0
    vec3 normal;  // 3
    vec2 uv;      // 6
    vec4 color;   // 8
    vec4 joint0;  // 12
    vec4 weight0; // 16
    vec4 tangent; // 20
};

layout(binding = 0, set = 0) uniform accelerationStructureEXT topLevelAS;
layout(binding = 2, set = 0) buffer Vertices{float v[];} vertices[];
layout(binding = 3, set = 0) buffer Indices{uint i[];} indices[];
layout(binding = 4, set = 0) uniform sampler2D textureSamplers[];
layout(binding = 5, set = 0) uniform UBO
{
	mat4 invView;
	mat4 invProj;
    vec3 sunDir;
} ubo;
layout(binding = 6, set = 0) uniform InstanceDataOnDevice
{
	mat4 worldMatrix;
    int meshIndex;
    int baseColorTextureIndex;
    int normalTextureIndex;
    int occlusionTextureIndex;
    vec4 baseColorFactor;
} instanceData[];

layout(location = 0) rayPayloadInEXT HitPayload payLoad;
layout(location = 1) rayPayloadEXT bool occluded;
hitAttributeEXT vec3 attribs;

Vertex unpack(uint meshIndex, uint index)
{
    uint vertexSize = 24;
    uint offset = index * vertexSize;

    Vertex v;
    v.pos     = vec3(vertices[meshIndex].v[offset +  0], vertices[meshIndex].v[offset +  1], vertices[meshIndex].v[offset + 2]);
    v.normal  = vec3(vertices[meshIndex].v[offset +  3], vertices[meshIndex].v[offset +  4], vertices[meshIndex].v[offset + 5]);
    v.uv      = vec2(vertices[meshIndex].v[offset +  6], vertices[meshIndex].v[offset +  7]);
    v.color   = vec4(vertices[meshIndex].v[offset +  8], vertices[meshIndex].v[offset +  9], vertices[meshIndex].v[offset + 10], vertices[meshIndex].v[offset + 11]);
    v.tangent = vec4(vertices[meshIndex].v[offset + 20], vertices[meshIndex].v[offset + 21], vertices[meshIndex].v[offset + 22], vertices[meshIndex].v[offset + 23]);

	return v;
}

vec3 getInstanceColor()
{
    int r = gl_InstanceID / 36;
    int g = gl_InstanceID % 36 / 6;
    int b = gl_InstanceID %6;
    vec3 color;
    color.r = 0.15 * r + 0.25;
    color.g = 0.15 * g + 0.25;
    color.b = 0.15 * b + 0.25;
    return color;
}

uint pcg(inout uint state)
{
  uint prev = state * 747796405u + 2891336453u;
  uint word = ((prev >> ((prev >> 28u) + 4u)) ^ prev) * 277803737u;
  state     = prev;
  return (word >> 22u) ^ word;
}

float rand(inout uint seed)
{
  uint val = pcg(seed);
  return (float(val) * (1.0 / float(0xffffffffu)));
}

void main()
{
    // Get mesh index
    uint meshIndex = instanceData[gl_InstanceID].meshIndex;

    // Get vertices
	Vertex v0 = unpack(meshIndex, indices[meshIndex].i[3 * gl_PrimitiveID + 0]);
	Vertex v1 = unpack(meshIndex, indices[meshIndex].i[3 * gl_PrimitiveID + 1]);
	Vertex v2 = unpack(meshIndex, indices[meshIndex].i[3 * gl_PrimitiveID + 2]);

    // Get attributes
    const vec3 barycentricCoords = vec3(1.0f - attribs.x - attribs.y, attribs.x, attribs.y);
    vec3 pos     = gl_WorldRayOriginEXT + gl_WorldRayDirectionEXT * gl_HitTEXT;
	vec2 uv      = v0.uv      * barycentricCoords.x + v1.uv      * barycentricCoords.y + v2.uv      * barycentricCoords.z;
	vec3 normal  = v0.normal  * barycentricCoords.x + v1.normal  * barycentricCoords.y + v2.normal  * barycentricCoords.z;
	vec4 tangent = v0.tangent * barycentricCoords.x + v1.tangent * barycentricCoords.y + v2.tangent * barycentricCoords.z;

    // Normal texture
    int normalTextureIndex = instanceData[gl_InstanceID].normalTextureIndex;
    if(normalTextureIndex != -1){
        vec3 T = normalize(tangent.xyz);
        vec3 N = normalize(normal);
        vec3 B = cross(N, tangent.xyz) * tangent.w;
        mat3 TBN = mat3(T, B, N); // tangent space -> model space

        vec3 texNormal = texture(textureSamplers[normalTextureIndex], uv).rgb;
        texNormal = normalize(texNormal * 2.0 - 1.0);
        normal = normalize(TBN * texNormal);
    }

    uint aoSampleCount = 4;
    uint occludedCount = 0;
    for(int i = 0; i < aoSampleCount; i++){
        occluded = true;
        traceRayEXT(
            topLevelAS,
            gl_RayFlagsTerminateOnFirstHitEXT | gl_RayFlagsOpaqueEXT | gl_RayFlagsSkipClosestHitShaderEXT,
            0xff, // cullMask
            0,    // sbtRecordOffset
            0,    // sbtRecordStride
            1,    // missIndex
            pos,
            0.0001,
            sunDir,
            10000.0,
            1     // payloadLocation
        );
        if(occluded){
            occludedCount = occludedCount + 1;
        }
    }


    payLoad.contribution = pow(diffuse, vec3(1 / 2.2));
    payLoad.hitPosition = pos;
    payLoad.hitNormal = normal;
}
