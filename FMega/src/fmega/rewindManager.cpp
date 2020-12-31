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
				DeleteInvalidEntities();
				DeleteHistory();
			}
		}
		else {
			Record();
		}
	}

	void RewindManager::DeleteInvalidEntities() {
		m_Scene->ForeachEntity([this](Entity* ee) {

			if (m_Frames[m_Head].nameToIndex.find(ee->GetName()) ==
				m_Frames[m_Head].nameToIndex.end()) {

				ee->Destroy();
			}
		});
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
			e->Save(&frame.data[0] + index);
		});
	}

	void RewindManager::StartRewind() {
		m_Rewinding = true;
		m_RewindStartTime = m_Scene->GetGame()->GetTime();
	}

	float RewindManager::GetTargetTime() {
		float t = m_Scene->GetGame()->GetTime();
		float recDuration = m_MaxTime - m_MinTime;
		float x = (t - m_RewindStartTime) / recDuration + 0.01f;
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
		float bestDiff = 1e4f;
		int idx = m_Head;

		int bestEarlier = -1;
		float bestEarlierTime = target - 1e4f;
		int bestLater = -1;
		float bestLaterTime = target + 1e4f;
		for (int i = 0; i < m_Size; i++) {

			// TODO: Binary search
			float t = m_Frames[idx].time;
			float diff = glm::abs(t - target);
			if (diff < bestDiff) {
				best = idx;
				bestDiff = diff;
			}
			if (t < target && bestEarlierTime < t) {
				bestEarlierTime = t;
				bestEarlier = idx;
			}
			if (t > target && bestLaterTime > t) {
				bestLaterTime = t;
				bestLater = idx;
			}

			idx = (idx + 1) % m_MaxSize;
		}
		bool hasEarlier = true;
		bool hasLater = true;
		if (bestEarlier == -1) {
			hasEarlier = false;
			bestEarlier = m_Head;
		}
		if (bestLater == -1) {
			hasLater = false;
			bestLater = m_Head;
		}

		DataFrame& frame = m_Frames[best];
		DataFrame& earlier = m_Frames[bestEarlier];
		DataFrame& later = m_Frames[bestLater];
		float amount = (target - bestEarlierTime) / (bestLaterTime - bestEarlierTime);
		if (!hasEarlier) {
			amount = 1.f;
		}
		if (!hasLater) {
			amount = 0.f;
		}
		m_Scene->ForeachEntity([this, &frame, &earlier, &later, amount, target](Entity* ee) {
			FMegaEntity* e = (FMegaEntity*)ee;

			if (frame.nameToIndex.find(e->GetName()) == frame.nameToIndex.end() ||
				earlier.nameToIndex.find(e->GetName()) == earlier.nameToIndex.end() ||
				later.nameToIndex.find(e->GetName()) == later.nameToIndex.end()) {
				return;
			}

			auto index = frame.nameToIndex[e->GetName()];
			auto ptr = &frame.data[0] + index;
			e->LoadStatic(ptr);
			uint staticSize = e->GetStaticSaveSize();

			auto indexEarlier = earlier.nameToIndex[e->GetName()];
			auto ptrEarlier = &earlier.data[0] + indexEarlier;
			auto dynamicEarlier = ptrEarlier + staticSize;
			float* dynamicEarlierFloats = (float*)dynamicEarlier;

			auto indexLater = later.nameToIndex[e->GetName()];
			auto ptrLater = &later.data[0] + indexLater;
			auto dynamicLater = ptrLater + staticSize;
			float* dynamicLaterFloats = (float*)dynamicLater;

			uint dynamicSize = e->GetSaveSize() - e->GetStaticSaveSize();
			uint numFloats = dynamicSize / 4;
			
			std::vector<float> dynamicData(numFloats);
			e->InterpolateDynamicData(dynamicEarlierFloats, dynamicLaterFloats, amount, dynamicData.data(), numFloats);
			e->LoadDynamic((byte*)dynamicData.data());
		});
		return bestLater != m_Head;
	}

	void RewindManager::DeleteHistory() {
		m_Size = 0;
	}

}
