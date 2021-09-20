#include "Entity/Entity.h"
#include "btBulletDynamicsCommon.h"

namespace Ry
{

	void Entity::Update(float Delta)
	{
		if (PhysicsEnabled)
		{
			// Update transform position to reflect physics body
			btTransform ObjectTransform = Body->Body->getWorldTransform();
			btVector3 ObjectPos = ObjectTransform.getOrigin();

			float& XRot = RootTransform->GetRelativeTransform().rotation.x;
			float& YRot = RootTransform->GetRelativeTransform().rotation.y;
			float& ZRot = RootTransform->GetRelativeTransform().rotation.z;

			// Set the position and orientation of the object
			RootTransform->GetRelativeTransform().position = Ry::Vector3(ObjectPos.getX(), ObjectPos.getY(), ObjectPos.getZ());
			Body->Body->getOrientation().getEulerZYX(ZRot, YRot, XRot);

			RootTransform->GetRelativeTransform().rotation = Ry::Vector3(RAD_TO_DEG(XRot), RAD_TO_DEG(YRot), RAD_TO_DEG(ZRot));
		}

		for (SharedPtr<Ry::Component> Component : Components)
		{
			Component->Update(Delta);
		}
	}

	void Entity::InitPhysics()
	{

	}
	
	void Entity::OnSpawn()
	{

	}
	
}