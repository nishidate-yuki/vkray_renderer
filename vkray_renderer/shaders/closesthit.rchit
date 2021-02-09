#version 460
#extension GL_EXT_ray_tracing : enable
#extension GL_EXT_nonuniform_qualifier : enable

#include "globals.glsl"

struct Vertex
{
    vec3 pos;
    vec3 normal;
    vec2 uv;
    vec4 color;
    vec4 joint0;
    vec4 weight0;
    vec4 tangent;
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
} instanceData[];

layout(location = 0) rayPayloadInEXT HitPayload payLoad;
layout(location = 1) rayPayloadEXT bool shadowed;
hitAttributeEXT vec3 attribs;

Vertex unpack(uint meshIndex, uint index)
{
    uint vertexSize = 24;
    uint offset = index * vertexSize;

    Vertex v;
    v.pos    = vec3(vertices[meshIndex].v[offset + 0], vertices[meshIndex].v[offset + 1], vertices[meshIndex].v[offset + 2]);
    v.normal = vec3(vertices[meshIndex].v[offset + 3], vertices[meshIndex].v[offset + 4], vertices[meshIndex].v[offset + 5]);
    v.uv     = vec2(vertices[meshIndex].v[offset + 6], vertices[meshIndex].v[offset + 7]);
    v.color  = vec4(vertices[meshIndex].v[offset + 8], vertices[meshIndex].v[offset + 9], vertices[meshIndex].v[offset + 10], vertices[meshIndex].v[offset + 11]);

	return v;
}

void main()
{
    // Get indices
    uint meshIndex = instanceData[gl_InstanceID].meshIndex;
    uint baseColorTextureIndex = instanceData[gl_InstanceID].baseColorTextureIndex;

    // Get vertices
	Vertex v0 = unpack(meshIndex, indices[meshIndex].i[3 * gl_PrimitiveID + 0]);
	Vertex v1 = unpack(meshIndex, indices[meshIndex].i[3 * gl_PrimitiveID + 1]);
	Vertex v2 = unpack(meshIndex, indices[meshIndex].i[3 * gl_PrimitiveID + 2]);

    // Get attributes
    const vec3 barycentricCoords = vec3(1.0f - attribs.x - attribs.y, attribs.x, attribs.y);
    vec3 pos = gl_WorldRayOriginEXT + gl_WorldRayDirectionEXT * gl_HitTEXT;
	vec2 uv = v0.uv * barycentricCoords.x + v1.uv * barycentricCoords.y + v2.uv * barycentricCoords.z;
	vec3 normal = v0.normal * barycentricCoords.x + v1.normal * barycentricCoords.y + v2.normal * barycentricCoords.z;

    // Sample textures
    vec3 baseColor = texture(textureSamplers[baseColorTextureIndex], uv).xyz;
    baseColor = pow(baseColor, vec3(2.2));

    // Lighting
    vec3 lighting = vec3(max(4.0 * dot(normal, normalize(ubo.sunDir)), 0.3));
    shadowed = true;
    traceRayEXT(
        topLevelAS,
        gl_RayFlagsTerminateOnFirstHitEXT | gl_RayFlagsOpaqueEXT | gl_RayFlagsSkipClosestHitShaderEXT,
        0xff, // cullMask
        1,    // sbtRecordOffset
        0,    // sbtRecordStride
        1,    // missIndex
        pos,
        0.001,
        normalize(ubo.sunDir),
        10000.0,
        1     // payloadLocation
    );
    if(shadowed){
        lighting = vec3(0.3);
    }
    vec3 diffuse = baseColor * lighting;

    payLoad.contribution = pow(diffuse, vec3(1 / 2.2));
    payLoad.hitPosition = pos;
    payLoad.hitNormal = normal;
}
