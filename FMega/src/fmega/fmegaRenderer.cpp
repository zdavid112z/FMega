#include "fmega.h"
#include "fmegaRenderer.h"
#include "fmegaScene.h"
#include "utils/random.h"

namespace fmega {

	FMegaRenderer::FMegaRenderer(FMegaScene* scene, float playerRadius) :
		m_Scene(scene)
	{
		m_SimpleShader = new Shader("assets/shaders/simple.vert", "assets/shaders/simple.frag", sizeof(glm::mat4));
		m_PlatformShader = new Shader("assets/shaders/platform.vert", "assets/shaders/platform.frag", sizeof(DynamicPlatformBuffer));
		m_PlayerShader = new Shader("assets/shaders/player.vert", "assets/shaders/player.frag", sizeof(DynamicPlayerBuffer));
		m_SkyboxShader = new Shader("assets/shaders/skybox.vert", "assets/shaders/skybox.frag", 0);
		m_DynamicSceneBuffer = new GPUBuffer(BufferType::UNIFORM, nullptr, sizeof(SceneBuffer), BufferUsage::DYNAMIC_DRAW);
		m_StaticPlayerBuffer = new GPUBuffer(BufferType::UNIFORM, nullptr, sizeof(StaticPlayerBuffer), BufferUsage::DYNAMIC_DRAW);
		m_UIBuffer = new GPUBuffer(BufferType::UNIFORM, nullptr, sizeof(UIBuffer), BufferUsage::DYNAMIC_DRAW);

		StaticPlayerBuffer buffer;
		buffer.radius = playerRadius;
		m_StaticPlayerBuffer->SetSubdata((byte*)&buffer, sizeof(StaticPlayerBuffer), 0);

		UIBuffer uibuffer;
		uibuffer.viewProjection = scene->UICamera->viewProjection;
		m_UIBuffer->SetSubdata((byte*)&uibuffer, sizeof(UIBuffer), 0);

		m_Shake = false;
		m_ShakeTime = 0;
	}

	FMegaRenderer::~FMegaRenderer()
	{
		delete m_UIBuffer;
		delete m_StaticPlayerBuffer;
		delete m_SkyboxShader;
		delete m_PlayerShader;
		delete m_SimpleShader;
		delete m_PlatformShader;
		delete m_DynamicSceneBuffer;
	}

	void FMegaRenderer::RenderMesh(Mesh* mesh, MeshRenderData data, bool isUI)
	{
		if (!isUI) {
			data.model = m_ShakeMatrix * data.model;
		}
		auto key = std::make_pair(isUI, mesh);
		auto it = m_Queue.find(key);
		if (it == m_Queue.end()) {
			m_Queue[key] = { data };
			it = m_Queue.find(key);
		}
		else {
			it->second.push_back(data);
		}
		int batchSize = it->first.second->GetVBO(1)->GetSize() / sizeof(MeshRenderData);
		if (it->second.size() == batchSize) {
			RenderMesh(it, true);
		}
	}

	void FMegaRenderer::RenderAll()
	{
		m_SimpleShader->Bind();
		m_SimpleShader->SetBuffer("UIBuffer", m_UIBuffer);

		for (auto it = m_Queue.begin(); it != m_Queue.end(); it++) {
			RenderMesh(it, false);
		}
	}

	void FMegaRenderer::RenderMesh(QueueType::iterator& it, bool bindShader)
	{
		glDisable(GL_DEPTH_TEST);
		auto& key = it->first;
		auto& data = it->second;

		if (data.empty()) {
			return;
		}

		if (bindShader) {
			m_SimpleShader->Bind();
			m_SimpleShader->SetBuffer("UIBuffer", m_UIBuffer);
		}

		key.second->GetVBO(1)->SetSubdata((byte*)data.data(), data.size() * sizeof(MeshRenderData), 0);
		key.second->DrawNow(data.size());

		data.clear();
		glEnable(GL_DEPTH_TEST);
	}

	void FMegaRenderer::RenderDigit(Mesh* segment, MeshRenderData data, int digit) {
		glm::vec3 positions[] = {
			glm::vec3(0, 1, 0),
			glm::vec3(0.5f, 0.5f, 0),
			glm::vec3(0.5f, -0.5f, 0),
			glm::vec3(0, -1, 0),
			glm::vec3(-0.5f, -0.5f, 0),
			glm::vec3(-0.5f, 0.5f, 0),
			glm::vec3(0, 0, 0),
		};
		bool rotated[] = {
			0, 1, 1, 0, 1, 1, 0
		};
		bool segmentsOn[10][7] = {
			{ 1, 1, 1, 1, 1, 1, 0 },
			{ 0, 1, 1, 0, 0, 0, 0 },
			{ 1, 1, 0, 1, 1, 0, 1 },
			{ 1, 1, 1, 1, 0, 0, 1 },
			{ 0, 1, 1, 0, 0, 1, 1 },
			{ 1, 0, 1, 1, 0, 1, 1 },
			{ 1, 0, 1, 1, 1, 1, 1 },
			{ 1, 1, 1, 0, 0, 0, 0 },
			{ 1, 1, 1, 1, 1, 1, 1 },
			{ 1, 1, 1, 1, 0, 1, 1 },
		};
		float scale = 0.4f;
		glm::mat4 rot90 = glm::rotate(glm::mat4(1), glm::half_pi<float>(), glm::vec3(0, 0, 1));
		glm::mat4 originalModel = data.model;
		glm::vec4 visibleOpacity = data.opacity;
		glm::vec4 invisibleOpacity = data.opacity * 0.1f;
		for (int i = 0; i < 7; i++) {
			glm::mat4 model = glm::translate(glm::mat4(1), positions[i]);
			if (rotated[i]) {
				model = model * rot90;
			}
			model = glm::scale(model, glm::vec3(scale, scale, 1.f));
			data.model = originalModel * model;
			data.opacity = segmentsOn[digit][i] ? visibleOpacity : invisibleOpacity;
			RenderMesh(segment, data, true);
		}
	}

	void FMegaRenderer::SetShake(bool shake) {
		if (shake != m_Shake) {
			m_ShakeTime = 0;
		}
		m_Shake = shake;
	}

	void FMegaRenderer::Prepare(float delta, float skyboxOffset)
	{
		SceneBuffer bufferData;
		bufferData.targetZ = -15.f;
		glm::mat4 view = m_Scene->GetCamera()->view;
		if (m_Shake) {
			m_ShakeTime += delta;
			float angle1 = Random::NextFloat(glm::vec2(0, glm::two_pi<float>()));
			float angle2 = Random::NextFloat(glm::vec2(0, glm::two_pi<float>()));
			float amount = m_ShakeTime * 1.f + 0.1f;
			m_ShakeMatrix = glm::translate(glm::mat4(1), glm::vec3(cosf(angle2) * sinf(angle1), sinf(angle2), cosf(angle2) * cosf(angle2)) * amount);
			view = m_ShakeMatrix * view;
		}
		glm::mat4 projection = m_Scene->GetCamera()->projection;
		bufferData.viewProjection = projection * view;
		view[3] = glm::vec4(0, 0, 0, 1);
		bufferData.invViewProjection = glm::inverse(projection * view);
		bufferData.eyePosition = m_Scene->GetCamera()->position;
		bufferData.cameraOffset = skyboxOffset;
		m_DynamicSceneBuffer->SetSubdata((byte*)&bufferData, sizeof(bufferData), 0);
	}

	void FMegaRenderer::RenderPlatform(Mesh* mesh, const glm::mat4& model, const glm::vec4& color) {
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		m_PlatformShader->Bind();
		DynamicPlatformBuffer buffer;
		buffer.model = model;
		buffer.color = color;
		m_PlatformShader->SetDynamicObjectBuffer("DynamicObjectBuffer", (byte*)&buffer);
		m_PlatformShader->SetBuffer("DynamicSceneBuffer", m_DynamicSceneBuffer);
		mesh->DrawNow();
		glDisable(GL_BLEND);
	}

	void FMegaRenderer::RenderSkybox() {
		glDepthMask(GL_FALSE);
		m_SkyboxShader->Bind();
		m_SkyboxShader->SetBuffer("DynamicSceneBuffer", m_DynamicSceneBuffer);
		m_Scene->BoxMesh->DrawNow();
		glDepthMask(GL_TRUE);
	}

	void FMegaRenderer::RenderPlayer(const glm::mat4& model, float height, float animTime) {
		m_PlayerShader->Bind();
		DynamicPlayerBuffer buffer;
		buffer.model = model;
		buffer.height = height;
		buffer.animTime = animTime;
		m_PlayerShader->SetDynamicObjectBuffer("DynamicObjectBuffer", (byte*)&buffer);
		m_PlayerShader->SetBuffer("DynamicSceneBuffer", m_DynamicSceneBuffer);
		m_PlayerShader->SetBuffer("StaticBuffer", m_StaticPlayerBuffer);
		m_Scene->SphereMesh->DrawNow();
	}

}