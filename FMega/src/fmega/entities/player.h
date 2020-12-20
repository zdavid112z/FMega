#pragma once

#include "fmegaEntity.h"
#include "../cameras/fpCamera.h"
#include "../cameras/tpCamera.h"

namespace fmega {

	class Platform;

	class Player : public FMegaEntity {
	public:
		Player(const std::string& name, Entity* parent, FMegaScene* scene, float radius);
		virtual ~Player();

		virtual void Update(float delta) override;
		virtual void Render(float delta) override;

		glm::vec2 GetMinMaxSpeed();
		bool IsSlowDying();
	private:
		void Die(float isSlow);
		void HandlePlatformCollision();
		void HandleLostAnimation(float delta);
		void CalcHeight(float delta);
		void OnLandOnPlatform(Platform* p);
		void HandleInput(float delta);
		float CalcFuelMultiplier();
		bool IsOnTopOf(Platform* p);
		void HandleCollision(Platform* p, float delta);

		Mesh* m_Box;
		float m_FuelBoxHeight;
		glm::vec4 m_FuelColor;
		glm::vec4 m_FuelEmptyColor;

		float m_MaxSpeedHeight;
		float m_MaxSpeedFlicker;
		glm::vec4 m_MaxSpeedColor;

		TPCamera* m_TPCamera;
		FPCamera* m_FPCamera;
		GameCamera* m_CurrentCamera;

		float m_FuelBonusB;
		float m_Fuel;
		float m_MaxFuel;
		std::vector<float> m_Speeds;
		int m_SpeedIndex;

		float m_FuelGain;
		float m_FuelLoss;
		float m_MaxSpeedTime;
		float m_MaxSpeedTimer;
		float m_LostCollisionMultiplier;

		float m_HeightK1;
		float m_HeightK2;
		float m_HeightK3;
		glm::vec2 m_HeightClamp;
		float m_AnimSpeed;
		bool m_ForceOnTop = false;
		float m_OnLandHeightImpulse = 4.f;
		float m_AnimTarget = 0.f;
		float m_AnimTime = 0.f;
		bool m_Lost = false;
		bool m_Grounded = false;
		float m_FallGravity;
		float m_LowJumpGravity;
		float m_RiseGravity;
		float m_CurrentGravity;
		float m_VelocityY = 0;
		float m_MoveSpeed;
		float m_PlayerZ = -4.f;
		float m_BottomY = 5.f;
		float m_Radius;
		float m_Height;
		float m_HeightVelocity = 0;
		float m_HeightImpulse = 0;
		float m_JumpVelocity;
		float m_JumpMaxHeight;
		float m_RotationSpeed;
	};

}