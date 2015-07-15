#version 440 core

#define INT32_MAX 2147483647

layout(r32ui, binding = 0) readonly uniform uimage2D indexBuffer;

layout(location = 0) uniform samplerBuffer siteBuffer;

layout(location = 1) uniform int stepSize;

layout(location = 2) uniform uint resolution;

out uint fsout_pixelIndex;

in vec2 gsout_texCoord;

void main()
{
	ivec2 texCoord = ivec2(gl_FragCoord.xy);
	uint pixelIndex = imageLoad(indexBuffer, texCoord).r - 1;
		
	int k[3];
	k[0] = -stepSize;
	k[1] = 0;
	k[2] = stepSize;
	
	float minDistance = pixelIndex < INT32_MAX ? length(ivec2(texelFetch(siteBuffer, int(pixelIndex)).xy * resolution) - ivec2(gl_FragCoord.xy)) : INT32_MAX;
	
	for(uint i = 0; i < 3; i++)
	{
		for(uint j = 0; j < 3; j++)
		{
			if(i != 1 || j != 1)
			{
				uint siteIndex = imageLoad(indexBuffer, ivec2(texCoord.x + k[j], texCoord.y + k[i])).r - 1;
				
				if(siteIndex != pixelIndex && siteIndex < INT32_MAX)
				{
					ivec2 siteCoords = ivec2(texelFetch(siteBuffer, int(siteIndex)).xy * resolution);
					
					float distance = length(siteCoords - ivec2(gl_FragCoord.xy));
					if(distance <= minDistance)
					{
						minDistance = distance;
						pixelIndex = siteIndex;//rewrite the pixels index
					}
				}
			}
		}
	}
	
	fsout_pixelIndex = pixelIndex + 1;
}