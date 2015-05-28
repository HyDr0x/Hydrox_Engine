#version 440 core

layout(early_fragment_tests) in;

#include "../../../../../include/Shader/Shaderincludes/MaterialData.glslh"

layout(location = 0) out vec4 fsout_color;
layout(location = 1) out vec4 fsout_normal;
layout(location = 2) out vec4 fsout_material;

layout(location = 0) uniform sampler2D texSampler;

in vec2 vsout_texCoord;
in vec3 vsout_normal;
flat in uint vsout_instanceIndex;

void main()
{
	fsout_normal = vec4(normalize(vsout_normal) * 0.5f + 0.5f, 0);
	fsout_color = vec4(texture(texSampler, vsout_texCoord).rgb, 1.0f);
	
	MaterialData thisMaterial = material[materialIndex[vsout_instanceIndex]];
	
	fsout_material = vec4(thisMaterial.diffuseRho, 
												thisMaterial.specularRho, 
												thisMaterial.ambientStrength, 
												thisMaterial.roughness);
}