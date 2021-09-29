#pragma once

#include "Entity2D.gen.h"
#include "Component2D.h"
#include "Scene2D.h"

namespace Ry
{
	class World2D;
	
	class SCENE2D_MODULE Entity2D
	{
	public:

		MulticastDelegate<Entity2D*> OnRenderStateDirty;
		MulticastDelegate<Entity2D*> OnPhysicsStateDirty;

		Entity2D(World2D* World);
		~Entity2D();
		
		virtual void Update(float Delta);

		void FireUpdate(float Delta);
		void UpdatePrimitives(); // Updates all render-ables

		bool CanEverUpdate() const;
		bool IsUpdateEnabled() const;

		bool IsVisible() const;

		Transform2D GetActorRelativeTransform();
		void SetActorRelativeTransform(const Transform2D& Transform);

		Ry::ArrayList<Ry::SharedPtr<Ry::Primitive2DComponent>>::ConstIterator IteratePrimitives() const;

		template<typename T, typename ...Types>
		SharedPtr<T> CreateComponent(Types... Args);

		void SetRootComponent(const Ry::SharedPtr<Ry::Transform2DComponent>& Root);
		Transform2DComponent* GetRootComponent() const;

		// NOTE: Must call this function before destroying entity
		void ClearComponents();

		void SetVisibility(bool bVisible);

		World2D* GetWorld() const;

		void CreatePhysicsState();

		virtual void OnBeginOverlap(Entity2D* OtherEnt, Physics2DComponent* OtherComp, Physics2DComponent* ThisComp) {};
		virtual void OnEndOverlap(Entity2D* OtherEnt, Physics2DComponent* OtherComp, Physics2DComponent* ThisComp) {};

		virtual void End()
		{
			for(SharedPtr<Physics2DComponent> Phys : PhysicsComponents)
			{
				Phys->DestroyPhysicsState();
			}

			ClearComponents();

			OwningWorld = nullptr;
		};
		
		void Kill();
		bool IsPendingKill();

	protected:

		Ry::ArrayList<Ry::SharedPtr<Ry::Component2D>> Components;
		Ry::ArrayList<Ry::SharedPtr<Ry::Primitive2DComponent>> PrimitiveComponents;
		Ry::ArrayList<Ry::SharedPtr<Ry::Physics2DComponent>> PhysicsComponents;
		
		Ry::SharedPtr<Transform2DComponent> RootComponent;

		bool bCanUpdate;
		bool bUpdateEnabled;

		bool bVisible;

		World2D* OwningWorld;

		bool bPendingKill;
	};

	template <typename T, typename ... Types>
	SharedPtr<T> Entity2D::CreateComponent(Types... Args)
	{
		SharedPtr<T> NewComp = Ry::MakeShared(new T{this, Args...});
		Components.Add(NewComp);

		// Check if we created a new physics component
		SharedPtr<Physics2DComponent> AsPhysics = CastShared<Physics2DComponent>(NewComp);
		if (AsPhysics.IsValid())
		{
			PhysicsComponents.Add(AsPhysics);
			OnPhysicsStateDirty.Broadcast(this);
		}

		// Check if we just created a new primitive component
		SharedPtr<Primitive2DComponent> AsPrimitive = CastShared<Primitive2DComponent>(NewComp);
		if(AsPrimitive.IsValid())
		{
			PrimitiveComponents.Add(AsPrimitive);
			OnRenderStateDirty.Broadcast(this);
		}

		SharedPtr<Transform2DComponent> AsTransform = CastShared<Transform2DComponent>(NewComp);
		if(!RootComponent.IsValid())
		{
			RootComponent = AsTransform;
		}

		return NewComp;
	}

	template<typename T, typename ...Types>
	SharedPtr<T> CreateEntity(Transform2D Transform, Types... Args)
	{
		SharedPtr<T> NewEnt = Ry::MakeShared(new T{ Args... });

		NewEnt->SetActorRelativeTransform(Transform);

		NewEnt->CreatePhysicsState();

		return NewEnt;
	}

	template<typename T, typename ...Types>
	SharedPtr<T> CreateEntity(Types... Args)
	{
		SharedPtr<T> NewEnt = Ry::MakeShared(new T{ Args... });

		NewEnt->CreatePhysicsState();

		return NewEnt;
	}

}
