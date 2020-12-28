#pragma once

#include "core/scene.h"
#include "graphics/mesh.h"
#include "graphics/shader.h"
#include "graphics/gpuBuffer.h"
#include "entities/collision.h"
#include "restartManager.h"
#include "entities/skybox.h"
#include "platformManager.h"
#include "rewindManager.h"
#include "graphics/texture2d.h"

namespace fmega {

	class FMegaRenderer;
	class Particles;

	class FMegaScene : public Scene {
	public:
		FMegaScene(Game* game);
		virtual ~FMegaScene();

		void UpdateGlobalTransforms();
		virtual void Update(float delta) override;
		virtual void Render(float delta) override;

		FMegaRenderer* GetRenderer() { return m_Renderer; }
		PlatformManager* GetPlatformManager() { return m_PlatformManager; }
		RewindManager* GetRewind() { return m_Rewind; }

		Texture2D* PlatformAlbedo;
		Texture2D* PlatformNormalmap;
		Texture2D* PlatformRoughness;
		Texture2D* PlatformMetalness;

		Mesh* BoxMesh;
		Mesh* SegmentMesh;
		Mesh* SphereMesh;
		Mesh* PickupMesh;
		float MoveSpeed;
		Camera* UICamera;
	protected:
		void Restart();
	protected:
		bool m_SlowDeath;
		float m_PlayerRadius;
		FMegaRenderer* m_Renderer;
		RestartManager* m_RestartManager;
		PlatformManager* m_PlatformManager;
		Skybox* m_Skybox = nullptr;
		RewindManager* m_Rewind;
	};

}