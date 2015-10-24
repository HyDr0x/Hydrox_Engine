#version 440 core

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

out flat vec3 gsout_pos3D;
out flat vec3 gsout_Xpg;
out flat vec3 gsout_phiPG;
out flat float gsout_cacheProxyMinDistanceG;

in vec3 vsout_pos3D[3];
in vec3 vsout_Xpg[3];
in vec3 vsout_phiPG[3];
in float vsout_cacheProxyMinDistanceG[3];

void main()
{
	for(uint i = 0; i < 3; i++)
	{
		gsout_pos3D = vsout_pos3D[i];
		gsout_Xpg = vsout_Xpg[i];
		gsout_phiPG = vsout_phiPG[i];
		gsout_cacheProxyMinDistanceG = vsout_cacheProxyMinDistanceG[i];
		gl_Position = gl_in[i].gl_Position;
		EmitVertex();
	}
	
	EndPrimitive();
}