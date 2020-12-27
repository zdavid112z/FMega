#pragma once

#include <thread>
#include <atomic>
#include "fmegaEntity.h"
#include "utils/colors.h"
#include "graphics/gpuBuffer.h"

namespace fmega {

	class Pickup : public FMegaEntity {
	public:
		Pickup(const std::string& name, Entity* parent, FMegaScene* scene, const glm::vec3& target);
		virtual ~Pickup();

		virtual void Update(float delta) override;
		virtual void Render(float delta) override;

		virtual byte* GetData(uint& size) override;

		static const float Height;

	private:
		union {
			struct {
				glm::vec3 m_Target;
			};
			byte m_Data[20];
		};
	};

}