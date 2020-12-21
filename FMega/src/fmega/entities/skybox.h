#pragma once

#include "fmega.h"
#include "fmegaEntity.h"

namespace fmega {

	class FMegaScene;

	class Skybox : public FMegaEntity {
	public:
		Skybox(const std::string& name, Entity* parent, FMegaScene* scene);
		~Skybox();

		virtual void Update(float delta) override;
		virtual void Render(float delta) override;
		virtual byte* GetData(uint& size) override;
		void SkyboxRender(float delta);

		union {
			struct {
				float Offset;
			};
			byte Data[4];
		};
	private:
		FMegaScene* m_Scene;
	};

}