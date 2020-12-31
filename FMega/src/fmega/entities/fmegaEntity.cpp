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

	uint FMegaEntity::GetStaticSaveSize() {
		uint res;
		GetData(res);
		return res;
	}

	uint FMegaEntity::GetSaveSize() {
		uint result;
		GetData(result);
		return result + sizeof(m_LocalTransform);
	}

	uint FMegaEntity::Save(byte* data) {
		uint size;
		byte* src = GetData(size);
		if (src != nullptr)
			memcpy(data, src, size);
		memcpy(data + size, &m_LocalTransform, sizeof(m_LocalTransform));
		size += sizeof(m_LocalTransform);
		return size;
	}

	uint FMegaEntity::LoadStatic(byte* data) {
		uint size;
		uint staticSize = GetStaticSaveSize();
		byte* dest = GetData(size);
		if (dest != nullptr && staticSize != 0)
			memcpy(dest, data, staticSize);
		return staticSize;
	}

	uint FMegaEntity::LoadDynamic(byte* data) {
		uint size;
		uint staticSize = GetStaticSaveSize();
		byte* dest = GetData(size);
		uint dynamicSpecificSize = size - staticSize;
		if (dest != nullptr && dynamicSpecificSize != 0)
			memcpy(dest, data, dynamicSpecificSize);
		memcpy(&m_LocalTransform, data + dynamicSpecificSize, sizeof(m_LocalTransform));
		return dynamicSpecificSize + sizeof(m_LocalTransform);
	}

	void FMegaEntity::InterpolateDynamicData(float* p1, float* p2, float amount, float* out, int count) {
		for (int i = 0; i < count; i++) {
			out[i] = glm::lerp(p1[i], p2[i], amount);
		}
	}

	void FMegaEntity::Update(float delta)
	{
		/*LOG_INFO("hi");
		if (
			m_LocalTransform.position.x <= -24.f ||
			m_LocalTransform.position.x >= 24.f ||
			m_LocalTransform.position.y <= -15.f ||
			m_LocalTransform.position.y >= 15.f) {
			Destroy();
		}*/
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
