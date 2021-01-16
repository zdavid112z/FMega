#pragma once

#include "fmegaEntity.h"

namespace fmega {

	class Player;
	class Light;

	class DiscoLight : public FMegaEntity {
	public:
		DiscoLight(const std::string& name, FMegaScene* scene, Player* player, float colorSeed, const glm::quat& initialRot, const glm::quat& vel);
		virtual ~DiscoLight();

		virtual void Update(float delta) override;
		virtual void Render(float delta) override;

		virtual byte* GetData(uint& size) override;

		const static float AngleSpeed;
		const static float ColorSpeed;
		const static float PointLightIntensity;
		const static float SpotLightIntensity;
		const static glm::vec3 PointLightAtten;
		const static glm::vec3 SpotLightAtten;
		const static glm::vec2 LightCutoff;
	private:
		glm::vec3 GetColor();

	private:

		union {
			struct {
				float m_ColorH;
				glm::quat m_CurrentRot;
				glm::quat m_RotVelocity;
			};
			byte m_Data[48];
		};

		Player* m_Player;
		Light* m_PointLight;
		Light* m_SpotLight;
	};

}