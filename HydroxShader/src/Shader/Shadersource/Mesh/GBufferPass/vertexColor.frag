#version 440 core

layout(early_fragment_tests) in;

#include "../../HydroxShader/include/Shader/Shaderincludes/MaterialData.glslh"
#include "../../HydroxShader/include/Shader/Shaderincludes/CameraUBO.glslh"

layout(location = 0) out vec4 fsout_color;
layout(location = 1) out vec4 fsout_normal;
layout(location = 2) out vec4 fsout_material;

in GeometryData
{
	vec3 barycentric;
	vec3 normal;
	flat uint instanceIndex;
	vec4 color;
} inData;

void main()
{
	fsout_normal = vec4(normalize(inData.normal) * 0.5f + 0.5f, float(inData.barycentric.x == 1.0 || inData.barycentric.y == 1.0 || inData.barycentric.z == 1.0));
	fsout_color = vec4(inData.color.rgb, 1.0f);
	
	MaterialData thisMaterial = material[materialIndex[inData.instanceIndex]];
	
	fsout_material = vec4(thisMaterial.metalness, 
												thisMaterial.reflectance, 
												thisMaterial.roughness0, 
												thisMaterial.roughness1);
}