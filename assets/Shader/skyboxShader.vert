#version 440 core

layout(std140, binding = 0) uniform cameraParameters
{
	mat4 viewMatrix;
	mat4 projectionMatrix;
	mat4 viewProjectionMatrix;
	vec4 eyePos;
};

layout(location = 0) in vec3 in_Pos;
layout(location = 1) in vec2 in_texCoord;

out vec3 vsout_texCoord;

void main()
{
	vsout_texCoord = vec3(in_texCoord, uint(gl_VertexID / 6));
	
	mat3 view = mat3(viewMatrix);
	vec4 pos = projectionMatrix * vec4(view * (3 * in_Pos), 1);
	
	gl_Position = vec4(pos.xyww);//so z == ~1
}