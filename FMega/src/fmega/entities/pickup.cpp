#include "fmega.h"
#include "pickup.h"
#include "../fmegaScene.h"
#include "core/game.h"
#include "../fmegaRenderer.h"
#include "light.h"
#include "utils/random.h"

namespace fmega {

	const float Pickup::Height = 1.5f;
	const float Pickup::Size = 1.f;

	Pickup::Pickup(const std::string& name, Entity* parent, FMegaScene* scene, const glm::vec3& target, PickupType type) :
		FMegaEntity("Pickup_" + name, parent, scene), m_Target(target) {

		m_LocalTransform.position = target;
		Type = type;

		AABBObject* col = new AABBObject(this, glm::mat4(1), glm::vec3(Size));
		Init(col);
	}

	Pickup::~Pickup() {

	}

	void Pickup::Update(float delta) {
		m_Color = TypeToColor();
		m_LocalTransform.position.z += delta * m_FMegaScene->MoveSpeed;

		if (m_LocalTransform.position.z > 150.f) {
			Destroy();
		}
	}

	glm::vec3 Pickup::TypeToColor() {
		float t = m_FMegaScene->GetGame()->GetTime();
		switch (Type)
		{
		case PickupType::DISCO:
			return glm::vec3(
				glm::sin(t * 14.f + 3.f * m_Target.z + 2.f * m_Target.x) * 0.5 + 0.5,
				glm::sin(t * 21.f + 2.f * m_Target.z + 4.f * m_Target.x) * 0.5 + 0.5,
				glm::sin(t * 17.f + 1.f * m_Target.z + 5.f * m_Target.x) * 0.5 + 0.5
			);
		case PickupType::REWIND:
			return glm::vec3(0, 1, 0);
		case PickupType::FARSIGHT:
			return glm::vec3(0, 0, 1);
		}
		return glm::vec3(1);
	}

	void Pickup::Render(float delta) {
		MeshRenderData d;
		d.albedoStrength = 0;
		d.opacity = 0.5f;
		d.color = glm::vec4(m_Color, 1);
		d.model = m_GlobalTransform;
		d.roughness = 0.15f;
		d.roughnessmapStrength = 0.f;
		d.metalness = 0.01f;
		QueueKey q;
		q.mesh = m_FMegaScene->GetAssets()->PickupMesh;
		q.ui = false;
		q.transparent = true;
		q.type = MeshType::MESH_3D;
		m_FMegaScene->GetRenderer()->RenderMesh(q, d);
	}

	byte* Pickup::GetData(uint& size) {
		size = sizeof(m_Data);
		return m_Data;
	}

}