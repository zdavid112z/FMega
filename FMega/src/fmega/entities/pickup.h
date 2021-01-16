#pragma once

#include "fmegaEntity.h"

namespace fmega {

	enum class PickupType {
		DISCO,
		REWIND,
		FARSIGHT,
		COUNT
	};

	class Pickup : public FMegaEntity {
	public:
		Pickup(const std::string& name, Entity* parent, FMegaScene* scene, const glm::vec3& target, PickupType type);
		virtual ~Pickup();

		virtual void Update(float delta) override;
		virtual void Render(float delta) override;

		bool IsTaken();
		void OnPlayerTake();
		virtual byte* GetData(uint& size) override;
		glm::vec3 TypeToColor();

		static const float Height;
		static const float Size;
		PickupType Type;

	private:
		union {
			struct {
				glm::vec3 m_Color;
				glm::vec3 m_Target;
				bool m_Taken;
			};
			byte m_Data[32];
		};
	};

}
