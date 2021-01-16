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
#include "entities/bars.h"

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

		m_Assets = new FMegaAssets();
		m_Assets->LoadAll();

		m_Renderer = new FMegaRenderer(this, m_PlayerRadius);
		m_Rewind = new RewindManager(this, 30, 4.f);

		EventListener l;
		l.function = [this](Event e) {
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
		m_Rewind->DeleteHistory();
		SlowDeath = false;
		for (auto e : m_Entities) {
			delete e;
		}
		m_Entities.clear();

		if (m_PlatformManager != nullptr) {
			delete m_PlatformManager;
		}
		m_PlatformManager = new PlatformManager();

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

		int numLamps = 12;
		float lampDist = 30.f;
		float lampResetZ = 50;
		for (int i = 0; i < numLamps; i++) {
			glm::vec3 m = glm::vec3(1);
			if (i % 3 == 0) {
				m = glm::vec3(1.2, 0.2, 0.1);
			}
			Lamp* lamp = new Lamp(std::to_string(i), nullptr, this, numLamps, lampDist, i, lampResetZ);
			AddEntity(lamp);
			Light* light = new Light("Lamp" + std::to_string(i), lamp, this, glm::vec3(1, 1, 1.4) * m, 20.f, glm::vec3(1.f, 0.f, 0.f), glm::vec3(-1, 0, 0), glm::cos(glm::radians(glm::vec2(45, 60))));
			AddEntity(light);
			Light* lightp = new Light("Point" + std::to_string(i), lamp, this, glm::vec3(1, 1, 1.8) * m, 100.f, glm::vec3(1.f, 0.6f, 0.3f));
			lightp->GetLocalTransform().position.x = -1.5f;
			AddEntity(lightp);
		}
		AddEntity(new Bars("Bars", nullptr, this));
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

		m_Assets->DestroyAll();
		delete m_Assets;
		delete m_Rewind;
		delete m_RestartManager;
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
		glClearColor(1.f, 1.f, 1.f, 1.f);
		m_Assets->UpdateLazyAssets();
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
		m_Renderer->Prepare(adjDelta);
		if (!noClearColor) {
			glClearColor(1.f, 1.f, 1.f, 1.f);
		}
		Scene::Render(adjDelta);
		m_Renderer->RenderAll();
		m_RestartManager->Render(delta);
		m_Rewind->Render(delta);
		m_Renderer->RenderAll();

		m_PlatformManager->Update(delta);
	}

}