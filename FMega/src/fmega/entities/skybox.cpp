#include "fmega.h"
#include "skybox.h"
#include "../fmegaScene.h"
#include "../fmegaRenderer.h"
#include "../fmegaObjectFactory.h"

namespace fmega {

	Skybox::Skybox(const std::string& name, Entity* parent, FMegaScene* scene) :
		FMegaEntity(name, parent, scene)
	{
		m_Scene = scene;
		Offset = 0;
	}

	Skybox::~Skybox() {

	}

	byte* Skybox::GetData(uint& size) {
		size = sizeof(Data);
		return Data;
	}

	void Skybox::Update(float delta) {
		Offset += delta * m_Scene->MoveSpeed * 0.25f;
	}

	void Skybox::Render(float delta) {

	}

	void Skybox::SkyboxRender(float delta) {
		m_Scene->GetRenderer()->RenderSkybox();
	}

}