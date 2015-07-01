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

out float gsout_clipDepth;

in vec4 vsout_pos3D[1];
in float vsout_cacheArea[1];
in uint vsout_instanceID[1];

void main()
{
	ivec2 texCoord = ivec2((samples[vsout_instanceID[0]].xy + vec2(0.5)) * reflectiveShadowMapResolution);
	vec4 lightPos = imageLoad(indirectLightPositions, texCoord);
	vec3 lightNormal = imageLoad(indirectLightNormals, texCoord).xyz * 2.0 - 1.0;
	
	//vec2 texOffset = vec2(mod(float(vsout_instanceID[0]), SAMPLENUMBERROOT) * 2.0 / float(SAMPLENUMBERROOT), (float(vsout_instanceID[0]) / SAMPLENUMBERROOT) * 2.0 / float(SAMPLENUMBERROOT));
	
	float cacheLightDistance;
	vec3 transformedCachePosition;
	{
		transformedCachePosition = vsout_pos3D[0].xyz - lightPos.xyz;
	
		float lightRotationAngle = acos(lightNormal.z);
		vec3 rotationAxis = normalize(vec3(lightNormal.y, -lightNormal.x, 0.0));//normalize(cross(lightNormal.xyz, vec3(0, 0, 1)));
		
		vec4 q = createQuaternion(lightRotationAngle, rotationAxis);
		transformedCachePosition = applyQuaternion(transformedCachePosition, q);

		transformedCachePosition.z *= -1.0;
		
		cacheLightDistance = length(transformedCachePosition);
		transformedCachePosition /= cacheLightDistance;//create the normal of the point at the paraboloid to get the texture coordinates
		gsout_clipDepth = transformedCachePosition.z;
		transformedCachePosition.z += 1.0;//add the reflection vector(0, 0, 1)
		transformedCachePosition.xy /= transformedCachePosition.z;//divide through the new z value to get the normal
		
		transformedCachePosition.xy = (transformedCachePosition.xy - float(SAMPLENUMBERROOT) + 1.0) / float(SAMPLENUMBERROOT);
		
		transformedCachePosition.x += mod(float(vsout_instanceID[0]), SAMPLENUMBERROOT) * 2.0 / float(SAMPLENUMBERROOT);
		transformedCachePosition.y += (float(vsout_instanceID[0]) / SAMPLENUMBERROOT) * 2.0 / float(SAMPLENUMBERROOT) - (mod(float(vsout_instanceID[0]), SAMPLENUMBERROOT) * 2) / SAMPLENUMBER;
		
		float distance = (cacheLightDistance - 0.1) / (100.0 - 0.1);
			
		gl_Position = vec4(transformedCachePosition.x, transformedCachePosition.y, distance * 2.0 - 1.0, 1);
		EmitVertex();
		EndPrimitive();
	}
	/*
	//float solidAngle = vsout_cacheArea[0] / cacheLightDistance;
	float solidAngle = 2.0 / cacheLightDistance;
	uint sampleNumber = uint(ADAPTIVESAMPLENUMBER * (solidAngle / (2.0 * PI)));
	
	vec3 backProjetionPos = (reflectiveShadowLight[lightIndex].lightView * vec4(vsout_pos3D[0].xyz, 1)).xyz;
	backProjetionPos.z *= -1.0;
	float distance = length(backProjetionPos);
	backProjetionPos /= distance;//create the normal of the point at the paraboloid to get the texture coordinates
	backProjetionPos.xy /= (backProjetionPos.z + 1.0);//divide through the new z value to get the normal
		
	for(uint i = 0; i < 0; i++)
	{
		vec2 backprojTexCoord = backProjetionPos.xy + noise2(float(vsout_instanceID[0])) * 0.1;
		vec3 sampledPos = imageLoad(backprojectionBuffer, ivec2(backprojTexCoord.x * 1024, backprojTexCoord.y * 768)).xyz;
		vec3 transformedPosition = sampledPos - lightPos.xyz;
		
		float lightRotationAngle = acos(lightNormal.z);
		vec3 rotationAxis = normalize(vec3(lightNormal.y, -lightNormal.x, 0.0));//normalize(cross(lightNormal.xyz, vec3(0, 0, 1)));
		
		vec4 q = createQuaternion(lightRotationAngle, rotationAxis);
		transformedPosition = applyQuaternion(transformedPosition, q);

		transformedPosition.z *= -1.0;
		
		float distance = length(transformedPosition);
		transformedPosition /= distance;//create the normal of the point at the paraboloid to get the texture coordinates
		gsout_clipDepth = transformedPosition.z;
		transformedPosition.z += 1.0;//add the reflection vector(0, 0, 1)
		transformedPosition.xy /= transformedPosition.z;//divide through the new z value to get the normal
		
		transformedPosition.xy = (transformedPosition.xy - float(SAMPLENUMBERROOT) + 1.0) / float(SAMPLENUMBERROOT);
		
		transformedCachePosition.x += mod(float(vsout_instanceID[0]), SAMPLENUMBERROOT) * 2.0 / float(SAMPLENUMBERROOT);
		transformedCachePosition.y += (float(vsout_instanceID[0]) / SAMPLENUMBERROOT) * 2.0 / float(SAMPLENUMBERROOT) - (mod(float(vsout_instanceID[0]), SAMPLENUMBERROOT) * 2) / SAMPLENUMBER;
		
		distance = (distance - 0.1) / (100.0 - 0.1);
		
		gl_Position = vec4(transformedPosition.x, transformedPosition.y, distance * 2.0 - 1.0, 1);
		EmitVertex();
		EndPrimitive();
	}*/
}