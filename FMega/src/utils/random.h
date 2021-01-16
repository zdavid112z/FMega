#pragma once

#include "fmega.h"

namespace fmega {

	class Random
	{
	public:
		static void Init(uint32 seed = 0);
		static uint32 NextUint();
		static uint32 NextUint(uint32 lowerBound, uint32 upperBound);
		static int32 NextInt();
		static int32 NextInt(int32 lowerBound, int32 upperBound);
		static float NextFloat();
		static float NextFloat(const glm::vec2& range);
		static glm::vec2 NextVec2();
		static glm::vec3 NextVec3();
		static glm::vec3 NextVec3Normalized();
		static glm::vec4 NextVec4();
		static glm::quat NextQuat();

		template <typename T>
		static void Shuffle(std::vector<T>& v)
		{
			for (size_t i = 0; i < v.size(); i++)
			{
				size_t newPos = (size_t)NextUint(0, v.size());
				T aux = v[i];
				v[i] = v[newPos];
				v[newPos] = aux;
			}
		}		
	private:
		static uint32 m_Seed[5];
	};

}