#include "fmega.h"
#include "pickup.h"
#include "../fmegaScene.h"
#include "../fmegaRenderer.h"
#include "light.h"

namespace fmega {

	const float Pickup::Height = 1.5f;

	Pickup::Pickup(const std::string& name, Entity* parent, FMegaScene* scene, const glm::vec3& target) :
		FMegaEntity("Pickup_" + name, parent, scene), m_Target(target) {

		m_LocalTransform.position = target;
	}

	Pickup::~Pickup() {

	}

	void Pickup::Update(float delta) {
		m_LocalTransform.position.z += delta * m_FMegaScene->MoveSpeed;

		if (m_LocalTransform.position.z > 150.f) {
			Destroy();
		}
	}

	void Pickup::Render(float delta) {
		{
			MeshRenderData d;
			d.albedoStrength = 0;
			d.opacity = 0.5f;
			d.color = glm::vec4(1, 0, 0, 1);
			d.model = m_GlobalTransform;
			d.roughness = 0.15f;
			d.roughnessmapStrength = 0.f;
			d.metalness = 0.01f;
			QueueKey q;
			q.mesh = m_FMegaScene->PickupMesh;
			q.ui = false;
			q.transparent = true;
			q.type = MeshType::MESH_3D;
			m_FMegaScene->GetRenderer()->RenderMesh(q, d);
		}
	}

	byte* Pickup::GetData(uint& size) {
		size = sizeof(m_Data);
		return m_Data;
	}

}