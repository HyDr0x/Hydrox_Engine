#version 440 core

#extension ARB_shader_draw_parameters : enable

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

#include "../Header/MaterialData.glslh"
#include "../Header/CacheData.glslh"

#define INT32_MAX 2147483647

layout(location = 3) uniform uint globalCacheOffset;

layout(std430, binding = 0) buffer transformMatrixBuffer
{
	mat4 trfMatrix[];
};

layout(std430, binding = 2) buffer globalCacheBuffer
{
	CacheData globalCaches[];
};

layout(std430, binding = 5) buffer triangleIndexOffsetBuffer
{
	uint triangleIndexOffset[];
};

layout(std430, binding = 6) buffer cacheOffsetBuffer
{
	uint cacheIndexOffset[];
};

layout(std430, binding = 7) buffer triangleBuffer
{
	uvec2 triangleCacheIndices[];
};

layout(std430, binding = 8) buffer cacheBuffer
{
	CacheData caches[];
};

layout(std430, binding = 9) buffer meshIndexBuffer
{
	uint perMeshIndex[];
};

in vec3 vsout_normal[3];
in vec2 vsout_texCoord[3];
in vec4 vsout_cacheIndices0[3];
in vec4 vsout_cacheIndices1[3];
in uint vsout_instanceIndex[3];

out vec3 gsout_normal;
out vec2 gsout_texCoord;
flat out uvec4 gsout_cacheIndices[6];
flat out uint gsout_instanceIndex;

void main()
{
	uint meshIndex = perMeshIndex[vsout_instanceIndex[0]];
	uvec2 triangleCacheBorderIndices = triangleCacheIndices[triangleIndexOffset[meshIndex] + gl_PrimitiveIDIn];

	uint cacheIndexOffsetTMP = cacheIndexOffset[meshIndex];

	//cache transformation and output into global cache buffer
	if(triangleCacheBorderIndices.x < INT32_MAX)
	{
		triangleCacheBorderIndices = uvec2(triangleCacheBorderIndices.x + cacheIndexOffsetTMP, triangleCacheBorderIndices.y + cacheIndexOffsetTMP);
	
		MaterialData cacheMaterial = material[materialIndex[vsout_instanceIndex[0]]];
	
		for(uint i = triangleCacheBorderIndices.x; i < triangleCacheBorderIndices.y; i++)
		{
			globalCaches[globalCacheOffset + i].position = vec4((trfMatrix[vsout_instanceIndex[0]] * vec4(caches[i].position.xyz, 1.0f)).xyz, cacheMaterial.diffuseStrength);
			globalCaches[globalCacheOffset + i].normal = vec4(normalize(mat3(trfMatrix[vsout_instanceIndex[0]]) * vec3(caches[i].normal)), cacheMaterial.specularStrength);
			globalCaches[globalCacheOffset + i].specularExponent.x = cacheMaterial.specularExponent;
		}
	}

	uvec4 cacheIndices[6];
	for(uint i = 0;  i < 3; i++)
	{//bvec4 cast to ensure that unused indices which are zero wont get offseted
		cacheIndices[2 * i + 0] = uvec4(vsout_cacheIndices0[i]) + uvec4(bvec4(vsout_cacheIndices0[i])) * (cacheIndexOffsetTMP + globalCacheOffset);
		cacheIndices[2 * i + 1] = uvec4(vsout_cacheIndices1[i]) + uvec4(bvec4(vsout_cacheIndices1[i])) * (cacheIndexOffsetTMP + globalCacheOffset);
	}
	
	uvec4 bitMask;
	
	for(uint i = 0; i < 6; i++)
	{
		bitMask = uvec4(1);
		uint index = (2 + (i / 2) * 2) % 6;
		bitMask &= uvec4(bvec4(cacheIndices[i] - cacheIndices[index].x));
		bitMask &= uvec4(bvec4(cacheIndices[i] - cacheIndices[index].y));
		bitMask &= uvec4(bvec4(cacheIndices[i] - cacheIndices[index].z));
		bitMask &= uvec4(bvec4(cacheIndices[i] - cacheIndices[index].w));
		
		index++;
		bitMask &= uvec4(bvec4(cacheIndices[i] - cacheIndices[index].x));
		bitMask &= uvec4(bvec4(cacheIndices[i] - cacheIndices[index].y));
		bitMask &= uvec4(bvec4(cacheIndices[i] - cacheIndices[index].z));
		bitMask &= uvec4(bvec4(cacheIndices[i] - cacheIndices[index].w));
		cacheIndices[i] *= bitMask;
	}

	cacheIndices[0] -= uvec4(1);
	cacheIndices[1] -= uvec4(1);
	cacheIndices[2] -= uvec4(1);
	cacheIndices[3] -= uvec4(1);
	cacheIndices[4] -= uvec4(1);
	cacheIndices[5] -= uvec4(1);
	
	gsout_instanceIndex = vsout_instanceIndex[0];
	for(uint i = 0; i < 6; i++)
	{
		gsout_cacheIndices[i] = cacheIndices[i];
	}
	gsout_texCoord = vsout_texCoord[0];
	gsout_normal = vsout_normal[0];
	gl_Position = gl_in[0].gl_Position;
	EmitVertex();
	
	gsout_instanceIndex = vsout_instanceIndex[1];
	for(uint i = 0; i < 6; i++)
	{
		gsout_cacheIndices[i] = cacheIndices[i];
	}
	gsout_texCoord = vsout_texCoord[1];
	gsout_normal = vsout_normal[1];
	gl_Position = gl_in[1].gl_Position;
	EmitVertex();
	
	gsout_instanceIndex = vsout_instanceIndex[2];
	for(uint i = 0; i < 6; i++)
	{
		gsout_cacheIndices[i] = cacheIndices[i];
	}
	gsout_texCoord = vsout_texCoord[2];
	gsout_normal = vsout_normal[2];
	gl_Position = gl_in[2].gl_Position;
	EmitVertex();
	EndPrimitive();
}