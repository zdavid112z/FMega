#version 430 core

layout (location = 0) in vec4 aPosition;
layout (location = 1) in vec4 aColor;
layout (location = 2) in vec4 aNormal;
layout (location = 3) in mat4 aModel;
layout (location = 7) in vec4 aObjectColor;
layout (location = 8) in vec4 aOpacity;

layout(std140) uniform DynamicSceneBuffer
{
	mat4 uViewProjection;
	mat4 uInvViewProjection;
	vec3 uEyePosition;
	float uTargetZ;
	float uCameraOffset;
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
