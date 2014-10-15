#version 440 core

layout(location = 1) uniform mat3 trfMatrix;
layout(location = 5) uniform float z;

layout(location = 0) in vec2 in_Pos;
layout(location = 1) in vec2 in_texCoord;

out vec2 vsout_texCoord;

void main()
{
	vsout_texCoord = in_texCoord;
	vec3 pos = trfMatrix * vec3(in_Pos.xy, 1.0);
	gl_Position = vec4(pos.xy, z, 1.0);
}