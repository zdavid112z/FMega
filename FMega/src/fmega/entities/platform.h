#pragma once

#include <thread>
#include <atomic>
#include "fmegaEntity.h"
#include "utils/colors.h"
#include "graphics/gpuBuffer.h"

namespace fmega {

	enum class PlatformType {
		FUEL_LOSS,
		FUEL_GAIN,
		SPEED_BOOST,
		PLAIN,
		ACTIVATED,
		DEATH
	};

	class Platform : public FMegaEntity {
	public:
		Platform(const std::string& name, Entity* parent, FMegaScene* scene,
			float x, float z, float width, float length, PlatformType type);
		virtual ~Platform();

		virtual void Update(float delta) override;
		virtual void Render(float delta) override;

		void OnTouched();

		static hsv TypeToColor(PlatformType type);

		const float Length;
		const float Width;
		PlatformType Type;

		static const float StartingZ;
		static const float QualityPerUnit;
		static const float PlatformRandomness;
		static const float PlatformThickness;
		static const float PlatformColorRandomness;
		static const glm::vec2 PlatformXYRandomness;
	private:
		int m_JobID;
		uint32 m_NumIndices;
		Mesh* m_Mesh;
		GPUBuffer* m_VBO;
		GPUBuffer* m_IBO;
		glm::vec4 m_Color;
	};

}