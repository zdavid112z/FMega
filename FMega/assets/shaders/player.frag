#version 430 core

out vec4 outFragColor;

in VertexData
{
    vec3 vNormal;
	vec4 vColor;
	vec3 vSphereCoord;
} vData;

void main()
{
	vec4 plane = vec4(1, 1, 0, 0);
	float amount = dot(normalize(vData.vNormal), normalize(vec3(1, 1, 0)));
	amount = clamp(amount, 0, 1);
	amount = int(amount * 3.0) / 3.0;
	float d = dot(vec4(vData.vSphereCoord, 1.0), plane);
	float distanceSqToBand = d * d / dot(plane, plane);
	vec4 color = vData.vColor;
	if (distanceSqToBand < 0.1)
		color = vec4(0, 1, 1, 1);
	outFragColor = vec4(color.rgb * (amount * 0.2 + 0.8), color.a);
}