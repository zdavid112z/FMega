#include "fmega.h"
#include "lamp.h"
#include "tunnel.h"
#include "../fmegaScene.h"
#include "../fmegaRenderer.h"

namespace fmega {

	const float Lamp::WallAngle = glm::radians(30.f);

	Lamp::Lamp(const std::string& name, Entity* parent, FMegaScene* scene, int numLamps, float lampDistance, int index, float resetZ) :
		FMegaEntity("Lamp_" + name, parent, scene), m_RepeatDistance(numLamps * lampDistance), m_ResetZ(resetZ) {
		float sign = (index % 2) * 2 - 1;
		float z = resetZ - index * lampDistance;
		float angle = glm::half_pi<float>() + sign * WallAngle;
		float x = glm::cos(angle) * Tunnel::Radius;
		float y = glm::sin(angle) * Tunnel::Radius;
		m_LocalTransform.position = glm::vec3(x, y, z);
		m_LocalTransform.rotation.z = angle;
	}

	Lamp::~Lamp() {

	}

	void Lamp::Update(float delta)
	{
		float& z = m_LocalTransform.position.z;
		z += delta * m_FMegaScene->MoveSpeed;
		while (z > m_ResetZ) {
			z -= m_RepeatDistance;
		}
	}

	void Lamp::Render(float delta)
	{
		if (m_FMegaScene->SlowDeath) {
			return;
		}
		//m_FMegaScene->GetRenderer()->RenderMesh(m_FMegaScene->TunnelMesh,)
		MeshRenderData d;
		d.opacity = 1.f;
		d.model = m_GlobalTransform;
		d.albedoStrength = 0.f;
		d.color = glm::vec4(1.f, 1.f, 1.f, 1.0);
		d.roughnessmapStrength = 0.f;
		d.normalmapStrength = 0.f;
		d.metalness = 0.1f;
		d.metalnessmapStrength = 0.f;
		QueueKey k;
		k.mesh = m_FMegaScene->LampMesh;
		k.transparent = false;
		k.ui = false;
		k.type = MeshType::MESH_3D;
		m_FMegaScene->GetRenderer()->RenderMesh(k, d);
	}

	byte* Lamp::GetData(uint& size) {
		size = sizeof(m_Data);
		return m_Data;
	}

}