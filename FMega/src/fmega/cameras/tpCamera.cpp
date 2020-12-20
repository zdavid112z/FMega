#include "fmega.h"
#include "tpCamera.h"
#include "fmega/fmegaScene.h"
#include "../entities/player.h"

namespace fmega {

	TPCamera::TPCamera(FMegaScene* scene, Player* player, float fov) :
		GameCamera(scene, player, fov)
	{
		m_LocalTransform.position = glm::vec3(0, 6, 1);
		m_LocalTransform.rotation = glm::vec3(-glm::radians(30.f), 0, 0);
		m_FollowSpeed = 9.f;
	}

	TPCamera::~TPCamera()
	{
		
	}

	void TPCamera::Update(float delta)
	{
		m_LocalTransform.position.x = glm::lerp(
			m_LocalTransform.position.x, 
			m_Player->GetLocalTransform().position.x, 
			delta * m_FollowSpeed);

		GameCamera::Update(delta);
	}

	void TPCamera::OnRefocus() {
		GameCamera::OnRefocus();
		m_LocalTransform.position.x = m_Player->GetLocalTransform().position.x;
	}

	void TPCamera::OnFocusLost() {
		GameCamera::OnFocusLost();
	}

}
