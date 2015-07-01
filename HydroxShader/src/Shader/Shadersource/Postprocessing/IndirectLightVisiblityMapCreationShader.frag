#version 440 core

#define SAMPLENUMBER
#define SAMPLENUMBERROOT
#define INTERLEAVEDBLOCKSIZE

#include "../../../../include/Shader/Shaderincludes/CameraUBO.glslh"
#include "../../../../include/Shader/Shaderincludes/Quaternion.glslh"

layout(location = 0) uniform sampler2D depthSampler;
layout(location = 1) uniform sampler2DArray indirectShadowMapSampler;

layout(rgba32f, binding = 0) readonly uniform image2DArray indirectLightPositions;
layout(rgba32f, binding = 1) readonly uniform image2DArray indirectLightNormals;

layout(std140, binding = 1) uniform SampleBuffer
{
	vec4 samples[SAMPLENUMBER];
};

layout(location = 2) uniform uint reflectiveShadowMapResolution;
layout(location = 3) uniform uint reflectiveShadowMapNumber;

layout(location = 0) out float fsout_visibility;

in vec2 gsout_texCoord;

void main()
{
	float z = texture(depthSampler, gsout_texCoord).r;
	
	vec4 pos3D = invViewProjectionMatrix * vec4(vec3(gsout_texCoord.xy, z) * 2.0 - 1.0, 1);
	pos3D /= pos3D.w;
	
	float visibleIndirectLightNumber = 0.0;
	
	const uint interleavedSize = INTERLEAVEDBLOCKSIZE * INTERLEAVEDBLOCKSIZE;

	ivec2 blockCoord = ivec2(mod(gl_FragCoord.x, INTERLEAVEDBLOCKSIZE), mod(gl_FragCoord.y, INTERLEAVEDBLOCKSIZE));
	
	const uint startIndex = blockCoord.x + blockCoord.y * INTERLEAVEDBLOCKSIZE;
	
	for(uint j = 0; j < reflectiveShadowMapNumber; j++)
	{
		for(uint i = startIndex; i < SAMPLENUMBER; i += interleavedSize)
		{
			ivec3 indirectLightTexCoord = ivec3((samples[i].xy + vec2(0.5)) * reflectiveShadowMapResolution, j);
			vec4 lightPos = imageLoad(indirectLightPositions, indirectLightTexCoord);
			
			if(lightPos.w == 0.0) //cancels indirect lightsources which aren't initialized, because there is no geometry behind or lit
			{
				visibleIndirectLightNumber++;
				continue;
			}
			
			vec3 lightNormal = imageLoad(indirectLightNormals, indirectLightTexCoord).xyz * 2.0 - 1.0;
			
			vec3 transformedPosition = pos3D.xyz - lightPos.xyz;

			float lightRotationAngle = acos(lightNormal.z);
			vec3 rotationAxis = normalize(vec3(lightNormal.y, -lightNormal.x, 0.0));//normalize(cross(lightNormal.xyz, vec3(0, 0, 1)));
			
			vec4 q = createQuaternion(lightRotationAngle, rotationAxis);
			transformedPosition = applyQuaternion(transformedPosition, q);

			float distance = length(transformedPosition);
			
			transformedPosition.z *= -1.0;
			transformedPosition /= distance;//create the normal of the point at the paraboloid to get the texture coordinates
			transformedPosition.z += 1.0;//add the reflection vector(0, 0, 1)
			transformedPosition.xy /= transformedPosition.z;//divide through the new z value to get the normal
			
			transformedPosition.xy = (transformedPosition.xy * 0.5 + 0.5) / float(SAMPLENUMBERROOT);
			
			transformedPosition.x += mod(i, SAMPLENUMBERROOT) / float(SAMPLENUMBERROOT);
			transformedPosition.y += (i / SAMPLENUMBERROOT) / float(SAMPLENUMBERROOT);
			
			float depth = texture(indirectShadowMapSampler, vec3(transformedPosition.xy, j)).r;
/*
			float f = 100.0, n = 0.1;
			distance = -distance;
			distance = (distance * ((-f - n) / (f - n)) - (2.0 * f * n) / (f - n)) / (-distance) - 0.0019;*/
			distance = (distance - 0.1) / (100.0 - 0.1) - 0.015;
			
			//visibleIndirectLightNumber = depth;
			visibleIndirectLightNumber += clamp(ceil(depth - distance), 0, 1);
		}
	}
	
	//fsout_visibility = visibleIndirectLightNumber;
	fsout_visibility = visibleIndirectLightNumber / float(reflectiveShadowMapNumber * (SAMPLENUMBER / interleavedSize));
}