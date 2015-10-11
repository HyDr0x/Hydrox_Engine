#version 440 core

layout(location = 0) out vec4 color;

in float vsout_cacheOrVertex;

void main()
{
	color = vec4(1.0 - vsout_cacheOrVertex, vsout_cacheOrVertex, 0, 1);
}