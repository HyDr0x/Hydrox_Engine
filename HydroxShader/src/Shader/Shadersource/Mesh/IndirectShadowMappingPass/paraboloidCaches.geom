#version 440 core

#include "../../HydroxShader/include/Shader/Shaderincludes/VertexDeclaration.glslh"
#include "../../HydroxShader/include/Shader/Shaderincludes/LightData.glslh"
#include "../../HydroxShader/include/Shader/Shaderincludes/ParaboloidProjection.glslh"

layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

layout(rgba32f, binding = 0) readonly uniform image2D indirectLightPositions;
layout(rgba32f, binding = 1) readonly uniform image2D indirectLightNormals;
coherent layout(rgba32f, binding = 2) uniform image1D valQuaternions;

layout(location = 0) uniform uint reflectiveShadowMapResolution;
layout(location = 1) uniform uint indirectShadowLightRatio;
layout(location = 2) uniform ivec2 reflectiveShadowMapTexOffset;
layout(location = 3) uniform uint shadowMapsPerRow;
layout(location = 4) uniform uint shadowMapsPerTile;
layout(location = 5) uniform uint shadowMapsPerTileRow;

out float gsout_clipDepth;

in vec3 vsout_pos3D[1];
in vec3 vsout_cacheNormal[1];
in uint vsout_instanceID[1];
in uint vsout_vertexID[1];

void main()
{
	int reflectiveShadowMapSampleDistance = int(reflectiveShadowMapResolution / shadowMapsPerTileRow);
	ivec2 texCoord = reflectiveShadowMapTexOffset + reflectiveShadowMapSampleDistance * ivec2(mod(vsout_instanceID[0], shadowMapsPerTileRow), vsout_instanceID[0] / shadowMapsPerTileRow);
	
	vec4 lightPos = imageLoad(indirectLightPositions, texCoord);
	vec3 lightNormal = imageLoad(indirectLightNormals, texCoord).xyz * 2.0 - 1.0;

	vec3 x = normalize(cross(vsout_cacheNormal[0], vsout_cacheNormal[0] + vsout_cacheNormal[0].yxz));
	vec3 z = normalize(cross(vsout_cacheNormal[0], x));
	
	vec3 points[4];
	
	const float size = 0.05 * distance(vsout_pos3D[0], lightPos.xyz);

	points[0] = vsout_pos3D[0] - size * x + size * z;
	points[1] = vsout_pos3D[0] - size * x - size * z;
	points[2] = vsout_pos3D[0] + size * x + size * z;
	points[3] = vsout_pos3D[0] + size * x - size * z;

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
	
	vec4 q = createQuaternion(lightRotationAngle, rotationAxis);
	
	if(vsout_vertexID[0] == 0)//save quaternion only once
	{
		ivec2 quaternionTexCoord = texCoord / reflectiveShadowMapSampleDistance;
		imageStore(valQuaternions, quaternionTexCoord.x + quaternionTexCoord.y * int(shadowMapsPerRow), q);
	}
	
	memoryBarrier();

	vec3 positions[4];
	float clipDepths[4];
	
	for(uint i = 0; i < 4; i++)
	{
		vec3 transformedCachePosition = points[i] - lightPos.xyz;
		transformedCachePosition = applyQuaternion(transformedCachePosition, q);
		
		paraboloidProjection(transformedCachePosition, PARABOLOIDNEAR, PARABOLOIDFAR, transformedCachePosition, clipDepths[i]);
		
		if(1.0 < transformedCachePosition.x || transformedCachePosition.x < -1.0 || 
			 1.0 < transformedCachePosition.y || transformedCachePosition.y < -1.0 ||
			 1.0 < transformedCachePosition.z || transformedCachePosition.z < -1.0) return;
		
		float shadowMapSize =  1.0 / float(shadowMapsPerTileRow);
		transformedCachePosition.xy *= shadowMapSize;
		transformedCachePosition.xy += (-1.0 + shadowMapSize);//shift it into the lower left corner
		transformedCachePosition.xy += 2.0 * vec2(mod(vsout_instanceID[0], shadowMapsPerTileRow), vsout_instanceID[0] / shadowMapsPerTileRow) / float(shadowMapsPerTileRow);
		
		positions[i] = vec3(transformedCachePosition.xy, transformedCachePosition.z);
	}
	
	for(uint i = 0; i < 4; i++)
	{
		gsout_clipDepth = clipDepths[i];
		gl_Position = vec4(positions[i], 1.0);
		EmitVertex();
	}
	EndPrimitive();
}