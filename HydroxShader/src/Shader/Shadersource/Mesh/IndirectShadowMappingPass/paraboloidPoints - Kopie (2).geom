#version 440 core

layout(points) in;
layout(points, max_vertices = 1) out;

out float gsout_clipDepth;

in float vsout_clipDepth[1];

void main()
{
	gsout_clipDepth = vsout_clipDepth[0];
	gl_Position = gl_in[0].gl_Position;
	EmitVertex();
	EndPrimitive();
}