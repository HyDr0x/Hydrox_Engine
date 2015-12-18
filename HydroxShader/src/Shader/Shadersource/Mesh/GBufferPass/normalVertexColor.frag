#version 440 core

layout(early_fragment_tests) in;

#include "../../HydroxShader/include/Shader/Shaderincludes/MaterialData.glslh"
#include "../../HydroxShader/include/Shader/Shaderincludes/CameraUBO.glslh"

layout(location = 0) out vec4 fsout_color;
layout(location = 1) out vec4 fsout_normal;
layout(location = 2) out vec4 fsout_material;
layout(location = 3) out vec4 fsout_vertexNormal;

layout(location = 0) uniform sampler2D normalSampler;
layout(location = 1) uniform sampler2D metalSampler;
layout(location = 2) uniform sampler2D roughnessSampler;

in GeometryData
{
	vec2 texCoord;
	mat3 tangentToWorld;
	flat uint instanceIndex;
	vec4 color;
} inData;

void main()
{
	vec3 normal = inData.tangentToWorld * (texture(normalSampler, inData.texCoord).xyz * 2.0 - 1.0);

	fsout_normal = vec4(normalize(normal) * 0.5 + 0.5, 0.0);
	
	fsout_color = vec4(inData.color.rgb, 1.0);
	
	MaterialData thisMaterial = material[materialIndex[inData.instanceIndex]];
	
	fsout_material = vec4(texture(metalSampler, inData.texCoord).r, 
												thisMaterial.reflectance, 
												texture(roughnessSampler, inData.texCoord).r, 
												thisMaterial.roughness1);
												
	fsout_vertexNormal =  vec4(normalize(inData.tangentToWorld[2]) * 0.5 + 0.5, 0.0);
}