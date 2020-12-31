#include "fmega.h"
#include "game.h"

namespace fmega {

	Game* Game::Instance = nullptr;

	Game::Game(const GameData& data)
	{
		Instance = this;
		m_EventManager = new EventManager();
		m_Display = new Display(data.displayData, m_EventManager);
		m_UpdateTimer.Reset();
	}

	Game::~Game()
	{
		if (m_Scene) {
			delete m_Scene;
		}
		delete m_EventManager;
		delete m_Display;
	}

	void Game::ChangeScene(SceneChangeSchedule schedule) {
		m_ShouldChangeScene = true;
		m_SceneChangeSchedule = schedule;
	}

	void Game::OnSceneRestarted() {
		InitNewScene();
	}

	void Game::Update()
	{
		/*if (m_ShouldChangeScene)
		{
			if (m_SceneChangeSchedule.fromXML)
				SceneLoader::LoadSceneToGame(this, m_SceneChangeSchedule.path);
			else SceneIO::LoadScene(this, m_SceneChangeSchedule.path);
			m_ShouldChangeScene = false;
			InitNewScene();
		}
		m_UpdateTimeElapsed += m_UpdateTimer.GetDelta();

		m_Display->PollEvents();
		m_Display->Prepare();

		while (m_UpdateTimeElapsed >= m_UpdateInterval)
		{
			m_UpdateTimeElapsed -= m_UpdateInterval;
			UpdateData ud;
			ud.deltaTime = m_UpdateInterval;
			ud.flags = EntitySystem::FLAG_LOGIC;

			PreLogicUpdate();
			m_Scene->UpdateSystems(ud);
			m_Display->GetInput()->PostLogicUpdate();
		}
		UpdateData ud;
		ud.deltaTime = m_Timer.GetDelta();
		ud.flags = EntitySystem::FLAG_CAMERAS;
		m_Scene->UpdateSystems(ud);
		m_Display->Present();
		m_Timer.Update();
		m_UpdateTimer.Update();*/

		if (m_ShouldChangeScene)
		{
			m_ShouldChangeScene = false;
			if (m_Scene)
				delete m_Scene;
			m_Scene = m_SceneChangeSchedule.genNewScene();
			InitNewScene();
		}

		m_Display->PollEvents();

		float delta = m_UpdateTimer.GetDelta();
		m_Time += delta;
		m_Scene->Update(delta);
		m_Scene->Render(delta);

		m_Display->GetInput()->PostLogicUpdate();

		m_Display->Present();
		m_UpdateTimer.Update();
	}

	void Game::InitNewScene()
	{
		m_UpdateTimer.Reset();
		m_Display->PollEvents();
		m_Scene->Update(m_UpdateTimer.GetDelta());
		m_Display->GetInput()->PostLogicUpdate();
	}

	void Game::DeleteCurrentScene()
	{
		/*m_ComponentManager->DeleteAll();
		if (m_Scene)
		{
			delete m_Scene;
			m_Scene = nullptr;
		}*/
	}

}
