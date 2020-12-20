#pragma once

#include "event/eventManager.h"
#include "graphics/display.h"
#include "scene.h"

namespace fmega {
	

	struct GameData
	{
		DisplayData displayData;
	};

	struct SceneChangeSchedule
	{
		std::function<Scene* ()> genNewScene;
	};

	class Game
	{
	public:
		static Game* Instance;
	public:
		Game(const GameData& data);
		virtual ~Game();

		virtual void Update();

		Display* GetDisplay() { return m_Display; }
		Scene* GetScene() { return m_Scene; }
		EventManager* GetEventManager() { return m_EventManager; }
		void ChangeScene(SceneChangeSchedule schedule) { m_ShouldChangeScene = true; m_SceneChangeSchedule = schedule; }
		void DeleteCurrentScene();
		virtual void InitNewScene();
		float GetTime() { return m_Time; }
	protected:
		Display* m_Display;
		EventManager* m_EventManager;
		Scene* m_Scene = nullptr;
		Timer m_UpdateTimer;
		bool m_ShouldChangeScene = false;
		SceneChangeSchedule m_SceneChangeSchedule;
		float m_Time = 0;
	};

}