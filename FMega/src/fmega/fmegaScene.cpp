#include "fmega.h"
#include "fmegaScene.h"
#include "graphics/shader.h"
#include "fmegaObjectFactory.h"
#include "fmegaRenderer.h"
#include "utils/random.h"
#include "entities/player.h"
#include "entities/levelManager.h"
#include "core/game.h"
#include "gameOverEvent.h"
#include "entities/light.h"
#include "entities/tunnel.h"
#include "entities/lamp.h"

namespace fmega {

	FMegaScene::FMegaScene(Game* game) : Scene(game)
	{
		auto gameOverEventType = m_Game->GetEventManager()->RegisterEventType("GameOver");

		SlowDeath = false;
		MoveSpeed = 15.f;
		TargetZ = -15.f;

		m_PlayerRadius = 0.5f;

		m_PlatformManager = nullptr;
		UICamera = new Camera();
		UICamera->viewProjection = glm::ortho(-16.f, 16.f, -9.f, 9.f, -1000.f, 1000.f);

		/*PlatformAlbedo = new Texture2D("assets/textures/floor_tiles_06_diff_1k_metal.png", 3, TextureFilter::LINEAR, TextureWrap::REPEAT, true);
		PlatformNormalmap = new Texture2D("assets/textures/floor_tiles_06_nor_1k2.png", 3, TextureFilter::LINEAR, TextureWrap::REPEAT, true);
		PlatformRoughness = new Texture2D("assets/textures/floor_tiles_06_metal2.png", 3, TextureFilter::LINEAR, TextureWrap::REPEAT, true);
		PlatformMetalness = new Texture2D("assets/textures/floor_tiles_06_metal2.png", 3, TextureFilter::LINEAR, TextureWrap::REPEAT, true);*/

		PlatformAlbedo = new Texture2D("assets/textures/MetalPlates001_1K_Color.png", 3, TextureFilter::LINEAR, TextureWrap::REPEAT, true);
		PlatformNormalmap = new Texture2D("assets/textures/MetalPlates001_1K_Normal.png", 3, TextureFilter::LINEAR, TextureWrap::REPEAT, true);
		PlatformRoughness = new Texture2D("assets/textures/MetalPlates001_1K_Roughness.png", 3, TextureFilter::LINEAR, TextureWrap::REPEAT, true);
		PlatformMetalness = new Texture2D("assets/textures/MetalPlates001_1K_Metalness.png", 3, TextureFilter::LINEAR, TextureWrap::REPEAT, true);

		/*TunnelAlbedo = new Texture2D("assets/textures/Concrete_wall_02_1K_Base_Color.png", 3, TextureFilter::LINEAR, TextureWrap::REPEAT, true);
		TunnelNormalmap = new Texture2D("assets/textures/Concrete_wall_02_1K_Normal.png", 3, TextureFilter::LINEAR, TextureWrap::REPEAT, true);
		TunnelRoughness = new Texture2D("assets/textures/Concrete_wall_02_1K_Roughness.png", 3, TextureFilter::LINEAR, TextureWrap::REPEAT, true);*/

		TunnelAlbedo = new Texture2D("assets/textures/MetalPlates008_1K_Color.png", 3, TextureFilter::LINEAR, TextureWrap::REPEAT, true);
		TunnelNormalmap = new Texture2D("assets/textures/MetalPlates008_1K_Normal.png", 3, TextureFilter::LINEAR, TextureWrap::REPEAT, true);
		TunnelRoughness = new Texture2D("assets/textures/MetalPlates008_1K_Roughness.png", 3, TextureFilter::LINEAR, TextureWrap::REPEAT, true);
		TunnelMetalness = new Texture2D("assets/textures/MetalPlates008_1K_Metalness.png", 3, TextureFilter::LINEAR, TextureWrap::REPEAT, true);

		LampMesh = FMegaObjectFactory::GenLamp(4);
		TunnelMesh = FMegaObjectFactory::GenTunnel(1);
		BoxMesh = FMegaObjectFactory::GenBox(1024);
		SegmentMesh = FMegaObjectFactory::GenSegment(64);
		SphereMesh = FMegaObjectFactory::GenSphere(1);
		PickupMesh = FMegaObjectFactory::GenPickup(128);
		m_Renderer = new FMegaRenderer(this, m_PlayerRadius);
		m_Rewind = new RewindManager(this, 30, 4.f);

		EventListener l;
		l.function = [this](Event e) {
			// TODO: check if player has rewind
			GameOverEventData* data = (GameOverEventData*)e.data;
			SlowDeath = data->isSlow;
			m_RestartManager->OnPlayerLost(data->isSlow);
			return false;
		};
		m_Game->GetEventManager()->AddEventListener(gameOverEventType, l);

		m_RestartManager = new RestartManager(this);

		Restart();
	}

	void FMegaScene::Restart() {
		SlowDeath = false;
		for (auto e : m_Entities) {
			delete e;
		}
		m_Entities.clear();

		if (m_PlatformManager != nullptr) {
			delete m_PlatformManager;
		}
		m_PlatformManager = new PlatformManager();

		m_Skybox = new Skybox("Skybox", nullptr, this);
		AddEntity(m_Skybox);
		AddEntity(new Tunnel("Tunnel", nullptr, this));
		AddEntity(new LevelManager("Manager", nullptr, this));
		Player* player = new Player("Player", nullptr, this, m_PlayerRadius);
		AddEntity(player);

		glm::vec3 atten = glm::vec3(1.f, 0.1f, 0.001f);
		float intensity = 50.f;

		/*Light* mainLight1 = new Light("Main1", nullptr, this, glm::vec3(1, 1, 1), intensity, atten);
		mainLight1->GetLocalTransform().position = glm::vec3(5, 5, 2);
		AddEntity(mainLight1);

		Light* mainLight2 = new Light("Main2", nullptr, this, glm::vec3(1, 1, 1), intensity, atten);
		mainLight2->GetLocalTransform().position = glm::vec3(-5, 5, 2);
		AddEntity(mainLight2);

		Light* mainLight3 = new Light("Main3", nullptr, this, glm::vec3(1, 1, 1), intensity, atten);
		mainLight3->GetLocalTransform().position = glm::vec3(5, -5, 2);
		AddEntity(mainLight3);

		Light* mainLight4 = new Light("Main4", nullptr, this, glm::vec3(1, 1, 1), intensity, atten);
		mainLight4->GetLocalTransform().position = glm::vec3(-5, -5, 2);
		AddEntity(mainLight4);*/

		int numLamps = 16;
		float lampDist = 25.f;
		float lampResetZ = 80;
		for (int i = 0; i < numLamps; i++) {
			glm::vec3 m = glm::vec3(1);
			if (i % 3 == 0) {
				m = glm::vec3(1.2, 0.2, 0.1);
			}
			Lamp* lamp = new Lamp(std::to_string(i), nullptr, this, numLamps, lampDist, i, lampResetZ);
			AddEntity(lamp);
			Light* light = new Light("Lamp" + std::to_string(i), lamp, this, glm::vec3(1, 1, 1.4) * m, 20.f, glm::vec3(1.f, 0.f, 0.f), glm::vec3(-1, 0, 0), glm::vec2(45, 60));
			AddEntity(light);
			Light* lightp = new Light("Point" + std::to_string(i), lamp, this, glm::vec3(1, 1, 1.8) * m, 100.f, glm::vec3(1.f, 0.6f, 0.3f));
			lightp->GetLocalTransform().position.x = -1.5f;
			AddEntity(lightp);
		}
	}

	FMegaScene::~FMegaScene()
	{
		for (auto e : m_Entities) {
			delete e;
		}
		m_Entities.clear();

		if (m_PlatformManager != nullptr) {
			delete m_PlatformManager;
		}

		delete TunnelAlbedo;
		delete TunnelRoughness;
		delete TunnelNormalmap;
		delete TunnelMetalness;

		delete PlatformAlbedo;
		delete PlatformNormalmap;
		delete PlatformRoughness;
		delete PlatformMetalness;

		delete LampMesh;
		delete TunnelMesh;
		delete PickupMesh;
		delete m_Rewind;
		delete m_RestartManager;
		delete SphereMesh;
		delete BoxMesh;
		delete SegmentMesh;
		delete m_Renderer;
		delete UICamera;
	}

	void FMegaScene::UpdateGlobalTransforms() {
		for (Entity* e : m_Entities) {
			e->ClearGlobalModel();
		}

		for (Entity* e : m_Entities) {
			e->UpdateGlobalModel();
		}
	}

	void FMegaScene::Update(float delta)
	{
		if (m_RestartManager->ShouldRestart()) {
			m_Renderer->SetShake(ShakeType::NONE);
			Restart();
			m_Game->OnSceneRestarted();
			return;
		}

		UpdateGlobalTransforms();
		float adjDelta = m_RestartManager->GetAdjDelta(delta);
		m_Rewind->Update();
		if (!m_Rewind->IsRewinding()) {
			Scene::Update(adjDelta);
		}
		else {
			ForeachEntity([this, delta](Entity* ee) {
				FMegaEntity* e = (FMegaEntity*)ee;
				e->RewindUpdate(delta);
			});
		}
		m_RestartManager->Update(delta);
	}

	void FMegaScene::Render(float delta)
	{
		glEnable(GL_DEPTH_TEST);
		glDepthMask(GL_TRUE);
		glEnable(GL_CULL_FACE);
		glClearColor(0.1f, 0.15f, 0.3f, 1);
		bool noClearColor = m_RestartManager->HasLost() && SlowDeath;
		if (noClearColor) {
			glClear(GL_DEPTH_BUFFER_BIT);
		}
		else {
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		}
		if (m_RestartManager->HasLost() && SlowDeath) {
			m_Renderer->SetShake(ShakeType::BUILDUP);
		}

		float adjDelta = m_RestartManager->GetAdjDelta(delta);
		m_Renderer->Prepare(adjDelta, m_Skybox->Offset);
		if (!noClearColor) {
			m_Skybox->SkyboxRender(adjDelta);
		}
		Scene::Render(adjDelta);
		m_Renderer->RenderAll();
		m_RestartManager->Render(delta);
		m_Renderer->RenderAll();

		m_PlatformManager->Update(delta);
	}

}