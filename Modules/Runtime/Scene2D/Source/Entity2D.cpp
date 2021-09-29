#include "Entity2D.h"

namespace Ry
{
	Entity2D::Entity2D(World2D* World)
	{
		bCanUpdate = false;
		bUpdateEnabled = false;
		bVisible = true;

		this->OwningWorld = World;

		this->bPendingKill = false;
	}

	Entity2D::~Entity2D()
	{
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
			for (SharedPtr<Component2D> Comp : Components)
			{
				if (Comp->IsUpdateEnabled())
					Comp->Update(Delta);
			}
			
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

	Transform2D Entity2D::GetActorRelativeTransform()
	{
		return RootComponent->GetWorldTransform();
	}

	void Entity2D::SetActorRelativeTransform(const Transform2D& Transform)
	{
		RootComponent->SetRelativeTransform(Transform);
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
		PhysicsComponents.Clear();

		RootComponent.Reset(nullptr);
	}

	void Entity2D::SetVisibility(bool bVisible)
	{
		this->bVisible = bVisible;
		OnRenderStateDirty.Broadcast(this);
	}

	World2D* Entity2D::GetWorld() const
	{
		return OwningWorld;
	}

	void Entity2D::CreatePhysicsState()
	{
		for (SharedPtr<Physics2DComponent> Comp : PhysicsComponents)
		{
			Comp->CreatePhysicsState();
		}
	}

	void Entity2D::Kill()
	{
		bPendingKill = true;
	}

	bool Entity2D::IsPendingKill()
	{
		return bPendingKill;
	}
}
