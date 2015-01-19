#version 440 core

layout(location = 0) uniform sampler2D colorSampler;

layout(location = 1) uniform vec2 samplingRate;
layout(location = 2) uniform vec2 filterParam;//x == size, y == weight

out vec4 luminousFlux;

in vec2 gsout_texCoord;

void main()
{
	vec3 color = vec3(0.0f);
	for(uint i = 0; i < filterParam.x; i++)
	{
		color += filterParam.y * texture(colorSampler, gsout_texCoord + i * samplingRate).rgb;
	}
	
	luminousFlux = vec4(color, 1.0f);
}