#pragma once

#include "fmegaEntity.h"
#include "platform.h"

namespace fmega {

	class LevelManager : public FMegaEntity {
	public:
		LevelManager(const std::string& name, Entity* parent, FMegaScene* scene);
		virtual ~LevelManager();

		virtual void Update(float delta) override;
		virtual void Render(float delta) override;

		virtual byte* GetData(uint& size) override;
	private:
		void SpawnPlatforms();
		void GenerateNextConfiguration();
		PlatformType GeneratePlatform(
			const std::vector<float>& chances,
			const std::vector<PlatformType>& types);
	private:
		union {
			struct {
				float m_CurrentZ;
			};
			byte m_Data[4];
		};

		int m_PlatformId;
		int m_NextConfiguration;
		std::vector<PlatformType> m_NextPlatformTypes;
		int m_CurrentPlatform;
		int m_FuelBonusInterval;
		float m_SpawnZ;
		glm::vec2 m_PlatformDistances;
		glm::vec2 m_PlatformLengths;
		std::vector<float> m_GoodPlatformChances;
		std::vector<float> m_BadPlatformChances;
		float m_BadPlatformChance;
		std::vector<PlatformType> m_GoodPlatforms;
		std::vector<PlatformType> m_BadPlatforms;
		std::vector<float> m_PlatformWidths;
		std::vector<float> m_PlatformPositions;
	};

}