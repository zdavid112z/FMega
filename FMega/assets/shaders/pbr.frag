#ifndef FMEGA_PBR_GLSL
#define FMEGA_PBR_GLSL

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
	vec3 vUV;
	float vTexOpacity;
	mat3 vTBN;
	vec4 vColor;
	float vRoughness;
	float vRoughnessMapStrength;
	float vMetalness;
	float vMetalMapStrength;
	vec3 vInstNormal;
	float vNormalMapStrength;
} vData;

uniform sampler2D uAlbedoMap;
uniform sampler2D uNormalMap;
uniform sampler2D uRoughnessMap;
uniform sampler2D uMetalnessMap;

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

vec2 GetUV(vec3 uv);

float near = 0.1; 
float far  = 100.0; 
  
float LinearizeDepth(float depth) 
{
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * near * far) / (far + near - z * (far - near));	
}

void main()
{
	vec3 pos = vData.vPosition;
	vec2 uv = GetUV(vData.vUV);

	vec3 normalmapValue = mix(vData.vInstNormal, (texture(uNormalMap, uv) * 2.0 - 1.0).rgb, vData.vNormalMapStrength);
	float metalness = mix(vData.vMetalness, texture(uMetalnessMap, uv).r, vData.vMetalMapStrength);
	float roughness = mix(vData.vRoughness, texture(uRoughnessMap, uv).r, vData.vRoughnessMapStrength);

	vec3 normal = normalize(vData.vTBN * normalmapValue);

	vec4 texColor = pow(texture(uAlbedoMap, uv), vec4(2.2, 2.2, 2.2, 1.0));
	vec4 albedo = vec4(mix(vData.vColor, texColor, vData.vTexOpacity));
	albedo = pow(albedo, vec4(2.2, 2.2, 2.2, 1.0));
	vec3 F0 = vec3(0.04); 
    F0 = mix(F0, albedo.rgb, metalness);

	float fogAmount = CalcFogAmount(vData.vPosition, FogLimits);
	float alpha = (1.0 - fogAmount) * albedo.a;
	if (alpha <= 0.01) {
		//discard;
	}

	// vec3 light = CalcLights(pos, normal, albedo.rgb, metalness, roughness, F0);
	vec3 V = normalize(uEyePosition - pos);
	vec3 light = 
		CalcPointLight(pos, V, normal, F0, albedo.rgb, metalness, roughness, uPointLights[0]) + 
		CalcPointLight(pos, V, normal, F0, albedo.rgb, metalness, roughness, uPointLights[1]) + 
		CalcPointLight(pos, V, normal, F0, albedo.rgb, metalness, roughness, uPointLights[2]) + 
		CalcPointLight(pos, V, normal, F0, albedo.rgb, metalness, roughness, uPointLights[3]) +
		CalcPointLight(pos, V, normal, F0, albedo.rgb, metalness, roughness, uPointLights[4]) + 
		CalcPointLight(pos, V, normal, F0, albedo.rgb, metalness, roughness, uPointLights[5]) + 
		CalcPointLight(pos, V, normal, F0, albedo.rgb, metalness, roughness, uPointLights[6]) + 
		CalcPointLight(pos, V, normal, F0, albedo.rgb, metalness, roughness, uPointLights[7]) +
		CalcPointLight(pos, V, normal, F0, albedo.rgb, metalness, roughness, uPointLights[8]) + 
		CalcPointLight(pos, V, normal, F0, albedo.rgb, metalness, roughness, uPointLights[9]) + 
		CalcPointLight(pos, V, normal, F0, albedo.rgb, metalness, roughness, uPointLights[10]) + 
		CalcPointLight(pos, V, normal, F0, albedo.rgb, metalness, roughness, uPointLights[11]) +
		CalcPointLight(pos, V, normal, F0, albedo.rgb, metalness, roughness, uPointLights[12]) + 
		CalcPointLight(pos, V, normal, F0, albedo.rgb, metalness, roughness, uPointLights[13]) + 
		CalcPointLight(pos, V, normal, F0, albedo.rgb, metalness, roughness, uPointLights[14]) + 
		CalcPointLight(pos, V, normal, F0, albedo.rgb, metalness, roughness, uPointLights[15]) +
		CalcPointLight(pos, V, normal, F0, albedo.rgb, metalness, roughness, uPointLights[16]) + 
		CalcPointLight(pos, V, normal, F0, albedo.rgb, metalness, roughness, uPointLights[17]) + 
		CalcPointLight(pos, V, normal, F0, albedo.rgb, metalness, roughness, uPointLights[18]) + 
		CalcPointLight(pos, V, normal, F0, albedo.rgb, metalness, roughness, uPointLights[19]) +
		CalcPointLight(pos, V, normal, F0, albedo.rgb, metalness, roughness, uPointLights[20]) + 
		CalcPointLight(pos, V, normal, F0, albedo.rgb, metalness, roughness, uPointLights[21]) +
		CalcPointLight(pos, V, normal, F0, albedo.rgb, metalness, roughness, uPointLights[22]) + 
		CalcPointLight(pos, V, normal, F0, albedo.rgb, metalness, roughness, uPointLights[23]) + 
		CalcSpotLight(pos, V, normal, F0, albedo.rgb, metalness, roughness, uSpotLights[0]) +
		CalcSpotLight(pos, V, normal, F0, albedo.rgb, metalness, roughness, uSpotLights[1]) +
		CalcSpotLight(pos, V, normal, F0, albedo.rgb, metalness, roughness, uSpotLights[2]) +
		CalcSpotLight(pos, V, normal, F0, albedo.rgb, metalness, roughness, uSpotLights[3]) + 
		CalcSpotLight(pos, V, normal, F0, albedo.rgb, metalness, roughness, uSpotLights[4]) +
		CalcSpotLight(pos, V, normal, F0, albedo.rgb, metalness, roughness, uSpotLights[5]) +
		CalcSpotLight(pos, V, normal, F0, albedo.rgb, metalness, roughness, uSpotLights[6]) +
		CalcSpotLight(pos, V, normal, F0, albedo.rgb, metalness, roughness, uSpotLights[7]) +
		CalcSpotLight(pos, V, normal, F0, albedo.rgb, metalness, roughness, uSpotLights[8]) +
		CalcSpotLight(pos, V, normal, F0, albedo.rgb, metalness, roughness, uSpotLights[9]) +
		CalcSpotLight(pos, V, normal, F0, albedo.rgb, metalness, roughness, uSpotLights[10]) +
		CalcSpotLight(pos, V, normal, F0, albedo.rgb, metalness, roughness, uSpotLights[11]) +
		CalcSpotLight(pos, V, normal, F0, albedo.rgb, metalness, roughness, uSpotLights[12]) +
		CalcSpotLight(pos, V, normal, F0, albedo.rgb, metalness, roughness, uSpotLights[13]) +
		CalcSpotLight(pos, V, normal, F0, albedo.rgb, metalness, roughness, uSpotLights[14]) +
		CalcSpotLight(pos, V, normal, F0, albedo.rgb, metalness, roughness, uSpotLights[15]) +
		CalcSpotLight(pos, V, normal, F0, albedo.rgb, metalness, roughness, uSpotLights[16]) +
		CalcSpotLight(pos, V, normal, F0, albedo.rgb, metalness, roughness, uSpotLights[17]) +
		CalcSpotLight(pos, V, normal, F0, albedo.rgb, metalness, roughness, uSpotLights[18]) +
		CalcSpotLight(pos, V, normal, F0, albedo.rgb, metalness, roughness, uSpotLights[19]) +
		CalcSpotLight(pos, V, normal, F0, albedo.rgb, metalness, roughness, uSpotLights[20]) +
		CalcSpotLight(pos, V, normal, F0, albedo.rgb, metalness, roughness, uSpotLights[21]) +
		CalcSpotLight(pos, V, normal, F0, albedo.rgb, metalness, roughness, uSpotLights[22]) +
		CalcSpotLight(pos, V, normal, F0, albedo.rgb, metalness, roughness, uSpotLights[23]);
	
	vec3 ambient = vec3(0.1) * albedo.rgb;
	vec3 color = ambient + light;
	color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0/2.2));
	
	outFragColor = vec4(color, alpha);
	//float depth = LinearizeDepth(gl_FragCoord.z) / far;
	//outFragColor = vec4(vec3(fogAmount), 1.0);
	//outFragColor = vec4(0.4,0.3,0.2,1.0);
}

#endif
