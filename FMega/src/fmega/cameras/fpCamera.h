#pragma once

#include "gameCamera.h"

namespace fmega {

	class Player;

	class FPCamera : public GameCamera {
	public:
		FPCamera(FMegaScene* scene, Player* player, float fov);
		virtual ~FPCamera();

		virtual void Update(float delta) override;
		virtual void OnRefocus() override;
		virtual void OnFocusLost() override;
	private:
		uint32 m_Listener;
		glm::vec2 m_Sensitivity;
		glm::vec2 m_PitchLimits;
		glm::vec2 m_YawLimits;
	};

}