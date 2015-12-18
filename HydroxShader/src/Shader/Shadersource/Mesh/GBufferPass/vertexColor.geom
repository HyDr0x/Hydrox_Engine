#version 440 core

#extension GL_ARB_shader_draw_parameters : enable

#include "../../HydroxShader/include/Shader/Shaderincludes/CameraUBO.glslh"

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

layout(location = 5) uniform uint globalOccluderOffset;

layout(std430, binding = 2) buffer triangleIndexOffsetBuffer
{
	uint triangleIndexOffset[];
};

layout(std430, binding = 3) buffer occluderOffsetBuffer
{
	uint occluderIndexOffset[];
};

layout(std430, binding = 4) buffer triangleBuffer
{
	uvec2 triangleOccluderIndices[];
};

layout(std430, binding = 5) buffer occluderCoordinatesBuffer
{
	vec4 occluderCoordinates[];
};

layout(std430, binding = 6) buffer meshIndexBuffer
{
	uint perMeshIndex[];
};

layout(std430, binding = 7) buffer instanceOccluderOffsetBuffer
{
	uint perInstanceOccluderOffset[];
};

layout(std430, binding = 8) buffer transformedOccluderNormalCoordinatesBuffer
{
	vec4 transformedOccluderNormalCoordinates[];
};

in VertexData
{
	vec4 color;
	vec3 normal;
	flat uint instanceIndex;
} inData[];

out GeometryData
{
	vec3 normal;
	flat uint instanceIndex;
	vec4 color;
} outData;

void main()
{
	uint meshIndex = perMeshIndex[inData[0].instanceIndex];
	uvec2 triangleCacheBorderIndices = triangleOccluderIndices[triangleIndexOffset[meshIndex] + gl_PrimitiveIDIn];

	uint occluderIndexOffsetTMP = occluderIndexOffset[meshIndex];
	uint perInstanceOccluderOffsetTMP = perInstanceOccluderOffset[inData[0].instanceIndex];
	
	for(uint i = triangleCacheBorderIndices.x; i < triangleCacheBorderIndices.y; i++)
	{
		uint index = globalOccluderOffset + perInstanceOccluderOffsetTMP + i;

		vec3 occluderBarycentric = occluderCoordinates[occluderIndexOffsetTMP + i].xyz;
		transformedOccluderNormalCoordinates[2 * index] = vec4(occluderBarycentric.x * gl_in[0].gl_Position + occluderBarycentric.y * gl_in[1].gl_Position + occluderBarycentric.z * gl_in[2].gl_Position);
		transformedOccluderNormalCoordinates[2 * index + 1] = normalize(vec4(occluderBarycentric.x * inData[0].normal + occluderBarycentric.y * inData[1].normal + occluderBarycentric.z * inData[2].normal, 0.0));
	}
	
	for(uint i = 0; i < 3; i++)
	{
		outData.color = inData[i].color;
		outData.normal = inData[i].normal;
		outData.instanceIndex = inData[i].instanceIndex;
		gl_Position = viewProjectionMatrix * vec4(gl_in[i].gl_Position.xyz, 1.0);
		EmitVertex();
	}
	
	EndPrimitive();
}