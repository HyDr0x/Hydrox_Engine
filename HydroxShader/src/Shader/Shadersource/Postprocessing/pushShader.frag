#version 440 core

layout(location = 0) uniform sampler2DArray depthSampler;

layout(location = 1) uniform uint layer;

in vec2 gsout_texCoord;

void main()
{
	gl_FragDepth = texture(depthSampler, vec3(gsout_texCoord.xy, layer)).r;
}