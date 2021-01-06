#include "fmega.h"
#include "wall.h"
#include "platform.h"
#include "../fmegaScene.h"
#include "../fmegaRenderer.h"
#include "../fmegaObjectFactory.h"
#include "player.h"

namespace fmega {

	const float Wall::Thickness = 1.f;
	const float Wall::Height = 2.f;

	Wall::Wall(const std::string& name, Entity* parent, FMegaScene* scene,
		float x, float z, float width) :
		FMegaEntity("Wall_" + name, parent, scene)
	{
		m_Scene = scene;
		Broken = false;

		m_Mesh = nullptr;
		int gridWidth = int(width * Platform::QualityPerUnit);
		int gridHeight = int(Height * Platform::QualityPerUnit);
		uint32 numVertices = gridWidth * gridHeight * 12 * 6;
		uint32 numIndices = gridWidth * gridHeight * 12 * 6;
		m_NumIndices = numIndices;
		m_IBO = new GPUBuffer(BufferType::ELEMENT_ARRAY, nullptr, numIndices * sizeof(uint32), BufferUsage::STATIC_DRAW);
		m_VBO = new GPUBuffer(BufferType::ARRAY, nullptr, numVertices * sizeof(PlatformVertex), BufferUsage::STATIC_DRAW);

		byte* indices = m_IBO->Map(BufferAccessType::WRITE_ONLY);
		byte* vertices = m_VBO->Map(BufferAccessType::WRITE_ONLY);
		
		PlatformGenData data = {
			gridWidth,
			1,
			gridHeight,
			width,
			Thickness,
			Height,
			Platform::PlatformRandomness * 0.5f / Platform::QualityPerUnit,
			glm::vec3(0),
			Platform::PlatformColorRandomness,
			Platform::StartingZ,
			Platform::PlatformXYRandomness,
			(PlatformVertex*)vertices,
			(uint32*)indices
		};

		m_JobID = m_FMegaScene->GetPlatformManager()->SubmitPlatform(data);

		AABBObject* col = new AABBObject(this, glm::translate(glm::mat4(1), glm::vec3(0, -Height / 2.f, -Thickness / 2.f)), glm::vec3(width, Height, Thickness));
		Init(col);

		m_LocalTransform.position = glm::vec3(x, Height, z);
	}

	Wall::~Wall() {
		if (m_Mesh) {
			delete m_Mesh;
		}
		else {
			m_FMegaScene->GetPlatformManager()->Join(m_JobID);
			m_VBO->Unmap();
			m_IBO->Unmap();
			delete m_VBO;
			delete m_IBO;
		}
	}

	byte* Wall::GetData(uint& size) {
		size = sizeof(Data);
		return Data;
	}

	void Wall::Update(float delta) {
		m_LocalTransform.position.z += delta * m_FMegaScene->MoveSpeed;

		if (m_LocalTransform.position.z > 150.f) {
			Destroy();
		}
	}

	void Wall::OnHitByPlayer() {
		Broken = true;
	}

	void Wall::Render(float delta) {
		if (m_Mesh == nullptr && m_FMegaScene->GetPlatformManager()->IsDone(m_JobID)) {
			m_VBO->Unmap();
			m_IBO->Unmap();
			m_Mesh = FMegaObjectFactory::GenPlatformObject(m_VBO, m_IBO, m_NumIndices);
		}

		if (m_Mesh != nullptr) {
			m_FMegaScene->GetRenderer()->RenderPlatform(m_Mesh, m_GlobalTransform, glm::vec3(0.4, 0.5, 0.7), Broken ? Player::PlayerZ : 200.f);
		}
	}

}