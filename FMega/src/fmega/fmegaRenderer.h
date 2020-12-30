#pragma once

#include "graphics/shader.h"
#include "graphics/mesh.h"
#include "graphics/texture2d.h"
#include "graphics/gpuBuffer.h"
#include "fmegaObjectFactory.h"

#define NUM_POINT_LIGHTS 4
#define NUM_SPOT_LIGHTS  4

namespace fmega {

	class FMegaScene;

	#pragma pack(push,1)
	struct PointLight {
		glm::vec4 position;
		glm::vec4 color;
		glm::vec4 atten;
	};

	struct SpotLight {
		glm::vec4 position;
		glm::vec4 color;
		glm::vec4 atten;
		glm::vec4 direction;
		float innerCutoff;
		float outerCutoff;
		float unused1;
		float unused2;
	};

	struct SceneBuffer {
		glm::mat4 viewProjection;
		glm::mat4 invViewProjection;
		glm::vec3 eyePosition;
		float targetZ;
		float cameraOffset;
		float unused1;
		float unused2;
		float unused3;
		PointLight pointLights[NUM_POINT_LIGHTS];
		SpotLight spotLights[NUM_SPOT_LIGHTS];
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

	enum class MeshType : uint {
		MESH_3D,
		MESH_2D,
		COUNT,
		INVALID,
	};

	struct QueueKey {
		bool ui = false;
		bool transparent = false;
		MeshType type = MeshType::INVALID;
		Mesh* mesh = nullptr;
		Texture2D* albedo = nullptr;
		Texture2D* normal = nullptr;
		Texture2D* roughness = nullptr;
		Texture2D* metalness = nullptr;

		bool operator<(const QueueKey& k) const {
			if (ui != k.ui) {
				return ui < k.ui;
			}
			if (transparent != k.transparent) {
				return transparent < k.transparent;
			}
			return int(type) < int(k.type);
		}
	};

	class FMegaRenderer {
	public:
		using QueueType = std::map<QueueKey, std::vector<MeshRenderData>>;

		FMegaRenderer(FMegaScene* scene, float playerRadius);
		~FMegaRenderer();

		void Prepare(float delta, float skyboxOffset);
		void RenderMesh(Mesh* mesh, MeshRenderData data, bool isUI = false, MeshType type = MeshType::MESH_2D, bool isTransparent = false);
		void RenderMesh(QueueKey key, MeshRenderData data);
		void RenderDigit(Mesh* segment, MeshRenderData data, int digit);
		void RenderAll();
		void RenderPlatform(Mesh* mesh, const glm::mat4& model, const glm::vec4& color);
		void RenderPlayer(const glm::mat4& model, float height, float animTime);
		void RenderSkybox();
		void SetShake(bool shake);

	private:
		void RenderMesh(QueueType::iterator&, bool bindShader);
		void BindShaderAndTextures(QueueKey key);

	public:
		float m_ShakeTime;
		bool m_Shake;
		glm::mat4 m_ShakeMatrix = glm::mat4(1.f);
		FMegaScene* m_Scene;
		GPUBuffer* m_UIBuffer;
		GPUBuffer* m_DynamicSceneBuffer;
		GPUBuffer* m_StaticPlayerBuffer;
		Shader* m_SkyboxShader;
		Shader* m_PlatformShader;
		Shader* m_PlayerShader;
		Shader* m_Shaders[uint(MeshType::COUNT)];
		QueueType m_Queue;
	};

}