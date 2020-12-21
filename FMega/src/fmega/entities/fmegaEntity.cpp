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

	void FMegaEntity::Init(CollisionObject* collision)
	{
		m_Collision = collision;
	}

	FMegaEntity::~FMegaEntity()
	{
		if (m_Collision) {
			delete m_Collision;
		}
	}

	byte* FMegaEntity::GetData(uint& size) {
		size = 0;
		return nullptr;
	}

	uint FMegaEntity::GetSaveSize() {
		uint result;
		GetData(result);
		return result + sizeof(m_LocalTransform);
	}

	void FMegaEntity::Save(byte* data, uint& size) {
		byte* src = GetData(size);
		if (src != nullptr)
			memcpy(data, src, size);
		memcpy(data + size, &m_LocalTransform, sizeof(m_LocalTransform));
		size += sizeof(m_LocalTransform);
	}

	void FMegaEntity::Load(byte* data, uint& size) {
		byte* dest = GetData(size);
		if (dest != nullptr)
			memcpy(dest, data, size);
		memcpy(&m_LocalTransform, data + size, sizeof(m_LocalTransform));
		size += sizeof(m_LocalTransform);
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

	void FMegaEntity::RewindUpdate(float delta) {

	}

	void FMegaEntity::Render(float delta)
	{
		/*if (m_Mesh) {
			MeshRenderData data;
			data.model = m_GlobalTransform;
			data.color = Color;
			m_FMegaScene->GetRenderer()->RenderMesh(m_Mesh, data);
		}*/
	}

}
