#pragma once

#include "Entity2D.gen.h"
#include "Component2D.h"
#include "Scene2D.h"

namespace Ry
{
	class SCENE2D_MODULE Entity2D
	{
	public:

		MulticastDelegate<Entity2D*> OnRenderStateDirty;

		Entity2D();

		virtual void Update(float Delta);

		void FireUpdate(float Delta);
		void UpdatePrimitives(); // Updates all render-ables

		bool CanEverUpdate() const;
		bool IsUpdateEnabled() const;

		bool IsVisible() const;

		Ry::ArrayList<Ry::SharedPtr<Ry::Primitive2DComponent>>::ConstIterator IteratePrimitives() const;

		template<typename T, typename ...Types>
		SharedPtr<T> CreateComponent(Types... Args);

		void SetRootComponent(const Ry::SharedPtr<Ry::Transform2DComponent>& Root);
		Transform2DComponent* GetRootComponent() const;

		// NOTE: Must call this function before destroying entity
		void ClearComponents();

		void SetVisibility(bool bVisible);

	protected:

		Ry::ArrayList<Ry::SharedPtr<Ry::Component2D>> Components;
		Ry::ArrayList<Ry::SharedPtr<Ry::Primitive2DComponent>> PrimitiveComponents;
		
		Ry::SharedPtr<Transform2DComponent> RootComponent;

		bool bCanUpdate;
		bool bUpdateEnabled;

		bool bVisible;
	};

	template <typename T, typename ... Types>
	SharedPtr<T> Entity2D::CreateComponent(Types... Args)
	{
		SharedPtr<T> NewComp = Ry::MakeShared(new T{this, Args...});
		Components.Add(NewComp);

		// Check if we just created a new primitive component
		SharedPtr<Primitive2DComponent> AsPrimitive = CastShared<Primitive2DComponent>(NewComp);
		if(AsPrimitive.IsValid())
		{
			PrimitiveComponents.Add(AsPrimitive);
		}

		SharedPtr<Transform2DComponent> AsTransform = CastShared<Transform2DComponent>(NewComp);
		if(!RootComponent.IsValid())
		{
			RootComponent = AsTransform;
		}

		return NewComp;
	}

	template<typename T, typename ...Types>
	SharedPtr<T> CreateEntity(Types... Args)
	{
		SharedPtr<T> NewEnt = Ry::MakeShared(new T{ Args... });

		return NewEnt;
	}

}
