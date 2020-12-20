#pragma once

#include "fmega.h"
#include "graphics/transform.h"

namespace fmega {

	class Scene;

	class Entity {
	public:
		Entity(const std::string& name, Entity* parent, Scene* scene) :
			m_Name(name),
			m_Parent(parent),
			m_Scene(scene) {}
		virtual ~Entity() {}

		virtual void Render(float delta) = 0;
		virtual void Update(float delta) = 0;

		void ForceUpdateGlobalModel();
		void UpdateGlobalModel();
		void ClearGlobalModel();

		void Destroy();
		bool IsDestroyed();

		glm::mat4 GetGlobalModel() const { return m_GlobalTransform; }
		const Transform& GetLocalTransform() const { return m_LocalTransform; }
		Transform& GetLocalTransform() { return m_LocalTransform; }
		const std::string& GetName() { return m_Name; }
		Scene* GetScene() { return m_Scene; }
		Entity* GetParent() { return m_Parent; }
	protected:
		bool m_Destroyed = false;
		Scene* m_Scene;
		Entity* m_Parent;
		std::string m_Name;
		Transform m_LocalTransform;
		glm::mat4 m_GlobalTransform = glm::mat4(0);
	};

}