#pragma once

#include "fmega.h"
#include "core/entity/entity.h"

namespace fmega {

	enum class CollisionObjectType {
		CIRCLE,
		AABB,
	};

	class CollisionObject {
	public:
		CollisionObject(CollisionObjectType type, Entity* parent, const glm::mat4& model) : Parent(parent), Type(type), Model(model) {}

		glm::vec3 GetParentPosition();
		glm::mat4 GetGlobalModel();
		glm::vec3 GetPosition();

		const Entity* Parent;
		const CollisionObjectType Type;
		const glm::mat4 Model;
	};

	class CircleObject : public CollisionObject {
	public:
		CircleObject(Entity* parent, const glm::mat4& model, float radius) :
			CollisionObject(CollisionObjectType::CIRCLE, parent, model),
			Radius(radius) {
		}

		const float Radius;
	};

	class AABBObject : public CollisionObject {
	public:
		AABBObject(Entity* parent, const glm::mat4& model, const glm::vec3& size) :
			CollisionObject(CollisionObjectType::AABB, parent, model),
			Size(size) {
		}

		std::pair<glm::vec3, glm::vec3> GetBounds();

		const glm::vec3 Size;
	};

	class Collision {
	public:
		static bool TestCollision(CollisionObject* a, CollisionObject* b);
		static bool TestCollisionAABBvsAABB(AABBObject* a, AABBObject* b);
		static bool TestCollisionCircleVsCircle(CircleObject* a, CircleObject* b);
		static bool TestCollisionAABBvsCircle(AABBObject* a, CircleObject* b);

	};

}