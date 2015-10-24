#version 440 core

layout(points)in;
layout(triangle_strip, max_vertices = 4)out;

#include "../../HydroxShader/include/Shader/Shaderincludes/CameraUBO.glslh"

layout(location = 3) uniform mat3 texTrfMatrix;
layout(location = 4) uniform vec2 Scale;
layout(location = 5) uniform vec3 Translate;

out vec2 gsout_texCoord;

void main()
{
	vec3 camRight = vec3(viewMatrix[0][0], viewMatrix[1][0], viewMatrix[2][0]) * Scale[0] * 0.5;
	vec3 camUp = vec3(viewMatrix[0][1], viewMatrix[1][1], viewMatrix[2][1]) * Scale[1] * 0.5;
	vec4 pos;

	//////////////FIRST TRIANGLE//////////////////
	gsout_texCoord = (texTrfMatrix * vec3(0, 1, 1)).xy;
	pos = vec4(Translate - camRight - camUp, 1);
	gl_Position = viewProjectionMatrix * pos;
	EmitVertex();
	
	gsout_texCoord = (texTrfMatrix * vec3(0, 0, 1)).xy;
	pos = vec4(Translate - camRight + camUp, 1);
	gl_Position = viewProjectionMatrix * pos;
	EmitVertex();

	gsout_texCoord = (texTrfMatrix * vec3(1, 1, 1)).xy;
	pos = vec4(Translate + camRight - camUp,1);
	gl_Position = viewProjectionMatrix * pos;
	EmitVertex();

	///////////////SECOND TRIANGLE/////////////////
	gsout_texCoord = (texTrfMatrix * vec3(1, 0, 1)).xy;
	pos = vec4(Translate + camRight + camUp, 1);
	gl_Position = viewProjectionMatrix * pos;
	EmitVertex();
	EndPrimitive();
}