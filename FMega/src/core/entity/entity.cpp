#include "fmega.h"
#include "entity.h"

namespace fmega {

	void Entity::ForceUpdateGlobalModel()
	{
		if (m_Parent) {
			m_Parent->UpdateGlobalModel();
			glm::mat4 parentModel = m_Parent->GetGlobalModel();
			m_GlobalTransform = parentModel * m_LocalTransform.GetModelMatrix();
		}
		else {
			m_GlobalTransform = m_LocalTransform.GetModelMatrix();
		}
	}

	void Entity::UpdateGlobalModel()
	{
		if (m_GlobalTransform == glm::mat4(0)) {
			ForceUpdateGlobalModel();
		}
	}

	void Entity::ClearGlobalModel()
	{
		m_GlobalTransform = glm::mat4(0);
	}

	void Entity::Destroy()
	{
		m_Destroyed = true;
	}

	bool Entity::IsDestroyed()
	{
		return m_Destroyed;
	}

}
