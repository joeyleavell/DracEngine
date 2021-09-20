#include "Entity/Component/Component.h"
#include "Entity/Entity.h"
#include "Entity/Component/TransformComponent.h"

namespace Ry
{

	void Component::Update(float Delta)
	{

	}

	// void Component::SetOwningEntity(Entity* Parent)
	// {
	// 	this->OwningEntity = Parent;
	// }
	//
	// Entity* Component::GetOwningEntity() const
	// {
	// 	return OwningEntity;
	// }

	Transform Component::GetWorldTransform()
	{
		if (Parent)
		{
			return Parent->GetWorldTransform();
		}
		else
		{
			return Transform{};
		}
	}

	Transform Component::GetRootRelativeTransform()
	{
		// This is already the root
		if(!Parent)
			return Transform{};

		if (Parent && Parent->Parent)
		{
			return Parent->GetRootRelativeTransform();
		}
		else
		{
			return Transform{};
		}
	}

	TransformComponent* Component::GetParent()
	{
		return Parent;
	}

	void Component::SetParent(TransformComponent* Parent)
	{
		this->Parent = Parent;
	}
	
}