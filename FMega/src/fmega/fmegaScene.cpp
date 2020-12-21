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
		if (m_PlatformManager != nullptr) {
			delete m_PlatformManager;
		}
		m_PlatformManager = new PlatformManager();

		for (auto e : m_Entities) {
			delete e;
		}
		m_Entities.clear();

		m_Skybox = new Skybox("Skybox", nullptr, this);
		AddEntity(m_Skybox);
		AddEntity(new LevelManager("Manager", nullptr, this));
		AddEntity(new Player("Player", nullptr, this, m_PlayerRadius));
	}

	FMegaScene::~FMegaScene()
	{
		delete m_Rewind;
		delete m_PlatformManager;
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
		m_RestartManager->Render(delta);
		m_Renderer->RenderAll();

		m_PlatformManager->Update(delta);
	}

}