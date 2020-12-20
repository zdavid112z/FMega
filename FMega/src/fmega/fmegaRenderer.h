#pragma once

#include "graphics/shader.h"
#include "graphics/mesh.h"
#include "graphics/gpuBuffer.h"
#include "fmegaObjectFactory.h"

namespace fmega {

	class FMegaScene;

	#pragma pack(push,1)
	struct SceneBuffer {
		glm::mat4 viewProjection;
		glm::mat4 invViewProjection;
		glm::vec3 eyePosition;
		float targetZ;
		float cameraOffset;
	};

	struct DynamicPlayerBuffer {
		glm::mat4 model;
		float height;
		float animTime;
	};

	struct DynamicPlatformBuffer {
		glm::mat4 model;
		glm::vec4 color;
	};

	struct UIBuffer {
		glm::mat4 viewProjection;
	};

	struct StaticPlayerBuffer {
		float radius;
	};
	#pragma pack(pop)

	class FMegaRenderer {
	public:
		using QueueType = std::map< std::pair<bool, Mesh*>, std::vector<MeshRenderData> >;

		FMegaRenderer(FMegaScene* scene, float playerRadius);
		~FMegaRenderer();

		void Prepare(float delta);
		void RenderMesh(Mesh* mesh, MeshRenderData data, bool isUI = false);
		void RenderDigit(Mesh* segment, MeshRenderData data, int digit);
		void RenderAll();
		void RenderPlatform(Mesh* mesh, const glm::mat4& model, const glm::vec4& color);
		void RenderPlayer(const glm::mat4& model, float height, float animTime);
		void RenderSkybox();
		void SetShake(bool shake);

	private:
		void RenderMesh(QueueType::iterator&, bool bindShader);

	public:
		float m_CameraOffset;
		float m_ShakeTime;
		float m_PlayerRadius;
		bool m_Shake;
		glm::mat4 m_ShakeMatrix = glm::mat4(1.f);
		FMegaScene* m_Scene;
		GPUBuffer* m_UIBuffer;
		GPUBuffer* m_DynamicSceneBuffer;
		GPUBuffer* m_StaticPlayerBuffer;
		Shader* m_SkyboxShader;
		Shader* m_SimpleShader;
		Shader* m_PlatformShader;
		Shader* m_PlayerShader;
		QueueType m_Queue;
	};

}