#pragma once

#include "fmega.h"

namespace fmega {

	struct hsv {
		hsv() : h(0.0f), s(0.0f), v(0.0f), a(1.0f) {}
		hsv(const glm::vec3& v) : h(v.x), s(v.y), v(v.z), a(1.0f) {}
		hsv(const glm::vec4& v) : h(v.x), s(v.y), v(v.z), a(v.w) {}
		hsv(float h, float s, float v, float a = 1.0f) : h(h), s(s), v(v), a(a) {}

		float h, s, v, a;
	};
	
	class Colors {
	public:
		static hsv ToHSV(const glm::vec3& rgb);
		static glm::vec3 ToRGB(const hsv& hsv);
		static hsv ToHSV(const glm::vec4& rgb);
		static glm::vec4 ToRGBA(const hsv& hsv);
	};

};
