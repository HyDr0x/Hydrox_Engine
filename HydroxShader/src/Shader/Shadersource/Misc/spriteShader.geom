#version 440 core

layout(points)in;
layout(triangle_strip, max_vertices = 4)out;

layout(location = 0) uniform mat3 trfMatrix;
layout(location = 1) uniform mat3 texTrfMatrix;
layout(location = 2) uniform float spriteZ;

out vec2 gsout_texCoord;

void main()
{
	vec3 pos;

	//////////////FIRST TRIANGLE//////////////////
	pos = trfMatrix * vec3(0.0, 0.0, 1.0);
	gsout_texCoord = (texTrfMatrix * vec3(0.0, 0.0, 1.0)).xy;
	gl_Position = vec4(pos.xy, spriteZ, 1.0);
	EmitVertex();

	pos = trfMatrix * vec3(0.0, -1.0, 1.0);
	gsout_texCoord = (texTrfMatrix * vec3(0.0, 1.0, 1.0)).xy;
	gl_Position = vec4(pos.xy, spriteZ, 1.0);
	EmitVertex();

	pos = trfMatrix * vec3(1.0, 0.0, 1.0);
	gsout_texCoord = (texTrfMatrix * vec3(1.0, 0.0, 1.0)).xy;
	gl_Position = vec4(pos.xy, spriteZ, 1.0);
	EmitVertex();

	///////////////SECOND TRIANGLE/////////////////

	pos = trfMatrix * vec3(1.0, -1.0, 1.0);
	gsout_texCoord = (texTrfMatrix * vec3(1.0, 1.0, 1.0)).xy;
	gl_Position = vec4(pos.xy, spriteZ, 1.0);
	EmitVertex();
	EndPrimitive();
}