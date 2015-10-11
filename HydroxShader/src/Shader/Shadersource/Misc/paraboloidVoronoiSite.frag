#version 440 core

layout(early_fragment_tests) in;

layout(rg32f, binding = 0) writeonly uniform imageBuffer siteCoordinates2D;

layout(location = 0) out uint fsout_vertexIndex;

in vec2 gsout_site2D;
in float gsout_clipDepth;
in flat uint gsout_vertexIndex;

void main()
{
	if(gsout_clipDepth < 0.0)
	{
		discard;
	}
	
	imageStore(siteCoordinates2D, int(gsout_vertexIndex), vec4(gsout_site2D, 0, 0));
	
	fsout_vertexIndex = gsout_vertexIndex + 1;//WHY PLUS ONE?!
}