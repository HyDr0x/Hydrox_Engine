#version 440 core

#include "../../../../../include/Shader/Shaderincludes/VertexDeclaration.glslh"
#include "../../../../../include/Shader/Shaderincludes/LightData.glslh"
#include "../../../../../include/Shader/Shaderincludes/Quaternion.glslh"

#define PI 3.14159265359

#define SAMPLENUMBER
#define SAMPLENUMBERROOT
#define ADAPTIVESAMPLENUMBER

layout(points) in;
layout(points, max_vertices = ADAPTIVESAMPLENUMBER + 1) out;

layout(rgba32f, binding = 0) readonly uniform image2D indirectLightPositions;
layout(rgba32f, binding = 1) readonly uniform image2D indirectLightNormals;
layout(rgba32f, binding = 2) readonly uniform image2D backprojectionBuffer;

layout(std140, binding = 1) uniform SampleBuffer
{
	vec4 samples[SAMPLENUMBER];
};

layout(std430, binding = 0) buffer reflectiveShadowLightBuffer
{
	ShadowLightData reflectiveShadowLight[];
};

layout(location = 0) uniform uint reflectiveShadowMapResolution;
layout(location = 1) uniform uint lightIndex;

layout(location = POSITION) in vec4 in_Pos;
layout(location = SPECIAL0) in float in_cacheArea;

out float vsout_clipDepth;

void main()
{
	ivec2 texCoord = ivec2((samples[gl_InstanceID].xy + vec2(0.5)) * reflectiveShadowMapResolution);
	vec4 lightPos = imageLoad(indirectLightPositions, texCoord);
	vec3 lightNormal = imageLoad(indirectLightNormals, texCoord).xyz * 2.0 - 1.0;
	
	//vec2 texOffset = vec2(mod(float(gl_InstanceID), SAMPLENUMBERROOT) * 2.0 / float(SAMPLENUMBERROOT), (float(gl_InstanceID) / SAMPLENUMBERROOT) * 2.0 / float(SAMPLENUMBERROOT));
	
	float cacheLightDistance;
	vec3 transformedCachePosition;
	transformedCachePosition = in_Pos.xyz - lightPos.xyz;

	float lightRotationAngle = acos(lightNormal.z);
	vec3 rotationAxis = normalize(vec3(lightNormal.y, -lightNormal.x, 0.0));//normalize(cross(lightNormal.xyz, vec3(0, 0, 1)));
	
	vec4 q = createQuaternion(lightRotationAngle, rotationAxis);
	transformedCachePosition = applyQuaternion(transformedCachePosition, q);

	transformedCachePosition.z *= -1.0;
	
	cacheLightDistance = length(transformedCachePosition);
	transformedCachePosition /= cacheLightDistance;//create the normal of the point at the paraboloid to get the texture coordinates
	vsout_clipDepth = transformedCachePosition.z;
	transformedCachePosition.z += 1.0;//add the reflection vector(0, 0, 1)
	transformedCachePosition.xy /= transformedCachePosition.z;//divide through the new z value to get the normal
	
	transformedCachePosition.xy = (transformedCachePosition.xy - float(SAMPLENUMBERROOT) + 1.0) / float(SAMPLENUMBERROOT);
	
	transformedCachePosition.x += mod(float(gl_InstanceID), SAMPLENUMBERROOT) * 2.0 / float(SAMPLENUMBERROOT);
	transformedCachePosition.y += (float(gl_InstanceID) / SAMPLENUMBERROOT) * 2.0 / float(SAMPLENUMBERROOT) - (mod(float(gl_InstanceID), SAMPLENUMBERROOT) * 2) / SAMPLENUMBER;
	
	float distance = (cacheLightDistance - 0.1) / (100.0 - 0.1);
		
	gl_Position = vec4(transformedCachePosition.x, transformedCachePosition.y, distance * 2.0 - 1.0, 1);
}