#version 440 core

#include "../../../../../include/Shader/Shaderincludes/VertexDeclaration.glslh"
#include "../../../../../include/Shader/Shaderincludes/LightData.glslh"
#include "../../../../../include/Shader/Shaderincludes/ParaboloidProjection.glslh"

#define SHADOWSAMPLENUMBER
#define SHADOWSAMPLENUMBERROOT
#define ADAPTIVESAMPLENUMBER

layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

layout(rgba32f, binding = 0) readonly uniform image2D indirectLightPositions;
layout(rgba32f, binding = 1) readonly uniform image2D indirectLightNormals;
coherent layout(rgba32f, binding = 2) uniform image1D valQuaternions;

layout(std430, binding = 1) buffer samplingPattern
{
	vec4 samples[];
};

layout(location = 0) uniform uint reflectiveShadowMapResolution;

out float gsout_clipDepth;

in vec3 vsout_pos3D[1];
in vec3 vsout_cacheNormal[1];
in uint vsout_instanceID[1];
in uint vsout_vertexID[1];

void main()
{
	ivec2 texCoord = ivec2((samples[vsout_instanceID[0]].xy + vec2(0.5)) * reflectiveShadowMapResolution);
	vec4 lightPos = imageLoad(indirectLightPositions, texCoord);
	vec3 lightNormal = imageLoad(indirectLightNormals, texCoord).xyz * 2.0 - 1.0;

	vec3 x = normalize(cross(vsout_cacheNormal[0], vsout_cacheNormal[0] + vec3(0, 0.1, 0)));
	vec3 z = normalize(cross(vsout_cacheNormal[0], x));
	
	vec3 points[4];
	
	const float size = 0.1;

	points[0] = vsout_pos3D[0] - size * x + size * z;
	points[1] = vsout_pos3D[0] - size * x - size * z;
	points[2] = vsout_pos3D[0] + size * x + size * z;
	points[3] = vsout_pos3D[0] + size * x - size * z;

	//if(vsout_vertexID[0] == 0)
	{
		float lightRotationAngle = 0.0;
		vec3 rotationAxis = vec3(0, 1, 0);
		
		if(abs(lightNormal.z) < 0.99)
		{
			lightRotationAngle = acos(lightNormal.z);
			rotationAxis = normalize(vec3(lightNormal.y, -lightNormal.x, 0.0));//normalize(cross(lightNormal, vec3(0, 0, 1)));
		}
		else if(lightNormal.z > 0.99)
		{
			lightRotationAngle = PI;
			rotationAxis = normalize(vec3(0, 1, 0));//normalize(cross(lightNormal, vec3(0, 0, 1)));
		}
		
		imageStore(valQuaternions, int(vsout_instanceID[0]), createQuaternion(lightRotationAngle, rotationAxis));
	}
	
	memoryBarrier();

	vec4 q = imageLoad(valQuaternions, int(vsout_instanceID[0]));

	vec3 positions[4];
	float clipDepths[4];
	
	//vec2 texOffset = vec2(mod(float(vsout_instanceID[0]), SHADOWSAMPLENUMBERROOT) * 2.0 / float(SHADOWSAMPLENUMBERROOT), (float(vsout_instanceID[0]) / SHADOWSAMPLENUMBERROOT) * 2.0 / float(SHADOWSAMPLENUMBERROOT));
	
	for(uint i = 0; i < 4; i++)
	{
		vec3 transformedCachePosition = points[i] - lightPos.xyz;
		transformedCachePosition = applyQuaternion(transformedCachePosition, q);
		
		//paraboloidViewProjection(lightPos.xyz, lightNormal, points[i], PARABOLOIDNEAR, PARABOLOIDFAR, transformedCachePosition, clipDepths[i]);
		paraboloidProjection(transformedCachePosition, PARABOLOIDNEAR, PARABOLOIDFAR, transformedCachePosition, clipDepths[i]);
		
		if(1.0 < transformedCachePosition.x || transformedCachePosition.x < -1.0 || 
			 1.0 < transformedCachePosition.y || transformedCachePosition.y < -1.0 ||
			 1.0 < transformedCachePosition.z || transformedCachePosition.z < -1.0) return;
		
		transformedCachePosition.xy = (transformedCachePosition.xy - float(SHADOWSAMPLENUMBERROOT) + 1.0) / float(SHADOWSAMPLENUMBERROOT);

		//transformedCachePosition.xy += texOffset;
		transformedCachePosition.x += mod(float(vsout_instanceID[0]), SHADOWSAMPLENUMBERROOT) * 2.0 / float(SHADOWSAMPLENUMBERROOT);
		transformedCachePosition.y += (float(vsout_instanceID[0]) / SHADOWSAMPLENUMBERROOT) * 2.0 / float(SHADOWSAMPLENUMBERROOT) - (mod(float(vsout_instanceID[0]), SHADOWSAMPLENUMBERROOT) * 2) / SHADOWSAMPLENUMBER;

		positions[i] = transformedCachePosition;
	}
	
	for(uint i = 0; i < 4; i++)
	{
		gsout_clipDepth = clipDepths[i];
		gl_Position = vec4(positions[i], 1.0);
		EmitVertex();
	}
	EndPrimitive();
}