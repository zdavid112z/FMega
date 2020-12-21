#include "fmega.h"
#include "levelManager.h"
#include "fmega/fmegaScene.h"
#include "core/game.h"
#include "fmega/fmegaRenderer.h"
#include "utils/random.h"
#include "platform.h"

namespace fmega {

	LevelManager::LevelManager(const std::string& name, Entity* parent, FMegaScene* scene) :
		FMegaEntity(name, parent, scene)
	{
		m_PlatformId = 0;
		m_CurrentZ = 0.0f;
		m_SpawnZ = -100.f;
	
		m_CurrentPlatform = 0;
		m_FuelBonusInterval = 3;

		m_GoodPlatforms = { PlatformType::PLAIN, PlatformType::FUEL_GAIN };
		m_BadPlatforms = { PlatformType::DEATH, PlatformType::FUEL_LOSS, PlatformType::SPEED_BOOST };
		m_GoodPlatformChances = { 0.9f, 0.1f };
		m_BadPlatformChances = { 0.3f, 0.35f, 0.35f };
		m_BadPlatformChance = 0.5f;

		m_PlatformDistances = glm::vec2(5.f, 7.f);
		m_PlatformLengths = glm::vec2(40.f, 70.f);
		m_PlatformWidths = { 1.f, 2.f, 3.f };
		m_PlatformPositions = { -4.f, 0.f, 4.f };
		m_NextConfiguration = (1 << m_PlatformPositions.size()) - 1;
		m_NextPlatformTypes = { PlatformType::PLAIN, PlatformType::PLAIN, PlatformType::PLAIN };
	}

	LevelManager::~LevelManager()
	{
		
	}

	byte* LevelManager::GetData(uint& size) {
		size = sizeof(m_Data);
		return m_Data;
	}

	void LevelManager::Update(float delta)
	{
		m_CurrentZ += delta * m_FMegaScene->MoveSpeed;
		while (m_CurrentZ >= m_SpawnZ) {
			SpawnPlatforms();
		}
	}

	void LevelManager::SpawnPlatforms() {
		m_CurrentPlatform++;
		float distance = Random::NextFloat(m_PlatformDistances);
		float length = Random::NextFloat(m_PlatformLengths);
		int numPositions = m_PlatformPositions.size();
		int configuration = m_NextConfiguration;
		std::vector<PlatformType> types = m_NextPlatformTypes;

		GenerateNextConfiguration();

		for (int i = 0; i < numPositions; i++) {
			if (configuration & (1 << i)) {
				int width = Random::NextInt(0, m_PlatformWidths.size());
				Platform* p = new Platform(std::to_string(m_PlatformId++), nullptr, m_FMegaScene,
					m_PlatformPositions[i],
					m_CurrentZ, m_PlatformWidths[width],
					length, types.back());
				types.pop_back();
				m_FMegaScene->AddEntity(p);
			}
		}
		m_CurrentZ -= length + distance;
	}

	void LevelManager::GenerateNextConfiguration() {
		int numPositions = m_PlatformPositions.size();
		m_NextConfiguration = Random::NextInt(1, 1 << numPositions);
		m_NextPlatformTypes.clear();

		std::vector<bool> isPlatformGood;

		for (int i = 0; i < numPositions; i++) {
			if (m_NextConfiguration & (1 << i)) {
				bool isGood = Random::NextFloat() > m_BadPlatformChance;
				isPlatformGood.push_back(isGood);
			}
		}
		// Assure 1 good platform
		isPlatformGood.pop_back();
		isPlatformGood.push_back(true);
		Random::Shuffle(isPlatformGood);

		bool mustSpawnFuelGain = ((m_CurrentPlatform % m_FuelBonusInterval) == 0);
		for (bool isGood : isPlatformGood) {
			PlatformType type;
			if (isGood && mustSpawnFuelGain) {
				mustSpawnFuelGain = false;
				type = PlatformType::FUEL_GAIN;
			}
			else if (isGood) {
				type = GeneratePlatform(m_GoodPlatformChances, m_GoodPlatforms);
			}
			else {
				type = GeneratePlatform(m_BadPlatformChances, m_BadPlatforms);
			}
			m_NextPlatformTypes.push_back(type);
		}
		Random::Shuffle(m_NextPlatformTypes);
	}

	PlatformType LevelManager::GeneratePlatform(
		const std::vector<float>& chances, 
		const std::vector<PlatformType>& types) {

		float value = Random::NextFloat();
		int type;
		for (type = 0; type < int(chances.size()); type++) {
			if (value <= chances[type]) {
				break;
			}
			value -= chances[type];
		}
		type = glm::clamp(type, 0, int(chances.size()) - 1);
		return types[type];
	}

	void LevelManager::Render(float delta)
	{
		
	}
}
