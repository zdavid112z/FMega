#version 430 core

layout (location = 0) in vec4 aPosition;
layout (location = 1) in vec4 aColor;
layout (location = 2) in vec4 aNormal;
layout (location = 3) in mat4 aModel;
layout (location = 7) in vec4 aObjectColor;
layout (location = 8) in vec4 aOpacity;

layout(std140) uniform UIBuffer
{
	mat4 uViewProjection;
	mat4 uInvViewProjection;
	vec3 uEyePosition;
	float uTargetZ;
	float uCameraOffset;
};

out VertexData
{
	vec4 vColor;
} vData;

void main()
{
    gl_Position = uViewProjection * aModel * vec4(aPosition.xyz, 1.0);	
	vec4 color = vec4(mix(aColor.rgb, aObjectColor.rgb, aObjectColor.a), aColor.a * aOpacity.r);
	// vec4 color = vec4(aObjectColor.rgb, 1.0);
	// vec4 color = aOpacity;
	vData.vColor = color;
}
