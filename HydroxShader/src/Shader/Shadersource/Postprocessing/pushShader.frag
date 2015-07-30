#version 440 core

layout(location = 0) uniform sampler2DArray depthSampler;
layout(location = 1) uniform sampler2DArray referenceDepthSampler;

layout(location = 2) uniform uint layer;
layout(location = 3) uniform uint pyramidLevel;

in vec2 gsout_texCoord;

void main()
{
	float errorRate = 0.5 * pow(2.0, float(pyramidLevel));
	float referenceDepth = texture(referenceDepthSampler, vec3(gsout_texCoord.xy, layer)).r;
	float newDepth = texture(depthSampler, vec3(gsout_texCoord.xy, layer)).r;
	
	gl_FragDepth = abs(referenceDepth - newDepth) > errorRate ? newDepth : 1.0;
}