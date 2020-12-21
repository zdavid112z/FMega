#include "fmega.h"
#include "rewindManager.h"
#include "fmegaScene.h"
#include "core/game.h"

namespace fmega {

	RewindManager::RewindManager(FMegaScene* scene, int recordFPS, float recordDuration) {
		m_Scene = scene;
		m_FPS = recordFPS;
		m_RecDuration = recordDuration;
		m_MaxSize = int(m_FPS * m_RecDuration);
		m_Frames.resize(m_MaxSize);
		m_Head = 0;
		m_Size = 0;
		m_LastRecTime = m_Scene->GetGame()->GetTime();
		m_RecInterval = 1.f / float(m_FPS);
		m_MinTime = 0;
		m_MaxTime = 0;
		m_Rewinding = false;
		m_RewindStartTime = 0;
	}

	RewindManager::~RewindManager() {

	}

	void RewindManager::Update() {
		if (IsRewinding()) {
			m_Rewinding = Rewind();
			if (!m_Rewinding) {
				DeleteHistory();
			}
		}
		else {
			Record();
		}
	}

	bool RewindManager::IsRewinding() {
		return m_Rewinding;
	}

	bool RewindManager::CanRewind() {
		return m_Size == m_MaxSize;
	}

	void RewindManager::Record() {
		float t = m_Scene->GetGame()->GetTime();
		if (t - m_LastRecTime < m_RecInterval) {
			return;
		}
		m_LastRecTime += m_RecInterval;
		// Handle skipped frames
		if (t - m_LastRecTime >= m_RecInterval) {
			m_LastRecTime = t;
		}
		int tail = 0;
		if (m_Size == m_MaxSize) {
			tail = m_Head;
			m_Head = (m_Head + 1) % m_MaxSize;
		}
		else {
			tail = (m_Head + m_Size) % m_MaxSize;
			m_Size++;
		}
		m_MinTime = m_Frames[m_Head].time;
		m_MaxTime = m_LastRecTime;
		DataFrame& frame = m_Frames[tail];
		frame.time = m_LastRecTime;
		frame.nameToIndex.clear();
		frame.data.clear();
		m_Scene->ForeachEntity([this, &frame](Entity* ee) {
			FMegaEntity* e = (FMegaEntity*)ee;
			uint index = frame.data.size();
			frame.nameToIndex[e->GetName()] = index;
			frame.data.resize(index + e->GetSaveSize());
			uint size;
			e->Save(&frame.data[0] + index, size);
		});
	}

	void RewindManager::StartRewind() {
		m_Rewinding = true;
		m_RewindStartTime = m_Scene->GetGame()->GetTime();
	}

	float RewindManager::GetTargetTime() {
		float t = m_Scene->GetGame()->GetTime();
		float recDuration = m_MaxTime - m_MinTime;
		float x = (t - m_RewindStartTime) / recDuration;
		if (x < 1) {
			float s = glm::sign(x - 0.5f);
			float o = (s + 1.f) * 0.5f;
			float k = 1.f + 4.f * x;
			x = o - s * 0.5f * glm::pow(2 * (o - s * x), k);
		}
		return m_RewindStartTime - x * recDuration;
	}

	bool RewindManager::Rewind() {
		if (m_Size == 0) {
			return false;
		}
		float target = GetTargetTime();
		int best = m_Head;
		float bestDiff = 9999999.f;
		int idx = m_Head;
		for (int i = 0; i < m_Size; i++) {

			// TODO: Binary search
			float diff = glm::abs(m_Frames[idx].time - target);
			if (diff < bestDiff) {
				best = idx;
				bestDiff = diff;
			}

			idx = (idx + 1) % m_MaxSize;
		}

		DataFrame& frame = m_Frames[best];
		m_Scene->ForeachEntity([this, &frame](Entity* ee) {
			FMegaEntity* e = (FMegaEntity*)ee;
			uint index = frame.nameToIndex[e->GetName()];
			uint size;
			e->Load(&frame.data[0] + index, size);
		});
		return best != m_Head;
	}

	void RewindManager::DeleteHistory() {
		m_Size = 0;
	}

}
