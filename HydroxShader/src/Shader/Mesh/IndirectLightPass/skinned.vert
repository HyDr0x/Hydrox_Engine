#version 440 core

#extension GL_ARB_shader_draw_parameters : enable

#define INT32_MAX 2147483647
#define MAXBONES 64

#include "../../../../include/Shader/VertexDeclaration.glslh"
#include "../../../../include/Shader/CameraUBO.glslh"
#include "../../../../include/Shader/CacheData.glslh"
#include "../../../../include/Shader/IndirectLightData.glslh"
#include "../../../../include/Shader/Encodings.glslh"

layout(rgba32f, binding = 0) readonly uniform image2D globalCachePositionBuffer;
layout(rgba32f, binding = 1) readonly uniform image2D globalCacheNormalBuffer;

layout(rgba32f, binding = 2) readonly uniform image2D indirectLightPositionDBuffer;
layout(rgba32f, binding = 3) readonly uniform image2D indirectLightLuminousFluxDBuffer;

layout(rgba32f, binding = 4) readonly uniform image2D indirectLightPositionGBuffer;
layout(rgba32f, binding = 5) readonly uniform image2D indirectLightLuminousFluxGBuffer;

layout(std430, binding = 0) buffer boneMatrixBuffer
{
	mat4 boneMatrix[];
};

layout(std430, binding = 1) buffer instanceCacheOffsetBuffer
{
	uint perInstanceCacheOffset[];
};

layout(location = 2) uniform uint bufferResolution;

layout(location = 3) uniform uint globalCacheOffset;

layout(location = POSITION) in vec3 in_pos;
layout(location = NORMAL) in vec3 in_normal;
layout(location = BONEWEIGHTS) in vec4 in_boneWeights;
layout(location = BONEINDICES) in vec4 in_boneIndices;
layout(location = CACHEINDICES0) in vec4 in_cacheIndices0;
layout(location = CACHEINDICES1) in vec4 in_cacheIndices1;

out vec3 vsout_pos3D;
out vec3 vsout_Xpd;
out vec3 vsout_phiPD;
out vec3 vsout_Xpg;
out vec3 vsout_phiPG;

void main()
{
	mat4 skinningMatrix = mat4(0.0f);
	
	uint instanceIndex = uint(gl_InstanceID + gl_BaseInstanceARB);
	uint globalInstanceID = MAXBONES * instanceIndex;
	
	skinningMatrix += (boneMatrix[globalInstanceID + uint(in_boneIndices.x)] * in_boneWeights.x);
	skinningMatrix += (boneMatrix[globalInstanceID + uint(in_boneIndices.y)] * in_boneWeights.y);
	skinningMatrix += (boneMatrix[globalInstanceID + uint(in_boneIndices.z)] * in_boneWeights.z);
	skinningMatrix += (boneMatrix[globalInstanceID + uint(in_boneIndices.w)] * in_boneWeights.w);

	vsout_pos3D = (skinningMatrix * vec4(in_pos, 1.0f)).xyz;
	vec3 normal = normalize(mat3(skinningMatrix) * in_normal);
	
	vec3 camDir = normalize(eyePos.xyz - vsout_pos3D);
	vec3 reflectCamDir = reflect(-camDir, normal);
	
	uvec4 cacheIndicesTMP[2];
	//bvec4 cast to ensure that unused indices which are zero wont get an offset
	cacheIndicesTMP[0] = uvec4(in_cacheIndices0) + uvec4(bvec4(in_cacheIndices0)) * (perInstanceCacheOffset[instanceIndex] + globalCacheOffset);
	cacheIndicesTMP[1] = uvec4(in_cacheIndices1) + uvec4(bvec4(in_cacheIndices1)) * (perInstanceCacheOffset[instanceIndex] + globalCacheOffset);
	
	cacheIndicesTMP[0] -= uvec4(1);
	cacheIndicesTMP[1] -= uvec4(1);
	
	uint cacheIndices[8];
	cacheIndices[0] = uint(cacheIndicesTMP[0].x);
	cacheIndices[1] = uint(cacheIndicesTMP[0].y);
	cacheIndices[2] = uint(cacheIndicesTMP[0].z);
	cacheIndices[3] = uint(cacheIndicesTMP[0].w);
	cacheIndices[4] = uint(cacheIndicesTMP[1].x);
	cacheIndices[5] = uint(cacheIndicesTMP[1].y);
	cacheIndices[6] = uint(cacheIndicesTMP[1].z);
	cacheIndices[7] = uint(cacheIndicesTMP[1].w);
	
	float dmax = 0.0f;
	for(uint i = 0; i < 8; i++)
	{
		if(cacheIndices[i] < INT32_MAX)
		{
			ivec2 coord = ivec2(mod(cacheIndices[i], bufferResolution), cacheIndices[i] / bufferResolution);
			vec3 diff = vsout_pos3D - imageLoad(globalCachePositionBuffer, coord).xyz;
			dmax = max(dmax, dot(diff, diff));
		}
	}
	
	dmax = sqrt(dmax);
	
	vec3 Xpd = vec3(0.0f), phiPD = vec3(0.0f);
	vec3 Xpg = vec3(0.0f), phiPG = vec3(0.0f);
	
	float wGesD = 0.0f;
	float wGesG = 0.0f;
	
	for(uint i = 0; i < 8; i++)
	{
		if(cacheIndices[i] < INT32_MAX)
		{
			ivec2 coord = ivec2(mod(cacheIndices[i], bufferResolution), cacheIndices[i] / bufferResolution);
			CacheData cache;
			cache.position = imageLoad(globalCachePositionBuffer, coord);
			cache.normal = imageLoad(globalCacheNormalBuffer, coord);
			
			vec3 cacheNormal = normalize(decodeNormal(cache.normal.xy));
			
			float dir = max(1.0f - length(vsout_pos3D - cache.position.xyz) / dmax, 0.0001f);
			
			float wd = dir * sqrt(max(dot(cacheNormal, normal), 0));
			
			vec3 camCacheDir = normalize(eyePos.xyz - cache.position.xyz);
			float wg = dir * sqrt(max(dot(reflect(-camCacheDir, cacheNormal), reflectCamDir), 0));
			
			IndirectLightData indirectLightD;
			indirectLightD.position = imageLoad(indirectLightPositionDBuffer, coord);
			indirectLightD.luminousFlux = imageLoad(indirectLightLuminousFluxDBuffer, coord);
			
			IndirectLightData indirectLightG;
			indirectLightG.position = imageLoad(indirectLightPositionGBuffer, coord);
			indirectLightG.luminousFlux = imageLoad(indirectLightLuminousFluxGBuffer, coord);
			
			Xpd += wd * indirectLightD.position.xyz;
			Xpg += wg * indirectLightG.position.xyz;
				
			phiPD += wd * indirectLightD.luminousFlux.xyz;
			phiPG += wg * indirectLightG.luminousFlux.xyz;
			
			wGesD += wd;
			wGesG += wg;
		}
	}

	vsout_Xpd = wGesD > 0 ? Xpd / wGesD : vec3(0);
	vsout_Xpg = wGesG > 0 ? Xpg / wGesG : vec3(0);
	
	vsout_phiPD = wGesD > 0 ? phiPD / wGesD : vec3(0);
	vsout_phiPG = wGesG > 0 ? phiPG / wGesG : vec3(0);
	
	gl_Position = viewProjectionMatrix * vec4(vsout_pos3D, 1.0f);
}