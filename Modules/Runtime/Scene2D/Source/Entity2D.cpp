#include "Entity2D.h"

namespace Ry
{
	Entity2D::Entity2D()
	{
		bCanUpdate = false;
		bUpdateEnabled = false;
		bVisible = true;
	}

	void Entity2D::UpdatePrimitives()
	{
		// todo: call this function separate from tick
		for(SharedPtr<Primitive2DComponent> Comp : PrimitiveComponents)
		{
			Comp->DrawPrimitive();
		}
	}

	void Entity2D::FireUpdate(float Delta)
	{
		if(bUpdateEnabled)
		{
			Update(Delta);
		}
	}

	void Ry::Entity2D::Update(float Delta)
	{
		UpdatePrimitives();
	}

	bool Entity2D::CanEverUpdate() const
	{
		return bCanUpdate;
	}

	bool Entity2D::IsUpdateEnabled() const
	{
		return bUpdateEnabled;
	}

	bool Entity2D::IsVisible() const
	{
		return bVisible;
	}

	Ry::ArrayList<Ry::SharedPtr<Ry::Primitive2DComponent>>::ConstIterator Entity2D::IteratePrimitives() const
	{
		return PrimitiveComponents.begin();
	}

	void Entity2D::SetRootComponent(const Ry::SharedPtr<Ry::Transform2DComponent>& Root)
	{
		this->RootComponent = Root;
	}

	Transform2DComponent* Entity2D::GetRootComponent() const
	{
		return RootComponent.Get();
	}

	void Entity2D::ClearComponents()
	{
		Components.Clear();
		PrimitiveComponents.Clear();
	}

	void Entity2D::SetVisibility(bool bVisible)
	{
		this->bVisible = bVisible;
		OnRenderStateDirty.Broadcast(this);
	}
}
