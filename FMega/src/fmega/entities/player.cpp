#include "fmega.h"
#include "player.h"
#include "fmega/fmegaScene.h"
#include "core/game.h"
#include "platform.h"
#include "fmega/fmegaRenderer.h"
#include "utils/random.h"
#include "fmega/gameOverEvent.h"

namespace fmega {

	Player::Player(const std::string& name, Entity* parent, FMegaScene* scene, float radius) :
		FMegaEntity(name, parent, scene), m_Radius(radius)
	{
		m_MaxFuel = 10.f;
		m_Fuel = m_MaxFuel;
		m_Speeds = { 10.f, 15.f, 20.f, 25.f, 30.f };
		m_SpeedIndex = 1;
		m_FuelBonusB = 2.5f;

		m_FuelGain = 5.f;
		m_FuelLoss = 3.5f;
		m_MaxSpeedTime = 5.f;
		m_MaxSpeedTimer = 0.f;

		m_Box = m_FMegaScene->BoxMesh;
		m_FuelColor = glm::vec4(237, 245, 22, 255) / 255.f;
		m_FuelEmptyColor = glm::vec4(0, 0, 0, 1);
		m_FuelBoxHeight = 0.4f;

		m_RotationSpeed = 30.f;
		m_BottomY = 2.f;
		m_Height = 1.f;
		m_MoveSpeed = 8.f;
		m_LocalTransform.position = glm::vec3(0, 2.f, m_PlayerZ);
		m_JumpMaxHeight = 4.75f;
		m_RiseGravity = 25.f;
		m_FallGravity = 2.f;
		m_LowJumpGravity = 2.f;
		m_LostCollisionMultiplier = 50.f;
		m_HeightK1 = 20.f;
		m_HeightK2 = 0.4f;
		m_HeightK3 = 30.f;
		m_HeightClamp = glm::vec2(0.7f, 1.1f);

		m_MaxSpeedHeight = 0.25f;
		m_MaxSpeedFlicker = 10.f;
		m_MaxSpeedColor = glm::vec4(1, 0, 0, 1);

		m_AnimSpeed = 3.f;
		m_FallGravity *= m_RiseGravity;
		m_LowJumpGravity *= m_RiseGravity;
		m_JumpVelocity = sqrt(2 * m_RiseGravity * m_JumpMaxHeight);

		constexpr float fov = glm::radians(90.f);
		m_FPCamera = new FPCamera(m_FMegaScene, this, fov);
		m_TPCamera = new TPCamera(m_FMegaScene, this, fov);

		m_CurrentCamera = m_TPCamera;
		m_CurrentCamera->OnRefocus();
		m_FMegaScene->SetCamera(m_CurrentCamera->GetCamera());
	}

	Player::~Player()
	{
		delete m_FPCamera;
		delete m_TPCamera;
		delete m_Mesh;
	}

	glm::vec2 Player::GetMinMaxSpeed() {
		return glm::vec2(m_Speeds.front(), m_Speeds.back());
	}

	bool Player::IsSlowDying()
	{
		return m_Lost && m_ForceOnTop;
	}

	float Player::CalcFuelMultiplier() {
		float x = m_Speeds[m_SpeedIndex] / m_Speeds.back();
		return 1.f - glm::pow(m_FuelBonusB, -x);
	}

	void Player::Update(float delta)
	{
		HandleInput(delta);
		m_BottomY += m_VelocityY * delta - m_CurrentGravity * delta * delta * 0.5f;
		m_VelocityY -= m_CurrentGravity * delta;
		if (m_ForceOnTop) {
			m_BottomY = glm::max(0.f, m_BottomY);
		}

		HandlePlatformCollision();
		HandleLostAnimation(delta);

		CalcHeight(delta);

		m_LocalTransform.rotation.x -= delta * m_RotationSpeed * m_FMegaScene->MoveSpeed / m_Speeds.back();
		m_LocalTransform.position.y = m_BottomY + m_Radius * m_Height;

		m_CurrentCamera->Update(delta);
	}

	void Player::CalcHeight(float delta) {

		float k1 = m_HeightK1, k2 = m_HeightK2, k3 = m_HeightK3;
		m_HeightVelocity = k1 * (1.f - m_Height);
		m_HeightVelocity -= m_HeightImpulse;
		m_HeightImpulse = glm::max(0.f, m_HeightImpulse - delta * k3);

		if (m_Grounded) {
			m_HeightVelocity -= k2;
		}
		m_Height += m_HeightVelocity * delta;
		m_Height = glm::clamp(m_Height, m_HeightClamp.x, m_HeightClamp.y);

	}

	void Player::OnLandOnPlatform(Platform* p) {

		m_Grounded = true;
		m_BottomY = 0.f;

		if (p->Type != PlatformType::ACTIVATED) {
			switch (p->Type)
			{
			case PlatformType::DEATH:
				Die(true);
				break;
			case PlatformType::SPEED_BOOST:
				m_MaxSpeedTimer = m_MaxSpeedTime;
				m_AnimTarget = 0.f;
				break;
			case PlatformType::FUEL_GAIN:
				m_Fuel = glm::clamp(m_Fuel + m_FuelGain, 0.f, m_MaxFuel);
				m_AnimTarget = 1.f;
				break;
			case PlatformType::FUEL_LOSS:
				m_Fuel = glm::clamp(m_Fuel - m_FuelLoss, 0.f, m_MaxFuel);
				m_AnimTarget = 0.f;
				break;
			}
			if (p->Type != PlatformType::PLAIN) {
				m_AnimTime = 1.f - m_AnimTarget;
			}
			p->OnTouched();
		}

	}

	void Player::HandleInput(float delta) {

		auto input = m_FMegaScene->GetGame()->GetDisplay()->GetInput();

		if (input->WasKeyPressed(Key::C)) {
			m_CurrentCamera->OnFocusLost();
			m_CurrentCamera = m_CurrentCamera == m_FPCamera ? (GameCamera*)m_TPCamera : (GameCamera*)m_FPCamera;
			m_CurrentCamera->OnRefocus();
			m_FMegaScene->SetCamera(m_CurrentCamera->GetCamera());
		}

		if (m_MaxSpeedTimer > 0) {
			m_MaxSpeedTimer -= delta;
			m_SpeedIndex = m_Speeds.size() - 1;
		}
		else {
			if (input->WasKeyPressed(Key::W)) {
				m_SpeedIndex++;
			}

			if (input->WasKeyPressed(Key::S)) {
				m_SpeedIndex--;
			}

			m_SpeedIndex = glm::clamp(m_SpeedIndex, 0, int(m_Speeds.size()) - 2);
		}

		float speed = m_Speeds[m_SpeedIndex];
		m_FMegaScene->MoveSpeed = speed;
		m_Fuel -= glm::max(0.f, delta * CalcFuelMultiplier());

		if (!m_Lost && input->IsKeyDown(Key::SPACE) && m_Grounded) {
			m_VelocityY = m_JumpVelocity;
			m_Grounded = false;
		}

		if (input->IsKeyDown(Key::A)) {
			m_LocalTransform.position.x -= delta * m_MoveSpeed;
		}

		if (input->IsKeyDown(Key::D)) {
			m_LocalTransform.position.x += delta * m_MoveSpeed;
		}

		m_CurrentGravity = m_RiseGravity;
		if (m_VelocityY < 0) {
			m_CurrentGravity = m_FallGravity;
		}

		if (m_VelocityY > 0 && !input->IsKeyDown(Key::SPACE)) {
			m_CurrentGravity = m_LowJumpGravity;
		}

		m_AnimTime = glm::lerp(m_AnimTime, m_AnimTarget, delta * m_AnimSpeed);
	}

	void Player::HandleLostAnimation(float delta) {
		if (!m_Lost || m_ForceOnTop) {
			return;
		}

		m_FMegaScene->ForeachEntity([this, delta](Entity* e) {
			if (StringUtils::StartsWith(e->GetName(), "Platform_")) {
				Platform* p = (Platform*)e;
				HandleCollision(p, delta);
			}
		});
	}

	bool Player::IsOnTopOf(Platform* p) {
		float x = m_LocalTransform.position.x;
		float z = m_LocalTransform.position.z;
		float px = p->GetLocalTransform().position.x;
		float pz = p->GetLocalTransform().position.z;
		float pw = p->Width;
		float pl = p->Length;
		return x >= px - pw / 2.f && x <= px + pw / 2.f &&
			   pz >= z && z >= pz - pl;
	}

	void Player::HandleCollision(Platform* p, float delta) {
		// Assumes player center is not inside the platform
		glm::vec3 oCenter = m_LocalTransform.position;
		glm::vec3 pSizes = glm::vec3(p->Width, p->PlatformThickness, p->Length) / 2.f;
		glm::vec3 pCenter = p->GetLocalTransform().position - glm::vec3(0, 0, p->Length / 2.f);
		glm::vec3 diff = oCenter - pCenter;
		glm::vec3 clamped = glm::clamp(diff, -pSizes, pSizes);
		glm::vec3 closest = pCenter + clamped;
		glm::vec3 oCenterToClosest = closest - oCenter;
		float oCenterToClosestLength = glm::length(oCenterToClosest);

		if (oCenterToClosestLength > m_Radius) {
			return;
		}

		if (oCenterToClosestLength < 1e-4f) {
			return;
		}

		float normalLength = m_Radius - oCenterToClosestLength;
		glm::vec3 oCenterToClosestN = glm::normalize(oCenterToClosest);
//		LOG_INFO(oCenter, pSizes, pCenter, diff, clamped, closest, oCenterToClosest, oCenterToClosestLength, normalLength, oCenterToClosestN, );
		m_LocalTransform.position -= oCenterToClosestN * normalLength * glm::min(1.f, delta * m_LostCollisionMultiplier);
	}

	void Player::Die(float isSlow) {
		if (m_Lost) {
			return;
		}
		m_Lost = true;
		GameOverEventData data;
		data.isSlow = isSlow;
		m_FMegaScene->GetGame()->GetEventManager()->RegisterEvent({ &data }, "GameOver");

		if (isSlow) {
			m_ForceOnTop = true;
			m_AnimSpeed *= 2.f;
			m_AnimTarget = 0.0f;
			m_AnimTime = 1.f - m_AnimTarget;
			m_HeightK2 = 7.0f;
			m_HeightClamp.x = 0.1f;
		}
	}

	void Player::HandlePlatformCollision() {
		if (m_Lost || m_BottomY > 0 || m_ForceOnTop) {
			return;
		}

		bool wasGrounded = m_Grounded;
		m_Grounded = false;
		m_FMegaScene->ForeachEntity([this](Entity* e) {
			if (StringUtils::StartsWith(e->GetName(), "Platform_")) {
				Platform* p = (Platform*)e;
				if (IsOnTopOf(p)) {
					OnLandOnPlatform(p);
				}
			}
		});
		if (!m_Grounded) {
			Die(false);
		}
		else {
			if (m_Fuel <= 0) {
				Die(true);
			}
			else {
				m_VelocityY = 0;
				if (!wasGrounded)
					m_HeightImpulse = m_OnLandHeightImpulse;
			}
		}
	}

	void Player::Render(float delta)
	{
		float fuelAmount = m_Fuel / m_MaxFuel;

		{
			MeshRenderData data;
			data.model = glm::translate(glm::mat4(1.f), glm::vec3(-16.f + fuelAmount * 16.f, -9.f + m_FuelBoxHeight / 2.f, 0)) *
				glm::scale(glm::mat4(1.f), glm::vec3(fuelAmount * 16.f, m_FuelBoxHeight / 2.f, 1));
			data.color = m_FuelColor;
			data.opacity = glm::vec4(1.f);
			m_FMegaScene->GetRenderer()->RenderMesh(m_FMegaScene->BoxMesh, data, true);
		}

		{
			MeshRenderData data;
			data.model = glm::translate(glm::mat4(1.f), glm::vec3(16.f - (1.f - fuelAmount) * 16.f, -9.f + m_FuelBoxHeight / 2.f, 0)) *
				glm::scale(glm::mat4(1.f), glm::vec3((1.f - fuelAmount) * 16.f, m_FuelBoxHeight / 2.f, 1));
			data.color = m_FuelEmptyColor;
			data.opacity = glm::vec4(1.f);
			m_FMegaScene->GetRenderer()->RenderMesh(m_FMegaScene->BoxMesh, data, true);
		}

		if (m_MaxSpeedTimer > 0) {
			MeshRenderData data;
			data.model = glm::translate(glm::mat4(1.f), glm::vec3(0, -9.f + m_FuelBoxHeight + m_MaxSpeedHeight / 2.f, 0)) *
				glm::scale(glm::mat4(1.f), glm::vec3(16.f, m_MaxSpeedHeight / 2.f, 1));
			data.color = glm::vec4(glm::sin(m_MaxSpeedFlicker * m_FMegaScene->GetGame()->GetTime()) * 0.3f + 0.7f, 0.2f, 0.2f, 1.0f);
			data.opacity = glm::vec4(1.f);
			m_FMegaScene->GetRenderer()->RenderMesh(m_FMegaScene->BoxMesh, data, true);
		}

		if (m_CurrentCamera != m_FPCamera)
			m_FMegaScene->GetRenderer()->RenderPlayer(m_GlobalTransform, m_Height, m_AnimTime);
	}
}
