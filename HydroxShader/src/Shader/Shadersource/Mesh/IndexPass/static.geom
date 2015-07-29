#version 440 core

#extension GL_ARB_shader_draw_parameters : enable

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

#include "../../../../../include/Shader/Shaderincludes/CameraUBO.glslh"
#include "../../../../../include/Shader/Shaderincludes/MaterialData.glslh"
#include "../../../../../include/Shader/Shaderincludes/CacheData.glslh"
#include "../../../../../include/Shader/Shaderincludes/Encodings.glslh"

#define INT32_MAX 2147483647

layout(rgba32f, binding = 0) writeonly uniform imageBuffer globalCachePositionBuffer;
layout(rgba32f, binding = 1) writeonly uniform imageBuffer globalCacheNormalBuffer;
layout(r16f, binding = 2) writeonly uniform imageBuffer globalCacheAreaBuffer;

layout(std430, binding = 0) buffer transformMatrixBuffer
{
	mat4 trfMatrix[];
};

layout(std430, binding = 2) buffer triangleIndexOffsetBuffer
{
	uint triangleIndexOffset[];
};

layout(std430, binding = 3) buffer cacheOffsetBuffer
{
	uint cacheIndexOffset[];
};

layout(std430, binding = 4) buffer triangleBuffer
{
	uvec2 triangleCacheIndices[];
};

layout(std430, binding = 5) buffer cacheBuffer
{
	CacheData caches[];
};

layout(std430, binding = 6) buffer meshIndexBuffer
{
	uint perMeshIndex[];
};

layout(std430, binding = 7) buffer instanceCacheOffsetBuffer
{
	uint perInstanceCacheOffset[];
};

layout(location = 0) uniform uint globalCacheOffset;
layout(location = 1) uniform uint bufferResolution;

in vec4 vsout_cacheIndices0[3];
in vec4 vsout_cacheIndices1[3];
in uint vsout_instanceIndex[3];

flat out uvec4 gsout_cacheIndices[6];

void main()
{
	uint meshIndex = perMeshIndex[vsout_instanceIndex[0]];
	uvec2 triangleCacheBorderIndices = triangleCacheIndices[triangleIndexOffset[meshIndex] + gl_PrimitiveIDIn];

	uint cacheIndexOffsetTMP = cacheIndexOffset[meshIndex];
	uint perInstanceCacheOffsetTMP = perInstanceCacheOffset[vsout_instanceIndex[0]];
	
	//cache transformation and output into global cache buffer
	if(triangleCacheBorderIndices.x < INT32_MAX)
	{
		MaterialData cacheMaterial = material[materialIndex[vsout_instanceIndex[0]]];
	
		for(uint i = triangleCacheBorderIndices.x; i < triangleCacheBorderIndices.y; i++)
		{
			uint index = globalCacheOffset + perInstanceCacheOffsetTMP + i;
			
			mat4 modelMatrix = trfMatrix[vsout_instanceIndex[0]];
			
			imageStore(globalCachePositionBuffer, int(index), vec4((modelMatrix * vec4(caches[cacheIndexOffsetTMP + i].position.xyz, 1.0f)).xyz, cacheMaterial.diffuseRho));
			
			vec3 normal = normalize(mat3(modelMatrix) * caches[cacheIndexOffsetTMP + i].normal.xyz);
			imageStore(globalCacheNormalBuffer, int(index), vec4(encodeNormal(normal), cacheMaterial.specularRho, cacheMaterial.roughness));
			
			//extract matrix scale for area, expect uniform scale!
			float scale = sqrt(modelMatrix[0][0] * modelMatrix[0][0] + modelMatrix[0][1] * modelMatrix[0][1] + modelMatrix[0][2] * modelMatrix[0][2]);
			
			imageStore(globalCacheAreaBuffer, int(index), 10.0 * scale * scale * caches[cacheIndexOffsetTMP + i].area);
		}
	}
	
	uvec4 cacheIndices[6];
	for(uint i = 0;  i < 3; i++)
	{//bvec4 cast to ensure that unused indices which are zero wont get an offset
		cacheIndices[2 * i + 0] = uvec4(vsout_cacheIndices0[i]) + uvec4(bvec4(vsout_cacheIndices0[i])) * (perInstanceCacheOffsetTMP + globalCacheOffset);
		cacheIndices[2 * i + 1] = uvec4(vsout_cacheIndices1[i]) + uvec4(bvec4(vsout_cacheIndices1[i])) * (perInstanceCacheOffsetTMP + globalCacheOffset);
	}
	
	cacheIndices[0] -= uvec4(1);
	cacheIndices[1] -= uvec4(1);
	cacheIndices[2] -= uvec4(1);
	cacheIndices[3] -= uvec4(1);
	cacheIndices[4] -= uvec4(1);
	cacheIndices[5] -= uvec4(1);
	
	for(uint i = 0; i < 6; i++)
	{
		gsout_cacheIndices[i] = cacheIndices[i];
	}
	gl_Position = gl_in[0].gl_Position;
	EmitVertex();

	for(uint i = 0; i < 6; i++)
	{
		gsout_cacheIndices[i] = cacheIndices[i];
	}
	gl_Position = gl_in[1].gl_Position;
	EmitVertex();

	for(uint i = 0; i < 6; i++)
	{
		gsout_cacheIndices[i] = cacheIndices[i];
	}
	gl_Position = gl_in[2].gl_Position;
	EmitVertex();
	EndPrimitive();
}