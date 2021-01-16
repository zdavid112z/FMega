#pragma once

#include "fmegaEntity.h"

namespace fmega {

	class Bars : public FMegaEntity {
	public:
		Bars(const std::string& name, Entity* parent, FMegaScene* scene);
		virtual ~Bars();

		virtual void Update(float delta) override;
		virtual void Render(float delta) override;

		virtual byte* GetData(uint& size) override;
		virtual void InterpolateDynamicData(float* p1, float* p2, float amount, float* out, int count);

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