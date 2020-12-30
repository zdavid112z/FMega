#include "fmega.h"
#include "fmegaRenderer.h"
#include "fmegaScene.h"
#include "utils/random.h"
#include "entities/light.h"

namespace fmega {

	FMegaRenderer::FMegaRenderer(FMegaScene* scene, float playerRadius) :
		m_Scene(scene)
	{
		m_Shaders[uint(MeshType::MESH_2D)] = new Shader("assets/shaders/simple.vert", "assets/shaders/simple.frag", sizeof(glm::mat4));
		m_Shaders[uint(MeshType::MESH_3D)] = new Shader("assets/shaders/simple3d.vert", "assets/shaders/simple3d.frag", sizeof(glm::mat4));
		m_PlatformShader = new Shader("assets/shaders/platform.vert", "assets/shaders/simple3d.frag", sizeof(DynamicPlatformBuffer));
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
		for (int i = 0; i < uint(MeshType::COUNT); i++) {
			delete m_Shaders[i];
		}
		delete m_PlatformShader;
		delete m_DynamicSceneBuffer;
	}

	void FMegaRenderer::RenderMesh(Mesh* mesh, MeshRenderData data, bool isUI, MeshType type, bool isTransparent)
	{
		QueueKey k;
		k.mesh = mesh;
		k.ui = isUI;
		k.transparent = isTransparent;
		k.type = type;
		RenderMesh(k, data);
	}

	void FMegaRenderer::RenderMesh(QueueKey key, MeshRenderData data) {
		QueueType& q = m_Queue;
		if (!key.ui) {
			data.model = m_ShakeMatrix * data.model;
		}
		auto it = q.find(key);
		if (it == q.end()) {
			q[key] = { data };
			it = q.find(key);
		}
		else {
			it->second.push_back(data);
		}
	}

	void FMegaRenderer::RenderAll()
	{
		MeshType prevType = MeshType::INVALID;

		for (auto it = m_Queue.begin(); it != m_Queue.end(); it++) {
			BindShaderAndTextures(it->first);
			RenderMesh(it, false);
		}
	}

	void FMegaRenderer::BindShaderAndTextures(QueueKey key) {
		int i = int(key.type);
		m_Shaders[i]->Bind();
		if (key.type == MeshType::MESH_2D) {
			m_Shaders[i]->SetBuffer("UIBuffer", m_UIBuffer);
		}
		else if (key.type == MeshType::MESH_3D) {
			m_Shaders[i]->SetBuffer("DynamicSceneBuffer", m_DynamicSceneBuffer);
		}
		if (key.albedo) {
			m_Shaders[i]->SetTexture("uAlbedoMap", key.albedo);
		}
		if (key.normal) {
			m_Shaders[i]->SetTexture("uNormalMap", key.normal);
		}
		if (key.roughness) {
			m_Shaders[i]->SetTexture("uRoughnessMap", key.roughness);
		}
		if (key.metalness) {
			m_Shaders[i]->SetTexture("uMetalnessMap", key.metalness);
		}
	}

	void FMegaRenderer::RenderMesh(QueueType::iterator& it, bool bindShader)
	{
		MeshType type = it->first.type;
		if (type == MeshType::MESH_2D)
			glDisable(GL_DEPTH_TEST);
		else glEnable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		auto& key = it->first;
		auto& data = it->second;

		if (data.empty()) {
			return;
		}

		if (bindShader) {
			BindShaderAndTextures(key);
		}

		int batchSize = it->first.mesh->GetVBO(1)->GetSize() / sizeof(MeshRenderData);
		int remainingMeshes = it->second.size();
		int numBatches = (it->second.size() + batchSize - 1) / batchSize;
		for (int i = 0; i < numBatches; i++) {
			int currentStart = i * batchSize;
			int currentSize = glm::min(batchSize, remainingMeshes);
			remainingMeshes -= currentSize;

			key.mesh->GetVBO(1)->SetSubdata((byte*)(&data[currentStart]), currentSize * sizeof(MeshRenderData), 0);
			key.mesh->DrawNow(currentSize);
		}

		data.clear();
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
		float visibleOpacity = data.opacity;
		float invisibleOpacity = data.opacity * 0.1f;
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
		
		std::vector<PointLight> pointLights;
		std::vector<SpotLight> spotLights;
		m_Scene->ForeachEntity([this, &pointLights, &spotLights](Entity* e) {
			if (StringUtils::StartsWith(e->GetName(), "Light")) {
				Light* l = (Light*)e;
				switch (l->GetLightType()) {
				case LightType::POINT:
					pointLights.push_back(l->GetPointLightData());
					break;
				case LightType::SPOT:
					spotLights.push_back(l->GetSpotLightData());
					break;
				}
			}
		});

		std::sort(pointLights.begin(), pointLights.end(), [](const PointLight& a, const PointLight& b) {
			return glm::length2(a.position) < glm::length2(b.position);
		});
		std::sort(spotLights.begin(), spotLights.end(), [](const SpotLight& a, const SpotLight& b) {
			return glm::length2(a.position) < glm::length2(b.position);
		});

		for (int i = 0; i < NUM_POINT_LIGHTS; i++) {
			if (i >= (int)pointLights.size()) {
				bufferData.pointLights[i].color = glm::vec4(0);
			}
			else {
				bufferData.pointLights[i] = pointLights[i];
			}
		}

		for (int i = 0; i < NUM_SPOT_LIGHTS; i++) {
			if (i >= (int)spotLights.size()) {
				bufferData.spotLights[i].color = glm::vec4(0);
			}
			else {
				bufferData.spotLights[i] = spotLights[i];
			}
		}

		m_DynamicSceneBuffer->SetSubdata((byte*)&bufferData, sizeof(bufferData), 0);
	}

	void FMegaRenderer::RenderPlatform(Mesh* mesh, const glm::mat4& model, const glm::vec4& color) {
		glm::vec3 pos = glm::vec3(model * glm::vec4(0, 0, 0, 1));
		if (pos.z < -85.f) {
			return;
		}
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		m_PlatformShader->Bind();
		DynamicPlatformBuffer buffer;
		buffer.model = model;
		buffer.color = color;
		m_PlatformShader->SetDynamicObjectBuffer("DynamicObjectBuffer", (byte*)&buffer);
		m_PlatformShader->SetBuffer("DynamicSceneBuffer", m_DynamicSceneBuffer);

		m_PlatformShader->SetTexture("uAlbedoMap", m_Scene->PlatformAlbedo);
		m_PlatformShader->SetTexture("uNormalMap", m_Scene->PlatformNormalmap);
		m_PlatformShader->SetTexture("uRoughnessMap", m_Scene->PlatformRoughness);
		m_PlatformShader->SetTexture("uMetalnessMap", m_Scene->PlatformMetalness);

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