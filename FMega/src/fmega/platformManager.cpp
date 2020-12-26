#include "fmega.h"
#include "platformManager.h"

namespace fmega {

	PlatformManager::PlatformManager() {
		m_IsDone = true;
		m_IsWorking = false;
		m_HasWork = false;
		m_NextJob = 0;
		m_CurrentJob = -1;
		m_UpdateInterval = 0.05f;
		m_Timer = 0.0f;
		m_WorkerRunning = true;

		m_Worker = std::thread([this]() {
			while (m_WorkerRunning) {
				{
					std::unique_lock<std::mutex> lock(m_WorkerMutex);
					while (!m_HasWork && m_WorkerRunning) {
						m_WorkerCondition.wait(lock);
					}
				}
				if (!m_WorkerRunning) {
					break;
				}
				m_HasWork = false;
				PlatformGenData& data = m_CurrentData;
				FMegaObjectFactory::GenPlatformMeshData(
					data.vertices, data.indices,
					data.gridWidth, data.gridLength, data.gridHeight,
					data.width, data.length, data.height,
					data.randomness,
					data.color, data.colorRandomness,
					data.startingZ, data.xyRandomness);
				m_IsDone = true;
			}
		});
	}

	PlatformManager::~PlatformManager() {
		{
			std::unique_lock<std::mutex> lock(m_WorkerMutex);
			m_WorkerRunning = false;
			m_WorkerCondition.notify_one();
		}
		m_Worker.join();
	}

	void PlatformManager::Update(float delta) {
		/*m_Timer += delta;
		if (m_Timer >= m_UpdateInterval) {
			m_Timer -= m_UpdateInterval;
		}
		else {
			return;
		}*/
		if (m_IsWorking && m_IsDone) {
			m_CurrentJob++;
			m_IsWorking = false;
		}
		if (!m_IsWorking) {
			if (m_Queue.empty()) {
				return;
			}
			auto data = m_Queue.front();
			m_Queue.pop();
			m_IsWorking = true;
			m_IsDone = false;
			{
				std::unique_lock<std::mutex> lock(m_WorkerMutex);
				m_CurrentData = data;
				m_HasWork = true;
				m_WorkerCondition.notify_one();
			}
		}
	}

	int PlatformManager::SubmitPlatform(const PlatformGenData& data) {
		m_Queue.push(data);
		return m_NextJob++;
	}

	bool PlatformManager::IsDone(int id) {
		return id <= m_CurrentJob;
	}

	void PlatformManager::Join(int id) {
		while (id > m_CurrentJob) {
			Update(0.01f);
		}
	}

}
