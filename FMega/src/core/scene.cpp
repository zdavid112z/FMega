#include "fmega.h"
#include "scene.h"

namespace fmega {

	Scene::Scene(Game* game) : m_Game(game) {
	}

	Scene::~Scene() {
		for (auto e : m_Entities) {
			delete e;
		}
	}

	void Scene::Render(float delta) {
		for (auto e : m_Entities) {
			e->Render(delta);
		}
	}

	void Scene::Update(float delta) {
		for (size_t i = 0; i < m_Entities.size(); i++) {
			m_Entities[i]->Update(delta);
		}
		for (size_t i = 0; i < m_Entities.size(); i++) {
			if (m_Entities[i]->IsDestroyed()) {
				delete m_Entities[i];
				if (i + 1 != m_Entities.size())
					m_Entities[i] = m_Entities.back();
				m_Entities.pop_back();
				i--;
			}
		}
	}

	Entity* Scene::GetEntity(const std::string& name)
	{
		auto it = std::find_if(m_Entities.begin(), m_Entities.end(), [&](Entity* e) { return e->GetName() == name; });
		if (it != m_Entities.end()) {
			return *it;
		}
		return nullptr;
	}

	void Scene::AddEntity(Entity* e)
	{
		m_Entities.push_back(e);
		e->ForceUpdateGlobalModel();
	}

	void Scene::RemoveEntity(const std::string& name)
	{
		auto it = std::find_if(m_Entities.begin(), m_Entities.end(), [&](Entity* e) { return e->GetName() == name; });
		if (it != m_Entities.end()) {
			m_Entities.erase(it);
		}
	}

	void Scene::ForeachEntity(std::function<void(Entity*)> func)
	{
		for (auto e : m_Entities) {
			func(e);
		}
	}
}