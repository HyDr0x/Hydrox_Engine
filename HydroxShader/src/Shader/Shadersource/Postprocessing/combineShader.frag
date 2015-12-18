#version 440 core

layout(location = 0) uniform sampler2D colorSampler;
layout(location = 1) uniform sampler2D lightSampler;
layout(location = 2) uniform sampler2D indirectLightSampler;

layout(location = 0) out vec4 color;

in vec2 gsout_texCoord;

void main()
{
	vec4 albedo = texture(colorSampler, gsout_texCoord);
	vec4 light = texture(lightSampler, gsout_texCoord);
	vec4 indirectLight = texture(indirectLightSampler, gsout_texCoord);
	
	//vec4 gammaCorrectColor = ((indirectLight + light) * albedo) / 250.0;
	//color = vec4(RGBtoHSV(pow(gammaCorrectColor.rgb, vec3(1.0f / 1.73f))), 1.0);

	color = vec4(mix(albedo.rgb, light.rgb + (indirectLight.rgb * albedo.rgb), albedo.a), 1.0);
	//color = vec4(((indirectLight.rgb + light.rgb) * albedo.rgb * albedo.a + (1.0 - albedo.a) * albedo.rgb).rgb, 1.0);
	
	/*vec3 normalColor = ((indirectLight + light) * albedo).rgb;
	float Y = dot(vec3(0.299, 0.587, 0.114), normalColor);
	float U = (normalColor.b - Y) * 0.493f;
	float V = (normalColor.r - Y) * 0.877;
	color = vec4(Y, U, V, 1.0);
	luminance = vec4(Y, 0, 0, 0);*/
}