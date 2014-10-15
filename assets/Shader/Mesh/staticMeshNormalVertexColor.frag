#version 440 core

#include "../Header/MaterialData.glslh"

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
	vec3 normal = vsout_tangentToWorld * normalize(texture(normalSampler, vsout_texCoord).xyz * 2.0f - 1.0f);

	fsout_normal = vec4(normal * 0.5f + 0.5f, 0);
	
	fsout_color = vsout_color;
	
	MaterialData thisMaterial = material[materialIndex[vsout_instanceIndex]];
	
	fsout_material = vec4(thisMaterial.diffuseStrength, 
												thisMaterial.specularStrength, 
												thisMaterial.ambientStrength, 
												thisMaterial.specularExponent);
}