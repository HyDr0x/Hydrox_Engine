#version 440 core

#extension GL_ARB_shader_draw_parameters : enable

#define EPSILON 0.00001
#define INT32_MAX 2147483647

#include "../../../../../include/Shader/Shaderincludes/VertexDeclaration.glslh"
#include "../../../../../include/Shader/Shaderincludes/CameraUBO.glslh"
#include "../../../../../include/Shader/Shaderincludes/CacheData.glslh"
#include "../../../../../include/Shader/Shaderincludes/IndirectLightData.glslh"
#include "../../../../../include/Shader/Shaderincludes/Encodings.glslh"

layout(rgba32f, binding = 0) readonly uniform imageBuffer globalCachePositionBuffer;
layout(rgba32f, binding = 1) readonly uniform imageBuffer globalCacheNormalBuffer;

layout(rgba32f, binding = 2) readonly uniform image2D indirectLightPositionBuffer;
layout(rgba32f, binding = 3) readonly uniform image2D indirectLightLuminousFluxBuffer;

layout(std430, binding = 0) buffer transformMatrixBuffer
{
	mat4 trfMatrix[];
};

layout(std430, binding = 1) buffer instanceCacheOffsetBuffer
{
	uint perInstanceCacheOffset[];
};

layout(location = 3) uniform uint proxyLightMapResolution;

layout(location = 4) uniform uint globalCacheOffset;

layout(location = POSITION) in vec3 in_pos;
layout(location = NORMAL) in vec3 in_normal;
layout(location = CACHEINDICES0) in vec4 in_cacheIndices0;
layout(location = CACHEINDICES1) in vec4 in_cacheIndices1;

out vec3 vsout_pos3D;
out vec3 vsout_Xpd;
out vec3 vsout_phiPD;
out vec3 vsout_Xpg;
out vec3 vsout_phiPG;
out float cacheProxyMinDistanceD;
out float cacheProxyMinDistanceG;

void main()
{
	uint instanceIndex = uint(gl_InstanceID + gl_BaseInstanceARB);

	vsout_pos3D = (trfMatrix[instanceIndex] * vec4(in_pos, 1.0f)).xyz;
	vec3 normal = normalize(mat3(trfMatrix[instanceIndex]) * in_normal);

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
	
	float dmax = 0.0;
	for(uint i = 0; i < 8; i++)
	{
		if(cacheIndices[i] < INT32_MAX)
		{
			vec3 diff = vsout_pos3D - imageLoad(globalCachePositionBuffer, int(cacheIndices[i])).xyz;
			dmax = max(dmax, dot(diff, diff));
		}
	}
	
	dmax = sqrt(dmax);
	
	vec3 Xpd = vec3(0.0f), phiPD = vec3(0.0f);
	vec3 Xpg = vec3(0.0f), phiPG = vec3(0.0f);
	
	float wGesD = 0.0f;
	float wGesG = 0.0f;
	
	cacheProxyMinDistanceD = 0.0f;
	cacheProxyMinDistanceG = 0.0f;
		
	for(uint i = 0; i < 8; i++)
	{
		if(cacheIndices[i] < INT32_MAX)
		{
			ivec2 coord = ivec2(mod(cacheIndices[i], proxyLightMapResolution), cacheIndices[i] / proxyLightMapResolution);
			CacheData cache;
			cache.position = imageLoad(globalCachePositionBuffer, int(cacheIndices[i]));
			cache.normal = imageLoad(globalCacheNormalBuffer, int(cacheIndices[i]));
			
			vec3 cacheNormal = normalize(decodeNormal(cache.normal.xy));
			
			float dir = max(1.0 - length(vsout_pos3D - cache.position.xyz) / dmax, 0.0);
			
			float wd = dir * sqrt(max(dot(cacheNormal, normal), 0.0));
			
			vec3 camCacheDir = normalize(eyePos.xyz - cache.position.xyz);
			float wg = dir * sqrt(max(dot(reflect(-camCacheDir, cacheNormal), reflectCamDir), 0.0));
			
			IndirectLightData indirectLightD;
			indirectLightD.position = imageLoad(indirectLightPositionBuffer, ivec2(2 * coord.x, coord.y));
			indirectLightD.luminousFlux = imageLoad(indirectLightLuminousFluxBuffer, ivec2(2 * coord.x, coord.y));
			
			IndirectLightData indirectLightG;
			indirectLightG.position = imageLoad(indirectLightPositionBuffer, ivec2(2 * coord.x + 1, coord.y));
			indirectLightG.luminousFlux = imageLoad(indirectLightLuminousFluxBuffer, ivec2(2 * coord.x + 1, coord.y));
			
			cacheProxyMinDistanceD += wd * indirectLightD.position.w;
			cacheProxyMinDistanceG += wg * indirectLightG.position.w;
			
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
	
	cacheProxyMinDistanceD = wGesD > 0 ? cacheProxyMinDistanceD / wGesD : 0;
	cacheProxyMinDistanceG = wGesG > 0 ? cacheProxyMinDistanceG / wGesG : 0;
	
	gl_Position = viewProjectionMatrix * vec4(vsout_pos3D, 1);
}