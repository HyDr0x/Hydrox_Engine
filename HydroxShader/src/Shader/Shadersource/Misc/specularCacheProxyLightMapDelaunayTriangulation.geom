#version 440 core

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

out flat vec3 gsout_normal0;
out flat vec3 gsout_normal1;
out flat vec3 gsout_normal2;
out vec3 gsout_Xpd;
out vec3 gsout_phiPD;
out vec3 gsout_Xpg;
out vec3 gsout_phiPG;
out float gsout_cacheProxyMinDistanceD;
out float gsout_cacheProxyMinDistanceG;
out flat vec3 gsout_depth;
out flat uvec4 gsout_cacheIndices[6];

in float vsout_depth[3];
in vec3 vsout_normal[3];
in vec3 vsout_Xpd[3];
in vec3 vsout_phiPD[3];
in vec3 vsout_Xpg[3];
in vec3 vsout_phiPG[3];
in float vsout_cacheProxyMinDistanceD[3];
in float vsout_cacheProxyMinDistanceG[3];
in vec4 vsout_cacheIndices0[3];
in vec4 vsout_cacheIndices1[3];

void main()
{
	uvec4 cacheIndices[6];
	uvec4 bitMask;
	
	for(uint i = 0; i < 3; i++)
	{
		cacheIndices[2 * i + 0] = uvec4(vsout_cacheIndices0[i]) + uvec4(1);
		cacheIndices[2 * i + 1] = uvec4(vsout_cacheIndices1[i]) + uvec4(1);
	}
	
	for(uint i = 0; i < 6; i++)
	{
		bitMask = uvec4(1);
		uint index = (2 + (i / 2) * 2) % 6;
		bitMask &= uvec4(bvec4(cacheIndices[i] - cacheIndices[index].x));
		bitMask &= uvec4(bvec4(cacheIndices[i] - cacheIndices[index].y));
		bitMask &= uvec4(bvec4(cacheIndices[i] - cacheIndices[index].z));
		bitMask &= uvec4(bvec4(cacheIndices[i] - cacheIndices[index].w));
		
		index++;
		bitMask &= uvec4(bvec4(cacheIndices[i] - cacheIndices[index].x));
		bitMask &= uvec4(bvec4(cacheIndices[i] - cacheIndices[index].y));
		bitMask &= uvec4(bvec4(cacheIndices[i] - cacheIndices[index].z));
		bitMask &= uvec4(bvec4(cacheIndices[i] - cacheIndices[index].w));
		cacheIndices[i] *= bitMask;
	}
	
	cacheIndices[0] -= uvec4(1);
	cacheIndices[1] -= uvec4(1);
	cacheIndices[2] -= uvec4(1);
	cacheIndices[3] -= uvec4(1);
	cacheIndices[4] -= uvec4(1);
	cacheIndices[5] -= uvec4(1);

	for(uint i = 0; i < 3; i++)
	{
		for(uint j = 0; j < 6; j++)
		{
			gsout_cacheIndices[j]	= cacheIndices[j];
		}

		gsout_normal0 = vsout_normal[0];
		gsout_normal1 = vsout_normal[1];
		gsout_normal2 = vsout_normal[2];
		gsout_Xpd = vsout_Xpd[i];
		gsout_Xpg = vsout_Xpg[i];
		gsout_phiPD = vsout_phiPD[i];
		gsout_phiPG = vsout_phiPG[i];
		gsout_cacheProxyMinDistanceD = vsout_cacheProxyMinDistanceD[i];
		gsout_cacheProxyMinDistanceG = vsout_cacheProxyMinDistanceG[i];
		gsout_depth = vec3(vsout_depth[0], vsout_depth[1], vsout_depth[2]);
		gl_Position = gl_in[i].gl_Position;
		EmitVertex();
	}
	
	EndPrimitive();
}