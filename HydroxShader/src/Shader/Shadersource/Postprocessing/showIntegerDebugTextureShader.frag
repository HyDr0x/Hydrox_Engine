#version 440 core

#include "../../HydroxShader/include/Shader/Shaderincludes/CameraUBO.glslh"
#include "../../HydroxShader/include/Shader/Shaderincludes/Encodings.glslh"

layout(location = 0) uniform usampler2D colorSampler;

out vec4 color;

in vec2 gsout_texCoord;

void main()
{
	uvec4 val = texture(colorSampler, gsout_texCoord);
	color = vec4((float(val.r) / 50.0));//vec4(val.r == 0, val.r > 32768, 0, 0);//specular cache index counter
}