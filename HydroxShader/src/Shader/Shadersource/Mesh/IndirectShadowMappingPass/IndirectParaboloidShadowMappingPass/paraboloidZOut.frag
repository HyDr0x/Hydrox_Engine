#version 440 core

layout(early_fragment_tests) in;

in float gsout_clipDepth;
in vec4 gsout_pos3D;
in vec3 gsout_normal;

layout(location = 0) out vec3 fsout_pos3D;
layout(location = 1) out vec3 fsout_normal;

void main()
{
	if(gsout_clipDepth < 0.0)
	{
		discard;
	}
	
	fsout_pos3D = gsout_pos3D.xyz;
	fsout_normal = gsout_normal;
}