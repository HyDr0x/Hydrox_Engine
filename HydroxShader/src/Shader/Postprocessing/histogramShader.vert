#version 440 core

#define HISTOGRAMBINS (1.0f / 8.0f)

#include "../../../include/Shader/VertexDeclaration.glslh"

layout(location = 0) uniform sampler2D hsvColorSampler;

layout(location = POSITION) in vec2 in_Pos;

void main()
{
  float luminance = texture(hsvColorSampler, in_Pos).b;
	gl_Position = vec4(max(ceil(log2(max(luminance, 0.0001f))), 0) * HISTOGRAMBINS - 1.0f, 0, 0, 1);
	//gl_Position = vec4(-1, 0, 0, 1);
}