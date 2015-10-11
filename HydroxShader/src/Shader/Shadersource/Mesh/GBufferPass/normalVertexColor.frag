#version 440 core

layout(early_fragment_tests) in;

#include "../../../../../include/Shader/Shaderincludes/MaterialData.glslh"
#include "../../../../../include/Shader/Shaderincludes/CameraUBO.glslh"

layout(location = 0) out vec4 fsout_color;
layout(location = 1) out vec4 fsout_normal;
layout(location = 2) out vec4 fsout_material;

layout(location = 0) uniform sampler2D normalSampler;

in vec2 vsout_texCoord;
in mat3 vsout_tangentToWorld;
flat in uint vsout_instanceIndex;
in vec4 vsout_color;

void main()
{
	vec3 normal = vsout_tangentToWorld * (texture(normalSampler, vsout_texCoord).xyz * 2.0f - 1.0f);

	fsout_normal = vec4(normalize(normal) * 0.5f + 0.5f, 0);
	
	fsout_color = vec4(vsout_color.rgb, 1.0f);
	
	MaterialData thisMaterial = material[materialIndex[vsout_instanceIndex]];
	
	fsout_material = vec4(thisMaterial.diffuseRho, 
												thisMaterial.specularRho, 
												thisMaterial.ambientStrength, 
												thisMaterial.roughness);
}