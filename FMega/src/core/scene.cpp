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

		std::unordered_map<Entity*, int> deleted;
		int id = 0;
		bool done;
		do {
			done = true;
			for (size_t i = 0; i < m_Entities.size(); i++) {
				if (m_Entities[i]->IsDestroyed() || deleted.find(m_Entities[i]->GetParent()) != deleted.end()) {
					deleted[m_Entities[i]] = id++;
					done = false;
					if (i + 1 != m_Entities.size())
						m_Entities[i] = m_Entities.back();
					m_Entities.pop_back();
					i--;
				}
			}
		} while (!done);

		if (!deleted.empty()) {
			std::vector<std::pair<int, Entity*>> deletedOrder;
			for (auto p : deleted) {
				deletedOrder.push_back(std::make_pair(p.second, p.first));
			}
			std::sort(deletedOrder.begin(), deletedOrder.end(), std::greater<std::pair<int, Entity*>>());
			for (auto p : deletedOrder) {
				delete p.second;
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