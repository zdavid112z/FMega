#pragma once

#include "fmegaEntity.h"

namespace fmega {

	class Tunnel : public FMegaEntity {
	public:
		Tunnel(const std::string& name, Entity* parent, FMegaScene* scene);
		virtual ~Tunnel();

		virtual void Update(float delta) override;
		virtual void Render(float delta) override;

		virtual byte* GetData(uint& size) override;

		const static float Radius;
		const static float MoveSpeedModifier;

	private:
		union {
			struct {
				float m_RepeatDist;
			};
			byte m_Data[4];
		};
	};

}