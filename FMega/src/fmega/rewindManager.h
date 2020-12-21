#pragma once

#include "fmega.h"

namespace fmega {

	class FMegaScene;

	struct DataFrame {
		float time;
		std::unordered_map<std::string, uint> nameToIndex;
		std::vector<byte> data;
	};

	class RewindManager {
	public:
		RewindManager(FMegaScene* scene, int recordFPS, float recordDuration);
		~RewindManager();

		void Update();
		bool CanRewind();
		bool IsRewinding();
		void StartRewind();
		void DeleteHistory();

	private:
		void Record();
		float GetTargetTime();
		bool Rewind();

		bool m_Rewinding;
		float m_RewindStartTime;

		float m_MinTime;
		float m_MaxTime;
		float m_LastRecTime;
		float m_RecInterval;
		int m_Head;
		int m_Size;
		int m_MaxSize;
		std::vector<DataFrame> m_Frames;

		FMegaScene* m_Scene;
		int m_FPS;
		float m_RecDuration;
	};

}
