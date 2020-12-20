#pragma once

#include "fmega.h"
#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include "fmegaObjectFactory.h"

namespace fmega {

	class PlatformManager
	{
	public:
		PlatformManager();
		~PlatformManager();

		void Update(float delta);
		int SubmitPlatform(const PlatformGenData& data);
		bool IsDone(int id);
		void Join(int id);
	private:
		float m_Timer;
		float m_UpdateInterval;
		int m_CurrentJob;
		int m_NextJob;
		std::queue<PlatformGenData> m_Queue;
		std::thread m_Worker;
		std::mutex m_WorkerMutex;
		std::condition_variable m_WorkerCondition;
		bool m_IsWorking;
		std::atomic_bool m_HasWork;
		std::atomic_bool m_IsDone;
		std::atomic_bool m_WorkerRunning;
		PlatformGenData m_CurrentData;
	};

}