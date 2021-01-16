#include "fmega.h"
#include "bars.h"
#include "tunnel.h"
#include "../fmegaScene.h"
#include "../fmegaRenderer.h"

namespace fmega {

	Bars::Bars(const std::string& name, Entity* parent, FMegaScene* scene) :
		FMegaEntity("Bars_" + name, parent, scene), m_RepeatDistance(340.f), m_ResetZ(40.f) {
		m_LocalTransform.position.y = Tunnel::Radius * 0.85f;
	}

	Bars::~Bars() {

	}

	void Bars::Update(float delta)
	{
		float& z = m_LocalTransform.position.z;
		z += delta * m_FMegaScene->MoveSpeed;
		while (z > m_ResetZ) {
			z -= m_RepeatDistance;
		}
	}

	void Bars::Render(float delta)
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
		k.mesh = m_FMegaScene->GetAssets()->BarsMesh;
		k.transparent = false;
		k.ui = false;
		k.type = MeshType::MESH_3D;
		k.albedo = m_FMegaScene->GetAssets()->BarsAlbedo;
		k.normal = m_FMegaScene->GetAssets()->BarsNormalmap;
		k.roughness = m_FMegaScene->GetAssets()->BarsRoughness;
		k.metalness = m_FMegaScene->GetAssets()->BarsMetalness;
		m_FMegaScene->GetRenderer()->RenderMesh(k, d);
	}

	byte* Bars::GetData(uint& size) {
		size = sizeof(m_Data);
		return m_Data;
	}

	void Bars::InterpolateDynamicData(float* p1, float* p2, float amount, float* out, int count) {
		FMegaEntity::InterpolateDynamicData(p1, p2, amount, out, count);
		Transform* t1 = (Transform*)p1;
		Transform* t2 = (Transform*)p2;
		Transform* tout = (Transform*)out;
		if (t1->position.z > t2->position.z) {
			float diff = m_ResetZ - t1->position.z + t2->position.z - (m_ResetZ - m_RepeatDistance);
			tout->position.z = t1->position.z + diff * amount;
			while (tout->position.z > m_ResetZ) {
				tout->position.z -= m_RepeatDistance;
			}
		}
	}

}
