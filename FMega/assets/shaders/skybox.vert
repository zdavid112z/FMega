#version 430 core

#include "common.glsl"
#include "light.glsl"

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
	vec4 vPosition;
	vec3 vEyePosition;
	float vCameraOffset;
} vData;

void main()
{
	vec4 pos = vec4(aPosition.xy, 0.999, 1.0);
    gl_Position = pos;
	vData.vPosition = uInvViewProjection * pos;
	vData.vEyePosition = uEyePosition;
	vData.vCameraOffset = uCameraOffset;
}
