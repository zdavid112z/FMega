#version 430 core

layout (location = 0) in vec4 aPosition;
layout (location = 1) in vec4 aUV;
layout (location = 2) in vec4 aColor;
layout (location = 3) in vec4 aNormal;
layout (location = 4) in vec4 aTangent;
layout (location = 5) in mat4 aModel;
layout (location = 9) in vec4 aObjectColor;
layout (location = 10) in vec4 aInstNormal;
layout (location = 11) in vec4 aOpacity;
layout (location = 12) in vec4 aMetalness;

layout(std140) uniform UIBuffer
{
	mat4 uViewProjection;
	mat4 uViewProjection3d;
};

out VertexData
{
	vec4 vColor;
	vec2 vUV;
	float vTextureStrength;
	float vOpacity;
	flat int vTexture;
} vData;

void main()
{
	mat4 vp = uViewProjection3d * aOpacity.a + uViewProjection * (1.0 - aOpacity.a);
    gl_Position = vp * aModel * vec4(aPosition.xyz, 1.0);
	vec4 color = vec4(mix(aColor.rgb, aObjectColor.rgb, aObjectColor.a), aColor.a);
	vData.vColor = color;
	vData.vUV = aUV.xy;
	vData.vOpacity = aOpacity.r;
	vData.vTextureStrength = aOpacity.g;
	vData.vTexture = int(aOpacity.b + 0.5);
}
