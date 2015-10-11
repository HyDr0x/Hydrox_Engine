#version 440 core

#define INT32_MAX 2147483647
#define UINT16_MAX 65535

#include "../../../../include/Shader/Shaderincludes/CameraUBO.glslh"

layout(location = 1) uniform usampler2D indexTexture;

layout(std430, binding = 0) buffer projectedSpecularCachePositionBuffer
{
	ivec2 projectedSpecularCachePositions[];
};

layout(location = 0) uniform int stepSize;

out uint fsout_pixelIndex;

in vec2 gsout_texCoord;

/*
	try to only save the indices at the spawn points once
	sample the 2D coordinate of the spawn point directly, compare it with the actual pixel like with the index
	--> only one fetch per direction, 2d spawn point coordinates redirect to the id automatically
*/

void main()
{
	ivec2 texCoord = ivec2(gl_FragCoord.xy);
  uint pixelIndex = texelFetch(indexTexture, texCoord, 0).r;
	
	int k[3];
	k[0] = -stepSize;
	k[1] = 0;
	k[2] = stepSize;
	
	float minDistance = pixelIndex < UINT16_MAX ? length(projectedSpecularCachePositions[pixelIndex] - ivec2(gl_FragCoord.xy)) : INT32_MAX;
	
	for(uint i = 0; i < 3; i++)
	{
		for(uint j = 0; j < 3; j++)
		{
			if(i != 1 || j != 1)
			{
				ivec2 addedTexCoords = ivec2(texCoord.x + k[j], texCoord.y + k[i]);
				
				if(0 <= addedTexCoords.x && addedTexCoords.x < width && 0 <= addedTexCoords.y && addedTexCoords.y < height)
				{
					uint siteIndex = texelFetch(indexTexture, addedTexCoords, 0).r;
					
					if(siteIndex != pixelIndex && siteIndex < UINT16_MAX)
					{
						ivec2 siteCoords = projectedSpecularCachePositions[siteIndex];

						float distance = length(siteCoords - texCoord);
						if(distance < minDistance)
						{
							minDistance = distance;
							pixelIndex = siteIndex;//rewrite the pixels index
						}
					}
				}
			}
		}
	}
	
	fsout_pixelIndex = pixelIndex;
}