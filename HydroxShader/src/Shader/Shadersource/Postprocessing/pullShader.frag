#version 440 core

layout(location = 0) uniform sampler2DArray loadShadowMap;

layout(location = 1) uniform uint lightIndex;
layout(location = 2) uniform uint level;

in vec2 gsout_texCoord;

void main()
{
	float errorRate = 0.05 * pow(2, level);
	float oldDepths[4];

	ivec2 texCoord = 2 * ivec2(gl_FragCoord.xy);

	float newDepth = 0.0;
	float kernelSize = 0.0;
	
	oldDepths[0] = texelFetch(loadShadowMap, ivec3(texCoord.x    , texCoord.y, lightIndex), 0).r;
	oldDepths[1] = texelFetch(loadShadowMap, ivec3(texCoord.x + 1, texCoord.y, lightIndex), 0).r;
	oldDepths[2] = texelFetch(loadShadowMap, ivec3(texCoord.x    , texCoord.y + 1, lightIndex), 0).r;
	oldDepths[3] = texelFetch(loadShadowMap, ivec3(texCoord.x + 1, texCoord.y + 1, lightIndex), 0).r;
	
	for(uint j = 0; j < 4; j++)
	{
		if(oldDepths[j] < 1.0)
		{
			uint depthDifferences = 0;
			depthDifferences += abs(oldDepths[int(mod(j + 1, 4))] - oldDepths[j]) < errorRate || oldDepths[int(mod(j + 1, 4))] == 1.0 ? 1 : 0;
			depthDifferences += abs(oldDepths[int(mod(j + 2, 4))] - oldDepths[j]) < errorRate || oldDepths[int(mod(j + 2, 4))] == 1.0 ? 1 : 0;
			depthDifferences += abs(oldDepths[int(mod(j + 3, 4))] - oldDepths[j]) < errorRate || oldDepths[int(mod(j + 3, 4))] == 1.0 ? 1 : 0;

			if(depthDifferences >= 2)
			{
				newDepth += oldDepths[j];
				kernelSize++;
			}
		}
	}
	
	if(kernelSize == 0.0)
	{
		newDepth = 1.0;
	}
	else
	{
		newDepth /= kernelSize;
	}

	gl_FragDepth = newDepth;
}