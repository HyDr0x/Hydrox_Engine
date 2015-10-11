#version 440 core

layout(r8ui, binding = 0) readonly uniform uimage2D edgeImage;
layout(r32f, binding = 1) writeonly uniform image2D vertexOffsetImage;
layout(r8ui, binding = 2) writeonly uniform uimage2D vertexPositionImage;

layout(location = 0) out vec4 color;

void main()
{
	if(imageLoad(edgeImage, ivec2(gl_FragCoord.xy)).r >= 1)
	{
		//imageStore(vertexOffsetImage, ivec2(gl_FragCoord.xy), vec4(1.0));
		//imageStore(vertexPositionImage, ivec2(gl_FragCoord.xy), uvec4(1));
	}
	
	color = vec4(1, 0, 0, 1);
}