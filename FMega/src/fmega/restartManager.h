#pragma once

#include "fmega.h"

namespace fmega {
	
	class FMegaScene;

	struct RestartRect {
		glm::vec4 color;
		glm::vec2 origin;
		glm::vec2 size;
		int indexExpand;
		int indexCollapse;

		glm::vec2 calculatedSize = glm::vec2(0.f);
	};

	class RestartManager {
	public:
		RestartManager(FMegaScene* scene);
		~RestartManager();

		float GetAdjDelta(float delta);
		void Update(float realDelta);
		void Render(float realDelta);
		void OnPlayerLost(float isSlow);
		bool ShouldRestart();
		bool HasLost();
		void Reset();
	private:
		void ResetRects(int count);
		void UpdateRects(float delta);
	private:
		float m_DeltaFactor;
		bool m_SlowDeath;
		float m_RectsAnimTime;
		float m_TimeToRectsAnim;
		float m_LostToRestartTime;
		float m_Timer;
		bool m_Lost;
		FMegaScene* m_Scene;
		std::vector<RestartRect> m_Rects;
	};

}