#include "fmega.h"
#include "platform.h"
#include "fmega/fmegaScene.h"
#include "core/game.h"
#include "fmega/fmegaRenderer.h"
#include "utils/random.h"
#include "graphics/gpuBuffer.h"

namespace fmega {

	const float Platform::StartingZ = -30.f;
	const float Platform::QualityPerUnit = 2.0f;
	const float Platform::PlatformRandomness = 0.8f;
	const float Platform::PlatformThickness = 0.5f;
	const float Platform::PlatformColorRandomness = 0.1f;
	const glm::vec2 Platform::PlatformXYRandomness = glm::vec2(8.f, 4.f);

	Platform::Platform(const std::string& name, Entity* parent, FMegaScene* scene,
		float x, float z, float width, float length, PlatformType type) :
		FMegaEntity("Platform_" + name, parent, scene), Type(type), Length(length), Width(width)
	{
		m_Mesh = nullptr;
		int gridWidth = int(width * QualityPerUnit);
		int gridLength = int(length * QualityPerUnit);
		uint32 numVertices = gridWidth * gridLength * 18 * 2;
		uint32 numIndices = gridWidth * gridLength * 24 * 2;
		m_NumIndices = numIndices;
		m_IBO = new GPUBuffer(BufferType::ELEMENT_ARRAY, nullptr, numIndices * sizeof(uint32), BufferUsage::STATIC_DRAW);
		m_VBO = new GPUBuffer(BufferType::ARRAY, nullptr, numVertices * sizeof(PlatformVertex), BufferUsage::STATIC_DRAW);

		byte* indices = m_IBO->Map(BufferAccessType::WRITE_ONLY);
		byte* vertices = m_VBO->Map(BufferAccessType::WRITE_ONLY);

		PlatformGenData data = {
			gridWidth,
			gridLength,
			width,
			length,
			PlatformRandomness * 0.5f / QualityPerUnit,
			PlatformThickness,
			glm::vec3(0),
			PlatformColorRandomness,
			StartingZ,
			PlatformXYRandomness,
			(PlatformVertex*)vertices,
			(uint32*)indices
		};

		m_JobID = m_FMegaScene->GetPlatformManager()->SubmitPlatform(data);

		m_LocalTransform.position = glm::vec3(x, 0, z);
		m_Color = Colors::ToRGBA(TypeToColor(Type));
	}

	Platform::~Platform()
	{
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

	void Platform::OnTouched() {
		Type = PlatformType::ACTIVATED;
		m_Color = Colors::ToRGBA(TypeToColor(Type));
	}

	hsv Platform::TypeToColor(PlatformType type) {
		switch (type)
		{
		case fmega::PlatformType::DEATH:
			return hsv(glm::radians(7.f), 0.85f, 0.85f, 1.0f);
		case fmega::PlatformType::FUEL_LOSS:
			return hsv(glm::radians(63.f), 0.85f, 0.85f, 1.0f);
		case fmega::PlatformType::FUEL_GAIN:
			return hsv(glm::radians(133.f), 0.85f, 0.85f, 1.0f);
		case fmega::PlatformType::SPEED_BOOST:
			return hsv(glm::radians(37.f), 0.85f, 0.85f, 1.0f);
		case fmega::PlatformType::PLAIN:
			return hsv(glm::radians(217.f), 0.85f, 0.85f, 1.0f);
		case fmega::PlatformType::ACTIVATED:
			return hsv(glm::radians(275.f), 0.85f, 0.85f, 1.0f);
		}
		return hsv(glm::radians(304.f), 0.85f, 0.85f, 1.0f);
	}

	void Platform::Update(float delta)
	{
		m_LocalTransform.position.z += delta * m_FMegaScene->MoveSpeed;

		if (m_LocalTransform.position.z - Length > 5.f) {
			Destroy();
		}
	}

	void Platform::Render(float delta)
	{
		if (m_Mesh == nullptr && m_FMegaScene->GetPlatformManager()->IsDone(m_JobID)) {
			m_VBO->Unmap();
			m_IBO->Unmap();
			m_Mesh = FMegaObjectFactory::GenPlatformObject(m_VBO, m_IBO, m_NumIndices);
		}

		if (m_Mesh != nullptr) {
			m_FMegaScene->GetRenderer()->RenderPlatform(m_Mesh, m_GlobalTransform, m_Color);
		}
	}
}
