#version 440 core

layout(early_fragment_tests) in;

#include "../../HydroxShader/include/Shader/Shaderincludes/MaterialData.glslh"
#include "../../HydroxShader/include/Shader/Shaderincludes/CameraUBO.glslh"

layout(location = 0) out vec4 fsout_color;
layout(location = 1) out vec4 fsout_normal;
layout(location = 2) out vec4 fsout_material;
layout(location = 3) out vec4 fsout_vertexNormal;

layout(location = 0) uniform sampler2D texSampler;
layout(location = 1) uniform sampler2D metalSampler;
layout(location = 2) uniform sampler2D roughnessSampler;

in GeometryData
{
	vec3 barycentric;
	vec2 texCoord;
	vec3 normal;
	flat uint instanceIndex;
} inData;

void main()
{
	//float(inData.barycentric.x == 1.0 || inData.barycentric.y == 1.0 || inData.barycentric.z == 1.0)
	fsout_normal = vec4(normalize(inData.normal) * 0.5 + 0.5, float(inData.barycentric.x > 0.95 || inData.barycentric.y > 0.95 || inData.barycentric.z > 0.9));
	fsout_color = vec4(texture(texSampler, inData.texCoord).rgb, 0.0);
	
	MaterialData thisMaterial = material[materialIndex[inData.instanceIndex]];
	
	fsout_material = vec4(texture(metalSampler, inData.texCoord).r, 
												thisMaterial.reflectance, 
												texture(roughnessSampler, inData.texCoord).r, 
												thisMaterial.roughness1);
												
	fsout_vertexNormal = vec4(normalize(inData.normal) * 0.5 + 0.5, 0.0);
}