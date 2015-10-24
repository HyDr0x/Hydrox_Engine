#version 440 core

layout(location = 0) uniform sampler2D colorSampler;

layout(location = 3) uniform vec2 samplingDirection;
layout(location = 4) uniform uint kernelSize;

layout(location = 0) out vec4 color;

in vec2 gsout_texCoord;

void main()
{
	vec3 newColor = vec3(0);
	vec2 newTexCoord = gsout_texCoord - samplingDirection * float(kernelSize) * 0.5;
	
	for(uint i = 0; i < kernelSize; i++)
	{
		newColor += max(texture(colorSampler, newTexCoord).rgb, vec3(0));
		newTexCoord += samplingDirection;
	}
	
	newColor /= float(kernelSize);
	
  color = vec4(newColor, 1);
}