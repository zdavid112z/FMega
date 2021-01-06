#ifndef FMEGA_LIGHT_GLSL
#define FMEGA_LIGHT_GLSL

const float PI = 3.14159265359;

struct PointLight {
	vec4 position;
	vec4 color;
	vec4 atten;
};

struct SpotLight {
	vec4 position;
	vec4 color;
	vec4 atten;
	vec4 direction;
	float innerCutoff;
    float outerCutoff;
    float unused1;
    float unused2;
};

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(max(1.0 - cosTheta, 0.0), 5.0);
}  

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a      = roughness*roughness;
    float a2     = a*a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;
	
    float num   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
	
    return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float num   = NdotV;
    float denom = NdotV * (1.0 - k) + k;
	
    return num / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2  = GeometrySchlickGGX(NdotV, roughness);
    float ggx1  = GeometrySchlickGGX(NdotL, roughness);
	
    return ggx1 * ggx2;
}

vec3 CalcLight(vec3 L, vec3 V, vec3 N, vec3 F0, vec3 albedo, float metal, float roughness, vec3 radiance) {
    vec3 H = normalize(V + L);

    float NDF = DistributionGGX(N, H, roughness);
    float G   = GeometrySmith(N, V, L, roughness);
    vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);
        
    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - metal;
        
    vec3 numerator    = NDF * G * F;
    float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0);
    vec3 specular     = numerator / max(denominator, 0.001);
            
    float NdotL = max(dot(N, L), 0.0);
    return (kD * albedo / PI + specular) * radiance * NdotL;
}

float CalcAtten(vec3 pos, vec3 lightPos, vec3 atten) {
    float dist = length(lightPos - pos);
    return 1.0 / (atten.x + atten.y * dist + atten.z * dist * dist);
}

vec3 CalcPointLight(vec3 pos, vec3 V, vec3 N, vec3 F0, vec3 albedo, float metal, float roughness, PointLight l) {
    vec3 L = normalize(l.position.xyz - pos);
    float attenuation = CalcAtten(pos, l.position.xyz, l.atten.xyz);
    vec3 radiance = l.color.rgb * attenuation;
    return CalcLight(L, V, N, F0, albedo, metal, roughness, radiance);
}

vec3 CalcSpotLight(vec3 pos, vec3 V, vec3 N, vec3 F0, vec3 albedo, float metal, float roughness, SpotLight l) {
    vec3 L = normalize(l.position.xyz - pos);
    float attenuation = CalcAtten(pos, l.position.xyz, l.atten.xyz);
    vec3 radiance = l.color.rgb * attenuation;
    float d = dot(-L, l.direction.xyz);
    float intensity = clamp((d - l.outerCutoff) / (l.innerCutoff - l.outerCutoff), 0.0, 1.0);
    return CalcLight(L, V, N, F0, albedo, metal, roughness, radiance * intensity);
}

float CalcFogAmount(vec3 pos, vec2 fogLimits) {
    return smoothstep(fogLimits.x, fogLimits.y, pos.z);
}

#endif