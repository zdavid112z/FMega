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

		m_ShakeType = ShakeType::NONE;
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
			QueueValue v;
			v.data = { data };
			q[key] = v;
			it = q.find(key);
		}
		else {
			it->second.data.push_back(data);
		}
	}

	void FMegaRenderer::RenderMesh2D(Mesh* mesh, MeshRenderData data, bool isUI, bool isTransparent, Texture2D* texture) {
		QueueKey key;
		key.mesh = mesh;
		key.ui = isUI;
		key.transparent = isTransparent;
		key.type = MeshType::MESH_2D;
		QueueType& q = m_Queue;
		if (!isUI) {
			data.model = m_ShakeMatrix * data.model;
		}

		auto it = q.find(key);
		
		if (it == q.end()) {
			QueueValue v;
			data.textureID = 0;
			v.data = { data };
			if (texture != nullptr)
				v.textures.push_back(texture);
			q[key] = v;
			it = q.find(key);
		}
		else {
			if (texture == nullptr) {
				data.textureID = 0;
			}
			else {
				auto& textures = it->second.textures;
				auto itt = std::find(textures.begin(), textures.end(), texture);
				data.textureID = float(itt - textures.begin());
				if (itt == it->second.textures.end()) {
					textures.push_back(texture);
				}
			}
			it->second.data.push_back(data);
		}
	}

	void FMegaRenderer::RenderAll()
	{
		MeshType prevType = MeshType::INVALID;

		for (auto it = m_Queue.begin(); it != m_Queue.end(); it++) {
			if (it->first.ui && !m_DrawUI) {
				continue;
			}
			BindShaderAndTextures(it->first, it->second);
			RenderMesh(it, false);
		}
	}

	void FMegaRenderer::BindShaderAndTextures(QueueKey key, QueueValue& val) {
		int i = int(key.type);
		m_Shaders[i]->Bind();
		if (key.type == MeshType::MESH_2D) {
			m_Shaders[i]->SetBuffer("UIBuffer", m_UIBuffer);
			for (int j = 0; j < (int)val.textures.size(); j++) {
				m_Shaders[i]->SetTexture("uTextures[" + StringUtils::ToString(j) + "]", val.textures[j]);
			}
		}
		else if (key.type == MeshType::MESH_3D) {
			m_Shaders[i]->SetBuffer("DynamicSceneBuffer", m_DynamicSceneBuffer);
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
	}

	void FMegaRenderer::RenderMesh(QueueType::iterator& it, bool bindShader)
	{
		MeshType type = it->first.type;
		glEnable(GL_DEPTH_TEST);
		if (type == MeshType::MESH_2D) {
			glDepthMask(GL_FALSE);
		}
		else {
			glDepthMask(GL_TRUE);
		}
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		auto& key = it->first;
		auto& data = it->second;

		if (data.data.empty()) {
			return;
		}

		if (bindShader) {
			BindShaderAndTextures(key, data);
		}

		int batchSize = it->first.mesh->GetVBO(1)->GetSize() / sizeof(MeshRenderData);
		int remainingMeshes = it->second.data.size();
		int numBatches = (it->second.data.size() + batchSize - 1) / batchSize;
		for (int i = 0; i < numBatches; i++) {
			int currentStart = i * batchSize;
			int currentSize = glm::min(batchSize, remainingMeshes);
			remainingMeshes -= currentSize;

			key.mesh->GetVBO(1)->SetSubdata((byte*)(&data.data[currentStart]), currentSize * sizeof(MeshRenderData), 0);
			key.mesh->DrawNow(currentSize);
		}

		data.data.clear();
		data.textures.clear();
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

	void FMegaRenderer::SetDrawUI(bool b) {
		m_DrawUI = b;
	}

	void FMegaRenderer::SetShake(ShakeType type, float duration) {
		if (int(type) <= int(m_ShakeType) && type != ShakeType::NONE) {
			return;
		}
		if (m_ShakeType != ShakeType::DECAYING || type != ShakeType::BUILDUP) {
			switch (type)
			{
			case ShakeType::NONE:
			case ShakeType::BUILDUP:
				m_ShakeTime = 0;
				break;
			case ShakeType::DECAYING:
				m_ShakeTime = duration;
				break;
			default:
				break;
			}
		}
		m_ShakeType = type;
	}

	void FMegaRenderer::Prepare(float delta)
	{
		SceneBuffer bufferData;
		bufferData.targetZ = m_Scene->TargetZ;
		glm::mat4 view = m_Scene->GetCamera()->view;
		switch (m_ShakeType)
		{
		case ShakeType::NONE:
		case ShakeType::DECAYING:
			m_ShakeTime = glm::max(0.f, m_ShakeTime - delta);
			break;
		case ShakeType::BUILDUP:
			m_ShakeTime += delta;
			break;
		default:
			break;
		}
		if (m_ShakeTime == 0) {
			m_ShakeType = ShakeType::NONE;
		}
		if (m_ShakeType != ShakeType::NONE) {
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
		
		std::vector<PointLight> pointLights;
		std::vector<SpotLight> spotLights;
		m_Scene->ForeachEntity([this, &pointLights, &spotLights](Entity* e) {
			if (StringUtils::StartsWith(e->GetName(), "Light_")) {
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
				memset(&bufferData.pointLights[i], 0, sizeof(PointLight));
				bufferData.pointLights[i].atten.x = 1.f;
			}
			else {
				bufferData.pointLights[i] = pointLights[i];
			}
		}

		for (int i = 0; i < NUM_SPOT_LIGHTS; i++) {
			if (i >= (int)spotLights.size()) {
				memset(&bufferData.spotLights[i], 0, sizeof(SpotLight));
				bufferData.spotLights[i].atten.x = 1.f;
			}
			else {
				bufferData.spotLights[i] = spotLights[i];
			}
		}

		m_DynamicSceneBuffer->SetSubdata((byte*)&bufferData, sizeof(bufferData), 0);

		UIBuffer uibuffer;
		uibuffer.viewProjection = m_Scene->UICamera->viewProjection;
		uibuffer.viewProjection3d = bufferData.viewProjection;
		m_UIBuffer->SetSubdata((byte*)&uibuffer, sizeof(UIBuffer), 0);
	}

	void FMegaRenderer::RenderPlatform(Mesh* mesh, const glm::mat4& model, const glm::vec3& color, float destructZ) {
		glm::vec3 pos = glm::vec3(model * glm::vec4(0, 0, 0, 1));
		if (pos.z < -85.f) {
			return;
		}
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		m_PlatformShader->Bind();
		DynamicPlatformBuffer buffer;
		buffer.model = model;
		buffer.color = glm::vec4(color, destructZ);
		m_PlatformShader->SetDynamicObjectBuffer("DynamicObjectBuffer", (byte*)&buffer);
		m_PlatformShader->SetBuffer("DynamicSceneBuffer", m_DynamicSceneBuffer);

		m_PlatformShader->SetTexture("uAlbedoMap", m_Scene->GetAssets()->PlatformAlbedo);
		m_PlatformShader->SetTexture("uNormalMap", m_Scene->GetAssets()->PlatformNormalmap);
		m_PlatformShader->SetTexture("uRoughnessMap", m_Scene->GetAssets()->PlatformRoughness);
		m_PlatformShader->SetTexture("uMetalnessMap", m_Scene->GetAssets()->PlatformMetalness);

		mesh->DrawNow();
		glDisable(GL_BLEND);
	}

	void FMegaRenderer::RenderSkybox() {
		glDepthMask(GL_FALSE);
		m_SkyboxShader->Bind();
		m_SkyboxShader->SetBuffer("DynamicSceneBuffer", m_DynamicSceneBuffer);
		m_Scene->GetAssets()->BoxMesh->DrawNow();
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
		m_PlayerShader->SetTexture("uAlbedoMap", m_Scene->GetAssets()->PlayerAlbedo);
		m_PlayerShader->SetTexture("uNormalMap", m_Scene->GetAssets()->PlayerNormalmap);
		m_PlayerShader->SetTexture("uRoughnessMap", m_Scene->GetAssets()->PlayerRoughness);
		m_PlayerShader->SetTexture("uMetalnessMap", m_Scene->GetAssets()->PlayerMetalness);
		m_Scene->GetAssets()->SphereMesh->DrawNow();
	}

}