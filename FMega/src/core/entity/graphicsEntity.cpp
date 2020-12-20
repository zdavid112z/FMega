#include "fmega.h"
#include "graphicsEntity.h"

namespace fmega {

	GraphicsEntity::GraphicsEntity(const std::string& name, Entity* parent, Scene* scene, Mesh* mesh, Shader* shader) :
		Entity(name, parent, scene),
		m_Mesh(mesh),
		m_Shader(shader)
	{

	}

	GraphicsEntity::~GraphicsEntity()
	{

	}

	void GraphicsEntity::Render(float delta)
	{
		m_Shader->Bind();
		BindShaderUniforms(delta);
		m_Mesh->DrawNow();
	}

	void GraphicsEntity::Update(float delta)
	{
		
	}

	void GraphicsEntity::BindShaderUniforms(float delta)
	{

	}

}
