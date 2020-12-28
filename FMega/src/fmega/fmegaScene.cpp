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

namespace fmega {

	FMegaScene::FMegaScene(Game* game) : Scene(game)
	{
		auto gameOverEventType = m_Game->GetEventManager()->RegisterEventType("GameOver");

		m_SlowDeath = false;
		MoveSpeed = 15.f;

		m_PlayerRadius = 0.5f;

		m_PlatformManager = nullptr;
		UICamera = new Camera();
		UICamera->viewProjection = glm::ortho(-16.f, 16.f, -9.f, 9.f, -1000.f, 1000.f);

		BoxMesh = FMegaObjectFactory::GenBox(1024);
		SegmentMesh = FMegaObjectFactory::GenSegment(64);
		SphereMesh = FMegaObjectFactory::GenSphere(1);
		PickupMesh = FMegaObjectFactory::GenPickup(128);
		m_Renderer = new FMegaRenderer(this, m_PlayerRadius);
		m_Rewind = new RewindManager(this, 180, 4.f);

		EventListener l;
		l.function = [this](Event e) {
			// TODO: check if player has rewind
			GameOverEventData* data = (GameOverEventData*)e.data;
			m_SlowDeath = data->isSlow;
			m_RestartManager->OnPlayerLost(data->isSlow);
			return false;
		};
		m_Game->GetEventManager()->AddEventListener(gameOverEventType, l);

		m_RestartManager = new RestartManager(this);

		Restart();
	}

	void FMegaScene::Restart() {
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
		AddEntity(new LevelManager("Manager", nullptr, this));
		Player* player = new Player("Player", nullptr, this, m_PlayerRadius);
		AddEntity(player);

		glm::vec3 atten = glm::vec3(1.f, 0.1f, 0.001f);
		float intensity = 10.f;

		Light* mainLight1 = new Light("Main1", nullptr, this, glm::vec3(1, 1, 1), intensity, atten);
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
		AddEntity(mainLight4);
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
			Restart();
		}

		UpdateGlobalTransforms();
		float adjDelta = m_RestartManager->GetAdjDelta(delta);
		m_Rewind->Update();
		if (!m_Rewind->IsRewinding()) {
			Scene::Update(adjDelta);
			if (GetGame()->GetDisplay()->GetInput()->WasKeyPressed(Key::R)) {
				if (m_Rewind->CanRewind()) {
					m_Rewind->StartRewind();
				}
			}
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
		bool noClearColor = m_RestartManager->HasLost() && m_SlowDeath;
		if (noClearColor) {
			glClear(GL_DEPTH_BUFFER_BIT);
		}
		else {
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		}
		m_Renderer->SetShake(m_RestartManager->HasLost() && m_SlowDeath);

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