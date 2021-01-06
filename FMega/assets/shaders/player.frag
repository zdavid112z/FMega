#version 430 core

#include "pbr.frag"

vec2 GetUV(vec3 uv) {
	return vec2(0.5 + atan(uv.x, uv.z) / (2 * PI), 0.5 - asin(uv.y) / PI);
}
