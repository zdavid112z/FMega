#include "fmega.h"
#include "collision.h"
#include "core/entity/entity.h"

namespace fmega {

	glm::vec3 CollisionObject::GetParentPosition() {

		glm::mat4 transform = Parent->GetGlobalModel();
		return glm::vec3(transform[3][0], transform[3][1], transform[3][2]);
	}

	std::pair<glm::vec3, glm::vec3> AABBObject::GetBounds() {
		glm::vec3 center = GetParentPosition();
		glm::vec3 halfSize = Size / 2.f;
		return std::make_pair(center - halfSize, center + halfSize);
	}

	bool Collision::TestCollision(CollisionObject* a, CollisionObject* b) {

		if (a->Type == CollisionObjectType::AABB && b->Type == CollisionObjectType::AABB) {
			return TestCollisionAABBvsAABB((AABBObject*)a, (AABBObject*)b);
		}
		if (a->Type == CollisionObjectType::CIRCLE && b->Type == CollisionObjectType::CIRCLE) {
			return TestCollisionCircleVsCircle((CircleObject*)a, (CircleObject*)b);
		}
		if (a->Type == CollisionObjectType::AABB && b->Type == CollisionObjectType::CIRCLE) {
			return TestCollisionAABBvsCircle((AABBObject*)a, (CircleObject*)b);
		}
		if (a->Type == CollisionObjectType::CIRCLE && b->Type == CollisionObjectType::AABB) {
			return TestCollisionAABBvsCircle((AABBObject*)b, (CircleObject*)a);
		}
		LOG_WARN("Unimplemented collision test!");
		return false;
	}

	bool Collision::TestCollisionAABBvsAABB(AABBObject* a, AABBObject* b) {
		auto boundsA = a->GetBounds();
		auto boundsB = b->GetBounds();
		return
			boundsA.first.x <= boundsB.second.x &&
			boundsA.second.x >= boundsB.first.x &&
			boundsA.first.y <= boundsB.second.y &&
			boundsA.second.y >= boundsB.first.y &&
			boundsA.first.z <= boundsB.second.z &&
			boundsA.second.z >= boundsB.first.z;
	}

	bool Collision::TestCollisionCircleVsCircle(CircleObject* a, CircleObject* b) {
		auto centerA = a->GetParentPosition();
		auto centerB = b->GetParentPosition();
		auto diff = centerA - centerB;
		float rab = a->Radius + b->Radius;
		return glm::length2(diff) <= rab * rab;
	}

	bool Collision::TestCollisionAABBvsCircle(AABBObject* a, CircleObject* b) {
		LOG_ERROR("Unimplemented collision between aabb and cirlce!");
		return false;
	}

}