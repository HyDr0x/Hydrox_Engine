#version 440 core

#include "../../HydroxShader/include/Shader/Shaderincludes/CameraUBO.glslh"

layout(r16ui, binding = 0) writeonly uniform uimage2D cacheIndexMap0;
layout(r16ui, binding = 1) writeonly uniform uimage2D cacheIndexMap1;
layout(r16ui, binding = 2) writeonly uniform uimage2D cacheIndexMap2;
layout(r16ui, binding = 3) writeonly uniform uimage2D cacheIndexMap3;

layout(r32ui, binding = 4) uniform uimage2D cacheIndexAtomicCounter;

layout(location = 0) uniform sampler2D gBufferDepthMap;
layout(location = 1) uniform sampler2D gBufferNormalMap;

layout(location = 0) out vec4 color;

in vec3 gsout_normal;
in flat uint gsout_vertexID;

void main()
{
	ivec2 texCoords = ivec2(gl_FragCoord.xy);

	vec3 normal = normalize(texelFetch(gBufferNormalMap, texCoords, 0).xyz * 2.0 - 1.0);
	
	float linZGBuff = (2.0 * near * far / (far + near - texelFetch(gBufferDepthMap, texCoords, 0).r * (far - near))) / far;
	float linZFrameBuff = (2.0 * near * far / (far + near - gl_FragCoord.z * (far - near))) / far;
	
	if(imageLoad(cacheIndexAtomicCounter, texCoords).r < 3 && abs(linZGBuff - linZFrameBuff) < 0.01 && dot(normal, normalize(gsout_normal)) > 0.9)
	{
		uint vertexID = gsout_vertexID + 1;

		uint index = imageAtomicAdd(cacheIndexAtomicCounter, texCoords, 1);
		
		//uint index = imageLoad(cacheIndexAtomicCounter, texCoords).r;
		//imageStore(cacheIndexAtomicCounter, texCoords, uvec4(index + 1));
		
		if(index == 0)
		{
			imageStore(cacheIndexMap0, texCoords, uvec4(vertexID));
		}
		else if(index == 1)
		{
			imageStore(cacheIndexMap1, texCoords, uvec4(vertexID));
		}
		else if(index == 2)
		{
			imageStore(cacheIndexMap2, texCoords, uvec4(vertexID));
		}
		else if(index == 3)
		{
			imageStore(cacheIndexMap3, texCoords, uvec4(vertexID));
		}
	}

	//color = vec4(1, 0, 0, 1);
}