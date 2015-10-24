#version 440 core

//layout(early_fragment_tests) in;

layout(location = 0) uniform sampler2D gBufferMaterialSampler;

layout(location = 0) out vec4 fsout_position3D;
layout(location = 1) out vec4 fsout_normal;

in GeometryData
{
	vec3 pos3D;
	vec3 normal;
} inData;

void main()
{
	gl_FragDepth = gl_FragCoord.z - 0.0001;
	vec4 material = texelFetch(gBufferMaterialSampler, ivec2(gl_FragCoord.xy), 0);
	
	fsout_position3D = vec4(inData.pos3D, material.g);
	fsout_normal = vec4(inData.normal, material.a);
}