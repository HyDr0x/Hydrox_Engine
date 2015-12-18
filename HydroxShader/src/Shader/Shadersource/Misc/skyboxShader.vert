#version 440 core

#include "../../HydroxShader/include/Shader/Shaderincludes/VertexDeclaration.glslh"
#include "../../HydroxShader/include/Shader/Shaderincludes/CameraUBO.glslh"

layout(location = POSITION) in vec3 in_Pos;
layout(location = TEXTURE0) in vec2 in_texCoord;

out vec3 vsout_texCoord;

void main()
{
	vsout_texCoord = vec3(in_texCoord, uint(gl_VertexID / 6));
	
	mat3 view = mat3(viewMatrix);
	vec4 pos = projectionMatrix * vec4(view * (3 * in_Pos), 1);
	
	gl_Position = vec4(pos.xyww);//so z == ~1
}