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
	float fogAmount = smoothstep(-70, -85, vData.vPosition.z);
	vec4 diffuse = vData.vColor;
	// outFragColor = vec4(diffuse.xyz, 1.0 - fogAmount);
	outFragColor = vec4(1, 0, 0, 0.5);
}
