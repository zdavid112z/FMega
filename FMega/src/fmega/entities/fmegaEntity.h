#pragma once

#include "graphics/mesh.h"
#include "core/entity/entity.h"
#include "collision.h"

namespace fmega {

	class FMegaScene;

	class FMegaEntity : public Entity
	{
	public:
		FMegaEntity(const std::string& name, Entity* parent, FMegaScene* scene);
		virtual void Init(Mesh* mesh, CollisionObject* collision);
		virtual ~FMegaEntity();

		virtual void Update(float delta) override;
		virtual void Render(float delta) override;
		
		glm::vec4 Color = glm::vec4(0);

		CollisionObject* GetCollision() { return m_Collision; }
	protected:
		CollisionObject* m_Collision = nullptr;
		Mesh* m_Mesh = nullptr;
		FMegaScene* m_FMegaScene;
	};

}
