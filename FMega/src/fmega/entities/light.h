#pragma once

#include "fmegaEntity.h"
#include "utils/colors.h"
#include "../fmegaRenderer.h"

namespace fmega {

	enum class LightType {
		POINT,
		SPOT,
		NONE
	};

	class Light : public FMegaEntity {
	public:
		Light(const std::string& name, Entity* parent, FMegaScene* scene, const glm::vec3& color, float intensity, const glm::vec3& atten);
		Light(const std::string& name, Entity* parent, FMegaScene* scene, const glm::vec3& color, float intensity, const glm::vec3& atten, const glm::vec3& direction, const glm::vec2& cutoffAnglesDeg);
		virtual ~Light();

		virtual void Update(float delta) override;
		virtual void Render(float delta) override;

		LightType GetLightType();
		PointLight GetPointLightData();
		SpotLight GetSpotLightData();

		virtual byte* GetData(uint& size) override;

	private:
		glm::vec3 GetWorldPosition();
		glm::vec3 GetDirection();
	private:
		union {
			struct {
				LightType m_Type;
				float m_Intensity;
				glm::vec3 m_Color;
				glm::vec3 m_Atten;
				glm::vec3 m_Direction;
				glm::vec2 m_CutoffLimits;
			};
			byte m_Data[64];
		};
	};

}