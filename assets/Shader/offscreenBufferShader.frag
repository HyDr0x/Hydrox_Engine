#version 440 core

#include "Header/CameraUBO.glslh"

layout(binding = 0) uniform sampler2D colorSampler;

out vec4 color;

in vec2 gsout_texCoord;

void main()
{
	/*
	float z = texture(depthSampler, gsout_texCoord).r;
	float zn = (z - 0.5f) * 2.0f;
	zn = (2 * near * far) / (far + near - zn * (far - near));
	
	if(zn > 100.0f)
		zn = 0.5f;
	
	color = vec4(zn);//vec4(gsout_texCoord.x,gsout_texCoord.y,0,1);*/
	//color = (texture(normalSampler, gsout_texCoord) - 0.5f) * 2.0f;
	
	color = texture(colorSampler, gsout_texCoord);
}