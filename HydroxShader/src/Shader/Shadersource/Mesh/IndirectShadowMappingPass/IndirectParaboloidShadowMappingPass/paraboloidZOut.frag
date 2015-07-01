#version 440 core

in float vsout_clipDepth;
in vec4 vsout_pos3D;

layout(location = 0) out vec3 fsout_pos3D;

void main()
{
	if(vsout_clipDepth < 0.0)
	{
		discard;
	}
	
	fsout_pos3D = vsout_pos3D.xyz;
}