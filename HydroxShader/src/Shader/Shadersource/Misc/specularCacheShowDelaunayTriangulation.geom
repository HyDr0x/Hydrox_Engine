#version 440 core

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

void main()
{
	for(uint i = 0; i < 3; i++)
	{
		gl_Position = gl_in[i].gl_Position;
		EmitVertex();
	}
	EndPrimitive();
}