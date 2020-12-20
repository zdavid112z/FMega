#include "fmega.h"
#include "fpCamera.h"
#include "fmega/fmegaScene.h"
#include "../entities/player.h"
#include "core/game.h"

namespace fmega {

	FPCamera::FPCamera(FMegaScene* scene, Player* player, float fov) :
		GameCamera(scene, player, fov)
	{
		m_LocalTransform.position = player->GetLocalTransform().position;

		m_Sensitivity = glm::vec2(2.f);
		m_PitchLimits = glm::vec2(-glm::half_pi<float>(), glm::half_pi<float>());
		m_YawLimits = glm::vec2(-glm::half_pi<float>(), glm::half_pi<float>());

		m_Listener = m_FMegaScene->GetGame()->GetEventManager()->AddEventListener(
			EventType::MOUSE_MOTION, EventListener([this](Event e) {
				MouseMotionEventData* data = (MouseMotionEventData*)e.data;
				m_LocalTransform.rotation.x -= data->offset.y * m_Sensitivity.x;
				m_LocalTransform.rotation.y -= data->offset.x * m_Sensitivity.y;
				m_LocalTransform.rotation.x = glm::clamp(m_LocalTransform.rotation.x, m_PitchLimits.x, m_PitchLimits.y);
				m_LocalTransform.rotation.y = glm::clamp(m_LocalTransform.rotation.y, m_YawLimits.x, m_YawLimits.y);
				return false;
		}));
	}

	FPCamera::~FPCamera()
	{
		m_FMegaScene->GetGame()->GetEventManager()->RemoveEventListener(EventType::MOUSE_MOTION, m_Listener);
	}

	void FPCamera::Update(float delta)
	{
		m_LocalTransform.position = m_Player->GetLocalTransform().position;
		GameCamera::Update(delta);
	}

	void FPCamera::OnRefocus() {
		GameCamera::OnRefocus();
		m_LocalTransform.rotation = glm::vec3(0);
		m_FMegaScene->GetGame()->GetDisplay()->SetCursorGrabbed(true);
	}

	void FPCamera::OnFocusLost() {
		GameCamera::OnFocusLost();
		m_FMegaScene->GetGame()->GetDisplay()->SetCursorGrabbed(false);
	}
}
