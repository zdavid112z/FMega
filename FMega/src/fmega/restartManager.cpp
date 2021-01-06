#include "fmega.h"
#include "restartManager.h"
#include "fmegaScene.h"
#include "fmegaRenderer.h"

namespace fmega {

	RestartManager::RestartManager(FMegaScene* scene) :
		m_Scene(scene) {

		m_Timer = 0;
		m_Lost = false;
		m_TimeToRectsAnim = 1.0f;
		m_LostToRestartTime = 2.f;
		m_RectsAnimTime = 2.f;
		m_DeltaFactor = 0.1f;
		m_SlowDeath = false;
	}

	RestartManager::~RestartManager() {

	}

	float RestartManager::GetAdjDelta(float delta) {
		return m_SlowDeath ? m_DeltaFactor * delta : delta;
	}

	void RestartManager::Update(float delta) {
		m_Timer += delta;
		UpdateRects(delta);
	}

	void RestartManager::Render(float delta) {
		if (m_Timer >= m_TimeToRectsAnim && m_Timer <= m_TimeToRectsAnim + m_RectsAnimTime) {
			for (RestartRect& r : m_Rects) {
				glm::vec3 realCenter = glm::vec3(r.origin + r.calculatedSize / 2.f, 10);
				glm::mat4 model = glm::translate(glm::mat4(1), realCenter) * glm::scale(glm::mat4(1), glm::vec3(glm::abs(r.calculatedSize / 2.f), 1.f));
				{
					MeshRenderData data;
					data.model = model;
					data.color = r.color;
					data.opacity = 1.f;
					data.albedoStrength = 0.f;
					m_Scene->GetRenderer()->RenderMesh(m_Scene->GetAssets()->BoxMesh, data, true);
				}
			}
		}
	}

	bool RestartManager::HasLost() {
		return m_Lost;
	}

	void RestartManager::OnPlayerLost(float isSlow) {
		m_Lost = true;
		m_Timer = 0.f;
		m_SlowDeath = isSlow;
		ResetRects(3);
	}

	bool RestartManager::ShouldRestart() {
		if (m_Lost && m_Timer >= m_LostToRestartTime) {
			Reset();
			return true;
		}
		return false;
	}

	void RestartManager::Reset() {
		m_Lost = false;
		m_SlowDeath = false;
	}

	void RestartManager::ResetRects(int count) {
		m_Rects.clear();
		glm::vec4 colors[] = {
			glm::vec4(glm::vec3(0xEB, 0x5F, 0x5E) / 255.f, 1),
			glm::vec4(glm::vec3(0x72, 0xBA, 0xEB) / 255.f, 1),
			glm::vec4(glm::vec3(0xEB, 0xE9, 0x5B) / 255.f, 1),
			glm::vec4(glm::vec3(240, 194, 67) / 255.f, 1),
		};

		glm::vec2 rectSize = glm::vec2(32.f / count, 18.f / count);

		for (int i = 0; i < count; i++) {
			for (int j = 0; j < count; j++) {
				RestartRect r;
				r.color = colors[i];
				r.size = rectSize;
				int ci = i, cj = j;

				r.indexExpand = i * (count + 1);
				if (i % 2 == 0) {
					r.indexExpand += j;
				}
				else {
					r.indexExpand += count - j - 1;
					r.size.x = -r.size.x;
					cj++;
				}

				r.indexCollapse = count * (count + 1) + j * (count + 1);
				if (j % 2 == 0) {
					r.indexCollapse += i;
					r.size.y = -r.size.y;
					ci++;
				}
				else {
					r.indexCollapse += count - i - 1;					
				}

				r.origin = (glm::vec2(cj, ci) / glm::vec2(count, count)) * glm::vec2(32.f, 18.f) - glm::vec2(16.f, 9.f);
				r.calculatedSize = glm::vec2(0, r.size.y);
				m_Rects.push_back(r);
			}
		}
	}

	void RestartManager::UpdateRects(float delta) {
		float t = m_Timer - m_TimeToRectsAnim;
		if (t < 0) {
			return;
		}
		int count2 = m_Rects.size();
		int count = int(glm::sqrt(count2));
		t = (t / m_RectsAnimTime) * 2 * count * (count + 1);
		int id = int(t);
		for (RestartRect& r : m_Rects) {
			r.calculatedSize.x = r.size.x * glm::clamp(t - float(r.indexExpand), 0.f, 1.f);
			r.calculatedSize.y = r.size.y * (1.f - glm::clamp(t - float(r.indexCollapse), 0.f, 1.f));
		}
	}

}