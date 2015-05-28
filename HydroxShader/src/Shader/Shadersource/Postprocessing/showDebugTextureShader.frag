#version 440 core

#include "../../../../include/Shader/Shaderincludes/CameraUBO.glslh"

layout(location = 0) uniform sampler2D colorSampler;

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
	
	color = vec4(zn);//vec4(gsout_texCoord.x,gsout_texCoord.y, 0, 1);
	*/
	
	//color = pow(texture(colorSampler, gsout_texCoord), vec4(128.0f));//depth buffer output
	//color = texture(colorSampler, gsout_texCoord) * 2.0f - 1.0f;//normal buffer output
	//color = vec4(texture(colorSampler, gsout_texCoord)) / 400.0f;//index texture output
	color = vec4(texture(colorSampler, gsout_texCoord));//simple texture output
}