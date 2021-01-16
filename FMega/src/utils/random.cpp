#include "fmega.h"
#include "random.h"
#include <ctime>

namespace fmega {

	uint32 Random::m_Seed[5];

	void Random::Init(uint32 seed)
	{
		if(seed == 0)
			m_Seed[0] = (uint32) time(NULL);
		else m_Seed[0] = seed;
		for (uint32 i = 0; i < 4; i++)
		{
			uint32 x = m_Seed[i];
			x ^= x << 13;
			x ^= x >> 17;
			x ^= x << 5;
			m_Seed[i + 1] = x;
		}
	}

	uint32 Random::NextUint()
	{
		uint32_t s, t = m_Seed[3];
		t ^= t >> 2;
		t ^= t << 1;
		m_Seed[3] = m_Seed[2]; m_Seed[2] = m_Seed[1]; m_Seed[1] = s = m_Seed[0];
		t ^= s;
		t ^= s << 4;
		m_Seed[0] = t;
		return t + (m_Seed[4] += 362437);
	}

	uint32 Random::NextUint(uint32 lowerBound, uint32 upperBound)
	{
		if (upperBound <= lowerBound) return lowerBound;
		return NextUint() % (upperBound - lowerBound) + lowerBound;
	}

	int32 Random::NextInt()
	{
		return (int32)NextUint();
	}

	int32 Random::NextInt(int32 lowerBound, int32 upperBound)
	{
		if (upperBound == lowerBound) return lowerBound;
		return (int32)(NextUint() % (upperBound - lowerBound)) + lowerBound;
	}

	float Random::NextFloat()
	{
		return float(NextUint() >> 8) / float(0xffffff);
	}

	float Random::NextFloat(const glm::vec2& range)
	{
		return NextFloat() * (range.y - range.x) + range.x;
	}

	glm::vec2 Random::NextVec2()
	{
		return glm::vec2(NextFloat(), NextFloat());
	}

	glm::vec3 Random::NextVec3()
	{
		return glm::vec3(NextFloat(), NextFloat(), NextFloat());
	}

	glm::vec4 Random::NextVec4()
	{
		return glm::vec4(NextFloat(), NextFloat(), NextFloat(), NextFloat());
	}

	glm::vec3 Random::NextVec3Normalized() {
		glm::vec2 angles = NextVec2();
		float a = angles.x * glm::two_pi<float>();
		float b = angles.y * glm::pi<float>() - glm::half_pi<float>();
		return glm::vec3(
			cos(b) * sin(a),
			sin(b),
			cos(b) * cos(a));
	}

	glm::quat Random::NextQuat() {
		glm::vec3 axis = NextVec3Normalized();
		float angle = NextFloat() * glm::pi<float>();
		return glm::angleAxis(angle, axis);
	}

}