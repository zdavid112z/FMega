#include "fmega.h"
#include "skybox.h"
#include "fmegaScene.h"
#include "fmegaRenderer.h"
#include "fmegaObjectFactory.h"

namespace fmega {

	Skybox::Skybox(FMegaScene* scene) {
		m_Scene = scene;
	}

	Skybox::~Skybox() {

	}

	void Skybox::Update(float delta) {

	}

	void Skybox::Render(float delta) {
		m_Scene->GetRenderer()->RenderSkybox();
	}

}