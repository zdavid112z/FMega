#pragma once

#include "fmega.h"

namespace fmega {

	class FMegaScene;

	class Skybox {
	public:
		Skybox(FMegaScene* scene);
		~Skybox();

		void Update(float delta);
		void Render(float delta);
	private:
		FMegaScene* m_Scene;
	};

}