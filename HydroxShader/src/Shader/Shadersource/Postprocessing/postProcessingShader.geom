#version 440 core

layout(points)in;
layout(triangle_strip, max_vertices = 4)out;

out vec2 gsout_texCoord;

void main()
{
	//////////////FIRST TRIANGLE//////////////////
	gsout_texCoord = vec2(0.0, 0.0);
	gl_Position = vec4(-1.0, -1.0, 0.0, 1.0);
	EmitVertex();

	gsout_texCoord = vec2(1.0, 0.0);
	gl_Position = vec4(1.0, -1.0, 0.0, 1.0);
	EmitVertex();

	gsout_texCoord = vec2(0.0, 1.0);
	gl_Position = vec4(-1.0, 1.0, 0.0, 1.0);
	EmitVertex();

	///////////////SECOND TRIANGLE/////////////////

	gsout_texCoord = vec2(1.0, 1.0);
	gl_Position = vec4(1.0, 1.0, 0.0, 1.0);
	EmitVertex();
	EndPrimitive();
}