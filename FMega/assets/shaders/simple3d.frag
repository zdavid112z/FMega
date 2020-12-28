#version 430 core

#include "common.glsl"
#include "light.glsl"

out vec4 outFragColor;

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

in VertexData
{
    vec3 vPosition;
	vec2 vUV;
	float vTexOpacity;
	vec3 vNormal;
	vec4 vColor;
} vData;

uniform sampler2D uTexture0;

vec3 CalcLights(vec3 pos, vec3 N, vec3 albedo, float metalness, float roughness, vec3 F0) {
    vec3 V = normalize(uEyePosition - pos);
	vec3 light = vec3(0.0);
	for (int i = 0; i < NUM_POINT_LIGHTS; i++) {
		light += CalcPointLight(pos, V, N, F0, albedo, metalness, roughness, uPointLights[i]);
	}
	for (int i = 0; i < NUM_SPOT_LIGHTS; i++) {
		light += CalcSpotLight(pos, V, N, F0, albedo, metalness, roughness, uSpotLights[i]);
	}
	return light;
}

void main()
{
	vec3 pos = vData.vPosition;
	vec3 normal = normalize(vData.vNormal);
	vec4 texColor = texture(uTexture0, vData.vUV);
	vec4 albedo = vec4(mix(vData.vColor.rgb, texColor.rgb, vData.vTexOpacity), vData.vColor.a * texColor.a);
	albedo = pow(albedo, vec4(2.2, 2.2, 2.2, 1.0));
	float metalness = 0.1;
	float roughness = 0.1;
	vec3 F0 = vec3(0.04); 
    F0 = mix(F0, albedo.rgb, metalness);

	float fogAmount = CalcFogAmount(vData.vPosition, FogLimits);
	float alpha = (1.0 - fogAmount) * albedo.a;
	if (alpha <= 0.01) {
		discard;
	}

	vec3 light = CalcLights(pos, normal, albedo.rgb, metalness, roughness, F0);

	vec3 ambient = vec3(0.1) * albedo.rgb;
	vec3 color = ambient + light;
	color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0/2.2));  
	
	outFragColor = vec4(color, alpha);
}
