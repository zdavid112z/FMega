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
#include "fmegaAssets.h"

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
		FMegaAssets* GetAssets() { return m_Assets; }

		float MoveSpeed;
		float TargetZ;
		Camera* UICamera;
		bool SlowDeath;
	protected:
		void Restart();
	protected:
		float m_PlayerRadius;
		FMegaRenderer* m_Renderer;
		RestartManager* m_RestartManager;
		PlatformManager* m_PlatformManager;
		Skybox* m_Skybox = nullptr;
		RewindManager* m_Rewind;
		FMegaAssets* m_Assets;
	};

}