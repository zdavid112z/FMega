#pragma once

#include "fmegaEntity.h"

namespace fmega {

	class Lamp : public FMegaEntity {
	public:
		Lamp(const std::string& name, Entity* parent, FMegaScene* scene, int numLamps, float lampDistance, int index, float resetZ);
		virtual ~Lamp();

		virtual void Update(float delta) override;
		virtual void Render(float delta) override;

		virtual byte* GetData(uint& size) override;
		virtual void InterpolateDynamicData(float* p1, float* p2, float amount, float* out, int count);

		const static float WallAngle;
	private:
		union {
			struct {
				
			};
			byte m_Data[4];
		};
		const float m_RepeatDistance;
		const float m_ResetZ;
	};

}