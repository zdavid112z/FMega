#version 430 core

out vec4 outFragColor;

in VertexData
{
    vec3 vPosition;
	vec2 vUV;
	float vTexOpacity;
	vec3 vNormal;
	vec4 vColor;
} vData;

void main()
{
	float amount = dot(normalize(vData.vNormal), normalize(vec3(1, 1, 0)));
	amount = clamp(amount, 0, 1);
	amount = int(amount * 5.0) / 5.0;
	float fogAmount = smoothstep(-70, -85, vData.vPosition.z);
	vec4 diffuse = vData.vColor * (amount * 0.2 + 0.8);
	outFragColor = vec4(diffuse.xyz, 1.0 - fogAmount);
}