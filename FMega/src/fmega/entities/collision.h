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
		CollisionObject(CollisionObjectType type, Entity* parent) : Parent(parent), Type(type) {}

		glm::vec3 GetParentPosition();

		const Entity* Parent;
		const CollisionObjectType Type;
	};

	class CircleObject : public CollisionObject {
	public:
		CircleObject(Entity* parent, const glm::mat4& model, float radius) :
			CollisionObject(CollisionObjectType::CIRCLE, parent),
			Radius(radius),
			Model(model) {
		}

		const float Radius;
		const glm::mat4 Model;
	};

	class AABBObject : public CollisionObject {
	public:
		AABBObject(Entity* parent, const glm::mat4& model, const glm::vec3& size) :
			CollisionObject(CollisionObjectType::CIRCLE, parent),
			Size(size),
			Model(model) {
		}

		std::pair<glm::vec3, glm::vec3> GetBounds();

		const glm::vec3 Size;
		const glm::mat4 Model;
	};

	class Collision {
	public:
		static bool TestCollision(CollisionObject* a, CollisionObject* b);
		static bool TestCollisionAABBvsAABB(AABBObject* a, AABBObject* b);
		static bool TestCollisionCircleVsCircle(CircleObject* a, CircleObject* b);
		static bool TestCollisionAABBvsCircle(AABBObject* a, CircleObject* b);

	};

}