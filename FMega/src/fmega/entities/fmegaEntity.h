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
		virtual void Init(CollisionObject* collision);
		virtual ~FMegaEntity();

		virtual void RewindUpdate(float delta);
		virtual void Update(float delta) override;
		virtual void Render(float delta) override;

		virtual byte* GetData(uint& size);
		virtual uint GetSaveSize();
		virtual uint GetStaticSaveSize();
		virtual uint Save(byte* data);
		virtual uint LoadStatic(byte* data);
		virtual uint LoadDynamic(byte* data);
		virtual void InterpolateDynamicData(float* p1, float* p2, float amount, float* out, int count);

		CollisionObject* GetCollision() { return m_Collision; }
	protected:
		CollisionObject* m_Collision = nullptr;
		FMegaScene* m_FMegaScene;
	};

}
