#include "fmega.h"
#include "pickup.h"
#include "../fmegaScene.h"
#include "../fmegaRenderer.h"

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
			d.textureOpacity = 0;
			d.opacity = 0.5f;
			d.color = glm::vec4(1, 0, 0, 1);
			d.model = m_GlobalTransform;
			m_FMegaScene->GetRenderer()->RenderMesh(m_FMegaScene->PickupMesh, d, false, MeshType::MESH_3D, true);
		}
	}

	byte* Pickup::GetData(uint& size) {
		size = sizeof(m_Data);
		return m_Data;
	}

}