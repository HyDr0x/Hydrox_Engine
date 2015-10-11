#version 440 core

#include "../../../../include/Shader/Shaderincludes/CameraUBO.glslh"
#include "../../../../include/Shader/Shaderincludes/Encodings.glslh"

layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

out vec3 gsout_normal;
out flat uint gsout_vertexID;

in vec4 vsout_normalMat[1];
in flat uint vsout_vertexID[1];

void main()
{
	vec3 normal = decodeNormal(vsout_normalMat[0].xy);

	vec3 x = normalize(cross(normal, normal + vec3(0, 1.0, 0)));
	vec3 z = normalize(cross(normal, x));
	
	vec3 points[4];
	
	const float size = gl_in[0].gl_Position.w;

	points[0] = gl_in[0].gl_Position.xyz - size * x + size * z;
	points[1] = gl_in[0].gl_Position.xyz - size * x - size * z;
	points[2] = gl_in[0].gl_Position.xyz + size * x + size * z;
	points[3] = gl_in[0].gl_Position.xyz + size * x - size * z;
	
	for(uint i = 0; i < 4; i++)
	{
		gsout_normal = normal;
		gsout_vertexID = vsout_vertexID[0];
		gl_Position = viewProjectionMatrix * vec4(points[i], 1);
		EmitVertex();
	}
	EndPrimitive();
}