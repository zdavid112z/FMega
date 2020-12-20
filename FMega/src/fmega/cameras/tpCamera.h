#pragma once

#include "gameCamera.h"

namespace fmega {

	class Player;

	class TPCamera : public GameCamera {
	public:
		TPCamera(FMegaScene* scene, Player* player, float fov);
		virtual ~TPCamera();

		virtual void Update(float delta) override;
		virtual void OnRefocus() override;
		virtual void OnFocusLost() override;
	private:
		float m_FollowSpeed;
	};

}