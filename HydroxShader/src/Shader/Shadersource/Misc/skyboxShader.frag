#version 440 core

layout(early_fragment_tests) in;

layout(location = 0) uniform sampler2DArray texSampler;

layout(location = 0) out vec4 fsout_color;
layout(location = 1) out vec3 fsout_normal;
layout(location = 2) out vec4 fsout_material;
layout(location = 3) out vec4 fsout_pos;

in vec3 vsout_texCoord;

void main()
{
	fsout_color = texture(texSampler, vsout_texCoord);
	fsout_normal = vec3(0);
	fsout_material = vec4(0);
	fsout_pos = vec4(55555.0f);
}