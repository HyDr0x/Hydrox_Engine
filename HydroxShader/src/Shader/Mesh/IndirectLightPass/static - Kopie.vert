#version 440 core

#extension GL_ARB_shader_draw_parameters : enable

#define INT32_MAX 2147483647

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

layout(std430, binding = 0) buffer transformMatrixBuffer
{
	mat4 trfMatrix[];
};

layout(location = 2) uniform uint bufferResolution;

layout(location = POSITION) in vec3 in_pos;
layout(location = NORMAL) in vec3 in_normal;
layout(location = CACHEINDICES0) in vec4 in_cacheIndices0;
layout(location = CACHEINDICES1) in vec4 in_cacheIndices1;

out vec3 vsout_pos3D;
out vec3 vsout_Xpd;
out vec3 vsout_phiPD;
out vec3 vsout_Xpg;
out vec3 vsout_phiPG;

void main()
{
	uint instanceIndex = uint(gl_InstanceID + gl_BaseInstanceARB);

	vec3 pos3D = (trfMatrix[instanceIndex] * vec4(in_pos, 1.0f)).xyz;
	
	vec3 camDir = normalize(eyePos.xyz - pos3D);
	vec3 reflectCamDir = reflect(-camDir, in_normal);
	
	uint cacheIndices[8];
	cacheIndices[0] = uint(in_cacheIndices0.x);
	cacheIndices[1] = uint(in_cacheIndices0.y);
	cacheIndices[2] = uint(in_cacheIndices0.z);
	cacheIndices[3] = uint(in_cacheIndices0.w);
	cacheIndices[4] = uint(in_cacheIndices1.x);
	cacheIndices[5] = uint(in_cacheIndices1.y);
	cacheIndices[6] = uint(in_cacheIndices1.z);
	cacheIndices[7] = uint(in_cacheIndices1.w);
	
	float dmax = 0.0f;
	for(uint i = 0; i < 8; i++)
	{
		if(cacheIndices[i] < INT32_MAX)
		{
			ivec2 coord = ivec2(mod(cacheIndices[i], bufferResolution), cacheIndices[i] / bufferResolution);
			vec3 diff = pos3D - imageLoad(globalCachePositionBuffer, coord).xyz;
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
			
			float dir = max(1.0f - length(pos3D - cache.position.xyz) / dmax, 0.0001f);
			
			float wd = dir * sqrt(max(dot(cacheNormal, in_normal), 0));
			
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

	vsout_pos3D = pos3D;
	
	gl_Position = viewProjectionMatrix * vec4(pos3D, 1);
}