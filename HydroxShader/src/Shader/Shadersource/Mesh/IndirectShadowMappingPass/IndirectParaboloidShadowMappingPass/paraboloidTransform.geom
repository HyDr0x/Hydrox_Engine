#version 440 core

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

#include "../../../../../../include/Shader/Shaderincludes/LightData.glslh"
#include "../../../../../../include/Shader/Shaderincludes/ParaboloidProjection.glslh"

layout(std430, binding = 1) buffer reflectiveShadowLightBuffer
{
	ShadowLightData reflectiveShadowLight[];
};

layout(location = 0) uniform uint lightIndex;

out float gsout_clipDepth;
out vec4 gsout_pos3D;
out vec3 gsout_normal;

in vec3 vsout_normal[3];

void main()
{
	vec3 positions[3];
	float clipDepth[3];
	
	for(uint i = 0; i < 3; i++)
	{
		vec3 transformedPosition = (reflectiveShadowLight[lightIndex].lightView * vec4(gl_in[i].gl_Position.xyz, 1.0)).xyz;
		
		paraboloidProjection(transformedPosition, PARABOLOIDNEAR, PARABOLOIDFAR, transformedPosition, clipDepth[i]);
		
		//if(length(transformedPosition.xy) > 1.0) isInterior = false;
		if(1.0 < transformedPosition.x || transformedPosition.x < -1.0 || 
			 1.0 < transformedPosition.y || transformedPosition.y < -1.0 ||
			 1.0 < transformedPosition.z || transformedPosition.z < -1.0)
		{
			return;
		}
		
		positions[i] = transformedPosition;
	}
	
	for(uint i = 0; i < 3; i++)
	{
		gsout_clipDepth = clipDepth[i];
		gsout_pos3D = vec4(gl_in[i].gl_Position.xyz, 1.0);
		gsout_normal = vsout_normal[i];
		gl_Position = vec4(positions[i], 1.0);
		EmitVertex();
	}
	EndPrimitive();
}