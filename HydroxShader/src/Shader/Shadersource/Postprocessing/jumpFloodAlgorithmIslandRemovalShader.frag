#version 440 core

#define INT32_MAX 2147483647

#include "../../HydroxShader/include/Shader/Shaderincludes/CameraUBO.glslh"

layout(r16ui, binding = 0) readonly uniform uimage2D indexBuffer;

layout(std430, binding = 0) buffer projectedSpecularCachePositionBuffer
{
	ivec2 projectedSpecularCachePositions[];
};

out uint fsout_pixelIndex;

in vec2 gsout_texCoord;

void main()
{
	ivec2 texCoord = ivec2(gl_FragCoord.xy);
	uint pixelIndex = imageLoad(indexBuffer, texCoord).r;
		
	ivec2 sitePosition = projectedSpecularCachePositions[pixelIndex];
	
	ivec2 direction = clamp(sitePosition - texCoord, -1, 1);

	
	float minLength = INT32_MAX;
	uint newIndex = pixelIndex;

	if(direction.x != 0)
	{
		uint siteIndex = imageLoad(indexBuffer, ivec2(texCoord.x + direction.x, texCoord.y)).r;
		if(siteIndex != pixelIndex)
		{
			minLength = distance(sitePosition, projectedSpecularCachePositions[siteIndex]);
			newIndex = siteIndex;
		}
		else
		{
			fsout_pixelIndex = pixelIndex;
			return;
		}
	}

	if(direction.y != 0)
	{
		uint siteIndex = imageLoad(indexBuffer, ivec2(texCoord.x, texCoord.y + direction.y)).r;
		if(siteIndex != pixelIndex)
		{
			float length = distance(sitePosition, projectedSpecularCachePositions[siteIndex]);
			if(length < minLength)
			{
				minLength = length;
				newIndex = siteIndex;
			}
		}
		else
		{
			fsout_pixelIndex = pixelIndex;
			return;
		}
	}
	
	if(direction.x != 0 && direction.y != 0)
	{
		uint siteIndex = imageLoad(indexBuffer, texCoord + direction).r;
		if(siteIndex != pixelIndex)
		{
			float length = distance(sitePosition, projectedSpecularCachePositions[siteIndex]);
			if(length < minLength)
			{
				newIndex = siteIndex;
			}
		}
		else
		{
			fsout_pixelIndex = pixelIndex;
			return;
		}
	}
	
	fsout_pixelIndex = newIndex;
}