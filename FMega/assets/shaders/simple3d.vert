#version 430 core

#include "common.glsl"
#include "light.glsl"

layout (location = 0) in vec4 aPosition;
layout (location = 1) in vec4 aUV;
layout (location = 2) in vec4 aColor;
layout (location = 3) in vec4 aNormal;
layout (location = 4) in mat4 aModel;
layout (location = 8) in vec4 aObjectColor;
layout (location = 9) in vec4 aOpacity;

layout(std140) uniform DynamicSceneBuffer
{
	mat4 uViewProjection;
	mat4 uInvViewProjection;
	vec3 uEyePosition;
	float uTargetZ;
	float uCameraOffset;
	float uUnused1;
	float uUnused2;
	float uUnused3;
	PointLight uPointLights[NUM_POINT_LIGHTS];
	SpotLight uSpotLights[NUM_SPOT_LIGHTS];
};

out VertexData
{
    vec3 vPosition;
	vec2 vUV;
	float vTexOpacity;
	vec3 vNormal;
	vec4 vColor;
} vData;

void main()
{
	vec4 worldPos = aModel * vec4(aPosition.xyz, 1.0);
	vec4 worldNormal = aModel * vec4(aNormal.xyz, 0.0);
	vec4 color = vec4(mix(aColor.rgb, aObjectColor.rgb, aObjectColor.a), aColor.a * aOpacity.x);
	gl_Position = uViewProjection * worldPos;
	vData.vPosition = worldPos.xyz;
	vData.vUV = aUV.xy;
	vData.vNormal = worldNormal.xyz;
	vData.vColor = color;
	vData.vTexOpacity = aOpacity.y;
}
