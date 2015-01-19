#version 440 core

layout(location = 6) uniform sampler2D texSampler;

out vec4 color;

in vec2 gsout_texCoord;

void main()
{
	color = vec4(texture(texSampler, gsout_texCoord));//vec4(1,1,1,1);
}