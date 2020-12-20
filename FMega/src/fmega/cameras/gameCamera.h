#pragma once

#include "../entities/fmegaEntity.h"
#include "graphics/camera.h"

namespace fmega {

	class Player;

	class GameCamera {
	public:
		GameCamera(FMegaScene* scene, Player* player, float fov);
		virtual ~GameCamera();

		void UpdateFov();
		virtual void Update(float delta);

		virtual void SetFov(float fov);
		virtual void SetAspect(float aspect);
		virtual void OnRefocus();
		virtual void OnFocusLost();

		virtual glm::mat4 GetView();
		virtual glm::mat4 GetProjection();
		virtual glm::mat4 GetViewProjection();

		Camera* GetCamera() { return m_Camera; }
	protected:
		Transform m_LocalTransform;
		FMegaScene* m_FMegaScene;
		Player* m_Player;
		glm::mat4 m_Projection;
		Camera* m_Camera;
		glm::vec2 m_MinMaxSpeed;
		glm::vec2 m_MinMaxFov;
		float m_FovTarget;
		float m_FovMoveSpeed;
		float m_Aspect, m_Fov;
		uint32 m_ResizeListener;
	};

}