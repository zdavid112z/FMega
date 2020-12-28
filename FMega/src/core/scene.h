#pragma once

#include "fmega.h"
#include "entity/entity.h"
#include "graphics/camera.h"

namespace fmega {

	class Game;

	class Scene {
	public:
		Scene(Game* game);
		virtual ~Scene();

		virtual void Render(float delta);
		virtual void Update(float delta);

		Game* GetGame() { return m_Game; }
		Camera* GetCamera() { return m_Camera; }
		void SetCamera(Camera* camera) { m_Camera = camera; }

		Entity* GetEntity(const std::string& name);
		void AddEntity(Entity* e);
		void RemoveEntity(const std::string& name);
		void ForeachEntity(std::function<void(Entity*)> func);
	protected:
		Game* m_Game;
		std::vector<Entity*> m_Entities;
		std::vector<Entity*> m_EntitiesToAdd;
		Camera* m_Camera = nullptr;
	};

}