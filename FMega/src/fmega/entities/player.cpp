#include "fmega.h"
#include "player.h"
#include "fmega/fmegaScene.h"
#include "core/game.h"
#include "platform.h"
#include "fmega/fmegaRenderer.h"
#include "utils/random.h"
#include "fmega/gameOverEvent.h"
#include "tunnel.h"
#include "pickup.h"
#include "wall.h"
#include "discoLight.h"

namespace fmega {

	const float Player::PlayerZ = -4.f;

	Player::Player(const std::string& name, Entity* parent, FMegaScene* scene, float radius) :
		FMegaEntity(name, parent, scene),
		Radius(radius),
		m_MaxFuel(10.f),
		m_Speeds({ 10.f, 15.f, 20.f, 25.f, 30.f }),
		m_FuelBonusB(2.5f),
		m_FuelGain(5.f),
		m_FuelLoss(3.5f),
		m_MaxSpeedTime(5.f),
		m_FuelColor(glm::vec4(237, 245, 22, 255) / 255.f),
		m_FuelEmptyColor(glm::vec4(0, 0, 0, 1)),
		m_FuelBoxHeight(0.4f),
		m_RotationSpeed(30.f),
		m_MoveSpeed(8.f),
		m_JumpMaxHeight(4.75f),
		m_RiseGravity(25.f),
		m_FallGravity(2.f * m_RiseGravity),
		m_LowJumpGravity(2.f * m_RiseGravity),
		m_LostCollisionMultiplier(50.f),
		m_MaxSpeedHeight(0.25f),
		m_MaxSpeedFlicker(10.f),
		m_MaxSpeedColor(glm::vec4(1, 0, 0, 1)),
		m_JumpVelocity(sqrt(2 * m_RiseGravity * m_JumpMaxHeight)),
		m_OnLandHeightImpulse(4.f),
		m_DiscoTime(5.f),
		m_FarsightTime(15.f),
		m_MaxRewinds(3),
		m_DiscoTimer(0.f),
		m_FarsightTimer(0.f),
		m_NumRewinds(0),
		m_NearsightTargetZ(-15.f),
		m_FarsightTargetZ(-40.f),
		m_SightSpeed(0.25f),
		m_RenderTargetZ(-15.f),
		m_Score(0),
		m_ScoreWallbreak(2500),
		m_ScorePickup(250),
		m_ScoreDiscoBonus(750),
		m_DiscoAngle(0.f)
	{
		m_CameraIndex = 0;
		m_Fuel = m_MaxFuel;
		m_SpeedIndex = 1;
		
		m_MaxSpeedTimer = 0.f;

		m_BottomY = 2.f;
		m_Height = 1.f;
		m_CurrentGravity = m_FallGravity;
		
		m_LocalTransform.position = glm::vec3(0, 2.f, PlayerZ);
		m_HeightK1 = 20.f;
		m_HeightK2 = 0.4f;
		m_HeightK3 = 30.f;
		m_HeightClamp = glm::vec2(0.7f, 1.1f);

		m_HeightVelocity = 0.f;
		m_HeightImpulse = 0.f;
		m_AnimTarget = 0.f;
		m_AnimTime = 0.f;

		m_Lost = false;
		m_Grounded = false;
		m_ForceOnTop = false;

		m_VelocityY = 0.f;
		m_BottomY = 5.f;

		m_AnimSpeed = 3.f;

		CircleObject* col = new CircleObject(this, glm::mat4(1), Radius);
		Init(col);

		constexpr float fov = glm::radians(90.f);
		m_Cameras[0] = new FPCamera(m_FMegaScene, this, fov);
		m_Cameras[1] = new TPCamera(m_FMegaScene, this, fov);
		m_CameraIndex = 1;
		
		m_Cameras[m_CameraIndex]->OnRefocus();
		m_FMegaScene->SetCamera(m_Cameras[m_CameraIndex]->GetCamera());

		UpdateSceneSpeed();

		for (int i = 0; i < 8; i++) {

			glm::quat initial = Random::NextQuat();
			glm::quat vel = glm::angleAxis(Random::NextFloat(glm::radians(glm::vec2(45.f, 60.f))), Random::NextVec3Normalized());

			DiscoLight* l = new DiscoLight(StringUtils::ToString(i), scene, this, float(i), initial, vel);
			scene->AddEntity(l);
		}
	}

	Player::~Player()
	{
		delete m_Cameras[0];
		delete m_Cameras[1];
	}

	byte* Player::GetData(uint& size) {
		size = sizeof(m_Data);
		return m_Data;
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

	void Player::RewindUpdate(float delta) {
		UpdateSceneSpeed();
		m_Cameras[m_CameraIndex]->Update(delta);
	}

	void Player::Update(float delta)
	{
		HandleInput(delta);
		m_BottomY += m_VelocityY * delta - m_CurrentGravity * delta * delta * 0.5f;
		m_VelocityY -= m_CurrentGravity * delta;
		if (m_ForceOnTop) {
			m_BottomY = glm::max(0.f, m_BottomY);
		}

		HandleGameCollisions();
		UpdateEffects(delta);
		HandleLostAnimation(delta);

		CalcHeight(delta);

		m_LocalTransform.rotation.x -= delta * m_RotationSpeed * m_FMegaScene->MoveSpeed / m_Speeds.back();
		m_LocalTransform.position.y = m_BottomY + Radius * m_Height;

		m_Cameras[m_CameraIndex]->Update(delta);
	}

	void Player::UpdateEffects(float delta) {
		m_DiscoTimer = glm::max(m_DiscoTimer - delta, 0.f);
		m_FarsightTimer = glm::max(m_FarsightTimer - delta, 0.f);
		if (m_FarsightTimer > 0) {
			m_CurrentTargetZ = m_FarsightTargetZ;
		}
		else {
			m_CurrentTargetZ = m_NearsightTargetZ;
		}
		m_RenderTargetZ = glm::lerp(m_RenderTargetZ, m_CurrentTargetZ, delta * m_SightSpeed);
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

	void Player::UpdateSceneSpeed() {
		float speed = m_Speeds[m_SpeedIndex];
		m_FMegaScene->MoveSpeed = speed;
		m_FMegaScene->TargetZ = m_RenderTargetZ;
	}

	void Player::HandleInput(float delta) {

		auto input = m_FMegaScene->GetGame()->GetDisplay()->GetInput();

		if (input->WasKeyPressed(Key::C)) {
			m_Cameras[m_CameraIndex]->OnFocusLost();
			m_CameraIndex = 1 - m_CameraIndex;
			m_Cameras[m_CameraIndex]->OnRefocus();
			m_FMegaScene->SetCamera(m_Cameras[m_CameraIndex]->GetCamera());
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

		UpdateSceneSpeed();

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

		HandleTunnelCollision();
	}

	void Player::HandleTunnelCollision() {
		float r = Tunnel::Radius;
		glm::vec2 p = glm::vec2(m_LocalTransform.position.x, m_BottomY);
		float d = glm::length(p);
		if (d > r) {
			glm::vec2 pToO = -glm::normalize(p);
			m_LocalTransform.position.x += pToO.x * (d - r);
			m_BottomY += pToO.y * (d - r);
		}
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

		if (oCenterToClosestLength > Radius) {
			return;
		}

		if (oCenterToClosestLength < 1e-4f) {
			return;
		}

		float normalLength = Radius - oCenterToClosestLength;
		glm::vec3 oCenterToClosestN = glm::normalize(oCenterToClosest);
//		LOG_INFO(oCenter, pSizes, pCenter, diff, clamped, closest, oCenterToClosest, oCenterToClosestLength, normalLength, oCenterToClosestN, );
		m_LocalTransform.position -= oCenterToClosestN * normalLength * glm::min(1.f, delta * m_LostCollisionMultiplier);
	}

	void Player::Die(float isSlow) {
		if (m_NumRewinds > 0 && m_FMegaScene->GetRewind()->CanRewind()) {
			m_NumRewinds--;
			m_FMegaScene->GetRewind()->StartRewind();
			return;
		}
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

	void Player::HandlePickup(Pickup* p) {
		if (!Collision::TestCollision(GetCollision(), p->GetCollision()) || p->IsTaken()) {
			return;
		}
		switch (p->Type)
		{
		case PickupType::DISCO:
			m_Score += m_ScoreDiscoBonus;
			m_DiscoTimer = m_DiscoTime;
			break;
		case PickupType::REWIND:
			m_NumRewinds = glm::min(m_NumRewinds + 1, m_MaxRewinds);
			break;
		case PickupType::FARSIGHT:
			m_FarsightTimer = m_FarsightTime;
			break;
		default:
			break;
		}
		p->OnPlayerTake();
		m_Score += m_ScorePickup;
	}

	bool Player::CheckCollidesWith(Wall* w) {
		if (w->Broken || !Collision::TestCollision(GetCollision(), w->GetCollision())) {
			return false;
		}
		m_Score += m_ScoreWallbreak;
		w->OnHitByPlayer();
		return true;
	}

	void Player::HandleGameCollisions() {
		if (m_Lost || m_ForceOnTop) {
			return;
		}

		bool wasGrounded = m_Grounded;
		bool hitWall = false;
		m_Grounded = false;
		m_FMegaScene->ForeachEntity([this, &hitWall](Entity* e) {
			if (StringUtils::StartsWith(e->GetName(), "Platform_")) {
				Platform* p = (Platform*)e;
				if (m_BottomY <= 0 && IsOnTopOf(p)) {
					OnLandOnPlatform(p);
				}
			}
			else if (StringUtils::StartsWith(e->GetName(), "Pickup_")) {
				Pickup* p = (Pickup*)e;
				HandlePickup(p);
			}
			else if (StringUtils::StartsWith(e->GetName(), "Wall_")) {
				Wall* w = (Wall*)e;
				hitWall |= CheckCollidesWith(w);
			}
		});

		if (hitWall) {
			if (m_SpeedIndex >= int(m_Speeds.size()) - 1) {
				m_FMegaScene->GetRenderer()->SetShake(ShakeType::DECAYING, 0.3f);
			}
			else {
				Die(true);
			}
		}

		if (m_BottomY <= 0) {
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
	}

	void Player::Render(float delta)
	{
		float fuelAmount = m_Fuel / m_MaxFuel;

		{
			MeshRenderData data;
			data.model = glm::translate(glm::mat4(1.f), glm::vec3(-16.f + fuelAmount * 16.f, -9.f + m_FuelBoxHeight / 2.f, 0)) *
				glm::scale(glm::mat4(1.f), glm::vec3(fuelAmount * 16.f, m_FuelBoxHeight / 2.f, 1));
			data.color = m_FuelColor;
			data.opacity = 1.f;
			data.albedoStrength = 0.f;
			m_FMegaScene->GetRenderer()->RenderMesh(m_FMegaScene->GetAssets()->BoxMesh, data, true);
		}

		{
			MeshRenderData data;
			data.model = glm::translate(glm::mat4(1.f), glm::vec3(16.f - (1.f - fuelAmount) * 16.f, -9.f + m_FuelBoxHeight / 2.f, 0)) *
				glm::scale(glm::mat4(1.f), glm::vec3((1.f - fuelAmount) * 16.f, m_FuelBoxHeight / 2.f, 1));
			data.color = m_FuelEmptyColor;
			data.opacity = 1.f;
			data.albedoStrength = 0.f;
			m_FMegaScene->GetRenderer()->RenderMesh(m_FMegaScene->GetAssets()->BoxMesh, data, true);
		}

		if (m_MaxSpeedTimer > 0) {
			MeshRenderData data;
			data.model = glm::translate(glm::mat4(1.f), glm::vec3(0, -9.f + m_FuelBoxHeight + m_MaxSpeedHeight / 2.f, 0)) *
				glm::scale(glm::mat4(1.f), glm::vec3(16.f, m_MaxSpeedHeight / 2.f, 1));
			data.color = glm::vec4(glm::sin(m_MaxSpeedFlicker * m_FMegaScene->GetGame()->GetTime()) * 0.3f + 0.7f, 0.2f, 0.2f, 1.0f);
			data.opacity = 1.f;
			data.albedoStrength = 0.f;
			m_FMegaScene->GetRenderer()->RenderMesh(m_FMegaScene->GetAssets()->BoxMesh, data, true);
		}

		if (m_CameraIndex != 0) // FP Camera
			m_FMegaScene->GetRenderer()->RenderPlayer(m_GlobalTransform, m_Height, m_AnimTime);

		DrawUI();
	}

	void Player::DrawUI()
	{
		{
			int scoreToDisplay = glm::min(m_Score, 9999999);
			for (int i = 0; i < 7; i++) {
				int digit = scoreToDisplay % 10;
				scoreToDisplay /= 10;
				MeshRenderData data;
				data.model = glm::translate(glm::mat4(1), glm::vec3(15.f - i * 1.5f, 7.5f, -50));
				data.color = glm::vec4(0);
				m_FMegaScene->GetRenderer()->RenderDigit(m_FMegaScene->GetAssets()->SegmentMesh, data, digit);
			}
		}

		for (int i = 0; i < m_MaxRewinds; i++) {
			MeshRenderData data;
			data.model = glm::translate(glm::mat4(1), glm::vec3(14.5f - i * 2.5f, 5.5f, -50)) * glm::scale(glm::mat4(1), glm::vec3(1, 1, 1));
			if (i < m_NumRewinds) {
				data.color = glm::vec4(0.9f, 0.2f, 0.3f, 1.0);
			}
			else {
				data.color = glm::vec4(0, 0, 0, 1);
			}
			if (!m_FMegaScene->GetRewind()->CanRewind() || m_FMegaScene->GetRewind()->IsRewinding()) {
				data.color *= glm::vec4(glm::vec3(0.5f), 1.0f);
			}
			m_FMegaScene->GetRenderer()->RenderMesh(m_FMegaScene->GetAssets()->HeartMesh, data, true);
		}

		if (m_FarsightTimer > 0) {
			float op = glm::clamp(m_FarsightTimer * 0.33f, 0.f, 1.f);
			float scale = 1.5f / 2.f;
			MeshRenderData data;
			data.model = glm::translate(glm::mat4(1), glm::vec3(14.5f - m_MaxRewinds * 2.5f, 5.5f - 0.25f, -50)) * glm::scale(glm::mat4(1), glm::vec3(scale, scale, 1));
			data.color = glm::vec4(0);
			data.albedoStrength = 1;
			data.opacity = op;
			m_FMegaScene->GetRenderer()->RenderMesh2D(m_FMegaScene->GetAssets()->BoxMesh, data, true, true, m_FMegaScene->GetAssets()->BinocularsTexture);
		}
	}

	float Player::GetDiscoLightIntensity() {
		float x = glm::clamp(m_DiscoTimer * 0.5f, 0.f, 1.f);
		return x;
	}

}
