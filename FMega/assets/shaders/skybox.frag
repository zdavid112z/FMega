#version 430 core

out vec4 outFragColor;

in VertexData
{
	vec4 vPosition;
	vec3 vEyePosition;
	float vCameraOffset;
} vData;

const int kNumSections = 7;
const float PI = 3.14159265358979323846264338327950288;

void main()
{
	vec3 p = normalize(vData.vPosition.xyz);
	float angle = mod(atan(p.y, p.x) + 2 * PI, 2 * PI);
	float angleStep = (2.0 * PI) / kNumSections;
	float section = floor((angle / (2.0 * PI)) * kNumSections);

	float planeAngle = angleStep * (section + 0.5);
	vec4 plane = vec4(cos(planeAngle), sin(planeAngle), 0, cos(angleStep / 2.0));
	float len = -plane.w / dot(plane.xyz, p);
	vec3 l = p * len;
	vec3 sampleL = l + vec3(0, 0, vData.vCameraOffset * 0.025);

	vec3 stripes = vec3(1, 0.7, 4);
	float amount = sin(dot(sampleL, stripes));
	float m = 50.0;
	float val = clamp(m * amount, -1.0, 1.0) * 0.5 + 0.5;
	vec3 color = mix(vec3(0.7, 0.8, 0.75), vec3(0.7, 0.2, 0.3), val);
    
	float dist = length(l);
	float fogAmount = smoothstep(1.25, 2.0, l.z);
	color = mix(color, vec3(0.97), fogAmount);
    
    outFragColor = vec4( vec3(color), 1.0 );
}
