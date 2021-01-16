#version 430 core

#include "common.glsl"
#include "light.glsl"

layout (location = 0) in vec4 aPosition;
layout (location = 1) in vec4 aUV;
layout (location = 2) in vec4 aColor;
layout (location = 3) in vec4 aNormal;
layout (location = 4) in vec4 aTangent;
layout (location = 5) in vec4 aInitialRotation;
layout (location = 6) in vec4 aInitialPosition;
layout (location = 7) in vec4 aLocalOrigin;

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
    vec4 uColor;
};

out VertexData
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

vec4 slerp(vec4 v0, vec4 v1, float t) {
    // Only unit quaternions are valid rotations.
    // Normalize to avoid undefined behavior.
    v0 = normalize(v0);
    v0 = normalize(v0);

    // Compute the cosine of the angle between the two vectors.
    float d = dot(v0, v1);

    // If the dot product is negative, slerp won't take
    // the shorter path. Note that v1 and -v1 are equivalent when
    // the negation is applied to all four components. Fix by 
    // reversing one quaternion.
    if (d < 0.0) {
        v1 = -v1;
        d = -d;
    }

    const double DOT_THRESHOLD = 0.9995;
    if (d > DOT_THRESHOLD) {
        // If the inputs are too close for comfort, linearly interpolate
        // and normalize the result.

        vec4 result = v0 + t * (v1 - v0);
        return normalize(result);
    }

    // Since dot is in range [0, DOT_THRESHOLD], acos is safe
    float theta_0 = acos(d);        // theta_0 = angle between input vectors
    float theta = theta_0*t;          // theta = angle between v0 and result
    float sin_theta = sin(theta);     // compute this value only once
    float sin_theta_0 = sin(theta_0); // compute this value only once

    float s0 = cos(theta) - d * sin_theta / sin_theta_0;  // == sin(theta_0 - theta) / sin(theta_0)
    float s1 = sin_theta / sin_theta_0;

    return (s0 * v0) + (s1 * v1);
}

vec3 rotate(vec4 q, vec3 v) {
    vec3 QuatVector = vec3(q.x, q.y, q.z);
	vec3 uv = cross(QuatVector, v);
    vec3 uuv = cross(QuatVector, uv);

    return v + ((uv * vec3(q.w)) + uuv) * vec3(2.0);
}

void main()
{
	float currentZ = (uModel * aLocalOrigin).z;
    float destructZ = uColor.w;
	float amount;
    if (currentZ >= uTargetZ) {
        if (currentZ <= destructZ) {
            amount = 0;
        }
        else {
            amount = (currentZ - destructZ) / (35.0 - destructZ);
        }
    } else {
        amount = (currentZ - uTargetZ) / (aInitialPosition.z - uTargetZ);
    }
    float p = 1.0;
    if (amount < 1.0) {
        amount = pow(amount, p);
    } else {
        amount = p * amount - p + 1;
    }
    vec4 currentQuat = slerp(vec4(0, 0, 0, 1), aInitialRotation, amount);
	vec4 localRotatedPosition = vec4(rotate(currentQuat, aPosition.xyz), 1);
    vec4 globalRotatedPosition = uModel * (vec4(aLocalOrigin.xyz, 0.0) + localRotatedPosition);
    globalRotatedPosition.xy += aInitialPosition.xy * amount;
    gl_Position = uViewProjection * globalRotatedPosition;

    vec3 localNormal = normalize(rotate(currentQuat, aNormal.xyz));
    vec3 localTangent = normalize(rotate(currentQuat, aTangent.xyz));
    //vec3 localNormal = aNormal.xyz;
    vec3 globalNormal = normalize(uModel * vec4(localNormal, 0.0)).xyz;
    vec3 globalTangent = normalize(uModel * vec4(localTangent, 0.0)).xyz;
    vec3 globalBitangent = cross(globalNormal, globalTangent);

    vData.vPosition = globalRotatedPosition.xyz;
	vData.vUV = vec3(aUV.xy, 0.0);
	vData.vTBN = mat3(globalTangent, globalBitangent, globalNormal);
	vData.vColor = vec4(uColor.rgb, 1.0) + aColor;
	vData.vTexOpacity = 0.8;

    vData.vRoughness = 0.2;
	vData.vRoughnessMapStrength = 1.0;
	vData.vMetalness = 0.1;
	vData.vMetalMapStrength = 1.0;
	vData.vInstNormal = vec3(0, 0, 1);
	vData.vNormalMapStrength = 1.0;
}
