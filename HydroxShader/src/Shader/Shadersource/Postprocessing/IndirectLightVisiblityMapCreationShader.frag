#version 440 core

#define SAMPLENUMBER
#define SHADOWSAMPLENUMBER
#define SHADOWSAMPLENUMBERROOT
#define INTERLEAVEDBLOCKSIZE

#include "../../../../include/Shader/Shaderincludes/CameraUBO.glslh"
#include "../../../../include/Shader/Shaderincludes/ParaboloidProjection.glslh"

layout(location = 0) uniform sampler2D depthSampler;
layout(location = 1) uniform sampler2DArray indirectShadowMapSampler;

layout(rgba32f, binding = 0) readonly uniform image2D indirectLightPositions;
layout(rgba32f, binding = 1) readonly uniform image2D indirectLightNormals;

layout(std430, binding = 1) buffer samplingPattern
{
	vec4 samples[SAMPLENUMBER];
};

layout(location = 2) uniform uint reflectiveShadowMapResolution;

layout(location = 3) uniform uint reflectiveShadowMapIndex;

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
	
	for(uint i = startIndex; i < SHADOWSAMPLENUMBER; i += interleavedSize)
	{
		ivec2 indirectLightTexCoord = ivec2((samples[i].xy + vec2(0.5)) * reflectiveShadowMapResolution);
		vec4 lightPos = imageLoad(indirectLightPositions, indirectLightTexCoord);
		
		if(lightPos.w == 0.0) //cancels indirect lightsources which aren't initialized, because there is no geometry behind or lit
		{
			visibleIndirectLightNumber++;
			continue;
		}
		
		vec3 lightNormal = imageLoad(indirectLightNormals, indirectLightTexCoord).xyz * 2.0 - 1.0;

		float clipDepth;
		vec3 transformedPosition;
		paraboloidViewProjection(lightPos.xyz, lightNormal, pos3D.xyz, PARABOLOIDNEAR, PARABOLOIDFAR, transformedPosition, clipDepth);
		
		transformedPosition.xy = (transformedPosition.xy * 0.5 + 0.5) / float(SHADOWSAMPLENUMBERROOT);
		transformedPosition.z = transformedPosition.z * 0.5 + 0.5;
		
		transformedPosition.x += mod(i, SHADOWSAMPLENUMBERROOT) / float(SHADOWSAMPLENUMBERROOT);
		transformedPosition.y += (i / SHADOWSAMPLENUMBERROOT) / float(SHADOWSAMPLENUMBERROOT);

		float depth = texture(indirectShadowMapSampler, vec3(transformedPosition.xy, reflectiveShadowMapIndex)).r;

		//visibleIndirectLightNumber = depth;
		visibleIndirectLightNumber += clamp(ceil(depth - transformedPosition.z + 0.01), 0, 1);
	}
	
	//fsout_visibility = visibleIndirectLightNumber;
	fsout_visibility = visibleIndirectLightNumber / float(SHADOWSAMPLENUMBER / interleavedSize);
}