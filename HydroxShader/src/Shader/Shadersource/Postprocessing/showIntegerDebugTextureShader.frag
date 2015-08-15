#version 440 core

#include "../../../../include/Shader/Shaderincludes/CameraUBO.glslh"
#include "../../../../include/Shader/Shaderincludes/Encodings.glslh"

layout(location = 0) uniform usampler2D colorSampler;

out vec4 color;

in vec2 gsout_texCoord;

void main()
{
	color = vec4(texture(colorSampler, gsout_texCoord).r < 4);//specular cache index counter
}