#version 430 core

out vec4 outFragColor;

in VertexData
{
	vec4 vColor;
} vData;

void main()
{
	outFragColor = vData.vColor;
}