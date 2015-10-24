#version 440 core

layout(triangles) in;
layout(points, max_vertices = 3) out;

in VertexData
{
	vec3 pos3D;
	vec3 normal;
} inData[];

out GeometryData
{
	vec3 pos3D;
	vec3 normal;
} outData;

void main()
{
	for(uint i = 0; i < 3; i++)
	{
		outData.pos3D = inData[i].pos3D;
		outData.normal = inData[i].normal;
		gl_Position = vec4(gl_in[i].gl_Position);
		EmitVertex();
		EndPrimitive();
	}
}