#include "fmega.h"
#include "gameCamera.h"
#include "fmega/fmegaScene.h"
#include "core/game.h"
#include "../entities/player.h"

namespace fmega {

	GameCamera::GameCamera(FMegaScene* scene, Player* player, float fov) :
		m_FMegaScene(scene), m_Player(player), m_Fov(fov)
	{
		m_Aspect = m_FMegaScene->GetGame()->GetDisplay()->GetAspect();
		m_Projection = glm::perspective(m_Fov, m_Aspect, 0.01f, 1000.f);
		m_FovTarget = m_Fov;
		m_MinMaxSpeed = player->GetMinMaxSpeed();
		m_MinMaxFov = glm::vec2(60.f, 120.f);
		m_FovMoveSpeed = 5.f;
		m_Camera = new Camera();

		m_ResizeListener = m_FMegaScene->GetGame()->GetEventManager()->AddEventListener(EventType::RESIZE, EventListener(
		[this](Event e) {
			ScreenResizeEventData* data = (ScreenResizeEventData*)e.data;
			SetAspect(float(data->resolution.x) / data->resolution.y);
			return false;
		}));
	}

	GameCamera::~GameCamera()
	{
		m_FMegaScene->GetGame()->GetEventManager()->RemoveEventListener(EventType::RESIZE, m_ResizeListener);
		delete m_Camera;
	}

	void GameCamera::UpdateFov() {
		if (m_Player->IsSlowDying()) {
			m_FovTarget = glm::radians(179.f);
		}
		else {
			float amount = (m_FMegaScene->MoveSpeed - m_MinMaxSpeed.x) / (m_MinMaxSpeed.y - m_MinMaxSpeed.x);
			m_FovTarget = glm::radians(glm::lerp(m_MinMaxFov.x, m_MinMaxFov.y, amount));
		}
	}

	void GameCamera::Update(float delta)
	{
		UpdateFov();
		SetFov(glm::lerp(m_Fov, m_FovTarget, delta * m_FovMoveSpeed));

		m_Camera->view = GetView();
		m_Camera->projection = GetProjection();
		m_Camera->viewProjection = m_Camera->projection * m_Camera->view;
		m_Camera->position = m_LocalTransform.position;
	}

	glm::mat4 GameCamera::GetView() {
		glm::mat4 view =
			glm::eulerAngleXYZ(-m_LocalTransform.rotation.x, -m_LocalTransform.rotation.y, -m_LocalTransform.rotation.z) *
			glm::translate(glm::mat4(1), -m_LocalTransform.position);
		return view;
	}

	void GameCamera::SetAspect(float aspect) {
		m_Aspect = aspect;
		m_Projection = glm::perspective(m_Fov, m_Aspect, 0.01f, 1000.f);
	}

	void GameCamera::SetFov(float fov) {
		m_Fov = fov;
		m_Projection = glm::perspective(m_Fov, m_Aspect, 0.01f, 1000.f);
	}

	glm::mat4 GameCamera::GetProjection() {
		return m_Projection;
	}

	glm::mat4 GameCamera::GetViewProjection() {
		return GetProjection() * GetView();
	}

	void GameCamera::OnRefocus() {
		UpdateFov();
		SetFov(m_FovTarget);
	}

	void GameCamera::OnFocusLost() {

	}

}
