#version 440 core

#include "../../../../include/Shader/MaterialData.glslh"

layout(location = 0) out vec4 fsout_color;
layout(location = 1) out vec4 fsout_normal;
layout(location = 2) out vec4 fsout_material;

in vec3 vsout_normal;
flat in uint vsout_instanceIndex;

void main()
{
	fsout_normal = vec4(vsout_normal * 0.5f + 0.5f, 0);
	fsout_color = vec4(1.0f, 0.0f, 0.0f, 1.0f);
	
	MaterialData thisMaterial = material[materialIndex[vsout_instanceIndex]];
	
	fsout_material = vec4(thisMaterial.diffuseStrength, 
												thisMaterial.specularStrength, 
												thisMaterial.ambientStrength, 
												thisMaterial.specularExponent);
}