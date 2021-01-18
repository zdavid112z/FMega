#pragma once

#include "graphics/shader.h"
#include "graphics/mesh.h"
#include "graphics/texture2d.h"
#include "graphics/gpuBuffer.h"
#include "fmegaObjectFactory.h"

#define NUM_POINT_LIGHTS 24
#define NUM_SPOT_LIGHTS  24

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
		glm::mat4 viewProjection3d;
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

	enum class ShakeType {
		NONE = 0,
		DECAYING = 1,
		BUILDUP = 2,
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
			if (type != k.type) {
				return int(type) < int(k.type);
			}
			if (mesh != k.mesh) {
				return mesh < k.mesh;
			}
			
			if (albedo != k.albedo) {
				return albedo < k.albedo;
			}
			if (normal != k.normal) {
				return normal < k.normal;
			}
			if (roughness != k.roughness) {
				return roughness < k.roughness;
			}
			if (metalness != k.metalness) {
				return metalness < k.metalness;
			}

			return false;
		}
	};

	struct QueueValue {
		std::vector<MeshRenderData> data;
		std::vector<Texture2D*> textures;
	};

	class FMegaRenderer {
	public:
		using QueueType = std::map<QueueKey, QueueValue>;

		FMegaRenderer(FMegaScene* scene, float playerRadius);
		~FMegaRenderer();

		void Prepare(float delta);
		void RenderMesh(Mesh* mesh, MeshRenderData data, bool isUI = false, MeshType type = MeshType::MESH_2D, bool isTransparent = false);
		void RenderMesh2D(Mesh* mesh, MeshRenderData data, bool isUI = false, bool isTransparent = false, Texture2D* texture = nullptr);
		void RenderMesh(QueueKey key, MeshRenderData data);
		void RenderDigit(Mesh* segment, MeshRenderData data, int digit);
		void RenderAll();
		void RenderPlatform(Mesh* mesh, const glm::mat4& model, const glm::vec3& color, float destructZ);
		void RenderPlayer(const glm::mat4& model, float height, float animTime);
		void RenderSkybox();
		void SetShake(ShakeType type, float duration = 0.f);
		void SetDrawUI(bool);

	private:
		void RenderMesh(QueueType::iterator&, bool bindShader);
		void BindShaderAndTextures(QueueKey key, QueueValue& val);

	public:
		bool m_DrawUI = true;
		float m_ShakeTime;
		ShakeType m_ShakeType;
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