#version 440 core

layout(r8ui, binding = 0) readonly uniform uimage2D edgeImage;
layout(r32f, binding = 1) writeonly uniform image2D vertexOffsetImage;

layout(location = 0) out vec4 color;

void main()
{
	color = vec4(1, 0, 0, 1);
	//color = vec4(float(imageLoad(edgeImage, ivec2(gl_FragCoord.xy)).r), 0, 0, 1);
}