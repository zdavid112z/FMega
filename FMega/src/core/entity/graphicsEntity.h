#pragma once

#include "entity.h"
#include "graphics/mesh.h"
#include "graphics/shader.h"

namespace fmega {

	class GraphicsEntity : public Entity {
	public:
		GraphicsEntity(const std::string& name, Entity* parent, Scene* scene, Mesh* mesh, Shader* shader);
		virtual ~GraphicsEntity();

		virtual void Render(float delta);
		virtual void Update(float delta) = 0;
		virtual void BindShaderUniforms(float delta);

	protected:
		Mesh* m_Mesh;
		Shader* m_Shader;
	};

}
