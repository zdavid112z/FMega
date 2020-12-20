#include "fmega.h"
#include "fmegaEntity.h"
#include "fmega/fmegaScene.h"
#include "fmega/fmegaRenderer.h"
#include "graphics/mesh.h"

namespace fmega {

	FMegaEntity::FMegaEntity(const std::string& name, Entity* parent, FMegaScene* scene) :
		Entity(name, parent, (Scene*)scene),
		m_FMegaScene(scene)
	{

	}

	void FMegaEntity::Init(Mesh* mesh, CollisionObject* collision)
	{
		m_Mesh = mesh;
		m_Collision = collision;
	}

	FMegaEntity::~FMegaEntity()
	{
		if (m_Collision) {
			delete m_Collision;
		}
	}

	void FMegaEntity::Update(float delta)
	{
		if (
			m_LocalTransform.position.x <= -24.f ||
			m_LocalTransform.position.x >= 24.f ||
			m_LocalTransform.position.y <= -15.f ||
			m_LocalTransform.position.y >= 15.f) {
			Destroy();
		}
	}

	void FMegaEntity::Render(float delta)
	{
		if (m_Mesh) {
			MeshRenderData data;
			data.model = m_GlobalTransform;
			data.color = Color;
			m_FMegaScene->GetRenderer()->RenderMesh(m_Mesh, data);
		}
	}

}
