#version 440 core

#include "../../HydroxShader/include/Shader/Shaderincludes/LightData.glslh"
#include "../../HydroxShader/include/Shader/Shaderincludes/ParaboloidProjection.glslh"

#define SAMPLENUMBER
#define SAMPLENUMBERROOT
#define ADAPTIVESAMPLENUMBER

layout(points) in;
layout(points, max_vertices = 1) out;

layout(std430, binding = 0) buffer reflectiveShadowLightBuffer
{
	ShadowLightData reflectiveShadowLight[];
};

layout(location = 0) uniform uint reflectiveShadowMapResolution;
layout(location = 1) uniform uint lightIndex;

out vec2 gsout_site2D;
out float gsout_clipDepth;
out flat uint gsout_vertexIndex;

in flat uint vsout_vertexIndex[1];

void main()
{
	float cacheLightDistance;
	vec3 transformedCachePosition = (reflectiveShadowLight[lightIndex].lightView * vec4(gl_in[0].gl_Position.xyz, 1.0)).xyz;

	paraboloidProjection(transformedCachePosition, PARABOLOIDNEAR, PARABOLOIDFAR, transformedCachePosition, gsout_clipDepth);
	
	if(1.0 > transformedCachePosition.x && transformedCachePosition.x > -1.0 || 
		 1.0 > transformedCachePosition.y && transformedCachePosition.y > -1.0 ||
		 1.0 > transformedCachePosition.z && transformedCachePosition.z > -1.0)
	{
		gsout_vertexIndex = vsout_vertexIndex[0];
		
		gsout_site2D = transformedCachePosition.xy * 0.5 + 0.5;
		gl_Position = vec4(transformedCachePosition.xy, transformedCachePosition.z - 0.0001, 1);
		EmitVertex();
		EndPrimitive();
	}
}