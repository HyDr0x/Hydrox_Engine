#version 440 core

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in VertexData
{
	vec3 normal;
	flat uint instanceIndex;
} inData[];

out GeometryData
{
	vec3 barycentric;
	vec3 normal;
	flat uint instanceIndex;
} outData;

void main()
{
	for(uint i = 0; i < 3; i++)
	{
		outData.barycentric = vec3(float(i == 0), float(i == 1), float(i == 2));
		outData.normal = inData[i].normal;
		outData.instanceIndex = inData[i].instanceIndex;
		gl_Position = vec4(gl_in[i].gl_Position);
		EmitVertex();
	}
	
	EndPrimitive();
}