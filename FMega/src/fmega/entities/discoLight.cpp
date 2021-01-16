#include "fmega.h"
#include "player.h"
#include "light.h"
#include "discoLight.h"
#include "../fmegaScene.h"
#include "../fmegaRenderer.h"

namespace fmega {

	const float DiscoLight::AngleSpeed = 10.f;
	const float DiscoLight::ColorSpeed = 5.f;
	const float DiscoLight::PointLightIntensity = 25.f;
	const float DiscoLight::SpotLightIntensity = 250.f;
	const glm::vec3 DiscoLight::PointLightAtten = glm::vec3(1.f, 0.6f, 0.3f);
	const glm::vec3 DiscoLight::SpotLightAtten = glm::vec3(1.f, 0.f, 0.f);
	const glm::vec2 DiscoLight::LightCutoff = glm::cos(glm::radians(glm::vec2(30.f, 45.f)));

	DiscoLight::DiscoLight(const std::string& name, FMegaScene* scene, Player* player, float colorSeed, const glm::quat& initialRot, const glm::quat& vel) :
		FMegaEntity("DiscoLight_" + name, nullptr, scene), m_Player(player), m_ColorH(colorSeed), m_CurrentRot(initialRot), m_RotVelocity(vel) {

		m_LocalTransform.position = m_Player->GetLocalTransform().position;
		m_PointLight = new Light("PointDisco_" + name, this, scene, GetColor(), PointLightIntensity, PointLightAtten);
		m_SpotLight = new Light("SpotDisco_" + name, this, scene, GetColor(), SpotLightIntensity, SpotLightAtten, glm::vec3(1, 0, 0), LightCutoff);
		scene->AddEntity(m_PointLight);
		scene->AddEntity(m_SpotLight);
	}

	DiscoLight::~DiscoLight() {

	}

	void DiscoLight::Update(float delta)
	{
		m_ColorH += delta * ColorSpeed;
		float intensity = m_Player->GetDiscoLightIntensity();
		glm::quat v = glm::slerp(glm::quat(1.f, 0.f, 0.f, 0.f), m_RotVelocity, delta * AngleSpeed);
		
		m_CurrentRot *= v;
		glm::vec3 d = glm::rotate(m_CurrentRot, glm::vec3(1, 0, 0));
		glm::vec3 o = m_Player->GetLocalTransform().position;
		m_LocalTransform.position = o + d * (m_Player->Radius + 1.f);
		m_PointLight->Init(GetColor(), intensity * PointLightIntensity, PointLightAtten);
		m_SpotLight->Init(GetColor(), intensity * SpotLightIntensity, SpotLightAtten, d, LightCutoff);
	}

	void DiscoLight::Render(float delta)
	{
		
	}

	byte* DiscoLight::GetData(uint& size) {
		size = sizeof(m_Data);
		return m_Data;
	}

	glm::vec3 DiscoLight::GetColor() {
		hsv h(glm::mod(m_ColorH, glm::two_pi<float>()), 1.f, 1.f, 1.f);
		return Colors::ToRGB(h);
	}

}