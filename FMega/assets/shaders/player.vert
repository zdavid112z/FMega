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

layout(std140) uniform DynamicObjectBuffer
{
	mat4 uModel;
	float uPlayerHeight;
	float uPlayerAnimTime;
};

layout(std140) uniform StaticBuffer
{
	float uPlayerRadius;
};

out VertexData
{
    vec3 vNormal;
	vec4 vColor;
	vec3 vSphereCoord;
} vData;

void main()
{
	vec3 abc = vec3(1.0 / (uPlayerHeight * uPlayerHeight), uPlayerHeight, uPlayerHeight);
	vec4 position = vec4(aPosition.xyz * abc * uPlayerRadius, 1.0);

	vec3 rotatedPosition = (uModel * vec4(position.xyz, 0)).xyz;
	float theta = atan(rotatedPosition.z, rotatedPosition.x);
	float offset = sin(theta * 18.71345 + rotatedPosition.y * 14.1543171) * 0.4;
	float k = 0.3;
	float diff = rotatedPosition.y - uPlayerAnimTime * 2 + 1;
	float animAmount = max(0, -(diff - k) * (diff + k) / (k * k));
	float distortion = offset * animAmount * 4 * uPlayerAnimTime * (1 - uPlayerAnimTime);
	position = vec4(position.xyz * (1.0 + distortion), 1.0);

	vec4 globalPosition = uModel * position;
    gl_Position = uViewProjection * globalPosition;

	vec4 normal = vec4(aNormal.x * abc.y * abc.z, aNormal.y * abc.x * abc.z, aNormal.z * abc.x * abc.y, 0.0);
    vData.vNormal = (uModel * normal).xyz;
    vData.vColor = aColor;
	vData.vSphereCoord = aPosition.xyz;
}
