#include "fmega.h"
#include "tunnel.h"
#include "../fmegaScene.h"
#include "../fmegaRenderer.h"

namespace fmega {

	const float Tunnel::MoveSpeedModifier = 1.f;
	const float Tunnel::Radius = 20.f;

	Tunnel::Tunnel(const std::string& name, Entity* parent, FMegaScene* scene) :
		FMegaEntity("Tunnel_" + name, parent, scene) {
		m_RepeatDist = Radius * glm::two_pi<float>();
	}

	Tunnel::~Tunnel() {

	}

	void Tunnel::Update(float delta)
	{
		float& z = m_LocalTransform.position.z;
		z += delta * m_FMegaScene->MoveSpeed * MoveSpeedModifier;
		z = glm::mod(z, m_RepeatDist);
	}

	void Tunnel::Render(float delta)
	{
		if (m_FMegaScene->SlowDeath) {
			return;
		}
		//m_FMegaScene->GetRenderer()->RenderMesh(m_FMegaScene->TunnelMesh,)
		MeshRenderData d;
		d.opacity = 1.f;
		d.model = m_GlobalTransform;
		d.albedoStrength = 1.f;
		d.color = glm::vec4(0.2, 0.3, 0.5, 1.0);
		d.roughnessmapStrength = 1.f;
		d.normalmapStrength = 1.f;
		d.metalness = 0.1f;
		d.metalnessmapStrength = 1.f;
		QueueKey k;
		k.mesh = m_FMegaScene->TunnelMesh;
		k.transparent = false;
		k.ui = false;
		k.type = MeshType::MESH_3D;
		k.albedo = m_FMegaScene->TunnelAlbedo;
		k.normal = m_FMegaScene->TunnelNormalmap;
		k.roughness = m_FMegaScene->TunnelRoughness;
		k.metalness = m_FMegaScene->TunnelMetalness;
		m_FMegaScene->GetRenderer()->RenderMesh(k, d);
	}

	byte* Tunnel::GetData(uint& size) {
		size = sizeof(m_Data);
		return m_Data;
	}

	void Tunnel::InterpolateDynamicData(float* p1, float* p2, float amount, float* out, int count) {
		FMegaEntity::InterpolateDynamicData(p1, p2, amount, out, count);
		Transform* t1 = (Transform*)p1;
		Transform* t2 = (Transform*)p2;
		Transform* tout = (Transform*)out;
		if (t1->position.z > t2->position.z) {
			float diff = t2->position.z + m_RepeatDist - t1->position.z;
			tout->position.z = glm::mod(t1->position.z + diff * amount, m_RepeatDist);
		}
	}

}