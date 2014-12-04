#version 440 core

layout(early_fragment_tests) in;

#include "../Header/MaterialData.glslh"

layout(location = 0) out vec4 fsout_color;
layout(location = 1) out vec4 fsout_normal;
layout(location = 2) out vec4 fsout_material;

in vec3 vsout_normal;
in vec4 vsout_color;
flat in uint vsout_instanceIndex;

void main()
{
	fsout_normal = vec4(vsout_normal * 0.5f + 0.5f, 0);
	fsout_color = vsout_color;
	
	MaterialData thisMaterial = material[materialIndex[vsout_instanceIndex]];
	
	fsout_material = vec4(thisMaterial.diffuseStrength, 
												thisMaterial.specularStrength, 
												thisMaterial.ambientStrength, 
												thisMaterial.specularExponent);
}