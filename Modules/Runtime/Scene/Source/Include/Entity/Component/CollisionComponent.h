#pragma once

#include "Core/Core.h"
#include "Component.h"
#include "Physics.h"

namespace Ry
{

	class SCENE_MODULE CollisionComponent : public Component
	{
	public:

		bool CollisionEnabled;

		CollisionComponent() = default;
		virtual ~CollisionComponent() = default;

		virtual SharedPtr<RigidBody> ConstructBody() = 0;	
	};

	class SCENE_MODULE BoxCollisionComponent : public CollisionComponent
	{
	public:
		
		float Width;
		float Height;
		float Depth;

		BoxCollisionComponent() = default;
		virtual ~BoxCollisionComponent() = default;
		
		SharedPtr<RigidBody> ConstructBody() override
		{
			Transform RootRelative = GetRootRelativeTransform();
			BoundingBox BB;
			BB.MinX = RootRelative.position.x - Width / 2;
			BB.MaxX = RootRelative.position.x + Width / 2;
			BB.MinY = RootRelative.position.y - Height / 2;
			BB.MaxY = RootRelative.position.y + Height / 2;
			BB.MinZ = RootRelative.position.z - Depth / 2;
			BB.MaxZ = RootRelative.position.z + Depth / 2;

			return Ry::MakeBoxBody(GetWorldTransform(), 0.0f, BB);
		}

	};

	
}
