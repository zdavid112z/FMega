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
		virtual void RewindUpdate(float delta) override;

		virtual byte* GetData(uint& size) override;

		glm::vec2 GetMinMaxSpeed();
		bool IsSlowDying();
	private:
		void Die(float isSlow);
		void HandlePlatformCollision();
		void HandleTunnelCollision();
		void HandleLostAnimation(float delta);
		void CalcHeight(float delta);
		void OnLandOnPlatform(Platform* p);
		void HandleInput(float delta);
		float CalcFuelMultiplier();
		bool IsOnTopOf(Platform* p);
		void HandleCollision(Platform* p, float delta);
		void UpdateSceneSpeed();

		GameCamera* m_Cameras[2];

		union {
			struct {
				int m_NumLives;

				float m_Fuel;
				int m_SpeedIndex;
				float m_MaxSpeedTimer;

				float m_HeightK1;
				float m_HeightK2;
				float m_HeightK3;
				glm::vec2 m_HeightClamp;
				float m_Height;
				float m_HeightVelocity;
				float m_HeightImpulse;

				float m_AnimSpeed;
				bool m_ForceOnTop;

				float m_AnimTarget;
				float m_AnimTime;
				bool m_Lost;
				bool m_Grounded;

				float m_CurrentGravity;
				float m_VelocityY;
				float m_BottomY;
			};
			byte m_Data[100];
		};

		int m_CameraIndex;

		const std::vector<float> m_Speeds;

		const float m_FuelBoxHeight;
		const glm::vec4 m_FuelColor;
		const glm::vec4 m_FuelEmptyColor;

		const float m_MaxSpeedHeight;
		const float m_MaxSpeedFlicker;
		const glm::vec4 m_MaxSpeedColor;

		const float m_FuelBonusB;
		const float m_MaxFuel;

		const float m_FuelGain;
		const float m_FuelLoss;
		const float m_MaxSpeedTime;
		const float m_LostCollisionMultiplier;

		const float m_OnLandHeightImpulse;
		const float m_RiseGravity;
		const float m_FallGravity;
		const float m_LowJumpGravity;
		const float m_JumpMaxHeight;
		const float m_JumpVelocity;

		const float m_MoveSpeed;
		const float m_PlayerZ;
		const float m_Radius;
		const float m_RotationSpeed;
	};

}