#include "fmega.h"
#include "light.h"
#include "../fmegaScene.h"
#include "../fmegaRenderer.h"

namespace fmega {

	Light::Light(const std::string& name, Entity* parent, FMegaScene* scene, const glm::vec3& color, float intensity, const glm::vec3& atten) :
		FMegaEntity("Light_" + name, parent, scene) {

		m_Type = LightType::POINT;
		m_Color = color;
		m_Atten = atten;
		m_Intensity = intensity;
	}

	Light::Light(const std::string& name, Entity* parent, FMegaScene* scene, const glm::vec3& color, float intensity, const glm::vec3& atten, const glm::vec3& direction, const glm::vec2& cutoffAnglesDeg) :
		FMegaEntity("Light_" + name, parent, scene) {

		m_Type = LightType::SPOT;
		m_Color = color;
		m_Atten = atten;
		m_Intensity = intensity;
		m_Direction = glm::normalize(direction);
		m_CutoffLimits = glm::cos(glm::radians(cutoffAnglesDeg));
	}

	Light::~Light() {

	}

	LightType Light::GetLightType() {
		return m_Type;
	}

	PointLight Light::GetPointLightData() {
		PointLight pl;
		pl.color = glm::vec4(m_Color * m_Intensity, 1.f);
		pl.position = glm::vec4(GetWorldPosition(), 1.f);
		pl.atten = glm::vec4(m_Atten, 0.f);
		return pl;
	}

	SpotLight Light::GetSpotLightData() {
		SpotLight sl;
		sl.color = glm::vec4(m_Color * m_Intensity, 1.f);
		sl.position = glm::vec4(GetWorldPosition(), 1.f);
		sl.atten = glm::vec4(m_Atten, 0.f);
		sl.direction = glm::vec4(GetDirection(), 0.f);
		sl.innerCutoff = m_CutoffLimits.x;
		sl.outerCutoff = m_CutoffLimits.y;
		return sl;
	}

	glm::vec3 Light::GetWorldPosition() {
		glm::vec4 p = glm::vec4(0, 0, 0, 1);
		p = m_GlobalTransform * p;
		return glm::vec3(p);
	}

	glm::vec3 Light::GetDirection() {
		glm::vec4 p = glm::vec4(m_Direction, 0.f);
		p = m_GlobalTransform * p;
		return glm::normalize(glm::vec3(p));
	}

	void Light::Update(float delta) {
		
	}

	void Light::Render(float delta) {
		
	}

	byte* Light::GetData(uint& size) {
		size = sizeof(m_Data);
		return m_Data;
	}

}