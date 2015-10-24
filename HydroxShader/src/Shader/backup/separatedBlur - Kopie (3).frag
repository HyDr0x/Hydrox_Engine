#version 440 core

layout(location = 0) uniform sampler2D colorSampler;
layout(location = 1) uniform sampler2D depthSampler;
layout(location = 2) uniform sampler2D normalSampler;

layout(location = 3) uniform vec2 imageSize;
layout(location = 4) uniform uint kernelSize;

layout(location = 0) out float newShadowValue;

in vec2 gsout_texCoord;

void main()
{
	float shadowValue = 0.0;
	vec2 samplingDirection = 1.0 / imageSize;
	vec2 newTexCoord = gsout_texCoord - samplingDirection * float(kernelSize) * 0.5;
	
	float origDepth = texture(depthSampler, gsout_texCoord).r;
	vec3 origNormal = texture(normalSampler, gsout_texCoord).rgb * 2.0 - 1.0;
	
	uint validSamples = 0;
	
	for(uint i = 0; i < kernelSize; i++)
	{
		for(uint j = 0; j < kernelSize; j++)
		{
			float newDepth = texture(depthSampler, newTexCoord).r;
			vec3 newNormal = texture(normalSampler, newTexCoord).rgb * 2.0 - 1.0;
			
			if(abs(origDepth - newDepth) < 0.0001 && dot(origNormal, newNormal) > 0.9)
			{
				validSamples++;
				shadowValue += max(texture(colorSampler, newTexCoord).r, 0.0);
			}
			newTexCoord.x += samplingDirection.x;
		}
		newTexCoord.x -= samplingDirection.x * float(kernelSize);
		newTexCoord.y += samplingDirection.y;
	}
	
	//shadowValue /= float(kernelSize * kernelSize);
	shadowValue /= float(validSamples);
	
  newShadowValue = shadowValue;
}