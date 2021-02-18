#pragma once

#include "Core/Core.h"
#include "Transform.h"
#include "PhysicsGen.h"
#include <cmath>

class btCollisionShape;
class btRigidBody;

namespace Ry
{

	struct RigidBody
	{
		btCollisionShape* Shape;
		btRigidBody* Body;
	};

	struct BoundingBox
	{
		float MinX = INFINITY;
		float MaxX = -INFINITY;
		float MinY = INFINITY;
		float MaxY = -INFINITY;
		float MinZ = INFINITY;
		float MaxZ = -INFINITY;

		void operator*=(Vector3 Scale)
		{
			MinX *= Scale.x;
			MaxX *= Scale.x;
			MinY *= Scale.y;
			MaxY *= Scale.y;
			MinZ *= Scale.z;
			MaxZ *= Scale.z;
		}
	};

	PHYSICS_MODULE SharedPtr<RigidBody> MakeBoxBody(Transform Initial, float Mass, BoundingBox Box);
}
