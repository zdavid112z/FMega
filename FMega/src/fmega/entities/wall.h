#pragma once

#include <thread>
#include <atomic>
#include "fmegaEntity.h"
#include "utils/colors.h"
#include "graphics/gpuBuffer.h"

namespace fmega {

	class FMegaScene;

	class Wall : public FMegaEntity {
	public:
		Wall(const std::string& name, Entity* parent, FMegaScene* scene,
			float x, float z, float width);
		~Wall();

		virtual void Update(float delta) override;
		virtual void Render(float delta) override;
		virtual byte* GetData(uint& size) override;

		static const float Thickness;
		static const float Height;

		union {
			struct {
				bool Broken;
			};
			byte Data[1];
		};
	private:
		int m_JobID;
		uint32 m_NumIndices;
		Mesh* m_Mesh;
		GPUBuffer* m_VBO;
		GPUBuffer* m_IBO;
		FMegaScene* m_Scene;
	};

}